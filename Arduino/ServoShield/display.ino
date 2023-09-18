// Draw the address programming screen
void updateFullDisplayAddress() {

  display.clearDisplay();
  display.setCursor(2, 2);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.print("Decoder ADDRESS");
  updateDisplayValueAddress();
}

// Draw the address value
void updateDisplayValueAddress() {

  sprintf(buffer, "Val %3d", newAddressValue);
  display.setCursor(2, 17);
  display.print(buffer);
  display.display();   
}

// Draw the servo configuration screen
void updateFullDisplayServo() {

  display.clearDisplay();

  sprintf(buffer, "OUT %2d, Set %s", outIndex + 1, getConfigItemName(outSubIndex));
  //display.drawRect(0, 0, 128, 32, SSD1306_WHITE);
  display.setCursor(2, 2);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.print(buffer);
  display.drawRect(50, 15, 68, 12, SSD1306_WHITE);
  updateDisplayValue(getCurrentValue());
}

void updateDisplayValue(byte value) {

  // Write value and update progress bar
  sprintf(buffer, "Val %3d", value);
  display.setCursor(2, 17);
  display.print(buffer);
  display.fillRect(52, 17, 64, 8, SSD1306_BLACK);
  display.fillRect(52, 17, value / 4, 8, SSD1306_WHITE);
  display.display(); 
}

char* getConfigItemName(int configIndex) {

  if(configIndex == 0) return "POS A";
  if(configIndex == 1) return "POS B";
  if(configIndex == 2) return "SPEED";
}