class Obstacle {
  public:
    uint16_t color = 0x001F;
    uint16_t background = 0x3FEF;
    float pos = 128;
    int width = 4;
    int gapHeight = 50;
    int gapSize = 50;

    void draw(TFT_ILI9163C tft, float speed);
    bool collide(Player player);
};

void Obstacle::draw(TFT_ILI9163C tft, float speed) {
  tft.fillRect(int(pos), 0, width, gapHeight, background);
  tft.fillRect(int(pos), gapHeight + gapSize, width, 128 - gapHeight - gapSize, background);
  pos -= speed;
  tft.fillRect(int(pos), 0, width, gapHeight, color);
  tft.fillRect(int(pos), gapHeight + gapSize, width, 128 - gapHeight - gapSize, color);
}

bool Obstacle::collide(Player player) {
  if (pos < player.posx + player.size && pos + width > player.posx) {
    if (gapHeight > player.posy || gapHeight + gapSize < player.posy + player.size) {
      return true;
    }
  }
  return false;
}

