#include <Wire.h>

#define BMP280_I2C_ADDRESS 0x76
#define BMP280_CHIP_ID     0x58

enum {
  BMP280_REGISTER_DIG_T1 = 0x88,
  BMP280_REGISTER_DIG_T2 = 0x8A,
  BMP280_REGISTER_DIG_T3 = 0x8C,
  BMP280_REGISTER_DIG_P1 = 0x8E,
  BMP280_REGISTER_DIG_P2 = 0x90,
  BMP280_REGISTER_DIG_P3 = 0x92,
  BMP280_REGISTER_DIG_P4 = 0x94,
  BMP280_REGISTER_DIG_P5 = 0x96,
  BMP280_REGISTER_DIG_P6 = 0x98,
  BMP280_REGISTER_DIG_P7 = 0x9A,
  BMP280_REGISTER_DIG_P8 = 0x9C,
  BMP280_REGISTER_DIG_P9 = 0x9E,
  BMP280_REGISTER_CHIPID = 0xD0,
  BMP280_REGISTER_VERSION = 0xD1,
  BMP280_REGISTER_SOFTRESET = 0xE0,
  BMP280_REGISTER_CAL26 = 0xE1, /**< R calibration = 0xE1-0xF0 */
  BMP280_REGISTER_STATUS = 0xF3,
  BMP280_REGISTER_CONTROL = 0xF4,
  BMP280_REGISTER_CONFIG = 0xF5,
  BMP280_REGISTER_PRESSUREDATA = 0xF7,
  BMP280_REGISTER_TEMPDATA = 0xFA,
};

struct BMP280_Calibration {
  uint16_t dig_T1;
  int16_t  dig_T2;
  int16_t  dig_T3;
  uint16_t dig_P1;
  int16_t  dig_P2;
  int16_t  dig_P3;
  int16_t  dig_P4;
  int16_t  dig_P5;
  int16_t  dig_P6;
  int16_t  dig_P7;
  int16_t  dig_P8;
  int16_t  dig_P9;
};

BMP280_Calibration calibration;
int32_t t_fine = 0;

void readCalibrationData() {
  uint8_t buffer[2];
  readRegisters(BMP280_REGISTER_DIG_T1, buffer, 2);
  calibration.dig_T1 = (buffer[1] << 8) | buffer[0];
  readRegisters(BMP280_REGISTER_DIG_T2, buffer, 2);
  calibration.dig_T2 = (buffer[1] << 8) | buffer[0];
  readRegisters(BMP280_REGISTER_DIG_T3, buffer, 2);
  calibration.dig_T3 = (buffer[1] << 8) | buffer[0];

  readRegisters(BMP280_REGISTER_DIG_P1, buffer, 2);
  calibration.dig_P1 = (buffer[1] << 8) | buffer[0];
  readRegisters(BMP280_REGISTER_DIG_P2, buffer, 2);
  calibration.dig_P2 = (buffer[1] << 8) | buffer[0];
  readRegisters(BMP280_REGISTER_DIG_P3, buffer, 2);
  calibration.dig_P3 = (buffer[1] << 8) | buffer[0];
  readRegisters(BMP280_REGISTER_DIG_P4, buffer, 2);
  calibration.dig_P4 = (buffer[1] << 8) | buffer[0];
  readRegisters(BMP280_REGISTER_DIG_P5, buffer, 2);
  calibration.dig_P5 = (buffer[1] << 8) | buffer[0];
  readRegisters(BMP280_REGISTER_DIG_P6, buffer, 2);
  calibration.dig_P6 = (buffer[1] << 8) | buffer[0];
  readRegisters(BMP280_REGISTER_DIG_P7, buffer, 2);
  calibration.dig_P7 = (buffer[1] << 8) | buffer[0];
  readRegisters(BMP280_REGISTER_DIG_P8, buffer, 2);
  calibration.dig_P8 = (buffer[1] << 8) | buffer[0];
  readRegisters(BMP280_REGISTER_DIG_P9, buffer, 2);
  calibration.dig_P9 = (buffer[1] << 8) | buffer[0];
}

