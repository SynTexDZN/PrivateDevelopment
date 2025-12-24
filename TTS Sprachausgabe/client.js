const http = require('http');
const fs = require('fs');
const os = require('os');
const path = require('path');
const player = require('node-wav-player');
const formidable = require('formidable');

const PORT = 3000;

http.createServer((req, res) => {

    console.log(`[DEBUG] Request: ${req.method} ${req.url}`);

    if(req.url === '/speak' && req.method === 'POST')
    {
        // Formidable zum Parsen von multipart/form-data
        const form = new formidable.IncomingForm({ multiples : false });

        form.parse(req, (err, fields, files) => {

            if(err) 
            {
                console.error(`[ERROR] Form parse error: ${err.message}`);

                res.writeHead(400, { 'Content-Type' : 'text/plain' });
                res.end('Fehler beim Parsen des Formulars');

                return;
            }

            console.log('Fields:', fields);
            console.log('Files:', files);

            // Prüfen, ob das Binary-Feld existiert
            const uploadedFile = files.file[0];  // Name muss exakt 'file' sein

            if(!uploadedFile || !uploadedFile.filepath)
            {
                console.error('[ERROR] Audiofile nicht gefunden!');

                res.writeHead(400, { 'Content-Type' : 'text/plain' });
                res.end('Kein Audiofile gefunden');

                return;
            }

            try
            {
                const tmpFile = path.join(os.tmpdir(), `tts_${Date.now()}.wav`);

                fs.copyFileSync(uploadedFile.filepath, tmpFile);

                console.log(`[DEBUG] Temporary file created: ${tmpFile}`);

                player.play({ path : tmpFile }).then(() => {

                    console.log('Audioausgabe wurde gestartet!');

                    setTimeout(() => {
                        
                        fs.unlinkSync(tmpFile);

                        console.log(`[DEBUG] Temporary file removed: ${tmpFile}`);
                    
                    }, 60000);

                }).catch((err) => console.error(err));

                res.writeHead(200, { 'Content-Type': 'text/plain' });
                res.end('Audio wird abgespielt');
            }
            catch(err) 
            {
                console.error(`[ERROR] ${err.message}`);

                res.writeHead(500, { 'Content-Type' : 'text/plain' });
                res.end('Fehler: ' + err.message);
            }
        });

        return;
    }

    res.writeHead(404, { 'Content-Type' : 'text/plain' });
    res.end('Nicht gefunden');

}).listen(PORT, () => {

    console.log(`[DEBUG] Client-Server läuft auf http://localhost:${PORT}`);
});