const { Ollama } = require('ollama');

module.exports = class Agent
{
    constructor(name, params, prompt)
    {
        console.log(`${name}-Agent wurde gestartet!`);

        this.Ollama = new Ollama();

        this.params = {
            model : 'llama3.2:3b',
        };

        this.name = name;
        this.prompt = prompt;

        for(const x in params)
        {
            this.params[x] = params[x];
        }
    }

    run(prompt)
    {
        return new Promise((resolve) => {

            console.log(`${this.name}-Agent führt eine LLM Anfrage aus .. ( ${prompt} )`);

            prompt = `${this.prompt}

            ---
            
            INPUT:

            ${prompt}`;

            console.log(this.params);

            this.Ollama.generate({ ...this.params, prompt }).then((data) => {

                console.log(`${this.name}-Agent hat eine Antwort vom LLM erhalten: ${data.response}`);
                
                resolve(data.response);
            });
        });
    }
}