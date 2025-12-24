const http = require('http'), fs = require('fs'), os = require('os'), path = require('path');

const Logger = require('syntex-logger'), LoggerSpecial = require('./core/logger.js');

const player = require('node-wav-player'), formidable = require('formidable');

const PLUGIN_NAME = 'TTS Client', PORT = 3000;

class TTSClient
{
	constructor()
	{
		this.logger = new LoggerSpecial({ pluginName : PLUGIN_NAME }, { time : true });
		this.loggerSpecial = new Logger({ pluginName : PLUGIN_NAME }, { language : 'de', time : true });

        this.logger.log('warn', 'HTTP-Server wird gestartet ..');

        this.initWebServer();
	}

    initWebServer()
    {
        http.createServer((req, res) => {

            this.logger.debug(`Request: ${req.method} ${req.url}`);

            if(req.url === '/speak' && req.method === 'POST')
            {
                const form = new formidable.IncomingForm({ multiples : false });

                form.parse(req, (err, fields, files) => {

                    if(err) 
                    {
                        this.logger.log('error', `Form parse error: ${err.message}`);

                        res.writeHead(400, { 'Content-Type' : 'text/plain' });
                        res.end('Fehler beim Parsen des Formulars');

                        return;
                    }

                    this.logger.debug('Fields:', fields);
                    this.logger.debug('Files:', files);

                    const uploadedFile = files.file[0];

                    if(!uploadedFile || !uploadedFile.filepath)
                    {
                        this.logger.log('error', 'Audiofile nicht gefunden!');

                        res.writeHead(400, { 'Content-Type' : 'text/plain' });
                        res.end('Kein Audiofile gefunden');

                        return;
                    }

                    try
                    {
                        const tmpFile = path.join(os.tmpdir(), `tts_${Date.now()}.wav`);

                        fs.copyFileSync(uploadedFile.filepath, tmpFile);

                        this.logger.debug(`Temporary file created: ${tmpFile}`);

                        player.play({ path : tmpFile }).then(() => {

                            this.logger.debug('Audioausgabe wurde gestartet!');

                            setTimeout(() => {
                                
                                fs.unlinkSync(tmpFile);

                                this.logger.debug(`Temporary file removed: ${tmpFile}`);
                            
                            }, 60000);

                        }).catch((err) => this.logger.err(err));

                        res.writeHead(200, { 'Content-Type': 'text/plain' });
                        res.end('Audio wird abgespielt');
                    }
                    catch(err) 
                    {
                        this.logger.log('error', `${err.message}`);

                        res.writeHead(500, { 'Content-Type' : 'text/plain' });
                        res.end('Fehler: ' + err.message);
                    }
                });

                return;
            }

            res.writeHead(404, { 'Content-Type' : 'text/plain' });
            res.end('Nicht gefunden');

        }).listen(PORT, () => {

            this.logger.debug(`HTTP-Server läuft auf http://localhost:${PORT}`);
        });
    }
}

new TTSClient();