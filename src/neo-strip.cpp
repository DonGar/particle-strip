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
NeoStrip::NeoStrip(int pixelCount, int pin, uint8_t neoType) :
    ColorStrip(pixelCount, false),
    neoLibrary(pixelCount, pin, neoType) {
  this->neoLibrary.begin();
  drawSolid(BLACK);
}

void NeoStrip::drawPixel(Color color) {
  if (this->drawOffset >= this->pixelCount) {
    return;
  }

  this->neoLibrary.setColor(this->drawOffset, color.red, color.green, color.blue);
  this->drawOffset++;
}

void NeoStrip::finishDraw() {
  ColorStrip::finishDraw();

  this->neoLibrary.show();
}
