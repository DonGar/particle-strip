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

#ifndef TEXT_H
#define TEXT_H

#include<application.h>

#include "color.h"
#include "patterns.h"

//
// Convert Colors to/from Strings.
//

static const struct { String name; Color color; } COLOR_NAME_MAP[] = {
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
#define COLOR_NAME_MAP_SIZE (sizeof(COLOR_NAME_MAP) / sizeof(COLOR_NAME_MAP[0]))

static const String PATTERN_NAMES[] = {
  "SOLID", "PULSE", "CYLON", "ALTERNATE", "FLICKER", "LAVA", "TEST"
};

//
// This contains helpers for converting colors and patterns into simple
// text strings.
//

// Helper to convert a byte to a two digit HEX string.
inline String _colorToStringHelper(uint8_t b) {
  String result(b, HEX);
  return result.length() == 1 ? "0" + result : result;
}

// Returns string that can be turned back into a color.
// For primaries, can be a well known name (ie: 'RED').
// For other colors a hex value (ie: 0xFFFFFFFF).
inline String colorToString(Color color) {
  // Try to look up well known color names.
  for (int i = 0; i < COLOR_NAME_MAP_SIZE; i++) {
    if (color == COLOR_NAME_MAP[i].color) {
      return COLOR_NAME_MAP[i].name;
    }
  }

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

  // Try to look up well known color names.
  for (int i = 0; i < COLOR_NAME_MAP_SIZE; i++) {
    if (color == COLOR_NAME_MAP[i].name) {
      return COLOR_NAME_MAP[i].color;
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

//
// Convert Patterns to/from Strings.
//

inline String patternTypeToString(PatternType pattern) {
  return PATTERN_NAMES[pattern]  ;
}

inline PatternType stringToPatternType(String patternText) {
  // Search for the pattern.
  int patternOffset;
  for (patternOffset = 0; patternOffset < PATTERN_COUNT; patternOffset++) {
    if (patternText == PATTERN_NAMES[patternOffset]) {
      break;
    }
  }
  if (patternOffset == PATTERN_COUNT)
    return SOLID;

  return (PatternType)patternOffset;
}

// Get/Set the pattern based on a string of the form:
//   <PATTERN>,<COLOR>,<COLOR>,<SPEED>
//   Eg: "CYLON,BLUE,0x00050505,1000"
inline String patternToString(PatternDescription pattern) {

  // <PATTERN>,0xFFFFFF,0xFFFFFF,55
  return (patternTypeToString(pattern.pattern) + ',' +
          colorToString(pattern.a) + ',' +
          colorToString(pattern.b) + ',' +
          pattern.speed);
}

inline PatternDescription stringToPattern(String value) {
  PatternDescription newPattern;

  // These are the
  int wordBegin = 0;
  int wordEnd = value.indexOf(',');
  if (wordEnd <= 0)
    return PatternDescription();

  newPattern.pattern = stringToPatternType(value.substring(wordBegin, wordEnd));

  wordBegin = wordEnd + 1;
  wordEnd = value.indexOf(',', wordBegin);
  if (wordEnd <= 0)
    return PatternDescription();

  newPattern.a = stringToColor(value.substring(wordBegin, wordEnd));

  wordBegin = wordEnd + 1;
  wordEnd = value.indexOf(',', wordBegin);
  if (wordEnd <= 0)
    return PatternDescription();

  newPattern.a = stringToColor(value.substring(wordBegin, wordEnd));

  wordBegin = wordEnd + 1;

  String speedText = value.substring(wordBegin);
  newPattern.speed = strtol(speedText.c_str(), NULL, 10);

  if (newPattern.speed < 0)
    return PatternDescription();

  return newPattern;
}

#endif
