const { spawn } = require('child_process');

module.exports = class Agent
{
    constructor(name, prompt)
    {
        //console.log(`${name}-Agent wurde gestartet!`);

        this.name = name;
        this.prompt = prompt;
    }

    run(prompt)
    {
        const p = spawn('ollama', ['run', 'llama3.2:3b'], { stdio : ['pipe', 'pipe', 'inherit'] });

        //console.log(`${this.name}-Agent führt eine LLM Anfrage aus .. ( ${prompt} )`);

        p.stdin.write(this.prompt);

        p.stdin.write(`
            
            ---
            
            INPUT:
        `);

        p.stdin.write(prompt);

        p.stdin.end();

        let out = '';

        p.stdout.on('data', (d) => out += d.toString());

        return new Promise((resolve) => p.stdout.on('end', () => {

            //console.log(`${this.name}-Agent hat eine Antwort vom LLM erhalten: ${out}`);
            
            resolve(out);
        }));
    }
}