# particle-strip

Library to drawing animations to various types of LED strips from a
[Particle Core](https://store.particle.io/?product=spark-core)

SupportedHardware:
 * LPD8806 RGB LEDs [Example](http://www.adafru.it/306)
 * NeoPixel [Example](http://www.adafru.it/1463)
   * NeoPixel Strip - (WS2811/WS2812/WS2812B based)
   * May also support (not tested on hardware)
     * Radio Shack Tri-Color Strip with TM1803 controller 400kHz bitstream.
     * TM1829 pixels
 * DotStar [Example](http://www.adafru.it/2238)

Gives a hardware independent interface for each type of strip. Can
support multiple strips in parallel.

Contains a Pattern helper that can help with pattern animation for a
number of standardized patterns.
