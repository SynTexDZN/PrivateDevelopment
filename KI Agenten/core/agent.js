const { spawn } = require('child_process');

module.exports = class Agent
{
    constructor(platform)
    {
        this.basePrompt = 'Du bist ein induvidueller Agent. Du antwortest nur in Deutsch!';
    }

    runLLM(prompt)
    {
        const p = spawn('ollama', ['run', 'llama3.2:3b'], { stdio : ['pipe', 'pipe', 'inherit'] });

        p.stdin.write(prompt);
        p.stdin.write(prompt);
        p.stdin.end();

        let out = '';

        p.stdout.on('data', (d) => out += d.toString());

        return new Promise((resolve) => p.stdout.on('end', () => resolve(out)));
    }
}