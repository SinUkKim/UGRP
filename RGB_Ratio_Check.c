// RGB_Ratio_Check code (.ino)

#include <Wire.h>
#define ISL_I2C_ADDR 0x44
int RED = 2;
int GREEN = 3;
int BLUE = 4;
int i = 250;
int j = 0;
int k = 250;

#define DEVICE_ID 0x00
#define CONFIG_1 0x01
#define CONFIG_2 0x02
#define CONFIG_3 0x03
#define STATUS 0x08 
#define GREEN_L 0x09 
#define GREEN_H 0x0A
#define RED_L 0x0B
#define RED_H 0x0C
#define BLUE_L 0x0D
#define BLUE_H 0x0E


#define CFG1_MODE_RGB 0x05
#define CFG1_375LUX 0x00
#define CFG1_10KLUX 0x08
#define CFG1_16BIT 0x00

class SFE_ISL29125 {
public:
  SFE_ISL29125(uint8_t addr = ISL_I2C_ADDR);
  bool init();
  uint16_t readRed();
  uint16_t readGreen();
  uint16_t readBlue();

private:
  uint8_t _addr;
  uint16_t read16(uint8_t reg);
};

SFE_ISL29125::SFE_ISL29125(uint8_t addr) {
  _addr = addr;
}

bool SFE_ISL29125::init() {
  Wire.begin();
  Wire.setClock(50000); 
  delay(50);

  Wire.beginTransmission(_addr);
  Wire.write(DEVICE_ID);
  Wire.endTransmission();
  Wire.requestFrom(_addr, (uint8_t)1);
  if (Wire.read() != 0x7D) return false;

  Wire.beginTransmission(_addr);
  Wire.write(CONFIG_1);
  Wire.write(CFG1_MODE_RGB | CFG1_375LUX | CFG1_16BIT);
  Wire.endTransmission();


  return true;
}

uint16_t SFE_ISL29125::read16(uint8_t reg) {
  uint16_t data = 0;
  Wire.beginTransmission(_addr);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(_addr, (uint8_t)2);
  if (Wire.available() == 2) {
    data = Wire.read();
    data |= (Wire.read() << 8);
  }
  return data;
}

uint16_t SFE_ISL29125::readRed() { return read16(RED_L); }
uint16_t SFE_ISL29125::readGreen() { return read16(GREEN_L); }
uint16_t SFE_ISL29125::readBlue() { return read16(BLUE_L); }

SFE_ISL29125 RGB_sensor;

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  Serial.begin(115200);
  delay(100);
  
  if (RGB_sensor.init()) {
    Serial.println("Sensor Initialization Successful");
  } else {
    Serial.println("Sensor Initialization Failed!");
    while (1);
  }
}

void loop() {
  analogWrite(RED, i);
  analogWrite(GREEN, j);
  analogWrite(BLUE, k);
  
  unsigned int red = RGB_sensor.readRed();
  unsigned int green = RGB_sensor.readGreen();
  unsigned int blue = RGB_sensor.readBlue();

  unsigned int total = red + green + blue;

  if (total == 0) {
    Serial.println("No color detected.");
  } else {
    float redRatio = (red / (float)total) * 100.0;
    float greenRatio = (green / (float)total) * 100.0;
    float blueRatio = (blue / (float)total) * 100.0;

    float totalRatio = redRatio + greenRatio + blueRatio;
    redRatio = (redRatio / totalRatio) * 100.0;
    greenRatio = (greenRatio / totalRatio) * 100.0;
    blueRatio = (blueRatio / totalRatio) * 100.0;

    Serial.print("Red: "); Serial.print(redRatio, 2); Serial.print("%, ");
    Serial.print("Green: "); Serial.print(greenRatio, 2); Serial.print("%, ");
    Serial.print("Blue: "); Serial.print(blueRatio, 2); Serial.println("%");
  }

  Serial.flush(); 
  delay(2000); 
}
