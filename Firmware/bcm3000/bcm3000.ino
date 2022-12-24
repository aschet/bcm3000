
// Arduino Firmware for BCM-3000 Beer Color Meter
// Copyright 2023 Thomas Ascher <thomas.ascher@gmx.at>
// SPDX-License-Identifier: MIT
//
// Install the following libraries via the library manager:
// EasyButton 2.0.1, LiquidCrystal I2C 1.1.2, fp64lib 1.1.20

#define DEBUG_SERIAL
#define HIGH_PRECISION

#ifdef HIGH_PRECISION
  #include "double.h"
  typedef Double t_double;
  float64_t& valueRef(Double& f) {
    return f.value;
  }
  const float64_t& valueRef(const Double& f) {
    return f.value;
  }
#else
  typedef double t_double;
  double& valueRef(double& f) {
    return f;
  }
  const double& valueRef(const double& f) {
    return f;
  }
#endif

#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EasyButton.h>

constexpr char* VERSION_DEV = "BCM-3000 MK I";
constexpr char* VERSION_SW = "1.0";
// Set the following value according to your instrument specific water intensity.
static const t_double DEFAULT_WATER_INTENSITY = 0.997;
static const t_double NAN_VALUE = NAN;
constexpr uint8_t LED_430NM_PIN = 4;
constexpr uint8_t PHD_430NM_PIN = A0;
constexpr uint8_t BTN_MEASURE_BEER_PIN = 6;
constexpr uint8_t BTN_MEASURE_WATER_PIN = 7;
constexpr uint32_t BTN_2ND_OP_DELAY = 1000;
constexpr int EEPROM_ADDR_UNIT = 0;
constexpr int EEPROM_ADDR_WATER_INTENSITY = 1;

#ifdef DEBUG_SERIAL

inline void debugPrint(const char* caption, const t_double& value) {
  Serial.print(caption);
  Serial.print(": ");
  Serial.println(value, 4);
}

#endif

class LED {
public:
  LED(uint8_t pin)
    :m_pin(pin) {
  }

  void init() {
    pinMode(m_pin, OUTPUT);
  }

  void setOn(bool on) {
    if (on) {
      digitalWrite(m_pin, HIGH);
    }
    else {
      digitalWrite(m_pin, LOW);
    }
  }

private:
  uint8_t m_pin;
};

class PhotoDiode {
public:
  PhotoDiode(uint8_t pin)
    :m_pin(pin) {
  }

  t_double read() const {
    static t_double VOLTAGE_DIVISOR = 1024;
    t_double voltage = analogRead(m_pin);
    return voltage / VOLTAGE_DIVISOR;
  }

  t_double readAverage() const {
    constexpr uint8_t NUMBER_READS = 10;
    const t_double READS_DIVISOR = NUMBER_READS;
    t_double value = 0.0;
    for (uint8_t i = 0; i < NUMBER_READS; ++i) {
      value += read();
      delay(50);
    }
    value /= READS_DIVISOR;
    return value;    
  }

private:
  uint8_t m_pin;
};

bool isUnitEBC = true;
t_double waterIntensity = DEFAULT_WATER_INTENSITY;
t_double beerIntensity = NAN_VALUE;

LiquidCrystal_I2C lcd(0x27, 16, 2);
LED led430NM(LED_430NM_PIN);
PhotoDiode phd430NM(A0);
EasyButton btnMeasureWater(BTN_MEASURE_WATER_PIN);
EasyButton btnMeasureBeer(BTN_MEASURE_BEER_PIN);

void printUnit() {
  lcd.setCursor(6, 0);
  if (isUnitEBC) {
    lcd.print(" EBC");
  } else {
    lcd.print(" SRM");    
  }
}

void storeUnit() {
  EEPROM.put(EEPROM_ADDR_UNIT, isUnitEBC);
}

void restoreUnit() {
  EEPROM.get(EEPROM_ADDR_UNIT, isUnitEBC);
  printUnit();
}

void printAdjustStatus(bool on) {
  lcd.setCursor(0, 0);
  if (on) {
    lcd.print("ADJ");
  } else {
    lcd.print("   ");
  }
}

void storeWaterIntensity() {
  EEPROM.put(EEPROM_ADDR_WATER_INTENSITY, valueRef(waterIntensity));
}

