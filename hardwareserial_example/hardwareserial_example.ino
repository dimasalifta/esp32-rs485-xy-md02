#include <ModbusMaster.h>

#define MAX485_RO      26
#define MAX485_DI      25
#define MAX485_DE      32
#define MAX485_RE  33

HardwareSerial MAX485(1);  // Gunakan UART untuk MAX485

ModbusMaster XYMD02;

unsigned long previousMicros = 0;  // Waktu terakhir sensor dibaca
const unsigned long interval = 1500000;  // Interval pembacaan dalam mikrodetik (1.5 detik)
const unsigned long delay_transmission = 5000;
void preTransmission() {
  digitalWrite(MAX485_RE, 1);
  digitalWrite(MAX485_DE, 1);
  delayMicroseconds(delay_transmission);  // Kurangi delay untuk respons lebih cepat
}

void postTransmission() {
  delayMicroseconds(delay_transmission);
  digitalWrite(MAX485_RE, 0);
  digitalWrite(MAX485_DE, 0);
}

void setup() {
  Serial.begin(115200);
  MAX485.begin(9600, SERIAL_8N1, MAX485_RO, MAX485_DI); // Baud rate sesuai sensor

  pinMode(MAX485_RE, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE, 0);
  digitalWrite(MAX485_DE, 0);

  XYMD02.begin(1, MAX485); // Slave ID sensor (default 1)
  XYMD02.preTransmission(preTransmission);
  XYMD02.postTransmission(postTransmission);
}

void loop() {
  unsigned long currentMicros = micros();

  if (currentMicros - previousMicros >= interval) {
    previousMicros = currentMicros;  // Update waktu pembacaan terakhir

    uint8_t result_XYMD02;
    float temperature, humidity;

    // 🔴 Baca sensor XYMD02 dari register 0x0000
    result_XYMD02 = XYMD02.readInputRegisters(0x0000, 2);
    if (result_XYMD02 == XYMD02.ku8MBSuccess) {
      int16_t rawTemp = XYMD02.getResponseBuffer(0);
      temperature = rawTemp / 10.0;  // Konversi sesuai spesifikasi sensor
      int16_t rawHumidity = XYMD02.getResponseBuffer(1);
      humidity = rawHumidity / 10.0; // Konversi sesuai spesifikasi sensor

      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.print(" °C\t");
      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.println(" %RH");
    } else {
      Serial.print("Sensor XYMD02 Read Error: 0x");
      Serial.println(result_XYMD02, HEX);
    }
  }
}
