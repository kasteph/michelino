michelino
=========

An Arduino-powered car. It does basic collision detection and can be controlled via [Zulip](https://github.com/surenm/remote-controller)
and the [web](https://github.com/surenm/remote-controller).

Hardware
--------

Jesse and I did a quick Google search to see if anyone had written a blog post on making an Arduino car and found that
Miguel Grinberg had [written a series](http://blog.miguelgrinberg.com/category/Arduino). I read
through his tutorial and found that while the rest of it was a bit too much for what we were trying to accomplish, the
[parts and components post](blog.miguelgrinberg.com/post/building-an-arduino-robot-part-i-hardware-components) were extremely invaluable.

We bought all of the parts recommended by Miguel except for the Bluetooth Slave. We decided early on that we wanted the car to
be controlled via Zulip and possibly a web interface so we bought the [Adafruit CC3000 WiFi shield](http://www.adafruit.com/product/1491) instead.

The Sketch
----------

To make the motors run, we followed [this tutorial](http://www.instructables.com/id/Arduino-Motor-Shield-Tutorial/) from Instructables. It sets up two channels for each motor and brake--we decided not to implement brake like the author did in this tutorial since we were using the Adafruit CC3000 WiFi shield and it had overlapping pins with the Arduino Motor Shield. Instead, our brake works by setting speed to 0.

To connect to the Internet, we used the [Adafruit_CC3000 library](https://github.com/adafruit/Adafruit_CC3000_Library). We set up the HTTP Server using the [HTTPServer.ino example](https://github.com/adafruit/Adafruit_CC3000_Library/blob/master/examples/HTTPServer/HTTPServer.ino) as a foundation and integrated it into our car code. With the server we have the following endpoints:

* `/start`
* `/stop`
* `/forward`
* `/backward`
* `/left`
* `/right`

Using the above endpoints, Suren and I were able to make wrappers to control the car via the Internet.
