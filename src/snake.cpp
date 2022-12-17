#include <Adafruit_NeoPixel.h>

class Snake {
  public:

  enum class Health {alive, death_rattle, dead};

  int head_location = 0;
  int body_max = 0;
  int body_min = 0;
  int length = 7;
  int direction = 1;
  int speed = 5;
  uint32_t head_color;
  uint32_t body_color;
  uint32_t stripe_color;
  Adafruit_NeoPixel *strip;
  Health health = Health::alive;
  int death_fade_steps = 0;

  Snake(Adafruit_NeoPixel *strip) {
    this->strip = strip;
    this->head_color = strip->Color(255, 0, 0);
    this->body_color = strip->Color(0, 255, 0);
    this->stripe_color = strip->Color(64, 255, 64);
  }

  void print() {
    Serial.printf("Snake @ %d (%d-%d:%d) %d\n", head_location, body_min, body_max, length, direction);
  }

  int wrap(int loc) {
    if (loc < 0) {
      loc = strip->numPixels() + loc;
    }
    return loc % strip->numPixels();
  }

  void die() {
    health = Health::death_rattle;
    death_fade_steps = 128;
  }

  void step(int t) {
    // Serial.println("Step");
    // Serial.println(t);
    if (health == Health::death_rattle) {
      if (death_fade_steps > 0) {
        death_fade_steps--;
        uint32_t death_color = strip->Color(
            death_fade_steps * 2,
            death_fade_steps * 2,
            death_fade_steps * 2
        );
        head_color = death_color;
        body_color = death_color;
        stripe_color = death_color;
      } else {
        health = Health::dead;
      }
    } else {
      if (t % speed == 0) {
        if (direction) {
          head_location = wrap(head_location + 1);
          if (head_location >= wrap(body_max + 1)) {
            body_max = head_location;
          } else {
            body_min = head_location;
          }
          if (wrap(head_location - length) >= wrap(body_min + 1)) {
            body_min = wrap(head_location - length);
          }
        } else {
          head_location = wrap(head_location - 1);
          if (head_location <= wrap(body_min - 1)) {
            body_min = head_location;
          } else {
            body_max = head_location;
          }
          if (wrap(head_location + length) <= wrap(body_max - 1)) {
            body_max = wrap(head_location + length);
          }
        }
      }
    }
  }

  void draw() {
    if (health == Health::dead) {
      return;
    }
    int range = wrap(body_max - body_min);
    for (int i = 0; i < range; i++) {
      if (i % 6 >= 5) {
        strip->setPixelColor(wrap(i + body_min), stripe_color);
      } else {
        strip->setPixelColor(wrap(i + body_min), body_color);
      }
    }
    strip->setPixelColor(head_location, head_color);
  }

  void jumpTo(int location) {
    head_location = location;
    body_min = location;
    body_max = location;
  }

  bool collidesWith(Snake *other) {
    if (
        health == Health::death_rattle
        || health == Health::dead
        || other->health == Health::death_rattle
        || other->health == Health::dead) {
      // Ghost snakes do not collide
      return false;
    }
    if (body_max >= body_min) {
      return
          ((body_max >= other->body_max && body_min <= other->body_max)
        || (body_max >= other->body_min && body_min <= other->body_min));

    } else {
      // This is when we wrap-around, everything is backwards
      // .... kinda
      return
          ((body_max >= other->body_max || body_min <= other->body_max)
        || (body_max >= other->body_min || body_min <= other->body_min));
    }
  }
};
