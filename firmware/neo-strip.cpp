/*-------------------------------------------------------------------------
  ParticleStrip is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  ParticleStrip is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with ParticleStrip.  If not, see
  <http://www.gnu.org/licenses/>.

  The original version of ParticleStrip is available at:
      'https://github.com/DonGar/particle-strip
  -------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
  Spark Core library that implements the ColorStrip interface for AdaFruit
  NeoPixel.

  Able to control:
    NeoPixel Strip - (WS2811/WS2812 based)

      Currently handles 800 KHz and 400kHz bitstream on Spark Core,
      WS2812, WS2812B and WS2811.

  May also support (not tested on hardware):
  - Radio Shack Tri-Color Strip with TM1803 controller 400kHz bitstream.
  - TM1829 pixels

  This is a new library (bugs are mine), but hardware timing very heavily
  borrows from Adafruit libraries by written by Phil Burgess / Paint Your
  Dragon, adapted to the Spark Core by Technobly (contributions by PJRC and
  other members of the open source community).

  https://github.com/technobly/SparkCore-NeoPixel

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!
  --------------------------------------------------------------------*/

#include "neo-strip.h"

//
// NeoStrip
//
NeoStrip::NeoStrip(int pixelCount, int pin, NeoStripType neoType) :
    ColorStrip(pixelCount),
    pin(pin),
    neoType(neoType),
    latchStartTime(0) {
  drawSolid(BLACK);
}

