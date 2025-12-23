const http = require('http');
const say = require('say');

const PORT = 3000;

const server = http.createServer((req, res) => {

    console.log(req.url, req.method);

    if(req.url === '/speak')
    {
        if(req.method === 'POST')
        {
            let body = '';

            req.on('data', (chunk) => {

                body += chunk.toString();
            });

            req.on('end', () => {

                console.log('END');

                try
                {
                    const { text } = JSON.parse(body);

                    if(!text) throw new Error('Kein Text angegeben');

                    say.speak(text);

                    res.writeHead(200, { 'Content-Type': 'text/plain' });
                    res.end('Text wird gesprochen.');
                }
                catch(err)
                {
                    res.writeHead(400, { 'Content-Type': 'text/plain' });
                    res.end('Fehler: ' + err.message);
                }
            });
        }
        else
        {    
            res.writeHead(200, { 'Content-Type': 'text/plain' });
            res.end('Text wird gesprochen.');
        }
    }
    else 
    {
        res.writeHead(404, { 'Content-Type': 'text/plain' });
        res.end('Nicht gefunden');
    }
});

server.listen(PORT, () => {

    console.log(`TTS-Server läuft auf http://localhost:${PORT}`);
});