void clearWaterIntensity() {
  EEPROM.put(EEPROM_ADDR_WATER_INTENSITY, valueRef(NAN_VALUE));  
}

void restoreWaterIntensity() {
  EEPROM.get(EEPROM_ADDR_WATER_INTENSITY, valueRef(waterIntensity));
  if (isnan(waterIntensity)) {
    waterIntensity = DEFAULT_WATER_INTENSITY;
  } else {
    printAdjustStatus(true);
  }
}

t_double correlaeToA430(const t_double& abs) {
  // Generate this calibration polynomial with the calibration.py file
  return t_double(7.2563e-01)+abs*(t_double(-2.0034e+00)+abs*(t_double(2.1415e+00)));
} 

void printMeasurement() {
  if (isnan(beerIntensity))
    return;

  static const t_double NEGATIVE_SIGN = -1.0;
  t_double a430 = log10(beerIntensity / waterIntensity) * NEGATIVE_SIGN;
#ifdef DEBUG_SERIAL
  debugPrint("abs", a430);
#endif

  a430 = correlaeToA430(a430);

  static const t_double SRM_FACTOR = 12.7;
  static const t_double EBC_FACTOR = 25.0;
  t_double value = isUnitEBC ? a430 * EBC_FACTOR : a430 * SRM_FACTOR;
  
  static const t_double RESULT_MIN = 0.0;
  static const t_double RESULT_MAX = 999.0;
  if (value < RESULT_MIN) {
    value = RESULT_MIN;
  } else if (value > RESULT_MAX) {
    value = RESULT_MAX;
  }
  
  constexpr int VALUE_BUFFER_SIZE = 6;
  static char valueBuffer[VALUE_BUFFER_SIZE];
  dtostrf(value, VALUE_BUFFER_SIZE - 1, 1, valueBuffer);
  lcd.setCursor(11, 0);
  lcd.print(valueBuffer);
}

t_double measureIntensity() { 
  t_double noise = phd430NM.readAverage();  
#ifdef DEBUG_SERIAL
  debugPrint("noise", noise);
#endif
  led430NM.setOn(true);
  delay(250);
  t_double intensity = phd430NM.readAverage() - noise;
  led430NM.setOn(false);
#ifdef DEBUG_SERIAL
  debugPrint("intensity", intensity);
#endif  
  return intensity;
}

void onMeasureWater() {
#ifdef DEBUG_SERIAL
  Serial.println("measure water");
#endif
  waterIntensity = measureIntensity();
  storeWaterIntensity();
  printAdjustStatus(true);
  printMeasurement();
}

void onResetWater() {
  waterIntensity = DEFAULT_WATER_INTENSITY;
#ifdef DEBUG_SERIAL
  Serial.println("reset water");
#endif
  clearWaterIntensity();
  printAdjustStatus(false);
  printMeasurement();
}

void onMeasureBeer() {
#ifdef DEBUG_SERIAL
  Serial.println("measure beer");
#endif  
  beerIntensity = measureIntensity();
  printMeasurement();
}

void onChangeUnit() {
  isUnitEBC = !isUnitEBC;
#ifdef DEBUG_SERIAL
  Serial.print("change unit: ");
  if (isUnitEBC) {
    Serial.println("EBC");
  } else {
    Serial.println("SRM");
  }
#endif
  storeUnit();
  printUnit();
  printMeasurement();
}

void setup() {
#ifdef DEBUG_SERIAL
  Serial.begin(9600);
#endif
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(VERSION_DEV);
  lcd.setCursor(0, 1);
  lcd.print("SW: v");
  lcd.print(VERSION_SW);
  led430NM.init();
  btnMeasureWater.begin();
  btnMeasureWater.onPressed(onMeasureWater);
  btnMeasureWater.onPressedFor(BTN_2ND_OP_DELAY, onResetWater);
  btnMeasureBeer.begin();
  btnMeasureBeer.onPressed(onMeasureBeer);
  btnMeasureBeer.onPressedFor(BTN_2ND_OP_DELAY, onChangeUnit);
  delay(3000);
  lcd.clear();
  restoreWaterIntensity();
  restoreUnit();
}

void loop() {
  btnMeasureWater.read();
  btnMeasureBeer.read();
}
