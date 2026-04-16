# messy monsters
🧌 Messy Monsters 👹 is a [Comon Mini Living Lab](https://comon.gent/en/messy-monsters) concept that makes cleaning up in the house more fun for the whole family, focussed on involving children in this process. 

The idea starts from a nest of monsters somewhere in the house (a stuffed nest-shaped whole full of socks, toys, handkerchiefs, etc.) that houses several messy monsters (stuffed animals in different colors and shapes). When a chore has to be done, the monster 'escapes' (since it love mess in the house). This implies that a parent hides the monster somewhere in the room where the chore has to be done. When activated, the other monsters in the nest start whining. To allow the monster to come back, first the chore has to be done. This is supported by a tablet app that makes this fun and interactive. When the chore is done, the monsters can to back to the nest and the monsters cheer from the nest.

## Test set-up of the first prototype
The first prototype is built upon a Figma prototype for the tablet app and an improvised nest with stuffed animals, combined with an Arduino R4 that allows a bluetooth wizard to control a series of led ring animations and sounds.

* Figma prototype

**Components of the nest**

* Two 64 cm diameter circlular wooden boards with 3 x 44cm holes for the led rinds cut out in the middle (top board only)
* 3 x 44 cm smokey acryl for the diffusion of the led rings
* Spaceers to hold them aparts
* Arduino R4 WiFi
* 3 x 24 led rings
* DFPlayer mini audio player
* 3W speaker
* 5V power supply
* WAGO connectors
* Jumper wires
* breadboard
* Resistors: 220 ohm for the led rings,1k ohm for the DFPlayer mini
* Stuffed animals
* Nest material (e.g. rope, fabric, etc.)

**Wiring diagram**
* 5V power and ground to the breadboard
* 5V power and ground from the breadboard to Arduino Vin and GND
* 5V power and ground from the breadboard to the DFPlayer mini
* 5V power and ground from the breadboard to the first led ring, and then in series to the second and third led ring
* D6 from Arduino (over a 220 ohm resistor) to Din of the first led ring, and then, through Dout in series to the Din of the second and the same for the third led ring
* D2 from Arduino (over a 1k ohm resistor) to the RX pin of the DFPlayer mini, and D3 from Arduino to the TX pin of the DFPlayer mini 
* Speaker + and - from the DFPlayer mini

Main code for the prototype: [main.ino](src/main.ino)

**Instructions for the wizard**
* Connect with the R4 using Bluetooth Serial Terminal on Android, or Bluetooth Serial Controller on iOS
* It advertises as "R4_Zones"
* Connect to the services you can see 
* Then use test commands to control the prototype:
    * '0' → all off,  matrix blank, play /MP3/0002.mp3 (cheering sound )
    * '1' → ring 1 fire,  matrix shows "1", play /MP3/0001.mp3 (whining sound)
    * '2' → ring 2 fire,  matrix shows "2", play /MP3/0001.mp3 (whining sound)
    * '3' → ring 3 fire,  matrix shows "3", play /MP3/0001.mp3 (whining sound)


## hardware tests
LED strips come with a single Din line (3 wires), or with a clock and data line (SPI, 4 wires).
* 🔴 [Test led strip SPI all red](/tests/led_red.ino)
* 🌈 [Test led strip SPI rainbow](/tests/led_rainbow.ino)
* 🚨[Test led stick Din chase](/tests/led_stick.ino)

Servo can be used to open and close a lock
* 🔓 Servo lock test: [Servo lock test](/tests/servo_lock.ino)

Audio can be played using a DFPlayer mini, which takes an SD card with audio files on it. The player can be controlled using serial commands.
* 🎵 [Test DF mini audio player](/tests/dfplayer.ino)

A cool thing about the R4 is the integrated matrix display, which can be used to show text and images.
* 🖼️ [Test matrix display](/tests/r4_ledmatrix.ino)

For WoZ prototyping BLE is most convenient (the R4 has built in BLE).
You can control this using Serial Bluetooth Terminal on Android, or Bluetooth Serial Controller on iOS.
* 📱 [Test BLE](/tests/BLE_control.ino)