# D1_mini_thermometer
This is a simple IOT thermometer and humidity sensor. 
It is possible to run from 18650 3.7V Rechargeable Li-ion Battery or anything else over 3.5V for a decent long time (it depend how long the sleeping period).

# Parts
* Wemos D1 mini
* DHT-22
* Resistor 330, 10K
* Molex plug

The 330R between rst and D0 is not obligatory, any resistor between 330R-520R is fine - suggested by wemos forum members. It need to wake up reliable from deepsleep.

#Library
https://github.com/adafruit/DHT-sensor-library

![](https://github.com/bunnyhu/d1_mini_thermometer/blob/master/wifi_thermometer_schem.png)
![](https://github.com/bunnyhu/d1_mini_thermometer/blob/master/wifi_thermometer_bb.png)
![](https://github.com/bunnyhu/d1_mini_thermometer/blob/master/final_item_top.jpg)
