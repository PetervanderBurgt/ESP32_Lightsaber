# Lightsaber based on ESP32

This project is a project I am working on for a long time and currently putting more effort in the code base.
I am happily inspired by the great work done by everyone at [FX SaberOS](https://github.com/Protonerd/FX-SaberOS/tree/master), but would like to create my own ESP32 based version.
As you will notice, the sounds are also from there, big thanks to them.
This was mainly due to the fact I wanted some more coding under my belt, and would like to know how tasks can be used.
Furthermore, I would like to see if I can incorporate some kind of nice BT Config menu or a webpage.

**Please do note that this is an ongoing project and not finished.**



## Requirements

Before all this happened I set out a couple of things that I would like to attain:



**Bug fixes:**

* [x] Look at instable connection with MPU6050

  * [x]  Check HW faults
  * [x]  Check SW faults
  * [x]  Check Weird startup bug

* [x] Have a look at task priorities



**Must haves:**

* [x] Light up all 100 LEDS
* [x] Two buttons to use
* [x]  Sound to play:

  * [x]  Boot sound
  * [x]  Saber on
  * [x]  Saber off
  * [x]  Hum sounds
  * [x]  Swing sounds
  * [x]  Clash sounds

* [x]  Config should be possible
* [x]  Keep settings on power on off
* [x]  Switch color / rainbow

**Could haves:**

* [x]  Flicker in leds
* [x]  Webpage for settings
* [x]  LED Functionality

  * [x]  Random buzzing for startup and power down as well, aside from hum
  * [x]  Clash color on clash sound
  * [x]  Tip melt
  * [x]  Blaster

    * [x]  Random places for blaster

  * [x]  Lockup

* [x]  Sounds

  * [x]  Config sounds
  * [x]  Blaster
  * [x]  Lockup

## Hardware gathering

All links given are based on availability in the Netherlands

|**Lightsaber Hilt**|**Amount**|**Minimum order**|**Price Per Piece**|
|-|-|-|-|
|[Wemos D1 Mini ESP32 (Dual Core)](https://nl.aliexpress.com/item/1005006026692511.html) (Tested with ESP32-WROOM-32)|1|€4,40|€4,40|
|[Buttons (19mm)](https://nl.aliexpress.com/item/1005004920346156.html), nice ones|2|€3,99|€1,99|
|[DFPlayer Mini](https://www.tinytronics.nl/nl/audio/audio-bronnen/dfrobot-dfplayer-mini-mp3-module) (An Original one, clones do not work!)|1|€8,00|€8,00|
|[Micro SD Card](https://nl.aliexpress.com/item/1005005857273248.html)|1|€4,79|€4,79|
|[MPU6050](https://nl.aliexpress.com/item/1005006579363624.html?spm=a2g0o.order_list.order_list_main.161.306179d2NNYWTh&gatewayAdapt=glo2nld)|1|€2,53|€2,53|
|[2W Speaker 28mm](https://www.aliexpress.com/p/order/index.html?spm=a2g0o.home.headerAcount.2.682e44f5dH63Bg)|1|€1,89|€0,95|
|[GX12 P3 Connector Male](https://nl.aliexpress.com/item/1005004207045147.html)|1|€6,59|€1,32|
|[5V Boost Converter 3A](https://nl.aliexpress.com/item/1005008051438437.html)|1|€1,61|€1,61|
|[TS-13 Slide Switch](https://nl.aliexpress.com/item/1005006060448283.html?)|1|€1,86|€0,93|
|[TP4056 Battery Charger](https://nl.aliexpress.com/item/1005004427739715.html?)|1|€2,41|€0,48|
|Random wires for Speaker|1|€0,00|€0,00|
|[18650 Battery](https://nl.aliexpress.com/item/1005008243319532.html) (This one is a bit overkill)|1|€10,79|€5,40|
|[Custom PCB](https://jlcpcb.com)|1|€10,01|€2,00|
|**Total**|-|€58,87|€34,40|


|**Lightsaber Blade**|**Amount**|**Minimum order**|**Price Per Piece**|
|-|-|-|-|
|[Acrylate Transparent tube 1000x25x2](https://kunststofshop.nl/acrylaat-plexiglas/acrylaat-buizen/transparant/acrylaat-buis-transparant-1000x25x2mm-1000x25x2mm/a-3493-20000034)|1|€7,50|€7,50|
|[COB Led Strips 100 LEDs/m 5mm 1m wide](https://nl.aliexpress.com/item/1005005922172231.html?spm=a2g0o.order_list.order_list_main.136.306179d2NNYWTh&gatewayAdapt=glo2nld) (tried 160 but too much power draw)|2|€13.18|€6,59|
|[GX12 P3 Connector Female](https://nl.aliexpress.com/item/1005004207045147.html) (See above)|0|€0,00|€0,00|
|Random wires for LED connections|3|€0,00|€0,00|
|[Clear Present Foil](https://www.jumbo.com/producten/hema-cadeaupapier-folie-70-x-500-cm-667439STK) (1m)|31|€0,99|€0,99|
|[Soft diffusion tube (8 x 18 mm)](https://nl.aliexpress.com/item/1005005913288707.html?spm=a2g0o.order_list.order_list_main.166.306179d2NNYWTh&gatewayAdapt=glo2nld)|1|€3,75|€3,75|
|**Total**|-|€24,43|€24,43|

|**Lightsaber 3D Prints**|**Amount**|**Price Per Piece**|
|-|-|-|
|Lightsaber Carriage Top|1|€1,99|
|Lightsaber Carriage Bottom|1|€1,95|
|Bottom Sleeve|1|€1,37|
|Top Sleeve Bottom|1|€0,44|
|Top Sleeve Top|1|€0,66|
|Blade Stopper Top|1|€0,99|
|Blade Stopper Bottom|1|€0,03|
|**Total**|-|€7,43|

|**All**|**Price**|
|-|-|
|Lightsaber Hilt|€34,40|
|Lightsaber Blade|€24,43|
|Lightsaber 3D Print|€7,43|
|**Total**|€66,26|

## Software BOM

All can be found within Arduino IDE 2.3.6

|**Software / Library**|**Version**|
|-|-|
|Arduino IDE|2.3.6|
|esp32 board library (Espressif Systems)|3.2.0|
|MPU6050 (Electronic Cats)|1.4.3|
|OneButton (Matthias Hertel)|2.6.1|
|FastLED (Daniel Garcia)|3.9.16|
|DFPlayer Mini Mp3 (Makuna)|1.2.3|
|ESP Async TCP (ESP32Async)|2.0.0|
|ESP Async WebServer (ESP32Async)|3.7.6|
|Async TCP (ESP32Async)|3.3.8|
|||



