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

#include <math.h>

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


class PatternDescription {
public:
  inline PatternDescription() :
    pattern(SOLID), a(BLACK), b(BLACK), speed(0) {}

  PatternType pattern;
  Color a;
  Color b;
  int speed;
};

inline bool operator ==(const PatternDescription &left, const PatternDescription &right) {
  return ((left.pattern == right.pattern) &&
          (left.a == right.a) &&
          (left.b == right.b) &&
          (left.speed == right.speed));
}

inline bool operator !=(const PatternDescription &left, const PatternDescription &right) {
  return !(left == right);
}



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

// Sample patterns
// Halloween:  FLICKER,0X005F1000,0X00000000,200
// Night time: SOLID,0x5f5f5f00,0x00000000,1000
// Midnight:   CYLON,0x00ff0000,0x00000000,1000
// Lava Lamp:  LAVA,0X01000000,0X00000000,200
// Doorbell:  PULSE,0X0000FF00,0X00000000,10

class Pattern {
  public:
    inline Pattern(ColorStrip* strip, String event_name="") :
        strip(strip) {

      // Start off by turning the strip off.
      this->active.pattern = SOLID;
      this->active.a = BLACK;
      this->active.speed = 100;

      // Set 'next' pattern to same.
      this->next = this->active;

      // Clear the working state.
      this->reset_workingstate();
    }

    inline void setPattern(PatternType pattern, Color a, Color b, int speed) {
      this->next.pattern = pattern;
      this->next.a = a;
      this->next.b = b;
      this->next.speed = speed;
    }

    // Returns true, if the Pattern was updated.
    inline bool drawUpdate() {
      unsigned long now = millis();

      if (now < this->nextDraw)
        return false;

      bool next_ready = false;

      switch (this->active.pattern) {
        case SOLID:
        case PATTERN_COUNT:
          next_ready = this->handle_solid();
          break;
        case PULSE:
          next_ready = this->handle_pulse();
          break;
        case CYLON:
          next_ready = this->handle_cylon();
          break;
        case ALTERNATE:
          next_ready = this->handle_alternate();
          break;
        case FLICKER:
          next_ready = this->handle_flicker();
          break;
        case LAVA:
          next_ready = this->handle_lava();
          break;
        case TEST:
          next_ready = this->handle_test();
          break;
      }

      this->initial = false;
      this->nextDraw = now + this->delay;

      if (next_ready &&
          this->next.pattern != PATTERN_COUNT) {
        // Switch to next pattern, reset next.
        this->active = this->next;
        this->next.pattern = PATTERN_COUNT;
        this->reset_workingstate();

        return true;
      }

      // The pattern wasn't updated.
      return false;
    }

  protected:
    inline void reset_workingstate() {
      this->delay = 0;
      this->initial = true;
      this->a = BLACK;
      this->b = BLACK;
      this->c = BLACK;
      this->go_right = true;
      this->position = 0;

      for (Blob *b = this->blob;
           b < (this->blob + BLOB_COUNT);
           b++) {
        b->pos = -1;
      }
    }

    inline bool handle_solid() {
      if (this->delay == 0) {
        this->delay = this->active.speed;
      }

      this->strip->drawSolid(expandSpecial(this->active.a));
      return true;
    }

    inline bool handle_pulse() {
      const static int steps = 0xFF;
      bool next_ready = false;

      if (this->initial) {
        this->delay = this->active.speed / steps;
      }

      // Bounce directions, if needed.
      if (this->position >= steps) {
        this->go_right = false;
        this->a = expandSpecial(this->active.a);
      }
      if (this->position <= 0) {
        this->go_right = true;
        this->b = expandSpecial(this->active.b);
        next_ready = !this->initial;
      }

      float ratio = this->position / (float)steps;
      Color drawColor = mixColor(this->a, this->b, ratio);
      this->strip->drawSolid(drawColor);

      // Increment.
      if (this->go_right) {
        this->position++;
      } else {
        this->position--;
      }

      return next_ready;
    }

    inline bool handle_cylon() {
      if (this->initial) {
        this->a = expandSpecial(this->active.a);
        this->b = expandSpecial(this->active.b);
        this->c = mixColor(this->a, this->b, 0.95);
      }

      bool next_ready = false;
      this->delay = (this->active.speed / (this->strip->getPixelCount() * 2));

      // Bounce directions, if needed. Delay longer at the bounce.
      if (this->position >= (this->strip->getPixelCount()-1)) {
        this->go_right = false;
        this->delay = this->delay * 3;
      }

      if (this->position <= 0) {
        this->go_right = true;
        this->delay = this->delay * 3;
        next_ready = !initial;
      }

      // Do the draw.
      for (int i = 0; i < this->strip->getPixelCount(); i++) {
        if (i == this->position - 1) {
          this->strip->drawPixel(this->c);
        } else if (i == this->position) {
          this->strip->drawPixel(this->a);
        } else if (i == this->position + 1) {
          this->strip->drawPixel(this->c);
        } else {
          this->strip->drawPixel(this->b);
        }
      }
      this->strip->finishDraw();

      // Increment.
      if (this->go_right) {
        this->position++;
      } else {
        this->position--;
      }

      return next_ready;
    }

