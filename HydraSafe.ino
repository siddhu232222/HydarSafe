#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define RPWM  7
#define LPWM  8

#define DHTPIN 2
#define DHTTYPE DHT11
#define ONE_WIRE_BUS 4

DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float roomTemp, waterTemp;
float target = 22.0;   // default

void setup() {
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);

  dht.begin();
  sensors.begin();

  Serial.begin(9600);
  Serial.println("=======================================");
  Serial.println("   <20°C → HOT (40°C)                  ");
  Serial.println("   >30°C → COLD (10°C)                 ");
  Serial.println("   20-30°C → NORMAL (22°C)             ");
  Serial.println("=======================================");
}

void loop() {
  roomTemp = dht.readTemperature();
  if (isnan(roomTemp)) roomTemp = 25.0;

  // YOUR EXACT LOGIC
  if (roomTemp < 20.0)      target = 40.0;   // Cold room → HOT water
  else if (roomTemp > 30.0) target = 10.0;   // Hot room → ICE COLD water
  else                      target = 22.0;   // Normal → perfect 22°C

  sensors.requestTemperatures();
  waterTemp = sensors.getTempCByIndex(0);
  if (waterTemp < -50) waterTemp = 25.0;

  Serial.print("Room: "); Serial.print(roomTemp,1);
  Serial.print("°C → Target: "); Serial.print(target,1);
  Serial.print("°C → Water: "); Serial.print(waterTemp,1); Serial.print("°C → ");

  // Control with ±3°C hysteresis
  if (waterTemp < target - 3) { heatFull(); Serial.println("HEATING FULL"); }
  else if (waterTemp > target + 3) { coolFull(); Serial.println("COOLING FULL"); }
  else { peltierOff(); Serial.println("PERFECT — OFF"); }

  delay(3000);
}

void coolFull() { analogWrite(LPWM, 255); analogWrite(RPWM, 0); }
void heatFull() { analogWrite(RPWM, 255); analogWrite(LPWM, 0); }
void peltierOff() { analogWrite(RPWM, 0); analogWrite(LPWM, 0); }