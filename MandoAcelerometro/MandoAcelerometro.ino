
//CONFIGURACION
#define Muestreo 50 //ms
#define OffsetX 0
#define OffsetY 0
//#define Debug

//MPU6050
#include <MPU6050_CompFilter.h>
MPU6050_CompFilter mpu(0x68);

//NRF24L01
byte direccion[5] = {'h', 'o', 'r', 'm', 'i'}; //Variable con la dirección del canal por donde se va a transmitir
#define CE_PIN 9
#define CSN_PIN 10
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(CE_PIN, CSN_PIN);
float angulos[2];

unsigned long millisactual;

void setup() {
  
#ifdef Debug
  Serial.begin(115200);
#endif

  //Iniciamos IMU
  mpu.Iniciar((float)Muestreo / 1000);
  mpu.setKcompFilter(0.98);

  //inicializamos el NRF24L01
  radio.begin();
  radio.openWritingPipe(direccion);

}

void loop() {
  if (millis() / Muestreo != millisactual) {
    millisactual = millis() / Muestreo;

    //Lectura del angulo
    mpu.Lectura(1, 1);
    angulos[0] = mpu.angX() + OffsetX;
    angulos[1] = mpu.angY() + OffsetY;

    //Envio por radiofrecuencia
    radio.write(angulos, sizeof(angulos));

#ifdef Debug
    Serial.print(angulos[0]);
    Serial.print(" ");
    Serial.println(angulos[1]);
#endif

  }

}