    inline bool handle_alternate() {
      if (this->initial) {
        this->delay = this->active.speed;
        this->a = expandSpecial(this->active.a);
        this->b = expandSpecial(this->active.b);
      }

      for (int i = 0; i < this->strip->getPixelCount(); i++) {
        Color pixelColor = ((i % 2) == this->go_right) ? this->a : this->b;
        this->strip->drawPixel(pixelColor);
      }
      this->strip->finishDraw();

      this->go_right = !this->go_right;
      return this->go_right;
    }

    // This attempts to simulate a light with a poor electrical connection (often
    // used at halloween). This is done by using a bounded drunkards walk across a
    // threshold that changes the lights state.
    //
    // speed values between 200 and 1000 are recommended for best effect (the larger
    // the range, the less frequent flicker behavior is).
    inline bool handle_flicker() {
      // this->position current light 'connection' strength.
      // this->active.speed    The range over which 'connection' can move.

      if (this->initial) {
        this->delay = 10;

        this->a = expandSpecial(this->active.a);
        this->b = expandSpecial(this->active.b);

        this->position = this->active.speed / 2;
      }

      bool next_ready = false;

      // -10, 0, 10  (steps of 10 used increase standard speeds)
      this->position += random(-1, 2) * 10;

      // Ensure this->position remains in range.
      if (this->position < 0) {
        this->position = 0;
        this->a = expandSpecial(this->active.a);
      }

      if (this->position > this->active.speed) {
        this->position = this->active.speed;
        this->b = expandSpecial(this->active.b);
        next_ready = true;
      }

      bool new_go_right = this->position >= (this->active.speed / 2);

      if (new_go_right != this->go_right) {
        this->go_right = new_go_right;
        this->strip->drawSolid(this->go_right ? this->a : this->b);
      }

      return next_ready;
    }

    inline bool handle_lava() {
      // Intialize all of our blobs to be off screen (so to speak).
      if (this->initial) {
        this->delay = 10;
        this->b = expandSpecial(this->active.b);

        // Initialize the strip.
        this->strip->drawSolid(BLACK);
      }

      // Read-Only.
      int pixelCount = this->strip->getPixelCount();
      Color *pixelBuffer = this->strip->getPixelBuffer();

      // Mutate our blobs.
      for (Blob *b = this->blob; b < (this->blob + BLOB_COUNT); b++) {
        b->duration--;

        // If it's not currently displayed.
        if (b->pos == -1) {
          if (b->duration <= 0) {
            b->pos = random(pixelCount);
            b->size = pixelCount - log(random(exp(pixelCount)));
            b->duration = random(this->active.speed);
            b->color = expandSpecial(this->active.a);
          }
        } else {
          if (b->duration <= 0) {
            b->pos = -1;
            b->duration = random(this->active.speed);
          }
        }
      }

      this->position++;
      bool backgroundFade = (this->position >= 3);
      if (backgroundFade) {
        this->position = 0;
      }

      for (int p = 0; p < pixelCount; p++) {
        Color pixel = pixelBuffer[p];

        if (backgroundFade) {
          pixel = morphColor(pixel, this->b);
        }

        for (Blob *b = this->blob; b < (this->blob + BLOB_COUNT); b++) {
          if (b->pos == -1)
            continue;

          int bMin = b->pos - b->size;
          int bMax = b->pos + b->size;

          // Is p inside the blob range, including wrapping.
          if (((p - pixelCount) > bMin && (p - pixelCount) < bMax) ||
              (p > bMin && p < bMax) ||
              ((p + pixelCount) > bMin && (p + pixelCount) < bMax)) {
            pixel = morphColor(pixel, b->color);
          }
        }

        this->strip->drawPixel(pixel);
      }

      this->strip->finishDraw();
      return true;
    }

    inline bool handle_test() {
      static int COLOR_COUNT = 4;
      static Color colors[] = {RED, GREEN, BLUE, WHITE};

      // If go_right == true draw SOLID colors.
      //    colors index = position.
      // if go_right == false draw one pixel with color.
      //    pixel       = position / COLOR_COUNT
      //    color index = position % COLOR_COUNT

      int max_position = this->go_right ?
          COLOR_COUNT : this->strip->getPixelCount() * COLOR_COUNT;

      if (this->position >= max_position) {
        this->go_right = !this->go_right;
        this->position = 0;
      }

      if (this->go_right) {
        this->strip->drawSolid(colors[this->position]);
        this->delay = this->active.speed;
      } else {
        int pixel = this->position / COLOR_COUNT;
        int color_index = this->position % COLOR_COUNT;

        for (int i = 0; i < this->strip->getPixelCount(); i++) {
          if (i == pixel) {
            this->strip->drawPixel(colors[color_index]);
          } else {
            this->strip->drawPixel(BLACK);
          }
        }
        this->strip->finishDraw();

        this->delay = this->active.speed / 2;
      }

      this->position++;
      return true;
    }

    typedef struct Blob {
        int pos;
        int size;
        int duration;
        Color color;
    } Blob;

    // Working values for all patterns.

    // Shared State between Pattern, and handler method.
    unsigned long delay;  // Delay before next draw.
    bool initial;         // Is this the first iteration for the pattern.

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

    unsigned long nextDraw;
};

#endif
