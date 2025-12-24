const http = require('http');
const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');
const { v4 : uuidv4 } = require('uuid');

const outputDir = '/data';

if (!fs.existsSync(outputDir)) fs.mkdirSync(outputDir);

const server = http.createServer((req, res) => {

	if(req.method === 'POST' && req.url === '/tts')
	{
		let body = '';

		req.on('data', chunk => body += chunk.toString());

		req.on('end', () => {

			const { text, model, speaker, language } = JSON.parse(body);
			const filename = `tts_${uuidv4()}.wav`;

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

				fs.createReadStream(path.join(outputDir, filename)).pipe(res);
			});
		});
	}
	else
	{
		res.writeHead(404); res.end('Not found');
	}
});

server.listen(3000);

console.log('Server läuft auf Port 3000 v1');