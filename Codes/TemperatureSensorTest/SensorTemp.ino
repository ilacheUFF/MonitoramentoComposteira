// Programa : Sensor de temperatura DS18B20
// Autor : FILIPEFLOP
// Adaptação : UFF / TER / Larissa / Ivanovich
// Biblioteca OneWire baixada diretamente do arduino
//Biblioteca DAllas, baixada do site FilipeFlop
//
 
#include <OneWire.h>
#include <DallasTemperature.h>

// Porta do pino de sinal do DS18B20
#define ONE_WIRE_BUS D2
 
// Define uma instancia do oneWire para comunicacao com o sensor
OneWire oneWire(ONE_WIRE_BUS);
 
// Armazena temperaturas minima e maxima
float tempMin = 999;
float tempMax = 0;
 
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1,sensor2,sensor3;
bool primeira = false;
 
void setup(void)
{
  Serial.begin(9600);
  sensors.begin();
  // Localiza e mostra enderecos dos sensores
  
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  
   
}
 
void mostra_endereco_sensor(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // Adiciona zeros se necessário
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
 
void loop()
{

  if (primeira == false)
  {
  Serial.println("Localizando sensores DS18B20...");
  Serial.print("Foram encontrados ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" sensores.");
  if (!sensors.getAddress(sensor1, 0)) 
     Serial.println("Sensor1 nao encontrado !"); 
  if (!sensors.getAddress(sensor2, 1)) 
     Serial.println("Sensor2 nao encontrado !");
  if (!sensors.getAddress(sensor3, 2)) 
     Serial.println("Sensor2 nao encontrado !"); 
  // Mostra o endereco do sensor encontrado no barramento
  Serial.print("Endereco sensor1: ");
  mostra_endereco_sensor(sensor1);
  Serial.print("Endereco sensor2: ");
  mostra_endereco_sensor(sensor2);
  Serial.print("Endereco sensor2: ");
  mostra_endereco_sensor(sensor2);
  Serial.println();
  Serial.println();
  primeira = true;
    
  }
  // Le a informacao do sensor
  sensors.requestTemperatures();
  float tempC1 = sensors.getTempC(sensor1);
  float tempC2 = sensors.getTempC(sensor2);
  float tempC3 = sensors.getTempC(sensor3);

  // Mostra dados no serial monitor
  Serial.print("Temp 1: ");
  Serial.print(tempC1);
  Serial.print("Temp 2: ");
  Serial.println(tempC2);
  Serial.print("Temp 3: ");
  Serial.println(tempC3);
  
  digitalWrite(LED_BUILTIN,LOW);
  delay(1500);
  digitalWrite(LED_BUILTIN,HIGH);
  delay(1500);
}
//FIM
