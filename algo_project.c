#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Proyek Praktikum Literasi Nusantara Jaya
// Kelompok: (isi nama anggota kelompok di sini)

#define BOOK_FILE "databuku.txt"
#define HISTORY_FILE "history.txt"
#define CODE_PREFIX "BK"

typedef struct {
    char code[16];
    char name[128];
    char category[128];
    double price;
} Book;

typedef struct {
    char code[16];
    int quantity;
    double total;
} History;

typedef struct {
    Book *items;
    size_t size;
    size_t capacity;
} BookList;

typedef struct {
    History *items;
    size_t size;
    size_t capacity;
} HistoryList;

// Trim newline characters from a string in place.
static void trim_newline(char *text) {
    if (!text) {
        return;
    }
    size_t len = strcspn(text, "\r\n");
    text[len] = '\0';
}

// Initialize an empty BookList.
static void init_book_list(BookList *list) {
    list->items = NULL;
    list->size = 0;
    list->capacity = 0;
}

// Initialize an empty HistoryList.
static void init_history_list(HistoryList *list) {
    list->items = NULL;
    list->size = 0;
    list->capacity = 0;
}

// Ensure BookList has enough capacity for one more item.
static int ensure_book_capacity(BookList *list) {
    if (list->size < list->capacity) {
        return 1;
    }
    size_t new_capacity = list->capacity == 0 ? 8 : list->capacity * 2;
    Book *resized = realloc(list->items, new_capacity * sizeof(Book));
    if (!resized) {
        return 0;
    }
    list->items = resized;
    list->capacity = new_capacity;
    return 1;
}

// Ensure HistoryList has enough capacity for one more item.
static int ensure_history_capacity(HistoryList *list) {
    if (list->size < list->capacity) {
        return 1;
    }
    size_t new_capacity = list->capacity == 0 ? 8 : list->capacity * 2;
    History *resized = realloc(list->items, new_capacity * sizeof(History));
    if (!resized) {
        return 0;
    }
    list->items = resized;
    list->capacity = new_capacity;
    return 1;
}

// Release all heap memory used by BookList.
static void free_book_list(BookList *list) {
    free(list->items);
    list->items = NULL;
    list->size = 0;
    list->capacity = 0;
}

// Release all heap memory used by HistoryList.
static void free_history_list(HistoryList *list) {
    free(list->items);
    list->items = NULL;
    list->size = 0;
    list->capacity = 0;
}

// Read a non-empty line from stdin with prompt.
static void prompt_line(const char *message, char *buffer, size_t size) {
    while (1) {
        printf("%s", message);
        if (!fgets(buffer, (int)size, stdin)) {
            clearerr(stdin);
            continue;
        }
        trim_newline(buffer);
        if (strlen(buffer) == 0) {
            puts("Input tidak boleh kosong.");
            continue;
        }
        return;
    }
}

// Read an integer within range from stdin.
static int prompt_int_range(const char *message, int min, int max) {
    char line[128];
    while (1) {
        printf("%s", message);
        if (!fgets(line, sizeof(line), stdin)) {
            clearerr(stdin);
            continue;
        }
        trim_newline(line);
        if (strlen(line) == 0) {
            puts("Input tidak boleh kosong.");
            continue;
        }
        char *endptr = NULL;
        long value = strtol(line, &endptr, 10);
        if (endptr == line || *endptr != '\0') {
            puts("Input harus berupa angka.");
            continue;
        }
        if (value < min || value > max) {
            printf("Input harus di antara %d dan %d.\n", min, max);
            continue;
        }
        return (int)value;
    }
}

// Read a floating point number greater than or equal to min_value.
static double prompt_double_min(const char *message, double min_value) {
    char line[128];
    while (1) {
        printf("%s", message);
        if (!fgets(line, sizeof(line), stdin)) {
            clearerr(stdin);
            continue;
        }
        trim_newline(line);
        if (strlen(line) == 0) {
            puts("Input tidak boleh kosong.");
            continue;
        }
        char *endptr = NULL;
        double value = strtod(line, &endptr);
        if (endptr == line || *endptr != '\0') {
            puts("Input harus berupa angka (boleh desimal).");
            continue;
        }
        if (value < min_value) {
            printf("Input harus lebih besar atau sama dengan %.2f.\n", min_value);
            continue;
        }
        return value;
    }
}

