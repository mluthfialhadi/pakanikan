## PAKANIKAN

Pakanikan is fish feeder with scheduling according to weight of the feed and monitored using Blynk App.
You can set the schedule and the weight each feeding time on the Blynk App. This project was made by me and my junior "Gyan Noval Havillah".
Pakanikan frame was made using aluminium square hollow 1 inch. <br/>

<br/>
<img src="Photos/pakanikan_in_action.png" alt="PAKANIKAN in Action" height="400">Pakanikan in action</img>

<br/>
<br/>
<img src="3D%20Design/Auger_Detailed.png" alt="Auger Mechanism" width="400">
Image of auger mechanism for exact feeding based on weight.
<br/>
<br/>
Because this is a project for undergraduate, so i didn't make the schematic. So you may look for pins used at the arduino file programs (.ino files).
You can see the diagram below for raw connection :
<br/>
```
                        BLYNK
                          |
                          |
DC Motor 775---Relay----ESP32----Loadcell
                          |
                          |
Stepper 28BYJ48-----Arduino Nano
```

### Where the motor stands
<br/>
<img src="Photos/how%20to%20mount%20motor.png" alt="How to Mount DC Motor" height="400">
The motor stands on Loadcell, so the motor will rotate only if the weight of the feed had the same value as set on the blynk app.

<br/>
<br/>
<br/>
<br/>
ps: Sorry for my bad English. English is my third language.
