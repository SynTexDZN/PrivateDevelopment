const DemoAgent = require('./core/agent');

class AgentManager
{
    constructor()
    {
        this.demo();
    }

    async demo()
    {
        const agent = new DemoAgent();

        await agent.runLLM('Hallo').then((res) => console.log('2', res));

        for(let i = 0; i < 10; i++)
        {
            await agent.runLLM('Erstelle eine kurze, knappe guten Morgen Nachricht mit maximal 2 Sätzen zum motivieren!').then((res) => console.log('2', res));;
        }
    }
}

console.log('-1');

new AgentManager();