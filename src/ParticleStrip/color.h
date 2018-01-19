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


// This will pick truely random red/green/blue components.
// This will tend more towards dirty/dim white than you would expect.
inline Color randomColor() {
  Color result;

  result.special = 0;
  result.red = random(0, 0x100);
  result.green = random(0, 0x100);
  result.blue = random(0, 0x100);

  return result;
}

// Each primary color is on or off. Usually looks better than pure random.
inline Color randomPrimaryColor() {
  Color result;

  result.special = 0;
  result.red = random(0, 2) ? 0xFF : 0;
  result.green = random(0, 2) ? 0xFF : 0;
  result.blue = random(0, 2) ? 0xFF : 0;

  return result;
}


// Turn a special color (like RANDOMs), and process into a concrete color.
// Normal colors are returned unmodified.
inline Color expandSpecial(Color color) {
  if (color.special == RANDOM.special)
    return randomColor();

  if (color.special == RANDOM_PRIMARY.special)
    return randomPrimaryColor();

  color.special = 0;
  return color;
}

// 0 left, 1.0 right. Rounding errors are possible.
inline Color mixColor(Color left, Color right, float ratio) {
  if (ratio < 0.0)
    ratio = 0.0;

  if (ratio > 1.0)
    ratio = 1.0;

  float l_ratio = 1.0 - ratio;
  float r_ratio = ratio;

  Color result;

  result.special = 0;
  result.red = (left.red * l_ratio) + (right.red * r_ratio);
  result.green = (left.green * l_ratio) + (right.green * r_ratio);
  result.blue = (left.blue * l_ratio) + (right.blue * r_ratio);

  return result;
}

inline Color dimColor(Color color, float brightness) {
  return mixColor(BLACK, color, brightness);
};

// Different effect from mixing. New color is one step towards target.
// Will reach target in 255 steps or less.
inline uint8_t morphShade(uint8_t base, uint8_t target) {
  if (base < target)
    base++;

  if (base > target)
    base--;

  return base;
}

inline Color morphColor(Color base, Color target) {
  Color result;

  result.special = 0;
  result.red = morphShade(base.red, target.red);
  result.green = morphShade(base.green, target.green);
  result.blue = morphShade(base.blue, target.blue);

  return result;
}

// Invert a colors values (255 - color).
inline Color invertColor(Color color) {
  Color result;

  result.special = 0;
  result.red = 255 - color.red;
  result.green = 255 - color.green;
  result.blue = 255 - color.blue;

  return result;
}

// Helper to convert a byte to a two digit HEX string.
inline String _colorToStringHelper(uint8_t b) {
  String result(b, HEX);
  return result.length() == 1 ? "0" + result : result;
}

// Returns string of form 0xFFFFFFFF
inline String colorToString(Color color) {
  // This is horrible, but works.
  String result = (
    "0x" +
    _colorToStringHelper(color.special) +
    _colorToStringHelper(color.red) +
    _colorToStringHelper(color.green) +
    _colorToStringHelper(color.blue)
  );

  result.toUpperCase();
  return result;
}



// Accepts output of above. Returns BLACK on failure.
inline Color stringToColor(String color) {
  typedef struct {
    String name;
    Color color;
  } nameToColor;

  const int COLOR_NAME_MAP_SIZE = 10;
  const nameToColor colorNameMap[COLOR_NAME_MAP_SIZE] = {
    "BLACK", BLACK,
    "WHITE", WHITE,
    "RED", RED,
    "GREEN", GREEN,
    "BLUE", BLUE,
    "YELLOW", YELLOW,
    "LIGHT_BLUE", LIGHT_BLUE,
    "PURPLE", PURPLE,
    "RANDOM", RANDOM,
    "RANDOM_PRIMARY", RANDOM_PRIMARY,
  };
  // Try to look up well known color names.
  for (int i = 0; i < COLOR_NAME_MAP_SIZE; i++) {
    if (color == colorNameMap[i].name) {
      return colorNameMap[i].color;
    }
  }

  // Otherwise, it's expected to be a HEX string of format "0xFFFFFFFF",
  // case insensitive.

  String prefix = color.substring(0, 2);
  prefix.toUpperCase();

  // If it's a bad string, default to BLACK.
  if (prefix != "0X" || color.length() != 10)
    return BLACK;

  Color result;

  // Read each byte in the expected order.
  result.special = strtol(color.substring(2, 4).c_str(), NULL, 16);
  result.red = strtol(color.substring(4, 6).c_str(), NULL, 16);
  result.green = strtol(color.substring(6, 8).c_str(), NULL, 16);
  result.blue = strtol(color.substring(8, 10).c_str(), NULL, 16);

  return result;
}

// Comparison Operators
inline bool operator ==(const Color &left, const Color &right) {
  return ((left.special == right.special) &&
          (left.red == right.red) &&
          (left.green == right.green) &&
          (left.blue == right.blue));
}

inline bool operator !=(const Color &left, const Color &right) {
  return !(left == right);
}

#endif