// Wait for user confirmation to continue.
static void pause_for_enter(void) {
    printf("Tekan Enter untuk kembali ke menu...");
    fflush(stdout);
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* discard */
    }
}

// Append a new book into BookList.
static int append_book(BookList *list, const Book *book) {
    if (!ensure_book_capacity(list)) {
        return 0;
    }
    list->items[list->size++] = *book;
    return 1;
}

// Append a new history entry into HistoryList.
static int append_history(HistoryList *list, const History *history) {
    if (!ensure_history_capacity(list)) {
        return 0;
    }
    list->items[list->size++] = *history;
    return 1;
}

// Load books from databuku.txt; ignore missing file.
static void load_books(BookList *books) {
    FILE *fp = fopen(BOOK_FILE, "r");
    if (!fp) {
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);
        if (strlen(line) == 0) {
            continue;
        }
        char *code = strtok(line, "|");
        char *name = strtok(NULL, "|");
        char *category = strtok(NULL, "|");
        char *price_text = strtok(NULL, "|");
        if (!code || !name || !category || !price_text) {
            continue;
        }
        Book book;
        snprintf(book.code, sizeof(book.code), "%s", code);
        snprintf(book.name, sizeof(book.name), "%s", name);
        snprintf(book.category, sizeof(book.category), "%s", category);
        book.price = strtod(price_text, NULL);
        append_book(books, &book);
    }
    fclose(fp);
}

// Load history entries from history.txt; ignore missing file.
static void load_history(HistoryList *history) {
    FILE *fp = fopen(HISTORY_FILE, "r");
    if (!fp) {
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);
        if (strlen(line) == 0) {
            continue;
        }
        char *code = strtok(line, "|");
        char *qty_text = strtok(NULL, "|");
        char *total_text = strtok(NULL, "|");
        if (!code || !qty_text || !total_text) {
            continue;
        }
        History item;
        snprintf(item.code, sizeof(item.code), "%s", code);
        item.quantity = (int)strtol(qty_text, NULL, 10);
        item.total = strtod(total_text, NULL);
        append_history(history, &item);
    }
    fclose(fp);
}

// Save all book data to databuku.txt.
static int save_books(const BookList *books) {
    FILE *fp = fopen(BOOK_FILE, "w");
    if (!fp) {
        return 0;
    }
    for (size_t i = 0; i < books->size; ++i) {
        const Book *book = &books->items[i];
        fprintf(fp, "%s|%s|%s|%.2f\n", book->code, book->name, book->category, book->price);
    }
    fclose(fp);
    return 1;
}

// Save all history data to history.txt.
static int save_history(const HistoryList *history) {
    FILE *fp = fopen(HISTORY_FILE, "w");
    if (!fp) {
        return 0;
    }
    for (size_t i = 0; i < history->size; ++i) {
        const History *item = &history->items[i];
        fprintf(fp, "%s|%d|%.2f\n", item->code, item->quantity, item->total);
    }
    fclose(fp);
    return 1;
}

// Print the main menu options.
static void print_menu(void) {
    puts("\n=== Menu Utama Literasi Nusantara Jaya ===");
    puts("1. Input Data Buku Baru");
    puts("2. View History Transaksi Penjualan");
    puts("3. View Buku");
    puts("4. Delete History");
    puts("5. Delete Buku");
    puts("6. Exit");
    puts("7. Input Transaksi Penjualan");
    puts("8. Sort Buku");
}

// Find the highest numeric suffix in book codes.
static int highest_book_suffix(const BookList *books) {
    int highest = 0;
    for (size_t i = 0; i < books->size; ++i) {
        const char *code = books->items[i].code;
        if (strncmp(code, CODE_PREFIX, strlen(CODE_PREFIX)) == 0) {
            const char *digits = code + strlen(CODE_PREFIX);
            if (*digits) {
                int value = (int)strtol(digits, NULL, 10);
                if (value > highest) {
                    highest = value;
                }
            }
        }
    }
    return highest;
}

// Generate the next book code using sequential numbering.
static void generate_next_code(const BookList *books, char *buffer, size_t size) {
    int next = highest_book_suffix(books) + 1;
    snprintf(buffer, size, "%s%03d", CODE_PREFIX, next);
}

