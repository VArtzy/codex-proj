.global _main
.align 2

_main:
    b _reboot
    b _terminate

_reboot:
    mov X0, #1
    mov X16, #55
    svc 0

_terminate:  
    mov X0, #69
    mov X16, #1
    svc 0
