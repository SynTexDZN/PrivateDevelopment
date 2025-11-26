const DemoAgent = require('./core/agent');

const AgentManager = require('./supervisor');

class Main
{
    constructor()
    {
        console.log(0);

        this.AgentManager = new AgentManager();

        console.log(1);

        this.AgentManager.run('Wie viel Uhr haben wir gerade?').then((res) => console.log('2', res));
    }

    async demo()
    {
        const agent = new DemoAgent();

        await agent.run('Hallo').then((res) => console.log('2', res));

        for(let i = 0; i < 10; i++)
        {
            await agent.run('Erstelle eine kurze, knappe guten Morgen Nachricht mit maximal 2 Sätzen zum motivieren!').then((res) => console.log('2', res));;
        }
    }
}

new Main();