// Display all books in a formatted table.
static void view_books(const BookList *books) {
    if (books->size == 0) {
        puts("Belum ada data buku.");
        return;
    }
    puts("\nDaftar Buku:");
    printf("%-5s %-8s %-25s %-20s %-10s\n", "No", "Kode", "Nama", "Jenis", "Harga");
    for (size_t i = 0; i < books->size; ++i) {
        printf("%-5zu %-8s %-25s %-20s Rp %.2f\n",
               i + 1,
               books->items[i].code,
               books->items[i].name,
               books->items[i].category,
               books->items[i].price);
    }
}

// Display all history transactions in a formatted table.
static void view_history(const HistoryList *history) {
    if (history->size == 0) {
        puts("Belum ada history transaksi.");
        return;
    }
    puts("\nHistory Transaksi:");
    printf("%-5s %-8s %-15s %-10s\n", "No", "Kode", "Jumlah Terjual", "Total");
    for (size_t i = 0; i < history->size; ++i) {
        printf("%-5zu %-8s %-15d Rp %.2f\n",
               i + 1,
               history->items[i].code,
               history->items[i].quantity,
               history->items[i].total);
    }
}

// Add a new book record using user input.
static void menu_input_book(BookList *books) {
    Book book;
    generate_next_code(books, book.code, sizeof(book.code));
    prompt_line("Masukkan nama buku: ", book.name, sizeof(book.name));
    prompt_line("Masukkan jenis buku: ", book.category, sizeof(book.category));
    book.price = prompt_double_min("Masukkan harga buku: ", 0.0);
    if (!append_book(books, &book)) {
        puts("Gagal menambahkan buku (memori tidak cukup).");
        return;
    }
    printf("Data buku dengan kode %s berhasil disimpan.\n", book.code);
}

// Remove a book by index supplied by the user.
static void menu_delete_book(BookList *books) {
    if (books->size == 0) {
        puts("Tidak ada data buku untuk dihapus.");
        return;
    }
    view_books(books);
    int index = prompt_int_range("Pilih nomor buku yang ingin dihapus: ", 1, (int)books->size);
    size_t pos = (size_t)(index - 1);
    for (size_t i = pos; i + 1 < books->size; ++i) {
        books->items[i] = books->items[i + 1];
    }
    books->size--;
    puts("Data Successfully delete..");
}

// Remove a history entry by index supplied by the user.
static void menu_delete_history(HistoryList *history) {
    if (history->size == 0) {
        puts("Tidak ada history untuk dihapus.");
        return;
    }
    view_history(history);
    int index = prompt_int_range("Pilih nomor history yang ingin dihapus: ", 1, (int)history->size);
    size_t pos = (size_t)(index - 1);
    for (size_t i = pos; i + 1 < history->size; ++i) {
        history->items[i] = history->items[i + 1];
    }
    history->size--;
    puts("Data Successfully delete..");
}

// Locate a book by code, returns pointer or NULL.
static Book *find_book_by_code(BookList *books, const char *code) {
    for (size_t i = 0; i < books->size; ++i) {
        if (strcmp(books->items[i].code, code) == 0) {
            return &books->items[i];
        }
    }
    return NULL;
}

// Input a new sales transaction and append to history.
static void menu_input_transaction(BookList *books, HistoryList *history) {
    if (books->size == 0) {
        puts("Tidak ada data buku. Silakan input data buku terlebih dahulu.");
        return;
    }
    char code[16];
    prompt_line("Masukkan kode buku: ", code, sizeof(code));
    Book *book = find_book_by_code(books, code);
    if (!book) {
        puts("Kode buku tidak ditemukan.");
        return;
    }
    int qty = prompt_int_range("Masukkan jumlah terjual: ", 1, 1000000);
    History item;
    snprintf(item.code, sizeof(item.code), "%s", book->code);
    item.quantity = qty;
    item.total = book->price * qty;
    if (!append_history(history, &item)) {
        puts("Gagal mencatat transaksi (memori tidak cukup).");
        return;
    }
    puts("Transaksi berhasil dicatat.");
}

