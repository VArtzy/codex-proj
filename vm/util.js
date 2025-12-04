import fs from 'fs';

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

export default readAsmProgram
