// const program1 = [
//     ["const", 0, 10], // r0 => 10
//     ["const", 1, 20], // r1 => 20
//     ["add", 0, 1], // r0 = r0 + r1
//     ["print", 0] // print r0
// ]; // 01 0A 02 03

const program = [
    ["const", 0, 5], // r0 => 5
    ["print", 0], // print
    ["add", 0, -1], // r0--
    ["jumpIfZero", 0, 5], // if r0 == 0 goto end
    ["jump", 1], // jump to print
    ["print", 0] // end
];

const vm = {
    regs: [0, 0, 0, 0], // 4 registers
    pc: 0 // program counter
};

while(vm.pc < program.length) {
    const instr = program[vm.pc];
    const op = instr[0];

    if (op === "const") {
        const [_, reg, val] = instr;
        vm.regs[reg] = val;
    } else if (op === "add") {
        const [_, reg, regIndex] = instr;
        if (regIndex === -1) {
            vm.regs[reg]--;
        } else {
            vm.regs[reg] += vm.regs[regIndex];
        }
    } else if (op === "jump") {
        const [_, targetIndex] = instr;
        vm.pc = targetIndex;
        continue;
    } else if (op === "jumpIfZero") {
        const [_, reg, targetIndex] = instr;
        if (vm.regs[reg] === 0) {
            vm.pc = targetIndex;
            continue;
        }
    } else if (op === "print") {
        const [_, reg] = instr;
        const val = vm.regs[reg];
        console.log(val);
    }

    vm.pc++;
}