// Bubble sort copy of books by name ascending and display result.
static void sort_books_by_name(const BookList *books) {
    if (books->size == 0) {
        puts("Tidak ada data buku untuk diurutkan.");
        return;
    }
    Book *copy = malloc(books->size * sizeof(Book));
    if (!copy) {
        puts("Gagal mengalokasikan memori untuk sorting.");
        return;
    }
    memcpy(copy, books->items, books->size * sizeof(Book));
    for (size_t i = 0; i < books->size; ++i) {
        for (size_t j = 0; j + 1 < books->size - i; ++j) {
            if (strcmp(copy[j].name, copy[j + 1].name) > 0) {
                Book temp = copy[j];
                copy[j] = copy[j + 1];
                copy[j + 1] = temp;
            }
        }
    }
    puts("\nHasil pengurutan berdasarkan nama (ascending):");
    printf("%-5s %-8s %-25s %-20s %-10s\n", "No", "Kode", "Nama", "Jenis", "Harga");
    for (size_t i = 0; i < books->size; ++i) {
        printf("%-5zu %-8s %-25s %-20s Rp %.2f\n",
               i + 1, copy[i].code, copy[i].name, copy[i].category, copy[i].price);
    }
    free(copy);
}

// Selection sort copy of books by price descending and display result.
static void sort_books_by_price(const BookList *books) {
    if (books->size == 0) {
        puts("Tidak ada data buku untuk diurutkan.");
        return;
    }
    Book *copy = malloc(books->size * sizeof(Book));
    if (!copy) {
        puts("Gagal mengalokasikan memori untuk sorting.");
        return;
    }
    memcpy(copy, books->items, books->size * sizeof(Book));
    for (size_t i = 0; i + 1 < books->size; ++i) {
        size_t max_index = i;
        for (size_t j = i + 1; j < books->size; ++j) {
            if (copy[j].price > copy[max_index].price) {
                max_index = j;
            }
        }
        if (max_index != i) {
            Book temp = copy[i];
            copy[i] = copy[max_index];
            copy[max_index] = temp;
        }
    }
    puts("\nHasil pengurutan berdasarkan harga (descending):");
    printf("%-5s %-8s %-25s %-20s %-10s\n", "No", "Kode", "Nama", "Jenis", "Harga");
    for (size_t i = 0; i < books->size; ++i) {
        printf("%-5zu %-8s %-25s %-20s Rp %.2f\n",
               i + 1, copy[i].code, copy[i].name, copy[i].category, copy[i].price);
    }
    free(copy);
}

// Display sorting sub-menu and dispatch selected action.
static void menu_sort_books(const BookList *books) {
    puts("\n=== Sub Menu Sorting Buku ===");
    puts("a. Urutkan berdasarkan nama (ascending, bubble sort)");
    puts("b. Urutkan berdasarkan harga (descending, selection sort)");
    puts("c. Kembali");
    while (1) {
        char choice[16];
        prompt_line("Pilih opsi (a/b/c): ", choice, sizeof(choice));
        if (strlen(choice) != 1) {
            puts("Pilihan tidak valid.");
            continue;
        }
        char option = (char)tolower((unsigned char)choice[0]);
        if (option == 'a') {
            sort_books_by_name(books);
            return;
        } else if (option == 'b') {
            sort_books_by_price(books);
            return;
        } else if (option == 'c') {
            return;
        }
        puts("Pilihan tidak valid.");
    }
}

// Persist data to files and release resources before exiting.
static void finalize_and_exit(BookList *books, HistoryList *history) {
    if (!save_books(books)) {
        puts("Gagal menyimpan databuku.txt.");
    }
    if (!save_history(history)) {
        puts("Gagal menyimpan history.txt.");
    }
}

// Entry point: load data, handle menu loop, and persist updates.
int main(void) {
    BookList books;
    HistoryList history;
    init_book_list(&books);
    init_history_list(&history);

    load_books(&books);
    load_history(&history);

    int running = 1;
    while (running) {
        print_menu();
        int choice = prompt_int_range("Pilih menu [1-8]: ", 1, 8);
        switch (choice) {
            case 1:
                menu_input_book(&books);
                pause_for_enter();
                break;
            case 2:
                view_history(&history);
                pause_for_enter();
                break;
            case 3:
                view_books(&books);
                pause_for_enter();
                break;
            case 4:
                menu_delete_history(&history);
                pause_for_enter();
                break;
            case 5:
                menu_delete_book(&books);
                pause_for_enter();
                break;
            case 6:
                finalize_and_exit(&books, &history);
                running = 0;
                break;
            case 7:
                menu_input_transaction(&books, &history);
                pause_for_enter();
                break;
            case 8:
                menu_sort_books(&books);
                pause_for_enter();
                break;
            default:
                puts("Menu tidak dikenal.");
                break;
        }
    }

    free_book_list(&books);
    free_history_list(&history);
    puts("Terima kasih telah menggunakan aplikasi.");
    return 0;
}
