const path = require('path');

var logger;

const originalStderrWrite = process.stderr.write.bind(process.stderr);

process.stderr.write = (chunk, encoding, callback) => {

	var reference, scriptPath, message = chunk;

	if(typeof chunk == 'string')
	{
		if(chunk.includes('[') && chunk.includes('] '))
		{
			message = chunk.substring(chunk.split('] ')[0].length + 2);
		}

		if(typeof message == 'string' && message.includes('at '))
		{
			var p = message.split('at ')[1];

			if(p.includes('\n'))
			{
				p = p.split('\n')[0];
			}

			if(p.includes('(') && p.includes(':'))
			{
				scriptPath = p.split('(')[1].split(':')[0];
			}

			var s = (p.match(/\//g) || []).length;

			if(s > 0 && p.includes(':'))
			{
				p = p.split('/')[s];

				reference = { script : p.split(':')[0], row : p.split(':')[1] };
			}
		}
	}

	if(logger != null && scriptPath != null && reference != null && scriptPath.includes(path.resolve(__dirname, '../..') + '/'))
	{
		logger.log('error', message, '( [' + reference.script + '] bei Zeile [' + reference.row + '] )');
	
		chunk = '';
	}

	return originalStderrWrite(chunk, encoding, callback);
};

module.exports = class Logger
{
	constructor(platform, options = {})
	{
		logger = this;

		this.time = options.time || false;
		this.levels = options.levels || {};

		if(platform.pluginName != null)
		{
			this.pluginName = platform.pluginName;
		}
		else
		{
			this.log('warn', 'Es wurde kein Plugin Name angegeben!');
		}
	}

	log(level)
	{
		var messages = [ ...arguments ], prefix = '';

		messages.splice(0, 1);

		if(this.levels[level] != false)
		{
			var color = '';

			if(level == 'success')
			{
				color = '\x1b[92m';
			}
			else if(level == 'update')
			{
				color = '\x1b[96m';
			}
			else if(level == 'read')
			{
				color = '\x1b[36m';
			}
			else if(level == 'info')
			{
				color = '\x1b[93m';
			}
			else if(level == 'warn')
			{
				color = '\x1b[93m';
			}
			else if(level == 'debug')
			{
				color = '\x1b[35m';
			}
			else if(level == 'connect')
			{
				color = '\x1b[96m';
			}
			else if(level == 'disconnect')
			{
				color = '\x1b[91m';
			}
			else
			{
				color = '\x1b[31m';
			}

			if(this.time)
			{
				var date = new Date(), weekDay = ['Sonntag', 'Montag', 'Dienstag', 'Mittwoch', 'Donnerstag', 'Freitag', 'Samstag'][date.getDay()];

				prefix += weekDay[0].toUpperCase() + weekDay.substring(1, 2) + ' ' + ('0' + date.getHours()).slice(-2) + ':' + ('0' + date.getMinutes()).slice(-2) + ':' + ('0' + date.getSeconds()).slice(-2) + ' ';
			}

			prefix += '[' + this.pluginName + '] ' + color + '[' + level.toUpperCase() + ']\x1b[0m';

			if(level == 'error')
			{
				console.log(prefix, messages[0]);

				for(var i = 1; i < messages.length; i++)
				{
					console.log(messages[i]);
				}
			}
			else
			{
				console.log(prefix, ...messages);
			}
		}
	}

	err(error)
	{
		if(error instanceof Object && error.message != null && error.stack != null)
		{
			var reference, stack = error.stack.split('\n');

			stack.splice(0, 1);

			stack = stack.join('\n');

			if(error.stack.includes('at '))
			{
				var p = error.stack.split('at ')[1];

				if(p.includes('\n'))
				{
					p = p.split('\n')[0];
				}

				var s = (p.match(/\//g) || []).length;

				if(s > 0 && p.includes(':'))
				{
					p = p.split('/')[s];

					reference = { script : p.split(':')[0], row : p.split(':')[1] };
				}
			}

			if(reference != null)
			{
				this.log('error', 'Fehler: ' + error.message + ' ( [' + reference.script + '] bei Zeile [' + reference.row + '] )', stack);
			}
			else
			{
				this.log('error', 'Fehler: ' + error.message, stack);
			}
		}
		else
		{
			this.log('error', error);
		}
	}

	debug()
	{
		this.log('debug', ...arguments);
	}
}