#include <GPNBadge.hpp>
#include <FS.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include <Fonts/Picopixel.h>

extern "C" {
#include "user_interface.h"
}
#include <FS.h>
#include "rboot.h"
#include "rboot-api.h"

#define BACKGROUND 0x3FEF

#include "player.h"
#include "obstacle.h"

Badge badge;

Player player;

const int obstacle_count = 4;
Obstacle obstacles[obstacle_count];

int min_obstacle_distance = 50;
int max_obstacle_distance = 80;
int min_gap_height = 0;
int max_gap_height = 98;
int min_gap_size = 50;
int max_gap_size = 80;

int fps = 20;

float speed = 0.5;

int seconds_per_speed = 15;

int furthest_obstacle = 3;
bool game_over = false;

bool button_pressed = false;

void setup() {  
  badge.init();
  badge.setBacklight(true);
  Serial.begin(115200);

  tft.fillScreen(BACKGROUND);
  tft.writeFramebuffer();

  
  badge.setGPIO(MQ3_EN,1);
  rboot_config rboot_config = rboot_get_config();
  SPIFFS.begin();
  File f = SPIFFS.open("/rom"+String(rboot_config.current_rom),"w");
  f.println("Badgy Bird\n");

  init_game();
}

void loop() {
  int startTime = millis();
  
  JoystickState joystick = badge.getJoystickState();

  if (joystick == JoystickState::BTN_ENTER) {
    if (game_over) {
      init_game();
    } else {
      if (!button_pressed) {
        player.jump();
        button_pressed = true;
      }
      //player.speed -= 0.2;
    }
  } else {
    button_pressed = false;
  }

  if (!game_over) {
    player.draw(tft);
    if (player.posy > 128 || player.posy < 0) {
      
      tft.fillScreen(0xf800);
      game_over = true;
    }
  }
  if (!game_over) {
      for (int i = 0; i < obstacle_count; i++) {
        obstacles[i].draw(tft, sqrt(speed));
        if (obstacles[i].collide(player)) {
          tft.fillScreen(0xf800);
          game_over = true;
          break;
        }
        if (obstacles[i].pos < -obstacles[i].width) {
          obstacles[i].pos = obstacles[furthest_obstacle].pos + random(min_obstacle_distance, max_obstacle_distance);
          obstacles[i].gapHeight = random(min_gap_height, max_gap_height);
          obstacles[i].gapSize = random(min_gap_size, max_gap_size);
          furthest_obstacle = i;
        }
      }
  }
  
  if (!game_over) {
    speed += 1.0 / (seconds_per_speed * fps);
  }

  int score_len = 7 + int(log10(speed)) + 2 + 1;
  char score_text[score_len];
  sprintf(score_text, "Score: %i", int(speed * 10));
  tft.setFont(&Picopixel);
  tft.setTextSize(1);
  tft.setTextColor(0x0000);
  
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(score_text, 0, 0, &x1, &y1, &w, &h);
  tft.fillRect(115 - w, 5, w + 7, h + 2, BACKGROUND);
  tft.setCursor(120 - w, 10);
  tft.print(score_text);
  
  int endTime = millis();
  tft.writeFramebuffer();
  delay(_min(1000 / fps - (endTime - startTime), 0));
}

void init_game() {
  
  randomSeed(1337);

  player.posy = 30;
  player.speed = 0;
  
  for (int i = 0; i < obstacle_count; i++) {
    obstacles[i] = Obstacle();
    obstacles[i].gapHeight = random(min_gap_height, max_gap_height);
    obstacles[i].gapSize = random(min_gap_size, max_gap_size);
    if (i == 0) {
      obstacles[i].pos += random(50);
    } else {
      obstacles[i].pos = obstacles[i-1].pos + random(min_obstacle_distance, max_obstacle_distance);
    }
  }
  speed = 0.5;
  furthest_obstacle = 3;

  button_pressed = false;

  tft.fillScreen(BACKGROUND);
  tft.writeFramebuffer();

  game_over = false;
}

