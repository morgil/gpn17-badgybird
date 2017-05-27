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

int obstacle_count = 4;
Obstacle obstacles[4];

int fps = 20;

float speed = 0.5;

int seconds_per_point = 2;

int furthest_obstacle = 3;
bool game_over = false;

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
      player.jump();
    }
  }

  if (!game_over) {
    player.draw(tft);
    if (player.posy > 128 || player.posy < 0) {
      game_over = true;
    }
  }
  if (!game_over) {
      for (int i = 0; i < obstacle_count; i++) {
        obstacles[i].draw(tft, speed);
        if (obstacles[i].collide(player)) {
          tft.fillScreen(0xf800);
          game_over = true;
          break;
        }
        if (obstacles[i].pos < -obstacles[i].width) {
          obstacles[i].pos = obstacles[furthest_obstacle].pos + random(25, 75);
          obstacles[i].gapHeight = random(0, 98);
          obstacles[i].gapSize = random(30, 80);
          furthest_obstacle = i;
        }
      }
  }
  
  if (!game_over) {
    speed += 1.0 / (seconds_per_point * fps);
  }
  
  tft.setFont(&Picopixel);
  tft.setTextSize(1);
  tft.setTextColor(0x0000);
  tft.setCursor(110,10);
  tft.print(int(speed));
  
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
    obstacles[i].gapHeight = random(0, 98);
    obstacles[i].gapSize = random(30, 80);
    if (i == 0) {
      obstacles[i].pos += random(50);
    } else {
      obstacles[i].pos = obstacles[i-1].pos + random(25, 75);
    }
  }
  speed = 0.5;
  furthest_obstacle = 3;

  tft.fillScreen(BACKGROUND);
  tft.writeFramebuffer();

  game_over = false;
}

