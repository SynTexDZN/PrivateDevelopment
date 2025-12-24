FROM ghcr.io/coqui-ai/tts:latest

# Node.js installieren
RUN apt-get update && apt-get install -y nodejs npm

WORKDIR /app
COPY package*.json server.js ./
RUN npm install

EXPOSE 3000

# ENTRYPOINT auf null setzen, damit CMD alleine läuft
ENTRYPOINT []

CMD ["node","server.js"]