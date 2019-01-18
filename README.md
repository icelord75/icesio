# iceSIO
iceSIO replacement board for CASIO 3208 modules (CA-53, CA-506 watches)

## Hardware
* MCU: Microchip **PIC16F19197** - 8bit MCU, 56kB flash, 4K RAM, 256b EEPROM
* Accelerometer: NXP **MMA8451QT** - 3-axis, 14-bit/8-bit digital accelerometer
* Compass: Honeywell **HMC5883L** or Isentek **IST8310** - 3-Axis Digital Compass
* Barosensor/Humidity/Temp: BOSCH **BME280** - Digital humidity, pressure and temperature sensor
* LED:
	*	ON Semiconductor **NCP5623CMUTBG** - Triple Output I2C Controlled RGB LED Driver
	*	Songxin **SX-0402-RGB** - SMD fullcolor RGB SMD0404 LED
* Battery: **CR2016** -> CR2020 / CR2025 / CR2320 upgradable?

## Features

## TODO Features
*	Timekeeping: Home and World time with automatic DST and assisted calibration
*	Perpetual calendar: up to year 2199
*	Calculator: Infix, Prefix or Reverse Polish notation (configurable), -9.999999*1099 — +9.9999999*1099 (visual 8 digits mantissa)
*	Alarm: 5 daily alarms + 5 weekly alarms + hour signal
*	Start/Stop watch: 5 stopwatches up to 23:59:59.59 with 0.01 second resolution
*	Timer: 5 countdown timers up to 99:59:59 with 1 second resolution
*	Signal: Ring Tone Text Transfer Language (RTTTL) alarm and timer signals
*	Minute repeater: audibly chimes the hours and minutes.
*	BackLight: full RGB backlight, alarm and timers (configured separately) lights
*	Sun: Sunrise and Sunset (Latitude presets + timezone)
*	Moon: Precise Moon phase (icon and date)
*	Equation of time
*	Accelerometer:
	*	tap-sensing control
	*	tilt sensor
	*	speedometer
	*	continuous accelerometer
	*	0-100kmh acceleration/deceleration
*	Compass:
	*	Regular compass
	*	Angle finder
*	Enviromental sensor:
	*	Current temperature (C°/F°)
	*	Tranding temperature - hour/12h/24h
	*	Current Humidity (%)
	*	Tranding humidity - hour/12h/24h
	*	Current Barometer (hPA, mBar, at, atm, Torr, mmHg, PSI, inHg)
	*	Tranding barometer - hour/12h/24h

## Related Projects

[goodwatch](https://github.com/travisgoodspeed/goodwatch) replacement circuit board for the Casio 3208 module, used in the Casio CA-53W and CA-506 calculator watches.

[Pluto](https://github.com/carrotIndustries/pluto) is a replacement PCB for the Casio F-91W that is also based on an MSP430.