void NeoStrip::finishDraw() {
  // If there is an unfinished latch from the previous draw, wait.
  this->finishLatch();

  volatile Color *pixel = this->pixelBuffer;
  volatile Color *pixelEnd = this->pixelBuffer + this->pixelCount;

  __disable_irq(); // Need 100% focus on instruction timing

  volatile uint32_t c; // 24-bit pixel color
  volatile uint32_t mask; // 8-bit mask

  // pixel points at pixel we are currently drawing.
  for (; pixel < pixelEnd; pixel++) {

      // Pack the colors for the current pixel in device specific order.
    switch (this->neoType) {
      case WS2812:
      case WS2812B:
        c = ((uint32_t)pixel->green << 16) | ((uint32_t)pixel->red <<  8) | pixel->blue;
        break;

      case WS2811:
        c = ((uint32_t)pixel->red << 16) | ((uint32_t)pixel->green <<  8) | pixel->blue;
        break;

      case TM1803:
        c = ((uint32_t)pixel->red << 16) | ((uint32_t)pixel->green <<  8) | pixel->blue;
        break;

      case TM1829:
        c = ((uint32_t)pixel->red << 16) | ((uint32_t)pixel->blue <<  8) | pixel->green;
        break;
    }

      // Write out all the bits in the packed pixel colors.
    for (mask = 0x800000; mask; mask >>= 1) {
      volatile bool pixelSubBit = c & mask;

      switch (this->neoType) {
        case WS2812:
        case WS2812B:
          PIN_MAP[pin].gpio_peripheral->BSRR = PIN_MAP[pin].gpio_pin; // HIGH
          if (pixelSubBit) {
            // if masked bit is high
            // WS2812 spec             700ns HIGH
            // Adafruit on Arduino    (meas. 812ns)
            // This lib on Spark Core (meas. 792ns)
            asm volatile(
              "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t"
              ::: "r0", "cc", "memory");
            // WS2812 spec             600ns LOW
            // Adafruit on Arduino    (meas. 436ns)
            // This lib on Spark Core (meas. 472ns)
            PIN_MAP[pin].gpio_peripheral->BRR = PIN_MAP[pin].gpio_pin; // LOW
            asm volatile(
              "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              ::: "r0", "cc", "memory");
          } else {
            // else masked bit is low
            // WS2812 spec             350ns HIGH
            // Adafruit on Arduino    (meas. 312ns)
            // This lib on Spark Core (meas. 306ns)
            asm volatile(
              "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              ::: "r0", "cc", "memory");
            // WS2812 spec             800ns LOW
            // Adafruit on Arduino    (meas. 938ns)
            // This lib on Spark Core (meas. 932ns)
            PIN_MAP[pin].gpio_peripheral->BRR = PIN_MAP[pin].gpio_pin; // LOW
            asm volatile(
              "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              ::: "r0", "cc", "memory");
          }
          break;

        case WS2811:
          PIN_MAP[pin].gpio_peripheral->BSRR = PIN_MAP[pin].gpio_pin; // HIGH
          if (pixelSubBit) {
            // if masked bit is high
            // WS2811 spec             1.20us HIGH
            // Adafruit on Arduino    (meas. 1.25us)
            // This lib on Spark Core (meas. 1.25us)
            asm volatile(
              "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              ::: "r0", "cc", "memory");
            // WS2811 spec             1.30us LOW
            // Adafruit on Arduino    (meas. 1.25us)
            // This lib on Spark Core (meas. 1.25us)
            PIN_MAP[pin].gpio_peripheral->BRR = PIN_MAP[pin].gpio_pin; // LOW
            asm volatile(
              "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              ::: "r0", "cc", "memory");
          } else {
            // else masked bit is low
            // WS2811 spec             500ns HIGH
            // Adafruit on Arduino    (meas. 500ns)
            // This lib on Spark Core (meas. 500ns)
            asm volatile(
              "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              ::: "r0", "cc", "memory");
            // WS2811 spec             2.000us LOW
            // Adafruit on Arduino    (meas. 2.000us)
            // This lib on Spark Core (meas. 2.000us)
            PIN_MAP[pin].gpio_peripheral->BRR = PIN_MAP[pin].gpio_pin; // LOW
            asm volatile(
              "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              ::: "r0", "cc", "memory");
          }
          break;

        case TM1803:
          PIN_MAP[pin].gpio_peripheral->BSRR = PIN_MAP[pin].gpio_pin; // HIGH
          if (pixelSubBit) {
            // if masked bit is high
            // TM1803 spec             1.36us HIGH
            // Pololu on Arduino      (meas. 1.31us)
            // This lib on Spark Core (meas. 1.36us)
            asm volatile(
              "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              ::: "r0", "cc", "memory");
            // TM1803 spec             680ns LOW
            // Pololu on Arduino      (meas. 1.024us)
            // This lib on Spark Core (meas. 670ns)
            PIN_MAP[pin].gpio_peripheral->BRR = PIN_MAP[pin].gpio_pin; // LOW
            asm volatile(
              "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
              ::: "r0", "cc", "memory");
            } else {
              // else masked bit is low
              // TM1803 spec             680ns HIGH
              // Pololu on Arduino      (meas. 374ns)
              // This lib on Spark Core (meas. 680ns)
              asm volatile(
                "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                ::: "r0", "cc", "memory");
              // TM1803 spec             1.36us LOW
              // Pololu on Arduino      (meas. 2.00us)
              // This lib on Spark Core (meas. 1.36us)
              PIN_MAP[pin].gpio_peripheral->BRR = PIN_MAP[pin].gpio_pin; // LOW
              asm volatile(
                "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                ::: "r0", "cc", "memory");
            }
            break;

          case TM1829:
            PIN_MAP[pin].gpio_peripheral->BRR = PIN_MAP[pin].gpio_pin; // LOW
            if (pixelSubBit) {
              // if masked bit is high
              // TM1829 spec             800ns LOW
              // This lib on Spark Core (meas. 792ns)
              asm volatile(
                "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                ::: "r0", "cc", "memory");
              // TM1829 spec             300ns HIGH
              // This lib on Spark Core (meas. 319ns)
              PIN_MAP[pin].gpio_peripheral->BSRR = PIN_MAP[pin].gpio_pin; // HIGH
              asm volatile(
                "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                ::: "r0", "cc", "memory");
            } else {
              // else masked bit is low
              // TM1829 spec             300ns LOW
              // This lib on Spark Core (meas. 306ns)
              asm volatile(
                "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                ::: "r0", "cc", "memory");
              // TM1829 spec             800ns HIGH
              // This lib on Spark Core (meas. 805ns)
              PIN_MAP[pin].gpio_peripheral->BSRR = PIN_MAP[pin].gpio_pin; // HIGH
              asm volatile(
                "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                ::: "r0", "cc", "memory");
            }
            break;
          } // Done writing one bit.
      } // Done drawing all bits in one pixel.
  } // Done drawing all pixels.

  __enable_irq();
  this->startLatch();

  ColorStrip::finishDraw();
}

void NeoStrip::startLatch() {
  // Remember when we started latching.
  this->latchStartTime = micros();
}

void NeoStrip::finishLatch() {
  int waitTime = 0; // wait time in microseconds.
  switch(neoType) {
    case WS2811:
    case WS2812:
    case WS2812B:
      waitTime = 50;
      break;
    case TM1803:
      waitTime = 24;
      break;
    case TM1829:
      waitTime = 500;
      break;
  }

  unsigned long timePassed = micros() - this->latchStartTime;

  if (timePassed < waitTime) {
    delayMicroseconds(waitTime - timePassed);
  }
}
