# OpenRGB Hue Entertainment Plugin: Status & Roadmap

Dieses Dokument beschreibt den aktuellen Entwicklungsstand, die bereits implementierten Funktionen und die nächsten Schritte, um das Plugin für den produktiven Einsatz fertigzustellen.

## 1. Aktuell implementierte Funktionen

Das Plugin befindet sich in einem fortgeschrittenen Entwicklungsstadium und die Kernfunktionalität ist implementiert. Es ist stabil, performant und sicher.

- **Stabile Entertainment-Synchronisation:** Synchronisiert OpenRGB-Geräte in Echtzeit mit einer Philips Hue Entertainment Area.
- **Performante Datenübertragung:** `SetLEDs` ist vollständig nicht-blockierend implementiert, um Hänger in der OpenRGB-Oberfläche zu verhindern. Ein Stresstest validiert dieses Verhalten.
- **Automatische Bridge-Erkennung:** Findet die Hue Bridge im lokalen Netzwerk automatisch über den `discovery.meethue.com`-Service.
- **Sichere Authentifizierung:** Führt den Push-Link-Authentifizierungsprozess sicher durch und speichert die erhaltenen Zugangsdaten in einer Konfigurationsdatei mit eingeschränkten Zugriffsrechten (`0600`).
- **Robuste DTLS-Verbindung:** Baut eine sichere DTLS-Verbindung zur Hue Bridge auf und stellt sie bei Unterbrechungen mit einem exponentiellen Backoff-Mechanismus automatisch wieder her.
- **Korrekte Farb-Frames:** Sendet korrekt formatierte JSON-Payloads für die Farbsteuerung, was zu einer fehlerfreien Darstellung führt.
- **Präzises Farb-Mapping:** Die `MappingEngine` verwendet Fließkomma-Arithmetik für eine genaue Farbdurchschnittsbildung und ist gegen Fehler (z. B. Division durch Null) abgesichert.
- **Automatisierte Builds:** Eine CI/CD-Pipeline via GitHub Actions baut das Plugin für Windows automatisch, was konsistente und zugängliche Releases sicherstellt.
- **Umfassende Dokumentation:** Die `README.md` erklärt die Installation, Konfiguration und den Build-Prozess detailliert.

## 2. Status der Benutzeroberfläche (UI)

**Aktuell gibt es noch keine sichtbare Benutzeroberfläche.**

Die technischen Grundlagen für eine GUI wurden zwar geschaffen (die Plugin-Struktur wurde erweitert, um ein Qt-Widget zu laden), die eigentliche Implementierung ist jedoch der nächste große, offene Punkt.

**Geplantes Design und Layout:**

Die UI wird sich nahtlos in OpenRGB integrieren und über einen Tab im Plugin-Bereich erreichbar sein. Das geplante Layout ist modular und benutzerfreundlich:

-   **Tab "Bridge":**
    -   Anzeige der aktuellen Bridge-IP (automatisch gefunden oder manuell eingetragen).
    -   Ein Button "Authentifizieren", der den Push-Link-Prozess startet und den Benutzer anleitet, den Knopf an der Bridge zu drücken.
    -   Eine Statusanzeige ("Verbunden", "Nicht verbunden", "Authentifizierung erforderlich").
-   **Tab "Entertainment Areas":**
    -   Eine Liste der auf der Bridge verfügbaren Entertainment-Bereiche.
    -   Die Möglichkeit, den gewünschten Bereich für die Synchronisation auszuwählen.
-   **Tab "LED-Mapping" (Fortgeschritten):**
    -   Eine visuelle Oberfläche, um OpenRGB-LEDs oder -Zonen den Lampen der Hue Entertainment Area zuzuordnen (z. B. per Drag & Drop).
-   **Tab "Einstellungen":**
    -   Optionen zur Konfiguration der Ziel-FPS für das Streaming.
    -   Logging-Einstellungen.

## 3. Offene Punkte für den produktiven Einsatz (Roadmap)

Der einzige verbleibende Punkt aus der ursprünglichen Analyse ist die UI. Für einen wirklich produktiven und benutzerfreundlichen Einsatz empfehle ich die folgende Roadmap:

1.  **GUI-Implementierung (P0):**
    -   **Aufgabe:** Implementierung des "Bridge"-Tabs, damit Benutzer den Verbindungsstatus sehen und die Authentifizierung einfach per Knopfdruck starten können. Dies ist der wichtigste Schritt, um die manuelle Konfiguration der JSON-Datei überflüssig zu machen.

2.  **Erweiterte GUI-Funktionen (P1):**
    -   **Aufgabe:** Implementierung des "Entertainment Areas"-Tabs, damit Benutzer bequem zwischen verschiedenen Setups (z. B. "Gaming-Ecke", "Heimkino") wechseln können, ohne die Konfigurationsdatei zu bearbeiten.

3.  **Verbesserung der Mapping-Logik (P2):**
    -   **Aufgabe:** Aktuell wird immer eine 1:1-, Durchschnitts- oder Interpolations-Zuordnung verwendet. Eine manuelle Konfiguration des Mappings (welche LED zu welcher Lampe) ist noch nicht möglich. Dies sollte über den "LED-Mapping"-Tab in der GUI konfigurierbar gemacht werden.

4.  **Beantwortung offener technischer Fragen:**
    -   **Aufgabe:** Die genauen Formate der Hue-API-Antworten und des DTLS-Streams sollten verifiziert werden, um die Implementierung noch robuster zu machen (siehe "Open Questions" im alten Dokument). Dies ist jedoch eher eine Detailverbesserung als eine blockierende Anforderung.
