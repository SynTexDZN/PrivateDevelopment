FROM ghcr.io/coqui-ai/tts:latest

RUN apt-get update && apt-get install -y nodejs npm

WORKDIR /app
COPY package*.json ./
COPY server.js ./
COPY core ./core

RUN npm install

EXPOSE 3000

ENTRYPOINT []

CMD ["node","server.js"]