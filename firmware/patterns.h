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

#ifndef PATTERN_H
#define PATTERN_H

#include "strip.h"

#define BLOB_COUNT (3)

typedef enum {
  SOLID,
  PULSE,
  CYLON,
  ALTERNATE,
  FLICKER,
  LAVA,
  TEST,
  PATTERN_COUNT
} PatternType;

// Each pattern uses colors A and B differently. All patterns support 'special'
// colors RANDOM and RANDOM_PRIMARY to allow randomization.
//
// Speed affects the display speed of different patterns (measured in MS).
// Roughly describes on 'cycle' of animation.
//
// SOLID: draws color A. Solid BLACK turns all lights off, minimum power draw.
// PULSE: morphs between color A, and color B.
// CYLON: Draws a moving Cylon style 'eye' in color A over a background of B.
// ALTERNATE: Each pixel is color a, then b, repeat. Alternate ever 'speed' ms.
// FLICKER: Simulate a halloween flickering light. color A is 'on', color B is
//          'off', speed ranges between 200-1000 are recommended.
// LAVA: A lava lampish effect. Draw 3 blobs of color A over background of B,
//       and morph them over time. Speed controls how long blobs last.
// TEST: A test pattern to ensure a strip is working properly. Flashes
//       Black, White, Red, Green, Blue, then repeates per-pixel.


// Standard usage is to initialize the pattern in 'setup()', and call
// 'drawUpdate()' in your 'loop()' method. Delays in the the loop method
// will block animation updates.
//
// It's generally safe to call "setPattern" to change the pattern at any time,
// but the change won't take effect until after a clean break in the current
// animation cycle.
//
// If an event_name is given to the constructor, then getText() will be
// published to it every time the pattern being displayed it updated.

class Pattern {
  public:
    Pattern(ColorStrip* strip, String event_name="");

    void setPattern(PatternType pattern, Color a, Color b, int speed);

    // Returns true, if the Pattern was updated.
    bool drawUpdate();

    // Get/Set the pattern based on a string of the form:
    //   <PATTERN>,<COLOR>,<COLOR>,<SPEED>
    //   Eg: "CYLON,0x000000ff,0x00000000,1000"
    String getText();
    int setText(String value);

  protected:
    void reset_workingstate();

    void handle_solid();
    void handle_pulse();
    void handle_cylon();
    void handle_alternate();
    void handle_flicker();
    void handle_lava();
    void handle_test();

    typedef struct PatternDescription {
        PatternType pattern;
        Color a;
        Color b;
        int speed;
    } PatternDescription;

    typedef struct Blob {
        int pos;
        int size;
        int duration;
        Color color;
    } Blob;

    // Working values for all patterns.

    // Shared State between Pattern, and handler method.
    unsigned long delay;  // Delay before next draw.
    bool next_ready;      // Is current pattern ready to exit.

    // Provided for handler methods.
    Color a;
    Color b;
    Color c;
    bool go_right;
    int position;

    Blob blob[BLOB_COUNT];

    // Member variables.
    ColorStrip* strip;
    PatternDescription active;
    PatternDescription next;

    String event_name;

    unsigned long nextDraw;
};

#endif
