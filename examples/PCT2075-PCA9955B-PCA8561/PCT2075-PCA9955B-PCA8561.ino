
#include <PCT2075.h>
#include <PCA8561.h>
#include <PCA9955B.h>
#include <LED.h>

PCT2075 sensor;
PCA8561 lcd;
PCA9955B ledd;

LED rgb[3][3] = {
  { LED(&ledd, 0), LED(&ledd, 3), LED(&ledd, 6) },
  { LED(&ledd, 1), LED(&ledd, 4), LED(&ledd, 7) },
  { LED(&ledd, 2), LED(&ledd, 5), LED(&ledd, 8) }
};

const uint8_t interruptPin = 2;
const uint8_t heaterPin = 3;
bool heater = true;
bool int_flag = false;

float temp_diff = 1.0;
float temp_high;
float temp_low;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  lcd.begin();  // This is necessary to enable display; device goes into power-on mode
  ledd.begin(0.1, PCA9955B::ARDUINO_SHIELD);

  pinMode(heaterPin, OUTPUT);

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), callback, FALLING);

  float temp = sensor.temp();

  temp_high = temp + 1 + temp_diff;
  temp_low = temp + 1;

  sensor.thresholds(temp + 1, temp + 2);
  sensor.os_mode(PCT2075::INTERRUPT);

  Serial.println("\n***** Hello, PCT2075! *****");
  Serial.print("Temperature at start: ");
  Serial.println(temp);

  Serial.print("Setting: Tos = ");
  Serial.print(temp + 2, 3);
  Serial.print(" / Thyst = ");
  Serial.println(temp + 1, 3);

  digitalWrite(heaterPin, heater);

  for (int i = 0; i < 100; i++) {
    led_color(i / 100.0);
    delay(10);
  }
}

void callback() {
  int_flag = true;
}

void loop() {
  static char s[6] = "****\n";

  Serial.print("heater: ");
  Serial.print(heater ? "ON   " : "OFF  ");

  float temp = sensor.temp();

  Serial.println(temp, 3);
  lcd.puts(dtostrf(temp, 4, 1, s));
  led_color(temp - temp_low);

  if (int_flag) {
    int_flag = false;
    heater = !heater;
    digitalWrite(heaterPin, heater);
  }
  delay(1000);
}

void led_color(float scale) {
  float lumG;
  float lumRB;

  scale = (scale < 0.0) ? 0.0 : scale;
  scale = (1.0 < scale) ? 1.0 : scale;

  lumG = pow(sin(PI * scale), 2);
  lumRB = pow(sin(PI * (scale + 0.5)), 2);
  rgb[1][0] = rgb[1][1] = rgb[1][2] = lumG;

  if (scale < 0.5) {
    rgb[0][0] = rgb[0][1] = rgb[0][2] = 0.0;
    rgb[2][0] = rgb[2][1] = rgb[2][2] = lumRB;
  } else {
    rgb[0][0] = rgb[0][1] = rgb[0][2] = lumRB;
    rgb[2][0] = rgb[2][1] = rgb[2][2] = 0.0;
  }
}
