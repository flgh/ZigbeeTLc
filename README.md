# Custom firmware for Zigbee 3.0 IoT devices on the TLSR825x chip

Based on an example from  [Telink's Zigbee SDK](http://wiki.telink-semi.cn/wiki/chip-series/TLSR825x-Series/).

Currently supported devices: [LYWSD03MMC](https://pvvx.github.io/ATC_MiThermometer/), [CGDK2](https://pvvx.github.io/CGDK2/), [MHO-C122](https://pvvx.github.io/MHO_C122), [MHO-C401N](https://pvvx.github.io/MHO_C401N), [TS0201_TZ3000](https://pvvx.github.io/TS0201_TZ3000/)

Please direct questions regarding use and configuration in [ZHA](https://www.home-assistant.io/integrations/zha/), [Zigbee2MQTT](https://www.zigbee2mqtt.io/), [Home Assistant](https://www.home-assistant.io/) to the appropriate resources.

In developing: [MHO-C401(old)](https://pvvx.github.io/MHO_C401), [MJWSD05MMC](https://pvvx.github.io/MJWSD05MMC), [CGG1-M](https://pvvx.github.io/CGG1), [TS0202_TZ3000](https://pvvx.github.io/TS0202_TZ3000)

Firmware readiness level - Beta versions 0.1.x.x.
Stable from version 0.1.1.0, but additional tests are required

* Due to high consumption, limited transmit power of +2 dBm is used. A moderately discharged CR2032 battery cannot withstand higher power.
* [Average consumption](https://github.com/pvvx/ZigbeeTLc/issues/37#issuecomment-1937844181) for Xiaomi LYWSD03MMC HW: B1.4 - ~15 uA.

## To flash the Zigbee firmware, use a Google Chrome, Microsoft Edge or Opera Browser.*

1. Go to the [Over-the-air Webupdater Page TelinkMiFlasher.html](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html)
2. If using Android, Windows, Linux: Ensure you enabled "experimental web platform features". Therefore copy the according link (i.e. chrome://flags/#enable-experimental-web-platform-features for Chrome), open a new browser tab, paste the copied URL. Now sten the _Experimental Web Platform features_ flag to _Enabled_. Then restart the browser.
3. In the Telink Flasher Page: Press `Connect:`. The browser should open a popup with visible Bluetooth devices. Choose the according target device (i.e. LYWSD03MMC) to pair.
4. After connection is established a _Do Acivation_ button appears. Press this button to start the decryption key process.
5. Now you can press the Zigbee Firmware button to directly flash the `Zigbee Firmware`:<br>Alternatively you can choose a specific firmware binary (i.e. the original firmware) via the file chooser. Example: in the file name field paste "https://github.com/devbis/z03mmc/releases/download/1.0.6/1141-0203-10063001-z03mmc.zigbee". If the firmware file is larger than 128 kilobytes, then you will have to download BLE version 4.6 or later. BLE versions below 4.6 do not support OTA downloads with fw sizes larger than 128 kilobytes. Xiaomi firmwares, excluding MJWSD05MMC, also do not support OTA downloads with fw sizes larger than 128 kilobytes.
6. Press Start Flashing. Wait for the firmware to finish.
7. The device should now show up in your Zigbee bridge (If joining is enabled, of course). If this does not happen, reinsert the battery and/or short-circuit the RESET and GND pins on the LYWSD03MMC board, and on sensors with a button, press the button and hold it for 7..8 seconds.

## Zigbee On/Off

A microswitch should be soldered between GND and Reset pads at the back of the LYWSD03MMC ![img Microswitch.jpg](https://github.com/flgh/ZigbeeTLc/blob/master/3dcase/photo_2024-01-12_13-51-32.jpg)
Other devices have not been tested.

Toggle command is sent to the coordinator if firmware is compiled without bind support.
Toggle command is sent to the bound device if [firmware](https://github.com/flgh/ZigbeeTLc/raw/master/bin/1141-020a-01203001-Z03MMC.zigbee) is compiled with binding and the on/off cluster is bound to another Zigbee device, it's also sent to the coordinator.

In ZHA, the LYWSD03MMC should be bound to the coordinator (it will report as opening by default) or another Zigbee device.

### 3d printed case

[LYWS_switch_module.stl](https://github.com/flgh/ZigbeeTLc/blob/master/3dcase/LYWS_switch_module.stl) and [LYWS_switch_mount.stl](https://github.com/flgh/ZigbeeTLc/blob/master/3dcase/LYWS_switch_mount.stl) case are suitable for an Omron B3F-6000 switch and wall mount.

### Zigbee2MQTT

An [external converter](https://github.com/flgh/ZigbeeTLc/blob/master/z2m/lywsd03mmc-z.js) should be used, the included custom device definition of Zigbee2MQTT >= 1.33.2 doesn't support OnOff toggleCommand functionnality.

## Action of the button

Action of the button (or contact "reset" to "gnd" for LYWSD03MMC):

* Short press - send Zigbee On/Off toggle command + temperature, humidity and battery data.
* Hold for 2 seconds - Switches temperature between Celsius and Fahrenheit. The change will immediately appear on the screen.
* Hold for 7 seconds - reset the binding and restart. The screen will briefly display “---” (for devices using a screen with fast refresh capabilities, not E-Ink). After restarting, the thermometer will wait forever for registration. Jerking the battery won't change anything.

* After flashing from BLE, the thermometer is already in standby mode for registration in the Zigbee network.

## Z2M

Use [External convertors](https://github.com/pvvx/ZigbeeTLc/tree/master/z2m)

## Zigbee OTA

[OTA update in ZHA](https://github.com/pvvx/ZigbeeTLc/issues/7)

Before flashing the firmware, check the compliance of the “Manufacturer Code” and “Image Type” between the OTA file and those issued by the Zigbee device.

Sample: "1141-020a-01233001-Z03MMC.zigbee" 

| Manufacturer Code | Image Type | File Version | Stack Version | Name | Ext OTA |
| -- | -- | -- | -- | -- | -- |
| 1141 | 020a | 0123 | 3001 | Z03MMC | zigbee |
| 0x1141 - Telink | 0x02 - TLSR825x, 0x0a - Xiaomi LYWSD03MMC | App release 0.1, App build 2.3 | Zigbee v3.0, Release 0.1 |  Z03MMC | OTA |

|Image Type| Device, note |
| -- | -- |
| 0x0201 |  MHO-C401 (old version) |
| 0x0202 |  CGG1 (old version) |
| 0x0203 |  LYWSD03MMC ver https://github.com/devbis/z03mmc |
| 0x0204 |  WATERMETER ver https://github.com/slacky1965/watermeter_zed |
| 0x0206 |  CGDK2 |
| 0x0207 |  CGG1 (new version) |
| 0x0208 |  MHO-C401 (new version) |
| 0x0209 |  MJWSD05MMC |
| 0x020A |  LYWSD03MMC ver https://github.com/pvvx/ZigbeeTLc |
| 0x020B |  MHO-C122 |
| 0x0211 |  TS0201-TZ3000 |

* To restore to BLE, use the Zigbee OTA file with the same number device from: https://github.com/pvvx/ATC_MiThermometer/tree/master/zigbee_ota

* Zigbee OTA for 128 KB firmware takes 677 seconds with an average consumption of about 1 mA.


## Display decimal places for temperature, humidity and battery charge in ZHA.

[ZHA patch for more precision display of data for all Zigbee devices.](https://github.com/pvvx/ZigbeeTLc/issues/6)

## Indication on LCD screen

A solid "BT" icon indicates a connection loss or the thermometer is not registered with the Zigbee network.

The flashing "BT" icon is called by the "identify" command.

[ZHA: Switch temperature between Celsius and Fahrenheit, Show smiley, Comfort parameters](https://github.com/pvvx/ZigbeeTLc/issues/28)


## Setting offsets for temperature and humidity.

[ZHA: Setting offsets for temperature and humidity](https://github.com/pvvx/ZigbeeTLc/issues/30)

## Current additions

1. Added project assembly using 'make' (Windows/linux) and the ability to import 'Existing Project' into "[Telink IoT Studio](http://wiki.telink-semi.cn/wiki/IDE-and-Tools/Telink_IoT_Studio/ )".

2. Default intervals for temperature and humidity reporting are set to 30-120 (min-max) seconds, reportableChanged to 0.1C and 0.5%.

3. Consumption has been optimized when the coordinator is disconnected or connection is lost. When performing _rejoin_, if the connection is broken, the thermometer consumes quite a lot. Monitor the work of your network coordinator - Zigbee does not like (almost cannot tolerate) coordinator outages.

4. Consumption has been optimized when polling the reset button or contact.

5. Additional adjustments to consumption consumption and correction of the report transmission functions algorithm.

6. Correspondence of transmitted and displayed measurements (correct rounding).

7. Reducing firmware size: removing unnecessary code from SDK, reorganization of battery voltage measurement (ADC), ...

8. Averaging battery level measurements (eliminates clutter on charts)

9. Switch temperature between Celsius and Fahrenheit

10. Adding button actions (ver 0.1.1.1)

11. Added Poll Control Cluster (ver 0.1.1.2)

12. Ver 0.1.1.4: According to the "ZigBee Cluster Library Specification" the "Thermostat Temperature Conversion" values have been corrected. Added On/Off smiley.  (ClusterID: 0x0204 (Thermostat User Interface Configuration), Attr: 0x0000 (TemperatureDisplayMode), ENUM8 = 0 in C, = 1 in F, Attr: 0x0002 (ScheduleProgrammingVisibility), ENUM8 = 0 show smiley On,  = 1 show smiley Off)

13. Ver 0.1.1.5: Description of the HVAC cluster ([for ZHA](https://github.com/pvvx/ZigbeeTLc/issues/28))

13. Ver 0.1.1.6: Setting offsets for temperature and humidity.

14. Ver 0.1.1.7: Add Comfort parameters

15. Ver 0.1.1.8: Easy version. Options disabled: "Groups", "TouchLink", "Find and Bind".

16. Ver 0.1.1.9: Fix configuration saving error

17. Ver 0.1.2.0: At the [request](https://github.com/pvvx/ZigbeeTLc/pull/50#issuecomment-1925906616) of @devbis, for differences in z2m, the device names have been renamed. "-z" will be added to the name Zigbee devices, "-bz" will be added to the [BLE and Zigbee](https://github.com/pvvx/BZdevice) devices, "-zb" to the [Zigbee2BLE](https://github.com/pvvx/Zigbee2BLE) devices.

18. Reenable "Find and Bind" to support On/Off cluster binding.

## Make

To build under Linux, run:

```
git clone https://github.com/pvvx/ZigbeeTLc
make install
make
```

Out log:
```
===================================================================
 Section|          Description| Start (hex)|   End (hex)|Used space
-------------------------------------------------------------------
 ramcode|   Resident Code SRAM|           0|        1360|    4960
    text|           Code Flash|        1360|       1E028|  117960
  rodata| Read Only Data Flash|           0|           0|       0
 cusdata|          Custom SRAM|      844FD8|      844FD8|       0
      nc|   Wasteful Area SRAM|      841360|      841400|     160
   ictag|     Cache Table SRAM|      841400|      841500|     256
  icdata|      Cache Data SRAM|      841500|      841D00|    2048
    data|       Init Data SRAM|      841D00|      841E94|     404
     bss|        BSS Data SRAM|      841EA0|      844FD8|   12600
 irq_stk|        BSS Data SRAM|      841EA0|      8420A0|     512
    cbss| Custom BSS Data SRAM|      844FD8|      844FD8|       0
   stack|       CPU Stack SRAM|      844FD8|      850000|   45096
   flash|       Bin Size Flash|           0|       1E1BC|  123324
-------------------------------------------------------------------
Start Load SRAM : 0 (ICtag: 0x0)
Total Used SRAM : 20440 from 65536
Total Free SRAM : 160 + stack[45096] = 45256
```

Under Window, after downloading the project, run “make install”:
```
git clone https://github.com/pvvx/ZigbeeTLc
make install
```
and import the project into 'Telink IoT Studio' as described in 'Telink_IoT_Studio_User_Guide.pdf', chapter "6 Import and build projects".

On Windows, it is possible to enable multi-threaded assembly to build the entire project from scratch in 2..3 seconds (make -j -> 3 seconds on 12 cores/24 threads).

---


The start of the project with Zigbee on TLSR825x was inspired by the following original works:

* https://github.com/slacky1965/watermeter_zed

* https://github.com/devbis/z03mmc

---

* BLE firmware for [TS0201_TZ3000](https://github.com/pvvx/BLE_THSensor)

---
