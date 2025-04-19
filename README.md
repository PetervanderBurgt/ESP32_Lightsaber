# Lightsaber based on ESP32

This project is a project I am working on for a long time and currently putting more effort in the code base.
I am happily inspired by the great work done by everyone at [FX SaberOS](https://github.com/Protonerd/FX-SaberOS/tree/master), but would like to create my own ESP32 based version.
As you will notice, the sounds are also from there, big thanks to them.
This was mainly due to the fact I wanted some more coding under my belt, and would like to know how tasks can be used.
Furthermore, I would like to see if I can incorporate some kind of nice BT Config menu or a webpage.

**Please do note that this is an ongoing project and not finished.**


## Requirements
Before all this happened I set out a couple of things that I would like to attain:

**Must haves:**
- [x] Light up all 100 LEDS
- [x] Two buttons to use
- [x]  Sound to play:
    - [x]  Boot sound
    - [x]  Saber on
    - [x]  Saber off 
    - [x]  Hum sounds
    - [x]  Swing sounds
    - [x]  Clash sounds
- [x]  Config should be possible
- [x]  Keep settings on power on off
- [x]  Switch color / rainbow

**Could haves:**
- [x]  Flicker in leds
- [ ]  Webpage for settings
- [x]  LED Functionality
    - [x]  Tip melt
    - [x]  Blaster
        - [x]  Random places for blaster
    - [x]  Lockup
- [x]  Sounds
    - [x]  Config sounds
    - [x]  Blaster
    - [x]  Lockup

## Hardware gathering
All links given are based on availability in the Netherlands

| **Lightsaber Core** | **Amount** |
| ----------- | ----------- |
| Wemos D1 Mini ESP32 (Dual Core) (Tested with ESP32-WROOM-32) | 1 |
| [Buttons (19mm)](https://nl.aliexpress.com/item/1005004920346156.html?spm=a2g0o.order_list.order_list_main.126.306179d2NNYWTh&gatewayAdapt=glo2nld), nice ones | 2 |
| [DFPlayer Mini](https://www.tinytronics.nl/nl/audio/audio-bronnen/dfrobot-dfplayer-mini-mp3-module) (An Original one, clones do not work!) | 1 |
| [MPU6050](https://nl.aliexpress.com/item/1005006579363624.html?spm=a2g0o.order_list.order_list_main.161.306179d2NNYWTh&gatewayAdapt=glo2nld) | 1 |
| [2W Speaker 28mm](https://www.aliexpress.com/p/order/index.html?spm=a2g0o.home.headerAcount.2.682e44f5dH63Bg) | 1 |
|  |  |

| **Lightsaber Blade** | **Amount** |
| ----------- | ----------- |
| [Acrylate Transparent tube 1000x25x2](https://kunststofshop.nl/acrylaat-plexiglas/acrylaat-buizen/transparant/acrylaat-buis-transparant-1000x25x2mm-1000x25x2mm/a-3493-20000034) | 1 |
| [COB Led Strips 100 LEDs/m 5mm 1m wide](https://nl.aliexpress.com/item/1005005922172231.html?spm=a2g0o.order_list.order_list_main.136.306179d2NNYWTh&gatewayAdapt=glo2nld) (tried 160 but too much power draw) | 2 |
| [Soft diffusion tube](https://nl.aliexpress.com/item/1005005913288707.html?spm=a2g0o.order_list.order_list_main.166.306179d2NNYWTh&gatewayAdapt=glo2nld) | 1 |
|  |  |


