spark-core-pid-temperature
==========================

Spark Core **PID** **temperature** controlling with **DS18B20**.

    //TODO: set point temperature via spark cloud

What is this?
-------------
Controlling PWM output to reach the point temperature. 
Doing this with a PID controller.

![http://www.podmerchant.com/newsletter/68-temperature-overshoot.jpg][3]

Used libraries
-------------

**PID**
[https://github.com/br3ttb/Arduino-PID-Library][1]
*(with a small change to make it work on a Spark Core)*

**DS18B20**
[https://github.com/krvarma/Dallas_DS18B20_SparkCore][2]

  [1]: https://github.com/br3ttb/Arduino-PID-Library
  [2]: https://github.com/krvarma/Dallas_DS18B20_SparkCore
  [3]: http://www.podmerchant.com/newsletter/68-temperature-overshoot.jpg
  