const Agent = require('../core/agent'), ErrorAgent = require('./error');

module.exports = class Supervisor extends Agent
{
    constructor()
    {
        super('Supervisor', { temperature : 0.2, top_p : 0.3, top_k : 20 }, `
            SYSTEM:
            Du bist ein Supervisor-Agent.
            Deine einzige Aufgabe ist es zu entscheiden, ob der INPUT vom internen Wissensspeicher beantwortet werden kann oder ob externe Tools bzw. Plugins erforderlich sind.
            Du darfst niemals raten oder interpretieren. Deine Entscheidungen folgen festen Regeln.

            ---

            TASK:
            Analysiere die User-Anfrage anhand folgender Entscheidungsregeln (hart, deterministisch):

            1. Dynamische Informationen (immer "external"):
            Wenn eine Information sich seit dem Trainingsdatum verändert haben kann → "external"
            Alle Anfragen, die zeitliche, zeitabhängige, aktuelle, gegenwartsbezogene oder veränderliche Informationen betreffen, sind immer und ohne Ausnahme → "external".
            Dazu gehören insbesondere folgende Begriffe oder Konzepte:
            - "Uhr", "Uhrzeit", "wie spät", "Time", "current time"
            - "Datum", "Tag", "Wochentag", "heute", "jetzt", "gerade", "aktuell"
            - "neuste", "neueste", "letzte", "jüngste", "zuletzt", "vorhin"
            - "News", "Nachrichten", "Update", "Trend", "aktuelle Lage"
            - Preise, Kurse, Märkte, Krypto, Aktien
            - Echtzeitdaten aller Art (Wetter, Verkehr, Systeme, APIs)
            Wenn einer dieser Begriffe vorkommt oder impliziert wird → "external".
            Keine Ausnahme. Keine Interpretation. Keine Wahrscheinlichkeiten.

            2. Externe Quellen (immer "external"):
            Folgende Fälle sind ebenfalls "external", selbst wenn das Thema statisch wäre:
            - Web-Recherche nötig
            - Dateien müssen gelesen werden
            - Bilder müssen analysiert werden
            - Code muss ausgeführt werden
            - Berechnungen nötig, die Fehlertoleranz < 1% erfordern (Finanzen, Kryptografie, Statistiken)
            - alles, was das LLM nicht selbst simulieren oder sicher vorhersehen kann

            3. Statisches Wissen (immer "internal"):
            Nur dann "internal", wenn:
            - es ein allgemeines, unveränderliches Faktum ist
            - ohne Zeitbezug
            - ohne Abhängigkeit von externen Daten
            - ohne Aktualitätsbezug
            - ohne Dateien, Webrecherche oder Codeausführung

            4. Mechanische Regeln zur Sicherheit:
            Wenn du dir nicht zu 100% sicher bist → "external".
            Wenn irgendein Wort *vage- zeitabhängiges Wissen andeutet → "external".

            ---

            OUTPUT-RULES:
            Gib ausschließlich folgendes JSON zurück:

            {
                "type": "internal" | "external",
                "reason": "kurze klare Begründung"
            }

            Keine Erklärungen. Nur dieses JSON. Keine weiteren Texte.
        `);
    }

    run(prompt)
    {
        return new Promise(async (resolve) => {

            const out = await super.run(prompt);

            try
            {
                const json = JSON.parse(out);

                if(json.type == 'internal')
                {
                    const a = new Agent('Interner', { temperature : 0.7, top_p : 0.8, top_k : 80 }, 'Antworte in Deutsch!');

                    a.run(prompt).then((res) => resolve(res));
                }
                else
                {
                    const a = new ErrorAgent();

                    a.run(`
                        Frage: ${prompt}
                        Antwort: Ich kann aktuell auf keine externen Daten zugreifen!
                    `).then((res) => resolve(res));
                }
            }
            catch(e)
            {
                const a = new ErrorAgent();

                a.run(`
                    Frage: ${prompt}
                    Antwort: Es gab einen Fehler bei deiner Anfrage!
                `).then((res) => resolve(res));
            }
        });
    }
}