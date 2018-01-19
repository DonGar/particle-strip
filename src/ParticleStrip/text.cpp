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

#include "text.h"

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

const String PATTERN_MAP[PATTERN_COUNT] = {
  "SOLID", "PULSE", "CYLON", "ALTERNATE", "FLICKER", "LAVA", "TEST"
};
