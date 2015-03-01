/*-------------------------------------------------------------------------
  SparkStrip is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  SparkStrip is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with SparkStrip.  If not, see
  <http://www.gnu.org/licenses/>.

  The original version of SparkStrip is available at:
      https://github.com/DonGar/spark-strip
  -------------------------------------------------------------------------*/

#include "color.h"


Color expandSpecial(Color color) {
    if (color.color == RANDOM.color)
        return randomColor();

    if (color.color == RANDOM_PRIMARY.color)
        return randomPrimaryColor();

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
Color dim_color(Color color, float brightness) {
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

// Returns format: "0xFFFFFFFF"
String colorToString(Color color) {
    String strValue = String(color.color, HEX);
    String header("0x00000000");

    return header.substring(0, header.length() - strValue.length()) + strValue;
}

// Accepts format: "0xFFFFFFFF"
Color stringToColor(String color) {
    Color result;
    result.color = strtol(color.c_str(), NULL, 16);

    return result;
}
