const DemoAgent = require('./core/agent');

const AgentManager = require('./supervisor');

class Main
{
    constructor()
    {
        this.AgentManager = new AgentManager();

        this.AgentManager.run('Wie viel Uhr haben wir gerade?');
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

new Main();