/*
 EMP-B Due
 V0.1 © 03/08/2014 William Koch

 Controlador de Empacotadeira com Arduino Due.
*/

// Constantes
  // Entradas
    const byte bt_geral = 22; // Entrada do botão de Liga/Desliga Geral
    const byte bt_dosador = 24; // Entrada do botão de Liga/Desliga do Dosador
    const byte bt_datador = 26; // Entrada do botão de Liga/Desliga do Datador
  // Saídas
    const byte led_geral = 23; // Saída Geral
    const byte led_dosador = 25; // Saída do Dosador
    const byte led_datador = 27; // Saída do Datador


// Variáveis
  // Configurações
    #define ModoBotoes INPUT_PULLUP // INPUT ou INPUT_PULLUP
    #define ModoLeds OUTPUT
    #define ALTO LOW // Inverter para INPUT_PULLUP
    #define BAIXO HIGH // Inverter para INPUT_PULLUP
    #define LIGA HIGH
    #define DESLIGA LOW
    #define VERDADEIRO true
    #define FALSO false
  // Botões de Entrada Um Clique
    unsigned long atraso = 50;
    int estbt_geral; // Estado do Botão Geral
    int estbta_geral = BAIXO; // Estado anterior do Botão Geral
    unsigned long atr_geral = 0; // Atraso do Botão Geral
    int estbt_dosador; // Estado do Botão Dosador
    int estbta_dosador = BAIXO; // Estado anterior do Botão Dosador
    unsigned long atr_dosador = 0; // Atraso do Botão Dosador
    int estbt_datador; // Estado do Botão Datador
    int estbta_datador = BAIXO; // Estado anterior do Botão Dosador
    unsigned long atr_datador = 0; // Atraso do Botão Dosador
  // Programa
    boolean stand_by = FALSO;
    boolean maquina_ligada = FALSO;
    boolean dosador_ligado = FALSO;
    boolean datador_ligado = FALSO;


void setup() {
  Serial.begin(9600);
  pinMode(bt_geral, ModoBotoes);
  pinMode(bt_dosador, ModoBotoes);
  pinMode(bt_datador, ModoBotoes);

  pinMode(led_geral, ModoLeds);
  pinMode(led_dosador, ModoLeds);
  pinMode(led_datador, ModoLeds);

  // Define estado inicial das saídas
  digitalWrite(led_geral, DESLIGA);
  digitalWrite(led_dosador, DESLIGA);
  digitalWrite(led_datador, DESLIGA);
}


void loop() {
  standBy();
  modoTeste();
}

void acionaGeral(){
  btUmClique(bt_geral, &estbt_geral, &estbta_geral, &atr_geral, &maquina_ligada);
}

void acionaDosador(){
  btUmClique(bt_dosador, &estbt_dosador, &estbta_dosador, &atr_dosador, &dosador_ligado);
}

void acionaDatador(){
  btUmClique(bt_datador, &estbt_datador, &estbta_datador, &atr_datador, &datador_ligado);
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
    stand_by = VERDADEIRO;
  }
}

void iniciaTrabalho(){

}

void btUmClique(byte botao, int *estado, int *est_ant, unsigned long *atr_ant, boolean *funcao){
  byte leitura = leEntrada(botao);
  if (leitura != *est_ant) {
    *atr_ant = millis();
  }

  if ((millis() - *atr_ant) > atraso) {
    if (leitura != *estado) {
      *estado = leitura;
      if (*estado == ALTO) {
        *funcao = !*funcao;
      }
    }
  }
  *est_ant = leitura;
}

byte leEntrada(byte pino){
  return digitalRead(pino);
}

void liga(byte saida, String texto){
  if (desligado(saida)){
    digitalWrite(saida, LIGA);
    escreveSerial(texto + " Ligado.");
  }
}

void desliga(byte saida, String texto){
  if (ligado(saida)){
    digitalWrite(saida, DESLIGA);
    escreveSerial(texto + " Desligado.");
  }
}

boolean ligado(byte saida){
  if (digitalRead(saida) == LIGA){
    return VERDADEIRO;
  } else {
    return FALSO;
  }
}

boolean desligado(byte saida){
  if (digitalRead(saida) == DESLIGA){
    return VERDADEIRO;
  } else {
    return FALSO;
  }
}

void escreveSerial(String texto){
  if (Serial){
    Serial.println(texto);
  }
}


void modoTeste(){
  if (maquina_ligada){
    liga(led_geral, "Led Geral");
  } else {
    desliga(led_geral, "Led Geral");
  }
  if (dosador_ligado){
    liga(led_dosador, "Led Dosador");
  } else {
    desliga(led_dosador, "Led Dosador");
  }
  if (datador_ligado){
    liga(led_datador, "Led Datador");
  } else {
    desliga(led_datador, "Led Datador");
  }
}
