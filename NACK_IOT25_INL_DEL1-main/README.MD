# Arduino Uno Bas-skelett för inlämningsuppgiften delmoment 1


I denna uppgift ska ni programmera ett enkelt passagesystem med pinkod. När programmet startar ska röd lampa lysa och grön lampa vara avstängd. När den grön knapp trycks ner ska den röda lampan börja blinka för att signalera att systemet är redo att ta emot en fyrsiffrig pinkod (1 - 7 - 7 - 2). Man har 5 sekunder på sig att slå in pinkoden. Varje gång en knapp på knappmatrisken trycks ner ska den gröna knappen blinka till snabbt för att visa användaren att knapptrycket registrerades. Slås rätt pinkod in så ska röd lampa slockna, grön lampa börja lysa konstant i 3 sekunder (vi simulera en solenoid som aktiveras så dörren går att öppna). Efter 3 sekunder ska systemet gå tillbaka till startläga, dvs röd lampa lyser konstant och gröna är släckt. Vid fel pinkod går vi tillbaka till startläge.

### Krav för G
Systemet fungerar som beskrivet ovan! Utöver det så:
1. Det ska finnas en implementation för knappmatrisen i drivers
   1. En init-funktion
   2. En screen-funktion
2. Systemet ska använda en State-machine
   1. IDLE state: Röd lampan lyser och vi väntar på att gröna knappen ska tryckas ner
   2. INPUT_AWAIT state: Röda lampan blinkar och knappmatrisen aktiveras och vi kan ta emot en pinkod. Den gröna knappen ska inte kunna tryckas ner i detta läge!
   3. ACCESS_GRANTED state: Röd lampa är släckt, grön lampa lyser och knappen och knappmatrisen är inaktiverade. 
   4. Vi har följande tillståndsförflyttningar
    ```mermaid
    stateDiagram-v2
        [*] --> IDLE

        IDLE --> INPUT_AWAIT : Gröna knappen trycks ner
        INPUT_AWAIT --> ACCESS_GRANTED : korrekt pinkod
        INPUT_AWAIT --> IDLE : timeout / felaktid pinkod

        ACCESS_GRANTED --> IDLE : session avslutas
    
### Förklaring
- `[*]` = startläge  
- `IDLE` = vänteläge  
- `INPUT_AWAIT` = väntar på input  
- `ACCESS_GRANTED` = åtkomst beviljad  
- Pilarna visar övergångar + vad som triggar dem  

### Krav för VG
Möjligheten att uppdatera pinkoden via UART-kommando givet att vi är i IDLE state! 
- Kommando:NEW PIN;\<old pin>;\<new pin>
- Kommandot ska vara spart i flash-minnet