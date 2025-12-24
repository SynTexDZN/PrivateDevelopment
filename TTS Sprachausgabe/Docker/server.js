const http = require('http'), fs = require('fs'), path = require('path'), { exec } = require('child_process');

const LoggerSpecial = require('./core/logger.js');

const { v4 : uuidv4 } = require('uuid');

const PLUGIN_NAME = 'TTS Server', PORT = 3000, OUTPUT_DIR = '/data';

class TTSServer
{
	constructor()
	{
		this.logger = new LoggerSpecial({ pluginName : PLUGIN_NAME }, { time : true });

        this.logger.log('warn', 'HTTP-Server wird gestartet ..');

		if(!fs.existsSync(OUTPUT_DIR)) fs.mkdirSync(OUTPUT_DIR);

        this.initWebServer();
	}

    initWebServer()
    {
		const server = http.createServer((req, res) => {

			if(req.method === 'POST' && req.url === '/tts')
			{
				let body = '';

				req.on('data', (chunk) => body += chunk.toString());

				req.on('end', () => {

					const { text, model, speaker, language } = JSON.parse(body), filename = `tts_${uuidv4()}.wav`;

					let command = `tts --model_name tts_models/de/thorsten/tacotron2-DDC --text "${text.replace(/"/g, '\\"')}" --out_path /data/${filename} --vocoder_name vocoder_models/de/thorsten/hifigan_v1`;

					if(model == 1)
					{
						command = `tts --model_name tts_models/multilingual/multi-dataset/xtts_v2 --text "${text.replace(/"/g, '\\"')}" --out_path /data/${filename} --speaker_idx "${speaker}" --language_idx ${language}`;
					}

					if(model == 2)
					{
						command = `tts --model_name tts_models/multilingual/multi-dataset/xtts_v2 --text "${text.replace(/"/g, '\\"')}" --out_path /data/${filename} --speaker_wav /voices/de_male.wav --language_idx ${language}`;
					}

					exec(command, (error) => {

						if(error)
						{
							res.writeHead(500); res.end(error.toString());

							return;
						}

						res.writeHead(200, {
							'Content-Type' : 'audio/wav',
							'Content-Disposition' : `attachment; filename="${filename}"`
						});

						fs.createReadStream(path.join(OUTPUT_DIR, filename)).pipe(res);
					});
				});
			}
			else
			{
				res.writeHead(404); res.end('Not found');
			}
		});

		server.listen(PORT);

		this.logger.debug(`HTTP-Server läuft auf http://localhost:${PORT}`);
	}
}

new TTSServer();