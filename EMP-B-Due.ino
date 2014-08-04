/*
 EMP-B Due
 V0.1 © 03/08/2014 William Koch

 Controlador de Empacotadeira com Arduino Due.
*/

// Constantes
  // Entradas
    const byte bt_geral = 22; // Entrada do botão de Liga/Desliga Geral
    const byte bt_dosador = 26; // Entrada do botão de Liga/Desliga do Dosador
  // Saídas
    const byte led_geral = 25; // Saída Geral
    const byte led_dosador = 29; // Saída do Dosador


// Variáveis
  // Botões de Entrada Um Clique
    long atraso = 50;
    int est_bt_geral;
    int est_bt_geral_ant = HIGH;
    long atr_geral_ant = 0;
    int est_bt_dosador;
    int est_bt_dosador_ant = HIGH;
    long atr_dosador = 0;
  // Estado Inicial de Saídas
    int est_led_geral = LOW;
    int est_led_dosador = LOW;


void setup() {
  pinMode(bt_geral, INPUT_PULLUP);
  pinMode(bt_dosador, INPUT_PULLUP);

  pinMode(led_geral, OUTPUT);
  pinMode(led_dosador, OUTPUT);

  // Define estado inicial das saídas
  digitalWrite(led_geral, est_led_geral);
  digitalWrite(led_dosador, est_led_dosador);
}


void loop() {
  botaoUmClique(bt_geral, &est_bt_geral, &est_bt_geral_ant, &atr_geral_ant, led_geral, &est_led_geral);
  botaoUmClique(bt_dosador, &est_bt_dosador, &est_bt_dosador_ant, &atr_dosador, led_dosador, &est_led_dosador);
}


void botaoUmClique(byte botao, int *estado, int *est_ant, long *atr_ant, byte saida, int *est_saida ){
  int reading = digitalRead(botao);
  if (reading != *est_ant) {
    *atr_ant = millis();
  }

  if ((millis() - *atr_ant) > atraso) {
    if (reading != *estado) {
      *estado = reading;
      if (*estado == LOW) {
        *est_saida = !*est_saida;
      }
    }
  }
  digitalWrite(saida, *est_saida);
  *est_ant = reading;
}