uint8_t readRegister(uint8_t reg) {
  Wire.beginTransmission(BMP280_I2C_ADDRESS); // Start I2C transmission
  Wire.write(reg);                              // Send register address
  Wire.endTransmission(false);                 // End transmission (keep connection active)

  Wire.requestFrom(BMP280_I2C_ADDRESS, (uint8_t)1); // Request 1 byte
  return Wire.read();                                // Read the byte
}

void readRegisters(uint8_t reg, uint8_t *buffer, uint8_t length) {
  Wire.beginTransmission(BMP280_I2C_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(BMP280_I2C_ADDRESS, length);
  for (uint8_t i = 0; i < length; i++) {
    buffer[i] = Wire.read();
  }
}

void writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(BMP280_I2C_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

void readRawData(int32_t &rawTemp, int32_t &rawPressure) {
  uint8_t buffer[3];
  readRegisters(BMP280_REGISTER_TEMPDATA, buffer, 3);

  rawTemp = uint32_t(buffer[0]) << 16 | uint32_t(buffer[1]) << 8 |
         uint32_t(buffer[2]);

  readRegisters(BMP280_REGISTER_PRESSUREDATA, buffer, 3);

  rawPressure = uint32_t(buffer[0]) << 16 | uint32_t(buffer[1]) << 8 |
         uint32_t(buffer[2]);
}

float compensateTemperature(int32_t rawTemp) {
  int32_t var1, var2, T;
  rawTemp >>= 4;
  var1 = ((((rawTemp >> 3) - ((int32_t)calibration.dig_T1 << 1))) *
          ((int32_t)calibration.dig_T2)) >> 11;
  var2 = (((((rawTemp >> 4) - ((int32_t)calibration.dig_T1)) *
            ((rawTemp >> 4) - ((int32_t)calibration.dig_T1))) >> 12) *
          ((int32_t)calibration.dig_T3)) >> 14;
  T = var1 + var2;
  t_fine = T;
  float _t = (T * 5 + 128) >> 8;
  return _t / 100;
}

uint32_t compensatePressure(int32_t rawPressure) {
  int64_t var1, var2, p;
  rawPressure >>= 4;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)calibration.dig_P6;
  var2 += ((var1 * (int64_t)calibration.dig_P5) << 17);
  var2 += (((int64_t)calibration.dig_P4) << 35);
  var1 = ((var1 * var1 * (int64_t)calibration.dig_P3) >> 8) +
         ((var1 * (int64_t)calibration.dig_P2) << 12);
  var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)calibration.dig_P1) >> 33;
  if (var1 == 0) return 0;
  p = 1048576 - rawPressure;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)calibration.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)calibration.dig_P8) * p) >> 19;
  p = ((p + var1 + var2) >> 8) + (((int64_t)calibration.dig_P7) << 4);
  return (uint32_t)p / 256;
}

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  
  if (readRegister(BMP280_REGISTER_CHIPID) == BMP280_CHIP_ID)
  {
    Serial.println("BMP280 chip match");
  }
  else
  {
    Serial.println("Error, maybe the wiring or sensor is faulty");
    while (1);
  }
  // Reset the sensor
  writeRegister(BMP280_REGISTER_SOFTRESET, 0xB6);
  delay(100);

  readCalibrationData();

  writeRegister(BMP280_REGISTER_CONTROL, 0x27); // Ctrl_meas: Temperature x2, Pressure x1, Normal mode
  writeRegister(BMP280_REGISTER_CONFIG, 0xA0);


}

void loop() {

  int32_t rawTemp, rawPressure;
  readRawData(rawTemp, rawPressure);

  float temperature = compensateTemperature(rawTemp);
  uint32_t pressure = compensatePressure(rawPressure);

  float pressureHpa = pressure / 100.0;

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Pressure: ");
  Serial.print(pressureHpa);
  Serial.println(" hPa");

  delay(2000);
  
}