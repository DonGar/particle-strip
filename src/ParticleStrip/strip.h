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

#ifndef STRIP_H
#define STRIP_H

#include "color.h"

//
// This class is an abstract interface for controlling a color strip. To use
// instantiate a hardware specific implementation, and call "drawPixel" once per
// pixel (extra calls are ignored). Call "finishDraw" to finalize an update to
// the strip.
//
// Actual LEDs may be updated during drawPixel, or during finishDraw, depending
// on hardware.
//

class ColorStrip {
  public:
    inline ColorStrip(int pixelCount, bool buffer=true) :
        pixelCount(pixelCount),
        drawOffset(0),
        pixelBuffer(NULL) {
      if (buffer) {
        this->pixelBuffer = (Color*)malloc(sizeof(Color) * pixelCount);
      }
    }

    virtual inline void drawPixel(Color color) {
      if (this->drawOffset >= this->pixelCount) {
        return;
      }

      this->pixelBuffer[this->drawOffset] = color;
      this->drawOffset++;
    }

    virtual inline void finishDraw() {
      this->drawOffset = 0;
    }

    virtual inline void drawSolid(Color color) {
      for (int i = 0; i < this->pixelCount; i++) {
        this->drawPixel(color);
      }

      this->finishDraw();
    }

    int getPixelCount() { return this->pixelCount; }
    Color* getPixelBuffer() { return this->pixelBuffer; }

  protected:
    int pixelCount;
    int drawOffset;
    Color* pixelBuffer;
};

#endif
