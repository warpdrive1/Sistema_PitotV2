#include <Wire.h>
#include "Adafruit_MPRLS.h"

#define TCA9548A 0x70

#define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
#define EOC_PIN    -1  // set to any GPIO pin to read end-of-conversion by 

#define MASA_MOLECULAR_AIRE   28.9645 // g/mol
#define ALTURA_BASE               0.0 // m
#define TEMPERATURA_BASE       288.15 // K
#define PRESION_BASE           101325 // Pa
#define RG                        287 // m2/s2·K
#define R                     0.08206 //L·atm/mol
#define GAMMA                     1.4 //
#define GRADIENTE_T            0.0065 // K/m
#define GRAVEDAD              9.80665 // m/s2

#define PIN_LM35 A0

float presion_estatica;
float presion_remanso;
float presion_referencia;
float temperatura_remanso;
float altura;
float altitud;
float densidad_aire;
float velocidad_incompresible;
float velocidad_compresible;


Adafruit_MPRLS mpr = Adafruit_MPRLS(RESET_PIN, EOC_PIN);

void TCA9548A_select(uint8_t bus)
{
  Wire.beginTransmission(TCA9548A);  // Función que nos permitirá seleccionar 
  Wire.write(1 << bus);              // el canal I2C del multiplexor
  Wire.endTransmission();
}

void setup() 
{
  
   Wire.begin();
   Serial.begin(115200);
   delay(1000);
   
   Serial.println("------ Prueba sistema Pitot ------ ");
   Serial.println("");
   Serial.println("-----------------------------------");


// Iniciamos el MPLRS del canal 2
TCA9548A_select(2);

  if (! mpr.begin()) 
  {
    Serial.println("Fallo al conectar con el sensor MPRLS (2)");
    while (1) 
    {
      delay(10);
    }
  }

// Iniciamos el MPLRS del canal 7
TCA9548A_select(7);

  if (! mpr.begin()) 
  {
    Serial.println("Fallo al conectar con el sensor MPRLS (7)");
    while (1) 
    {
      delay(10);
    }
  }
delay(1000);




// Cálculo de la presión de referencia ara determinar la altura

// Datos del MPLRS del canal (2)
   TCA9548A_select(2); // Seleccionamos canal (2) del multiplexor
   
    for (uint8_t i = 0; i<50 ; i++)
  {
   presion_referencia += mpr.readPressure(); 
  }
   presion_referencia = presion_referencia/50; // Presión utilizada para calcular altura teniendo en cuenta atmósfera ISA
  
  Serial.print("La presión de referencia es: ");
  Serial.print(presion_referencia);
  Serial.println(" hPa");

  altitud = 44330 * (1.0 - pow((presion_referencia*100)/PRESION_BASE, 0.1903));
  
  Serial.print("La altitud del terreno es: ");
  Serial.print(altitud);
  Serial.println(" m");
  Serial.println("");
  Serial.println("");
  Serial.println("----------------------------------------------------");
  Serial.println("  ");
  Serial.println("P_MPRLS(2) P_MPRLS(7)  T_LM35   Altura   Densidad_aire V_incomp  V_comp ");
  Serial.println("   Pa         Pa         ºC       m          kg/m3        m/s      m/s  ");
  Serial.println("-------------------------------------------------------------------------------------------------------------------------------------");
  delay(1000);
}



void loop() 
{
  
// Datos del MPLRS del canal (2)
   TCA9548A_select(2); // Seleccionamos canal (2) del multiplexor
   presion_remanso = mpr.readPressure()*100; // En Pa

// Datos del MPLRS del canal (7)
   TCA9548A_select(7); // Seleccionamos canal (7) del multiplexor
   presion_estatica = mpr.readPressure()*100;  // En Pa
      
// Datos de LM35
   temperatura_remanso = analogRead(PIN_LM35)*0.488759;
   temperatura_remanso = 15;
   
   altura = 44330 * (1.0 - pow(presion_estatica/PRESION_BASE, 0.1903)) - altitud; // Sacamos altura con la referencia de presiones en hPa, como se utiliza la presión
                                                                                  // de referencia a elevación cero, nos da alturas, si no, nos daria altitud
   densidad_aire = ((presion_estatica/101325) * MASA_MOLECULAR_AIRE)/(R * (temperatura_remanso+273));                                                                 

// Cálculo velocidad incompresible
/*if (presion_estatica <= presion_remanso)
 {
  velocidad_incompresible = 0;
 } else
  {*/
    velocidad_incompresible = sqrt((2*((presion_estatica)-(presion_remanso)))/densidad_aire);
  //}
  
 
// Cálculo velocidad compresible
 velocidad_compresible = sqrt(((2*GAMMA)/(GAMMA-1))*(RG*(temperatura_remanso+273))*pow((1-(presion_estatica/presion_remanso)), ((GAMMA-1)/(GAMMA))));

 
// Datos del sistema Pitot


  Serial.print(presion_estatica);
  Serial.print(",");
  Serial.print(presion_remanso);
  Serial.print(",");
  Serial.print(temperatura_remanso);
  Serial.print(",");
  Serial.print(altura);
  Serial.print(",");
  Serial.print(densidad_aire);
  Serial.print(",");
  Serial.print(velocidad_incompresible);
  Serial.print(",");
  Serial.print(velocidad_compresible);
  Serial.println(",");
  
  delay(1000);
   
}
