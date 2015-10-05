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

#include "color.h"


Color expandSpecial(Color color) {
  if (color.special == RANDOM.special)
    return randomColor();

  if (color.special == RANDOM_PRIMARY.special)
    return randomPrimaryColor();

  color.special = 0;
  return color;
}

Color randomColor() {
  Color result;

  result.special = 0;
  result.red = random(0, 0x100);
  result.green = random(0, 0x100);
  result.blue = random(0, 0x100);

  return result;
}

Color randomPrimaryColor() {
  Color result;

  result.special = 0;
  result.red = random(0, 2) ? 0xFF : 0;
  result.green = random(0, 2) ? 0xFF : 0;
  result.blue = random(0, 2) ? 0xFF : 0;

  return result;
}

// ratio 0 means all left color. ratio 1.0 means all right color.
Color mixColor(Color left, Color right, float ratio) {
  float l_ratio = 1.0 - ratio;
  float r_ratio = ratio;

  Color result;

  result.special = 0;
  result.red = (left.red * l_ratio) + (right.red * r_ratio);
  result.green = (left.green * l_ratio) + (right.green * r_ratio);
  result.blue = (left.blue * l_ratio) + (right.blue * r_ratio);

  return result;
}

// brightness is from 0.0 to 1.0.
Color dimColor(Color color, float brightness) {
  return mixColor(BLACK, color, brightness);
}

static uint8_t morphShade(uint8_t base, uint8_t target) {
  if (base < target)
    base++;

  if (base > target)
    base--;

  return base;
}

// This has no rounding errors if you want to slowly/smoothly move from color a
// to b. Transition steps based on diffence of the color difference. Requires at
// most 255 steps.
Color morphColor(Color base, Color target) {
  Color result;

  result.special = 0;
  result.red = morphShade(base.red, target.red);
  result.green = morphShade(base.green, target.green);
  result.blue = morphShade(base.blue, target.blue);

  return result;
}

// Helper to convert a byte to a two digit HEX string.
String b_s(uint8_t b) {
  String result(b, HEX);
  return result.length() == 1 ? "0" + result : result;
}

// Returns format: "0xFFFFFFFF"
String colorToString(Color color) {
  // This is horrible, but works.
  String result = (
    "0x" +
    b_s(color.special) +
    b_s(color.red) +
    b_s(color.green) +
    b_s(color.blue)
  );

  result.toUpperCase();
  return result;
}

typedef struct {
  String name;
  Color color;
} nameToColor;

#define COLOR_NAME_MAP_SIZE 10
nameToColor colorNameMap[COLOR_NAME_MAP_SIZE] = {
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

Color stringToColor(String color) {

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
