# esp-32 monitor project

## About

**This sub-project is part of a larger full-stack environment monitoring project**

## Description

* Microcontroller (monitor) is includes 3 temperature and humidity sensors.
* Monitor polls sensors, encodes data in JSON, and sends the request every 10 seconds.

## Technical

* JSON API
* WeActStudio ESP32-C3 Core Board
* (DHT-22)\[https://github.com/dvarrel/DHT22.git] Temperature \& Humidity Sensor
* (cJSON)\[https://github.com/DaveGamble/cJSON]

## TODOs

* Create a script that generates a one time key if none exists
* Create and use a .env file
