// Programa: Ampliando as portas analógicas do NodeMCU com multiplexador
// Autor : Leandro Chernicharo (p/ FILIPEFLOP.COM)
// Adaptação : UFF / TER / Larissa / Ivanovich
 
/*
 * Constante com o número de pinos (bits) do multiplexador
 * que serão utilizados para controlar a porta selecionada.
 * Caso você use um multiplexador diferente do que eu usei,
 * como o 74HC4051, por exemplo, que usa 3 bits para isso,
 * basta alterar aqui.
*/

/**********ATENÇÃO********************************/
/* O PINO B do multiplexor é o mais significativo */
#define NUM_PINOS_SELECAO_CANAL 2
 
/*
 * Pinos dos sensores analógicos. Os números representam as
 * portas dentro do multiplexador. Neste caso aqui, seria
 * o seguinte:
 *  Y0_PIN está na porta 0 de um dos canais (X0 ou Y0)
 *  Y1_PIN está na porta 1 de um dos canais (X1 ou Y1)
 *  Y2_PIN está na porta 2 de um dos canais (X2 ou Y2)
 * Estas constantes são utilizadas para controlar a abertura
 * correspondente antes da leitura dos dados do sensor.
 */
#define Y0_PIN 0
#define Y1_PIN 1
#define Y2_PIN 2
 
/*
 * Vetor com os pinos de seleção de canal no multiplexador.
 * Cada porta no vetor representa o pino ao qual está conectada:
 * D3 -> Pino B
 * D4 -> Pino A
 */
const int pinosSelecaoCanal[] = {D3, D4};
 
void setup() {
  Serial.begin(9600);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  //initPinosSelecaoCanal();
}
 
void loop() {
  /*
   * Antes da leitura dos dados na porta analógica (A0) do
   * NodeMCU, é preciso definir qual porta do multiplexador
   * será lida. Assim, sempre chamamos a função correspondente
   * para realizar essa tarefa para nós.
   */
  ativarPortaAnalogica(Y0_PIN);
  int UMID1 = analogRead(A0);
  delay(500);
 
  /*
   * Observe que a leitura é feita sempre na mesma porta
   * física do NodeMCU. O que vai mudar é a origem do sinal
   * a partir do multiplexador.
   */
  ativarPortaAnalogica(Y1_PIN);
  int UMID2 = analogRead(A0);
  delay(500);

  ativarPortaAnalogica(Y2_PIN);
  int UMID3 = analogRead(A0);
 
  Serial.print("Y0: ");
  Serial.print(UMID1);
  Serial.print("\tY1: ");
  Serial.print(UMID2);
  Serial.print("\tY2: ");
  Serial.println(UMID3);
 
  delay(100);
}
 
/*
 * Esta função abre a porta do multiplexador que desejamos
 * utilizar a partir do número do pino passado para ela. É
 * por isso que definimos as constantes dessas portas no
 * início do programa.
 */
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
 
/*
 * Esta função é muito importante. Ela coloca todas as portas
 * digitais do NodeMCU utilizadas para controlar os pinos
 * (A e B, neste caso) em modo de saída (OUTPUT). Dependendo
 * da porta que você escolher, seu comportamento padrão pode
 * ser o de entrada (INPUT), o que fará com que o programa não
 * funcione corretamente. Assim, para garantir o funcionamento
 * independente da porta, sempre chame esta função no setup.
 */
void initPinosSelecaoCanal() {
  for(int i = 0; i < NUM_PINOS_SELECAO_CANAL; i++) {
    pinMode(pinosSelecaoCanal[i], OUTPUT);
  }
}
