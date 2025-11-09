# esp-32 monitor project

## About

**This sub-project is part of a larger full-stack environment monitoring project**

## Description

* Microcontroller (device/monitor) includes 3 temperature and humidity sensors.
* Monitor polls sensors, encodes data in JSON, and sends the request every 10ish seconds.
* Monitor sends data over 2.4GHz wifi

## Technical

* JSON API
* [WeActStudio ESP32-C3 Core Board](https://github.com/FatCatLikesBeer/WeActStudioMiniCoreBoardDocs)
* [DHT-22 Temperature \& Humidity Sensor](https://github.com/dvarrel/DHT22.git)
* [cJSON](https://github.com/DaveGamble/cJSON)

## TODOs

* ~~Create a script that generates a one time key if none exists~~
* ~~Create and use a credentials.h file~~
