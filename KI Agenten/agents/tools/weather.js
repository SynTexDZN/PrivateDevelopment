const Agent = require('../../core/agent');

module.exports = class Weather extends Agent
{
    constructor(platform)
    {
        super('Wetter', `
            SYSTEM:
            Du bist der Wetter-Agent. Deine Aufgabe: Erstelle aus den im INPUT übergebenen 24-Stunden-Arrays (index 0..23) eine kurze, menschenlesbare Wetter-Zusammenfassung auf Deutsch.
            Wichtig: Du darfst nichts erfinden. Alle Aussagen MÜSSEN direkt aus dem INPUT ableitbar sein.
            Regeln:
            - Index i = i:00 Uhr.
            - Erlaube nur diese Ableitungen: min/max + Uhrzeit(en), erstes/letztes Vorkommen von Werten > 0 für Regen/Schnee, Stunden mit maximaler Regenwahrscheinlichkeit, einfache Aussagen über das Vorhandensein (z. B. "Es gibt Regenstunden" oder "kein Regen").
            - Keine Trend-Prognosen, keine Schätzungen, keine zusätzlichen Interpretationen.
            - Verwende natürliche Sprache (kein JSON). Formuliere kurz (2–6 Sätze).
            OUTPUT-RULES:
            - Maximal 6 Sätze.
            - Jede Behauptung muss auf den Arrays basieren.
            - Wenn Werte fehlen oder nicht 24 Elemente vorhanden sind, schreibe: "Daten unvollständig" und beende.

        `);

        this.data = {};

        this.RequestManager = platform.RequestManager;
    }

    loadData()
    {
        return new Promise((resolve) => {

            var coords = { latitude : 50.36, longitude : 8.5343 };
            var url = 'http://127.0.0.1:7777/serverside/dashboard/weather?latitude=' + coords.latitude + '&longitude=' + coords.longitude;

            console.log(url);

            this.RequestManager.fetch(url, { headers : { 'session' : '9ff1888de72e0265037df94d0c07f16a' } }).then((response) => {

                if(response != null && response.data instanceof Object && Object.keys(response.data).length > 0)
                {
                    //this.data = response.data;

                    this.data = {
                        temperatur : { today : [5, 3, 3, 4, 7, 12, 20, 21, 22, 22, 21, 20, 20, 21, 22, 22, 23, 24, 22, 20, 18, 15, 10, 6] },
                        schnee : { today : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0] },
                        regen : { today : [0, 0, 0, 0, 0, 0, 0, 20, 100, 100, 80, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0] }
                    }
                }

                resolve();
            });
        });
    }

    run()
    {
        return new Promise((resolve) => {

            this.loadData().then(() => {

                super.run(JSON.stringify(this.data)).then((out) => resolve(out));
            });
        });
    }
}