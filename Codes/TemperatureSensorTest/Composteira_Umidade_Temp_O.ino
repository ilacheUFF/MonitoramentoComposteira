/* Programa básico para Monitoramento de Compostagem */

#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <ESP8266WiFi.h>

/* defines - wi-fi */
#define SSID_REDE "Ixquina" /* coloque aqui o nome da rede que se deseja conectar */
#define SENHA_REDE "sucodeacerola" /* coloque aqui a senha da rede que se deseja conectar */


#define INTERVALO_ENVIO_THINGSPEAK 6000 /* intervalo entre envios de dados ao ThingSpeak (em ms) */
const unsigned long timeGetInfo =300000;

/* constantes e variáveis globais */
char endereco_api_thingspeak[] = "api.thingspeak.com";
String chave_escrita_thingspeak = "EXSIIV72QA9QKJHK";  /* Coloque aqui sua chave de escrita do seu canal */
unsigned long last_connection_time;

long lastConnectionTime; 
WiFiClient client;

 
#define DHTPIN D1 // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);
// These constants won't change. They're used to give names to the pins used:

#define NUM_PINOS_SELECAO_CANAL 2
#define Y0_PIN 0
#define Y1_PIN 1
#define Y2_PIN 2

const int pinosSelecaoCanal[] = {D3, D4};  // Analog input pin that the potentiometer is attached to

int UMID1 = 0;        // value read from the pot
int UMID2 = 0;
int UMID3 = 0;
float tempC1;
float tempC2;
float tempC3; //Valor da temperatura
float temp_ar=0;    //Valor da temperatura
float U_ar=0;    //Valor da umidade
//Pino de conexao do sensor de temperatura
#define ONE_WIRE_BUS D2

//Cria uma instancia OneWire para comunicacao com dispositivos no barramento
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//Array para armazenar os enderecos dos sensores
DeviceAddress sensorTemp1,sensorTemp2,sensorTemp3;
bool primeira = false;

char fields_a_serem_enviados[200] = {0};

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
/* SENSOR UMIDADE INTERNO*/  
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);;
/* SENSOR DE TEMPERATURA INTERNO*/
  if (primeira == false)
  {
  sensors.begin();  //Inicio de sensor DS18B20
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  // Localiza e mostra enderecos dos sensores
  Serial.println("Localizando sensores DS18B20...");
  Serial.print("Foram encontrados ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" sensores.");
   if (!sensors.getAddress(sensorTemp1, 0)) 
     Serial.println("SensorTemp1 nao encontrado !"); 
  if (!sensors.getAddress(sensorTemp2, 1)) 
     Serial.println("SensorTemp2 nao encontrado !");
  if (!sensors.getAddress(sensorTemp3, 2)) 
     Serial.println("SensorTemp3 nao encontrado !"); 
  // Mostra o endereco do sensor encontrado no barramento
  Serial.print("Endereco sensorTemp1: ");
  mostra_endereco_sensor(sensorTemp1);
  Serial.print("Endereco sensorTemp2: ");
  mostra_endereco_sensor(sensorTemp2);
  Serial.print("Endereco sensorTemp3: ");
  mostra_endereco_sensor(sensorTemp3);
  Serial.println();
  Serial.println();
  primeira = true;

  }
/* SENSOR DE TEMPERATURA E UMIDADE EXTERNO*/
  dht.begin();
  
/* Inicializa sensor de temperatura e umidade relativa do ar */
/* Inicializa e conecta-se ao wi-fi */
  last_connection_time = 0;    
  init_wifi();
  pinMode(LED_BUILTIN, OUTPUT);
 
}

void loop() {
  
  lerTempUmExt(); //Le a temperatura e umidade do ar
  lerUmidade();  // Le a umidade com o sensor capacitivo
  lerTemp();   // Le a temperatura dentro da composteira
  printarInfo(); //Mostra as informações no Serial
  
/*Se conectar ao WIFI do local*/  
  if (client.connected())
    {
        client.stop();
        Serial.println("- Desconectado do ThingSpeak");
        Serial.println();
    }
 
    /* Garante que a conexão wi-fi esteja ativa */
    verifica_conexao_wifi();
/*Envia os dados ao serviço do Thingspeak*/
   sprintf(fields_a_serem_enviados,"field1=%.2f&field2=%.2f&field3=%.2f&field4=%.2f&field5=%.2f&field6=%d&field7=%d&field8=%d", U_ar,temp_ar,tempC1,tempC2,tempC3,UMID1,UMID2,UMID3);
   envia_informacoes_thingspeak(fields_a_serem_enviados);

/* Aguardar 10 segundos antes de iniciar o processo */
  delay(10000);
}

