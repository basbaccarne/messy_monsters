# messy_monsters
test repo for the mini living lab

## tests
LED strips come with a single Din line (3 wires), or with a clock and data line (SPI, 4 wires).
* 🔴 [Test led strip SPI all red](/tests/led_red.ino)
* 🌈 [Test led strip SPI rainbow](/tests/led_rainbow.ino)
* 🚨[Test led stick Din chase](/tests/led_stick.ino)

Servo can be used to open and close a lock
* 🔓 Servo lock test: [Servo lock test](/tests/servo_lock.ino)

Audio can be played using a DFPlayer mini, which takes an SD card with audio files on it. The player can be controlled using serial commands.
* 🎵 [Test DF mini audio player](/tests/dfplayer.ino)

A cool thing about the R4 is the integrated display, which can be used to show text and images.
* 🖼️ [Test display](/tests/r4_ledmatrix.ino)

For WoZ prototyping BLE is most convenient (the R4 has built in BLE).
You can control this using Serial Bluetooth Terminal on Android, or Bluetooth Serial Controller on iOS.
* 📱 [Test BLE](/tests/BLE_control.ino)