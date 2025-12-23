const http = require('http');
const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');


// Ordner für generierte Audios
const outputDir = path.join(__dirname, 'audio');
if (!fs.existsSync(outputDir)) fs.mkdirSync(outputDir);

const server = http.createServer((req, res) => {
    if (req.method === 'POST' && req.url === '/tts') {
        let body = '';
        req.on('data', chunk => {
            body += chunk.toString();
        });
        req.on('end', () => {
            try {
                const { text } = JSON.parse(body);
                if (!text) throw new Error('Kein Text übergeben');

                const filename = `tts_${new Date().getTime()}.wav`;
                const outputPath = path.join(outputDir, filename);

                // Docker-Befehl ausführen
                const cmd = `docker run --rm -v ${outputDir}:/data ghcr.io/coqui-ai/tts:latest --model_name tts_models/de/thorsten/tacotron2-DDC --text "${text.replace(/"/g, '\\"')}" --out_path /data/${filename}`;
                exec(cmd, (error, stdout, stderr) => {
                    if (error) {
                        console.error(stderr);
                        res.writeHead(500, { 'Content-Type': 'application/json' });
                        res.end(JSON.stringify({ error: stderr }));
                        return;
                    }

                    // Audio als Download bereitstellen
                    res.writeHead(200, {
                        'Content-Type': 'audio/wav',
                        'Content-Disposition': `attachment; filename="${filename}"`
                    });
                    fs.createReadStream(outputPath).pipe(res);

                   

                    const audioFile = outputPath;

                    // Windows Media Player starten
                    exec(`start wmplayer "${audioFile}"`, (err) => {
                        if (err) console.error('Fehler beim Abspielen:', err);
                        else console.log('Audio wird abgespielt...');
                    });

                });
            } catch (err) {
                res.writeHead(400, { 'Content-Type': 'application/json' });
                res.end(JSON.stringify({ error: err.message }));
            }
        });
    } else {
        res.writeHead(404, { 'Content-Type': 'text/plain' });
        res.end('Not found');
    }
});

const PORT = 3000;
server.listen(PORT, () => {
    console.log(`TTS Server läuft auf http://localhost:${PORT}`);
});

/*
const http = require('http');
const say = require('say');

const PORT = 3000;

const server = http.createServer((req, res) => {

    if(req.url === '/speak')
    {
        if(req.method === 'POST')
        {
            let body = '';

            req.on('data', (chunk) => {

                body += chunk.toString();
            });

            req.on('end', () => {

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
            //say.speak('Das ist ein Beispiel für eine Sprachausgabe', 'Stefan');
            const { exec } = require('child_process');

exec('powershell -ExecutionPolicy Bypass -File "speak.ps1"', (err, stdout, stderr) => {
    if(err) console.error(err);
    else console.log('Gesprochen');
});



            
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
*/