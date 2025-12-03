const program = [
    ["const", 0, 10], // r0 => 10
    ["const", 1, 20], // r1 => 20
    ["add", 0, 1], // r0 = r0 + r1
    ["print", 0] // print r0
]; // 01 0A 02 03

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
        const [_, reg1, reg2] = instr;
        vm.regs[reg1] += vm.regs[reg2];
    } else if (op === "print") {
        const [_, reg] = instr;
        const val = vm.regs[reg];
        console.log(val);
    }

    vm.pc++;
}
