/*
 EMP-B Due
 V0.1 © 03/08/2014 William Koch

 Controlador de Empacotadeira com Arduino Due.
*/

// Constantes
  // Entradas
    const byte bt_geral = 22; // Entrada do botão de Liga/Desliga Geral
    const byte bt_dosador = 26; // Entrada do botão de Liga/Desliga do Dosador
    const byte bt_datador = 30; // Entrada do botão de Liga/Desliga do Datador
  // Saídas
    const byte led_geral = 25; // Saída Geral
    const byte led_dosador = 29; // Saída do Dosador
    const byte led_datador = 33; // Saída do Datador


// Variáveis
  // Botões de Entrada Um Clique
    long atraso = 50;
    int estbt_geral;
    int estbta_geral = HIGH;
    long atr_geral = 0;
    int estbt_dosador;
    int estbta_dosador = HIGH;
    long atr_dosador = 0;
    int estbt_datador;
    int estbta_datador = HIGH;
    long atr_datador = 0;
  // Estado Inicial de Saídas
    int est_led_geral = LOW;
    int est_led_dosador = LOW;
    int est_led_datador = LOW;
  // Programa
    boolean stand_by = false;
    boolean maquina_ligada = false;


void setup() {
  pinMode(bt_geral, INPUT_PULLUP);
  pinMode(bt_dosador, INPUT_PULLUP);
  pinMode(bt_datador, INPUT_PULLUP);

  pinMode(led_geral, OUTPUT);
  pinMode(led_dosador, OUTPUT);
  pinMode(led_datador, OUTPUT);

  // Define estado inicial das saídas
  digitalWrite(led_geral, est_led_geral);
  digitalWrite(led_dosador, est_led_dosador);
  digitalWrite(led_datador, est_led_datador);
}


void loop() {
  standBy();
}

void acionaGeral(){
  btUmClique(bt_geral, &estbt_geral, &estbta_geral, &atr_geral, led_geral, &est_led_geral);
}

void acionaDosador(){
  btUmClique(bt_dosador, &estbt_dosador, &estbta_dosador, &atr_dosador, led_dosador, &est_led_dosador);
}

void acionaDatador(){
  btUmClique(bt_datador, &estbt_datador, &estbta_datador, &atr_datador, led_datador, &est_led_datador);
}

void standBy(){
  if (stand_by){
    acionaGeral();
    acionaDosador();
    acionaDatador();
    if (maquina_ligada){
      iniciaTrabalho();
    }
  } else {
    delay(3000);
    stand_by = true;
  }
}

void iniciaTrabalho(){

}

void btUmClique(byte botao, int *estado, int *est_ant, long *atr_ant, byte saida, int *est_saida ){
  byte leitura = leEntrada(botao);
  if (leitura != *est_ant) {
    *atr_ant = millis();
  }

  if ((millis() - *atr_ant) > atraso) {
    if (leitura != *estado) {
      *estado = leitura;
      if (*estado == LOW) {
        *est_saida = !*est_saida;
      }
    }
  }
  digitalWrite(saida, *est_saida);
  *est_ant = leitura;
}

byte leEntrada(byte pino){
  return digitalRead(pino);
}

