const { spawn } = require('child_process');

module.exports = class Agent
{
    constructor(basePrompt)
    {
        console.log('Agent wurde gestartet!');

        this.basePrompt = basePrompt;
    }

    run(prompt)
    {
        const p = spawn('ollama', ['run', 'llama3.2:3b'], { stdio : ['pipe', 'pipe', 'inherit'] });

        console.log(`Agent führt LLM Anfrage aus .. ( ${prompt} )`);

        p.stdin.write(this.basePrompt);
        p.stdin.write(prompt);
        p.stdin.end();

        let out = '';

        p.stdout.on('data', (d) => out += d.toString());

        return new Promise((resolve) => p.stdout.on('end', () => {

            console.log(`Agent hat eine Antwort vom LLM erhalten: ${out}`);
            
            resolve(out);
        }));
    }
}