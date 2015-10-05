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

#ifndef COLOR_H
#define COLOR_H

#include<application.h>

//
// Type for storing RGB Color information.
//
// Helper methods are present for manipulating colors, and for randomly generating
// colors.
//

// Define "Color"
#pragma pack(push, 1)
typedef struct {
  uint8_t special;   // Normally zero, used by expandSpecial.
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} Color;
#pragma pack(pop)

// The colors go from 0x00 to 0x7F in brightness (the high bit is ignored).
#define BLACK           (Color{0x00, 0x00, 0x00, 0x00})
#define WHITE           (Color{0x00, 0xFF, 0xFF, 0xFF})

#define RED             (Color{0x00, 0xFF, 0x00, 0x00})
#define GREEN           (Color{0x00, 0x00, 0xFF, 0x00})
#define BLUE            (Color{0x00, 0x00, 0x00, 0xFF})

#define YELLOW          (Color{0x00, 0xFF, 0xFF, 0x00})
#define LIGHT_BLUE      (Color{0x00, 0x00, 0xFF, 0xFF})
#define PURPLE          (Color{0x00, 0xFF, 0x00, 0xFF})

// Special colors that are used with "expandSpecial"
#define RANDOM          (Color{0x01, 0x00, 0x00, 0x00})
#define RANDOM_PRIMARY  (Color{0x02, 0x00, 0x00, 0x00})

// Turn a special color (like RANDOMs), and process into a concrete color.
// Normal colors are returned unmodified.
Color expandSpecial(Color color);

// This will pick truely random red/green/blue components.
// This will tend more towards dirty/dim white than you would expect.
Color randomColor();

// Each primary color is on or off. Usually looks better than pure random.
Color randomPrimaryColor();

// 0 left, 1.0 right. Rounding errors are possible.
Color mixColor(Color left, Color right, float ratio);
Color dimColor(Color color, float brightness);

// Different effect from mixing. New color is one step towards target.
// Will reach target in 255 steps or less.
Color morphColor(Color base, Color target);

// Returns string of form 0xFFFFFFFF
String colorToString(Color color);

// Accepts output of above. Returns BLACK on failure.
Color stringToColor(String color);

#endif
