/*  Obtenci�n del angulo del MPU6050 aplicando un flitro complementario
    Autor: Javier Vargas
    Basado en Robologs --> https://robologs.net/
    https://creativecommons.org/licenses/by/4.0/
*/

#include "MPU6050_CompFilter.h"

//Constantes a dividir para obtener un valor coherente
#define A_R 16384.0  //Aceleracion
#define G_R 131.0    //Giroscopo

//Conversion de radianes a grados 180/PI
#define RAD_A_DEG = 57.295779

MPU6050_CompFilter::MPU6050_CompFilter(int direccion) {
MPU = direccion;
AcX, AcY, AcZ, GyX, GyY, GyZ = 0;
Angle[0], Angle[1] = 0;
VelAngle[0], VelAngle[1] = 0;
AngleAnt[0], AngleAnt[1] = 0;
t_m = 0;
K = 0.98;
};

void MPU6050_CompFilter::Iniciar(float t_muestreo){
  t_m = t_muestreo;
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}

float MPU6050_CompFilter::Lectura(boolean leerX, boolean leerY){
  //Leer los valores del Acelerometro de la IMU
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); //Pedir el registro 0x3B - corresponde al AcX
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); //A partir del 0x3B, se piden 6 registros
  AcX = Wire.read() << 8 | Wire.read(); //Cada valor ocupa 2 registros
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  //Leer los valores del Giroscopio
  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 4, true); //A diferencia del Acelerometro, solo se piden 4 registros
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();

  if (leerX) {
    //C�lculo del �ngulo a partir del aceler�metro
    Acc[0] = atan((AcY / A_R) / sqrt(pow((AcX / A_R), 2) + pow((AcZ / A_R), 2))) * RAD_TO_DEG;
    //C�lculo del �ngulo a partir del gir�scopo
    Gy[0] = GyX / G_R;
    //Valor anterior del �ngulo guardado
    AngleAnt[0] = Angle[0];
    //Aplicaci�n del flitro complementario
    Angle[0] = K * (Angle[0] + Gy[0] * t_m) + (1-K) * Acc[0];
    //Obtenci�n de la velocidad angular (�/s)
    VelAngle[0] = (Angle[0] - AngleAnt[0]) / t_m;
  }

  if (leerY) {
    //C�lculo del �ngulo a partir del aceler�metro
    Acc[1] = atan(-1 * (AcX / A_R) / sqrt(pow((AcY / A_R), 2) + pow((AcZ / A_R), 2))) * RAD_TO_DEG;
    //C�lculo del �ngulo a partir del gir�scopo
    Gy[1] = GyY / G_R;
    //Valor anterior del �ngulo guardado
    AngleAnt[1] = Angle[1];
    //Aplicaci�n del flitro complementario
    Angle[1] = K * (Angle[1] + Gy[1] * t_m) + (1-K) * Acc[1];
    //Obtenci�n de la velocidad angular (�/s)
    VelAngle[1] = (Angle[1] - AngleAnt[1]) / t_m;
  }
}

float MPU6050_CompFilter::angX(){
return Angle[0];
}

float MPU6050_CompFilter::angY(){
return Angle[1];
}

float MPU6050_CompFilter::VelAngX(){
return VelAngle[0];
}

float MPU6050_CompFilter::VelAngY(){
return VelAngle[1];
}

float MPU6050_CompFilter::Xgyro(){
return Gy[0];
}

float MPU6050_CompFilter::Ygyro(){
return Gy[1];
}

float MPU6050_CompFilter::Xacc(){
return Acc[0];
}

float MPU6050_CompFilter::Yacc(){
return Acc[1];
}


void MPU6050_CompFilter::setKcompFilter(float K_in){
    K = K_in;
}
