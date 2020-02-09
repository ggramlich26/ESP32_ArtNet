This project is made to run on an ESP32 board. It can be compiled using the
Sloeber IDE.

It is possible to controll WS2812b or WS2811 LED strips via ArtNet or in
stand alone mode. In the latter case dip switches or their simulation as a
variable can be used to set the effect running. It is also possible to run the
ESP as an access point so that other devices running the same software can be
synchronized to it.

It is also possible to change the WIFI credentials by connecting to the
webserver running on the ESP on port 80 using any browser.

Please configure config.h to your needs. It lets you specify the number of
LEDs on your strip, the strip length (this is necessary to have devices with
different LED densities running the effects with the same speed), the default
wifi SSID and password.

In order to reset the wifi credentials to their default values (as set in
config.h), set all DIP switches to 0 and restart the device.
