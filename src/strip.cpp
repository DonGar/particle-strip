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

#include "strip.h"

ColorStrip::ColorStrip(int pixelCount) :
    pixelCount(pixelCount),
    drawOffset(0) {
  this->pixelBuffer = (Color*)malloc(sizeof(Color) * pixelCount);
}

void ColorStrip::drawPixel(Color color) {
  if (this->drawOffset >= this->pixelCount) {
    return;
  }

  this->pixelBuffer[this->drawOffset] = color;
  this->drawOffset++;
}

void ColorStrip::finishDraw() {
  this->drawOffset = 0;
}

void ColorStrip::drawSolid(Color color) {
  for (int i = 0; i < this->pixelCount; i++) {
    this->drawPixel(color);
  }

  this->finishDraw();
}
