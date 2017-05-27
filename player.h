
class Player {
  public:
    uint16_t color = 0xF800;
    uint16_t background = 0x3FEF;
    float speed = 0;
    float posx = 10;
    float posy = 30;
    int size = 10;
    float gravity = 0.05;

    void draw(TFT_ILI9163C tft);
    void jump();
};

void Player::draw(TFT_ILI9163C tft) {
  tft.fillRect(int(posx), int(posy), size, size, background);
  speed += gravity;
  posy += speed;
  tft.fillRect(int(posx), int(posy), size, size, color);
}

void Player::jump() {
  speed = -1;
}

