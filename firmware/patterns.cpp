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

#include<application.h>
#include "patterns.h"
#include "math.h"


Pattern::Pattern(ColorStrip* strip, String event_name) :
    strip(strip) {

  // Start off by turning the strip off.
  this->active.pattern = SOLID;
  this->active.a = BLACK;
  this->active.speed = 100;

  // Set 'next' pattern to same.
  this->next = this->active;

  this->event_name = event_name;

  // Clear the working state.
  this->reset_workingstate();

  // You would think we should publish to event_name here, BUT, this constructor
  // is normally invoked from static initialization, and trying to publish an
  // event before "setup" is called will probably crash the firmware. By setting
  // the initial 'next' to the same as active, we'll cause our initial pattern
  // to be published when we switch to it.
}

void Pattern::setPattern(PatternType pattern, Color a, Color b, int speed) {
  this->next.pattern = pattern;
  this->next.a = a;
  this->next.b = b;
  this->next.speed = speed;
}

bool Pattern::drawUpdate() {
  unsigned long now = millis();

  if (now < this->nextDraw)
    return false;

  switch (this->active.pattern) {
    case SOLID:
    case PATTERN_COUNT:
      this->handle_solid();
      break;
    case PULSE:
      this->handle_pulse();
      break;
    case CYLON:
      this->handle_cylon();
      break;
    case ALTERNATE:
      this->handle_alternate();
      break;
    case FLICKER:
      this->handle_flicker();
      break;
    case LAVA:
      this->handle_lava();
      break;
    case TEST:
      this->handle_test();
      break;
  }

  this->nextDraw = now + this->delay;

  if (this->next_ready &&
      this->next.pattern != PATTERN_COUNT) {
    // Switch to next pattern, reset next.
    this->active = this->next;
    this->next.pattern = PATTERN_COUNT;

    // Publish the new active pattern.
    if (this->event_name != "") {
      Spark.publish(this->event_name, this->getText(), 60, PRIVATE);
    }

    // Reset the working state.
    this->reset_workingstate();
    return true;
  }

  // The pattern wasn't updated.
  return false;
}

const static String PATTERN_MAP[PATTERN_COUNT] = {
  "SOLID", "PULSE", "CYLON", "ALTERNATE", "FLICKER", "LAVA", "TEST"
};

String Pattern::getText() {
  // <PATTERN>,0xFFFFFF,0xFFFFFF,55
  return (PATTERN_MAP[this->active.pattern] + ',' +
          colorToString(this->active.a) + ',' +
          colorToString(this->active.b) + ',' +
          this->active.speed);
}

int Pattern::setText(String text) {
  // These are the four values we are trying to populate.
  int newPattern = SOLID;
  Color newColorA = BLACK;
  Color newColorB = BLACK;
  int newSpeed  = 0;

  // These are the
  int wordBegin = 0;
  int wordEnd = text.indexOf(',');
  if (wordEnd <= 0)
    return -1;

  String patternText = text.substring(wordBegin, wordEnd);

  // Search for the pattern.
  for (newPattern = 0; newPattern < PATTERN_COUNT; newPattern++) {
    if (patternText == PATTERN_MAP[newPattern]) {
      break;
    }
  }
  if (newPattern == PATTERN_COUNT)
    return -2;

  wordBegin = wordEnd + 1;
  wordEnd = text.indexOf(',', wordBegin);
  if (wordEnd <= 0)
    return -3;

  newColorA = stringToColor(text.substring(wordBegin, wordEnd));

  wordBegin = wordEnd + 1;
  wordEnd = text.indexOf(',', wordBegin);
  if (wordEnd <= 0)
    return -4;

  newColorB = stringToColor(text.substring(wordBegin, wordEnd));

  wordBegin = wordEnd + 1;

  String speedText = text.substring(wordBegin);
  newSpeed = strtol(speedText.c_str(), NULL, 10);

  if (newSpeed < 0)
    return -5;

  this->setPattern((PatternType)newPattern, newColorA, newColorB, newSpeed);
  return 0;
}

//
// Routines to implement Pattern animations.
//

void Pattern::reset_workingstate() {
  this->delay = 0;
  this->next_ready = false;
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


void Pattern::handle_solid() {
  if (this->delay == 0) {
    this->delay = this->active.speed;
    this->next_ready = true;
  }

  this->strip->drawSolid(expandSpecial(this->active.a));
}

void Pattern::handle_pulse() {
  static int steps = 0xFF;

  this->next_ready = false;

  bool initial = this->delay == 0;
  if (initial) {
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
    this->next_ready = !initial;
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
}

void Pattern::handle_cylon() {
  bool initial = this->delay == 0;
  if (initial) {
    this->a = expandSpecial(this->active.a);
    this->b = expandSpecial(this->active.b);
    this->c = mixColor(this->a, this->b, 0.95);
  }

  this->next_ready = false;
  this->delay = (this->active.speed / (this->strip->getPixelCount() * 2));

  // Bounce directions, if needed. Delay longer at the bounce.
  if (this->position >= (this->strip->getPixelCount()-1)) {
    this->go_right = false;
    this->delay = this->delay * 3;
  }

  if (this->position <= 0) {
    this->go_right = true;
    this->delay = this->delay * 3;
    this->next_ready = !initial;
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
}

void Pattern::handle_alternate() {
  if (this->delay == 0) {
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
  this->next_ready = this->go_right;
}

// This attempts to simulate a light with a poor electrical connection (often
// used at halloween). This is done by using a bounded drunkards walk across a
// threshold that changes the lights state.
//
// speed values between 200 and 1000 are recommended for best effect (the larger
// the range, the less frequent flicker behavior is).
void Pattern::handle_flicker() {
  // this->position current light 'connection' strength.
  // this->active.speed    The range over which 'connection' can move.

  if (this->delay == 0) {
    this->delay = 10;

    this->a = expandSpecial(this->active.a);
    this->b = expandSpecial(this->active.b);

    this->position = this->active.speed / 2;
  }

  this->next_ready = false;

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
    this->next_ready = true;
  }

  bool new_go_right = this->position >= (this->active.speed / 2);

  if (new_go_right != this->go_right) {
    this->go_right = new_go_right;
    this->strip->drawSolid(this->go_right ? this->a : this->b);
  }
}

void Pattern::handle_lava() {
  // Intialize all of our blobs to be off screen (so to speak).
  if (this->delay == 0) {
    this->delay = 10;
    this->b = expandSpecial(this->active.b);

    // Initialize the strip.
    this->strip->drawSolid(BLACK);
  }
  this->next_ready = true;

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
}

void Pattern::handle_test() {
  static int COLOR_COUNT = 4;
  static Color colors[] = {RED, GREEN, BLUE, WHITE};

  this->next_ready = true;

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
}
