/* Labrinto de bolas montado sobre una superficie con dos ejes movidos por dos servomotores que replican el ángulo obtenido desde un acelerómetro por radiofrecuencia.
   Autor: Javier Vargas. El Hormiguero.
   https://creativecommons.org/licenses/by/4.0/
*/
//PINES
#define pinServoX 5
#define pinServoY 4
#define pinBoton 15

//CONFIGURACION
#define OffsetX -2 //Offset
#define OffsetY 15
#define MaxX 25 //Angulo máximo permitido
#define MaxY 25 
#define FactorAng 0.3f //Relacion entre el angulo del acelerometro y el servo
#define PulsoMax 2193 //PWM a angulo max
#define PulsoMin 771 //PWM a angulo min
#define AngMax 120 //Angulo maximo del servo
//#define Debug

//NRF24L01
byte direccion[5] = {'h', 'o', 'r', 'm', 'i'}; //Variable con la dirección del canal por donde se va a transmitir
#define CE_PIN 10
#define CSN_PIN 9
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(CE_PIN, CSN_PIN);
float angulos[2];

//SERVOS
#include <Servo.h>
Servo servo[2];

unsigned long millisactual;

void setup() {

#ifdef Debug
  Serial.begin(115200);
#endif

  //Interruptor
  pinMode(14, OUTPUT);
  pinMode(pinBoton, INPUT_PULLUP);
  digitalWrite(14, LOW); //Pin A0 como gnd

  //Inicializar servos
  servo[0].attach(pinServoX);
  servo[1].attach(pinServoY);

  MoverServo(0, AngMax / 2 + OffsetX);
  MoverServo(1, AngMax / 2 + OffsetY);

  //inicializamos el NRF24L01
  radio.begin();
  radio.openReadingPipe(1, direccion);
  radio.startListening();   //empezamos a escuchar por el canal

}

/////////////////////////////////
//////////////LOOP///////////////
/////////////////////////////////

void loop() {

  //Palanca activada
  if (!digitalRead(pinBoton)) {

    //  Lectura de datos recibidos por RF
    if (radio.available()) {
      radio.read(angulos, sizeof(angulos));

      //Escalado y limite del angulo
      angulos[0] = constrain((float)FactorAng * angulos[0], -MaxX, MaxX);
      angulos[1] = constrain((float)FactorAng * angulos[1], -MaxY, MaxY);

      //Mover teblero
      MoverServo(0, (float)AngMax / 2 - angulos[0] + OffsetX);
      MoverServo(1, (float)AngMax / 2 + angulos[1] + OffsetY);

#ifdef Debug
      Serial.print(angulos[0]);
      Serial.print(" ");
      Serial.println(angulos[1]);
#endif
    }
  }

  //Palanca desactivada
  else {
    MoverServo(0, (float)AngMax / 2  + OffsetX);
    MoverServo(1, (float)AngMax / 2  + OffsetY);
  }
}

/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

void MoverServo(int d, float angulo) {
  //Calculo del ancho de pulso en us para tener mayor precision
  int t = (float)angulo / AngMax * (PulsoMax - PulsoMin) + PulsoMin;
  servo[d].writeMicroseconds(t);
}

