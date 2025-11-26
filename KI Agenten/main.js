const DemoAgent = require('./core/agent');

const Supervisor = require('./supervisor');

class Main
{
    constructor()
    {
        this.Supervisor = new Supervisor();

        console.log('1 Frage:', 'Wie viel Uhr haben wir gerade?');

        this.Supervisor.run('Wie viel Uhr haben wir gerade?').then((out) => {

            console.log('1 Antwort:', out);

            console.log('2 Frage:', 'Wer bist du?');
            
            this.Supervisor.run('Wer bist du?').then((out) => {
                
                console.log('2 Antwort:', out);
            });
        });

        //new Benchmark(100, this.Supervisor, 'Wie viel Uhr haben wir gerade?', 'internal');

        //new Benchmark(10, this.Supervisor, 'Wer bist du?', 'internal');

        //new Benchmark(10, this.Supervisor, 'Wer war Angela Merkel?', 'external');
    }

    async demo()
    {
        const agent = new DemoAgent();

        await agent.run('Hallo');

        for(let i = 0; i < 10; i++)
        {
            await agent.run('Erstelle eine kurze, knappe guten Morgen Nachricht mit maximal 2 Sätzen zum motivieren!');
        }
    }
}

class Benchmark
{
    constructor(iterations, agent, prompt, filter)
    {
        this.iterations = iterations;
        this.agent = agent;
        this.prompt = prompt;
        this.filter = filter;

        this.result = [];

        this.run();
    }

    run()
    {
        var promiseArray = [];

        for(let i = 0; i < this.iterations; i++)
        {
            promiseArray.push(new Promise((callback) => this.agent.run(this.prompt).then((out) => {

                if(out.includes(this.filter)) this.result.push(out);

                callback();
            })));
        }

        Promise.all(promiseArray).then(() => console.log(this.result, this.result.length))
    }
}

new Main();