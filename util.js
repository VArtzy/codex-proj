import fs from 'fs';

const opCode = Object.freeze({
    CONST: 0x01,
    ADD: 0x03,
    PRINT: 0x04,
    JUMPIFZERO: 0x05,
    JUMP: 0x06
});

function newReadAsmProgram(file) {
    let program = [];
    let counter = 0;
    const fprogram = fs.readFileSync(file)
        .toString()
        .split('\n');
    fprogram.forEach(inst => {
        const [op, l, r] = inst.replace(/r/g, '').replace(',', '').split(' ');
        program[counter] = opCode[op];
        program[counter + 1] = l;
        if (r !== undefined) {
            program[counter + 2] = r;
        }
        counter += 3;
    })
    return new Uint8Array(program);
}

/**
    * DEPRECATED -- use newReadAsmProgram instead
*/
function readAsmProgram(file) {
    let program = [];
    const fprogram = fs.readFileSync(file)
        .toString()
        .split('\n');
    fprogram.forEach(inst => {
        const [op, l, r] = inst.replace(/r/g, '').replace(',', '').split(' ');
        program.push([op.toLowerCase(), parseInt(l), parseInt(r)]);
    })
    return program;
}

export { readAsmProgram, newReadAsmProgram }
