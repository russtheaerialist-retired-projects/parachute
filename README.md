parachute
=========

Parachute Deployment system using arduino + servo + accelerometer

Circuit
==============

1. Arduino Nano V3
2. Some SPST toggle switch
3. [MMA7361 breakout board](http://www.vetco.net/catalog/product_info.php?products_id=12790)
4. A hobby servo (micro servo is fine)
5. 5v power supply, I'm using a usb battery pack, and any place I need 5v, I'm pulling off the arduino

![Circuit Diagram](https://raw.githubusercontent.com/RussTheAerialist/parachute/master/docs/breadboard.png)

*Note: The above circuit (and code) is using the internal pull up resistor on the arm pin. This
means that the board will go into power down mode and not be able to be woken up except via
a reset. You have two choices, you can either wire up a reset button, or wire up a 20KΩ resisitor
as a pull up resistor to the external power supply (what I ended up doing)*
