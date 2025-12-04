import fs from 'fs';

const opCode = Object.freeze({
    CONST: 0x01,
    ADD: 0x03,
    PRINT: 0x04,
    JUMPIFZERO: 0x05,
    JUMP: 0x06
});

function readAsmProgram(file) {
    let program = [];
    const fprogram = fs.readFileSync(file)
        .toString()
        .split('\n');
    fprogram.forEach(inst => {
        const [op, l, r] = inst.replace(/r/g, '').replace(',', '').split(' ');
        program.push([opCode[op], parseInt(l), parseInt(r)]);
    })
    return program;
}

export default readAsmProgram
