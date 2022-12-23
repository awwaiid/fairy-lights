
// Ideas:
//   * DONE Snake Stripes! Make it so that every 3-4 segments they have a different color stripe. These could start at like segment 8 to show that they are an adult
//   * DONE (via world reset) Max snake length?
//   * Big snakes are slower than small snakes
//   * Dead snakes turn a color and get eaten one segment at a time


using namespace std;
#include <Arduino.h>
#include <vector>
#include <Adafruit_NeoPixel.h>
#include "snake.cpp"

// #define LED_PIN 6
#define LED_PIN A3
#define LED_COUNT 200
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Global clock tick
int tick = 0;


void addSnake(vector<Snake> *snakes) {
  Serial.println("Adding a snake!");
  Snake s1 = Snake(&strip);
  // s1.head_color = strip.ColorHSV(random(65536));
  s1.head_color = strip.Color(255, 0, 0);
  uint32_t body_base_color = random(65536);
  s1.body_color = strip.ColorHSV(body_base_color);
  // s1.stripe_color = strip.ColorHSV((body_base_color + 10000) % 65535);
  s1.stripe_color = strip.ColorHSV(random(65536));
  s1.direction = random(2);
  s1.head_location = random(200);
  s1.body_min = s1.head_location;
  s1.body_max = s1.head_location;
  s1.speed = random(7) + 1;
  s1.length = 3;
  snakes->push_back(s1);
}

void snakeBounce() {
    strip.clear();
  Snake s1 = Snake(&strip);
  Snake s2 = Snake(&strip);
  s2.jumpTo(126);
  s2.speed = 3;
  s2.direction = 0;
  s2.head_color = strip.Color(0,0,255);
  s2.body_color = strip.Color(128,128,0);

  bool processingCollision = false;
  while (1) {
    tick++;


    if (s1.collidesWith(&s2)) {
      if(!processingCollision) {
        s1.direction = (s1.direction + 1) % 2;
        s2.direction = (s2.direction + 1) % 2;
        processingCollision = true;
      }
    } else {
      processingCollision = false;
    }

    s1.step(tick);
    s2.step(tick);

    s1.print();
    s2.print();

    strip.clear();
    s1.draw();
    s2.draw();
    strip.show();

    delay(20);
  }

}


void drawSnake(int head_location, uint32_t color, int length, int backwards) {
  for (int i = 0; i < length; i++) {
    if (!backwards) {
      strip.setPixelColor(head_location - i, color);
    } else {
      strip.setPixelColor(head_location + i, color);
    }
  }
}

void snakesCollide() {
  for (int i = 0; i < strip.numPixels() / 2; i++) {  // For each pixel in strip...
    strip.clear();
    for (int j = 0; j < 5; j++) {
      strip.setPixelColor(((i + j) % strip.numPixels()), strip.Color(128, 0, 32));
      strip.setPixelColor(((strip.numPixels() - (i + j)) % strip.numPixels()), strip.Color(32, 0, 128));
    }
    strip.show();  //  Update strip to match
    delay(100);    //  Pause for a moment
  }

  for (int i = 0; i < strip.numPixels() / 2; i++) {  // For each pixel in strip...
    strip.setPixelColor(((i + strip.numPixels() / 2) % strip.numPixels()), strip.Color(32, 255, 32));
    strip.setPixelColor(((strip.numPixels() / 2 - i) % strip.numPixels()), strip.Color(32, 255, 32));
    strip.show();  //  Update strip to match
    delay(100);    //  Pause for a moment
  }
}

void forwardSnake() {
  for (int i = 0; i < strip.numPixels(); i++) {  // For each pixel in strip...
    strip.clear();
    for (int j = 0; j < 5; j++) {
      strip.setPixelColor(((i + j) % strip.numPixels()), strip.Color(128, 0, 32));  //  Set pixel's color (in RAM)
    }
    strip.show();  //  Update strip to match
    delay(100);    //  Pause for a moment
  }
}

void reverseSnake() {
  for (int i = strip.numPixels() - 1; i >= 0; i--) {  // For each pixel in strip...
    strip.clear();
    for (int j = 0; j < 5; j++) {
      strip.setPixelColor(((i + j) % strip.numPixels()), strip.Color(128, 0, 32));  //  Set pixel's color (in RAM)
    }
    strip.show();  //  Update strip to match
    delay(100);    //  Pause for a moment
  }
}


void allPixelsAllColors() {
  for (int hue = 0; hue <= 65535; hue++) {
    for (int i = 0; i < strip.numPixels(); i++) {  // For each pixel in strip...
      strip.setPixelColor(i, strip.ColorHSV(hue, 255, 255));
    }
    strip.show();
    // delay(5);
  }
}

void allPixelsFade() {

  for (int hue = 0; hue <= 65535; hue += 1000) {
    for (int sat = 0; sat <= 255; sat += 10) {
      for (int i = 0; i < strip.numPixels(); i++) {  // For each pixel in strip...
        strip.setPixelColor(i, strip.ColorHSV(hue, sat, 255));
      }
      strip.show();
      delay(500);
    }
     for (int sat = 255; sat >= 0; sat -= 10) {
      for (int i = 0; i < strip.numPixels(); i++) {  // For each pixel in strip...
        strip.setPixelColor(i, strip.ColorHSV(hue, sat, 255));
      }
      strip.show();
      delay(500);
    }
  }
}

// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) {  // For each pixel in strip...
    strip.setPixelColor(i, color);               //  Set pixel's color (in RAM)
    strip.show();                                //  Update strip to match
    delay(wait);                                 //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for (int a = 0; a < 10; a++) {   // Repeat 10 times...
    for (int b = 0; b < 3; b++) {  //  'b' counts from 0 to 2...
      strip.clear();               //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for (int c = b; c < strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color);  // Set pixel 'c' to value 'color'
      }
      strip.show();  // Update strip with new contents
      delay(wait);   // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
    // strip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    strip.rainbow(firstPixelHue);
    // Above line is equivalent to:
    // strip.rainbow(firstPixelHue, 1, 255, 255, true);
    strip.show();  // Update strip with new contents
    delay(wait);   // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;           // First pixel starts at red (hue 0)
  for (int a = 0; a < 30; a++) {   // Repeat 30 times...
    for (int b = 0; b < 3; b++) {  //  'b' counts from 0 to 2...
      strip.clear();               //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for (int c = b; c < strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int hue = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue));  // hue -> RGB
        strip.setPixelColor(c, color);                        // Set pixel 'c' to value 'color'
      }
      strip.show();                 // Update strip with new contents
      delay(wait);                  // Pause for a moment
      firstPixelHue += 65536 / 90;  // One cycle of color wheel over 90 frames
    }
  }
}

void snakeSurvival() {
  vector<Snake> snakes;
  vector<Snake> dead_snakes;
  addSnake(&snakes);

  while (1) {
    tick++;
    strip.clear();

    // End of the world
    for (auto i = 0u; i < snakes.size(); i++) {
      if (snakes[i].length >= 200) {
        snakes.clear();
        dead_snakes.clear();
        return;
      }
    }

    // BABY SNAKES
    if(random(100) == 0) {
      addSnake(&snakes);
    }

    for (auto i = 0u; i < dead_snakes.size(); i++) {
      // Serial.println("Next Snake");
      dead_snakes[i].step(tick);
    }

    // Serial.println("Looping through snakes...");
    for (auto i = 0u; i < snakes.size(); i++) {
      // Serial.println("Next Snake");
      snakes[i].step(tick);
    }

    for (auto i = 0u; i < snakes.size(); i++) {
      for (auto j = i + 1; j < snakes.size(); j++) {
        // Serial.println("Collision check");
        if (snakes[i].collidesWith(&snakes[j])) {
          Serial.println(" ... COLLISION DETECTED");
          Serial.print("Snake i: ");
          snakes[i].print();
          Serial.print("Snake j: ");
          snakes[j].print();
          if (random(snakes[i].length + snakes[j].length) <= snakes[i].length) {
          // if (random(2) == 0) {
            // snakes[i].length += 1; // snakes[j].length;
            snakes[i].length += snakes[j].length;
            snakes[j].die();
            dead_snakes.push_back(snakes[j]);
            snakes.erase(snakes.begin() + j);
            Serial.printf("Snake i wins! ");
            snakes[i].print();
            Serial.println("Continuing with next sub-snake");
          } else {
            Serial.printf("Second snake wins before/after:\n");
            snakes[j].print();
            // snakes[j].length += 1; // snakes[i].length;
            snakes[j].length += snakes[i].length;

            Serial.printf("Snake j wins! ");
            snakes[j].print();

            snakes[i].die();
            dead_snakes.push_back(snakes[i]);
            snakes.erase(snakes.begin() + i);
            i--;
            Serial.println("Resetting to previous snake");
            break;
            // j = i + 1; // Since the "current" snake is eaten, start over looking for collisions
            // break;
          }
        }
      }

      // snakes[i].print();
    }

    for (auto i = 0u; i < dead_snakes.size(); i++) {
      dead_snakes[i].draw();
    }

    for (auto i = 0u; i < snakes.size(); i++) {
      snakes[i].draw();
    }
    // Serial.println("Done looping through snakes.");


    strip.show();
    delay(50);
  }
}

void setup() {
  randomSeed(analogRead(0));
  Serial.begin(9600);
  strip.begin();
  strip.show();
  strip.setBrightness(128);
}

void loop() {
  // Fill along the length of the strip in various colors...
  // colorWipe(strip.Color(128, 0, 32), 20); // Red
  // colorWipe(strip.Color(  0, 255,   0), 0); // Green
  // colorWipe(strip.Color(  0,   0, 255), 0); // Blue

  // Do a theater marquee effect in various colors...
  // theaterChase(strip.Color(127, 127, 127), 500); // White, half brightness
  // theaterChase(strip.Color(127,   0,   0), 50); // Red, half brightness
  // theaterChase(strip.Color(  0,   0, 127), 50); // Blue, half brightness

  // rainbow(50);             // Flowing rainbow cycle along the whole strip
  // theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant

  // allPixelsAllColors();
  // allPixelsFade();

  // forwardSnake();
  // reverseSnake();
  // snakesCollide();
  // snakeBounce();
  snakeSurvival();

}