/*FUNÇÕES DO PROGRAMA*/

void lerTempUmExt()
{
  U_ar = dht.readHumidity();
  temp_ar = dht.readTemperature();
}
void lerTemp()
{
  sensors.requestTemperatures();
  float tempC1 = sensors.getTempC(sensorTemp1);
  float tempC2 = sensors.getTempC(sensorTemp2);
  float tempC3 = sensors.getTempC(sensorTemp3);
}
void lerUmidade() {
ativarPortaAnalogica(Y0_PIN);
int UMID1 = analogRead(A0);
delay(500);
  
ativarPortaAnalogica(Y1_PIN);
int UMID2 = analogRead(A0);
delay(500);

ativarPortaAnalogica(Y2_PIN);
int UMID3 = analogRead(A0);
}

void printarInfo()
{
  Serial.print("T_ar= ");
  Serial.print(temp_ar);
  Serial.print(" | U_ar= ");
  Serial.print(U_ar);
  Serial.print(" | Temp1_in= ");
  Serial.print(tempC1);
  Serial.print(" | Temp2_in= ");
  Serial.println(tempC2);
  Serial.print(" | Temp3_in= ");
  Serial.println(tempC3);
  Serial.print(" | U1_in= ");
  Serial.print(UMID1);
  Serial.print(" | U2_in= ");
  Serial.print(UMID2);
  Serial.print(" | U_in3= ");
  Serial.println(UMID3);

    
  digitalWrite(LED_BUILTIN,LOW);
  delay(1500);
  digitalWrite(LED_BUILTIN,HIGH);
  delay(1500);
}

void ativarPortaAnalogica(int porta) {
/*
   * De acordo com a tabela de portas do 74HC4052,
   * a porta 2 (som) é aberta com o pino A LOW e o pino
   * B HIGH. A porta 3 (luz) é aberta com os A e B HIGH.
   */
  if(porta == Y0_PIN) {
      digitalWrite(pinosSelecaoCanal[0], LOW);
      digitalWrite(pinosSelecaoCanal[1], LOW);
  }

  if(porta == Y1_PIN){
      digitalWrite(pinosSelecaoCanal[0], LOW);
      digitalWrite(pinosSelecaoCanal[1], HIGH);
  }
  
  if(porta == Y2_PIN){
      digitalWrite(pinosSelecaoCanal[0], HIGH);
      digitalWrite(pinosSelecaoCanal[1], LOW);
  }
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

void envia_informacoes_thingspeak(String string_dados)
{
    if (client.connect(endereco_api_thingspeak, 80))
    {
        /* faz a requisição HTTP ao ThingSpeak */
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+chave_escrita_thingspeak+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(string_dados.length());
        client.print("\n\n");
        client.print(string_dados);
         
        last_connection_time = millis();
        Serial.println("- Informações enviadas ao ThingSpeak!");
      
    }
}
void init_wifi(void)
{
    Serial.println("------WI-FI -----");
    Serial.println("Conectando-se a rede: ");
    Serial.println(SSID_REDE);
    Serial.println("\nAguarde...");
 
    conecta_wifi();
}
 
/* Função: conecta-se a rede wi-fi
* Parametros: nenhum
* Retorno: nenhum
*/
void conecta_wifi(void)
{
    /* Se ja estiver conectado, nada é feito. */
    if (WiFi.status() == WL_CONNECTED)
    {
        return;
    }
     
    /* refaz a conexão */
    WiFi.begin(SSID_REDE, SENHA_REDE);
     
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
         delay(1000);                       // wait for a second
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
        delay(1000);   
    }
 
    Serial.println("Conectado com sucesso a rede wi-fi \n");
    Serial.println(SSID_REDE);

 
}
 
/* Função: verifica se a conexao wi-fi está ativa
* (e, em caso negativo, refaz a conexao)
* Parametros: nenhum
* Retorno: nenhum
*/
void verifica_conexao_wifi(void)
{
    conecta_wifi();
}
