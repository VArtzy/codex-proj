import { readAsmProgram, newReadAsmProgram } from "./util.js";

// const program1 = [
//     ["const", 0, 10], // r0 => 10
//     ["const", 1, 20], // r1 => 20
//     ["add", 0, 1], // r0 = r0 + r1
//     ["print", 0] // print r0
// ]; // 01 0A 02 03

// const program2 = [
//     ["const", 0, 5], // r0 => 5
//     ["print", 0], // print
//     ["add", 0, -1], // r0--
//     ["jumpIfZero", 0, 5], // if r0 == 0 goto end
//     ["jump", 1], // jump to print
//     ["print", 0] // end
// ];

// const program3 = readAsmProgram('inst.asm');

// const program4 = [
//     [0x01, 0, 10], // r0 => 10
//     [0x01, 1, 20], // r1 => 20
//     [0x03, 0, 1], // r0 = r0 + r1
//     [0x04, 0] // print r0
// ];

// const program5 = [
//     [0x01, 0, 5], // r0 => 5
//     [0x04, 0], // print
//     [0x03, 0, -1], // r0--
//     [0x05, 0, 5], // if r0 == 0 goto end
//     [0x06, 1], // jump to print
//     [0x04, 0] // end
// ];

// const program6 = new Uint8Array([
//     0x01, 0, 5, // r0 => 5
//     0x04, 0, // print r0
//     0x03, 0, -1, // r0--
//     0x05, 0, 13, // if r0 == 0 goto end
//     0x06, 3, // jump to print
//     0x04, 0, // end
// ]);

// const program = newReadAsmProgram('inst.asm');

const program = new Uint8Array([
    0x01, 0, 10,
    0x01, 1, 20,
    0x03, 0, 1,
    0x04, 0,
]);

const vm = {
    regs: [0, 0, 0, 0], // 4 registers
    pc: 0 // program counter
};

while (vm.pc < program.length) {
    const op = program[vm.pc];
    if (op === 0x01) {
        const reg = program[vm.pc + 1];
        const val = program[vm.pc + 2];
        vm.regs[reg] = val;
        vm.pc += 2;
    } else if (op === 0x03) {
        const reg = program[vm.pc + 1];
        const regIndex = program[vm.pc + 2];
        if (regIndex === 255) {
            vm.regs[reg]--;
        } else {
            vm.regs[reg] += vm.regs[regIndex];
        }
        vm.pc += 2;
    } else if (op === 0x06) {
        const targetIndex = program[vm.pc + 1];
        vm.pc = targetIndex;
        continue;
    } else if (op === 0x05) {
        const reg = program[vm.pc + 1];
        const targetIndex = program[vm.pc + 2];
        if (vm.regs[reg] === 0) {
            vm.pc = targetIndex;
            continue;
        }
        vm.pc += 2;
    } else if (op === 0x04) {
        const reg = program[vm.pc + 1];
        const val = vm.regs[reg];
        console.log(val);
        vm.pc++;
    } else {
        throw new SyntaxError('Opcodes not defined or bad structure');
    }
    vm.pc++;
}

// while(vm.pc < program.length) {
//     const instr = program[vm.pc];
//     const op = instr[0];
//
//     if (op === 0x01) {
//         const [_, reg, val] = instr;
//         vm.regs[reg] = val;
//     } else if (op === 0x03) {
//         const [_, reg, regIndex] = instr;
//         if (regIndex === -1) {
//             vm.regs[reg]--;
//         } else {
//             vm.regs[reg] += vm.regs[regIndex];
//         }
//     } else if (op === 0x06) {
//         const [_, targetIndex] = instr;
//         vm.pc = targetIndex;
//         continue;
//     } else if (op === 0x05) {
//         const [_, reg, targetIndex] = instr;
//         if (vm.regs[reg] === 0) {
//             vm.pc = targetIndex;
//             continue;
//         }
//     } else if (op === 0x04) {
//         const [_, reg] = instr;
//         const val = vm.regs[reg];
//         console.log(val);
//     } else {
//         throw SyntaxError('Opcodes not defined or bad structure');
//     }
//
//     vm.pc++;
// }
