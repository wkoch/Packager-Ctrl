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
  // Configurações
    #define ModoBotoes INPUT_PULLUP
    #define ModoLeds OUTPUT
    #define SinalAlto LOW
    #define SinalBaixo HIGH
    #define SaidaLigada HIGH
    #define SaidaDesligada LOW
    #define Verdadeiro true
    #define Falso false
  // Botões de Entrada Um Clique
    long atraso = 50;
    int estbt_geral;
    int estbta_geral = SinalBaixo;
    long atr_geral = 0;
    int estbt_dosador;
    int estbta_dosador = SinalBaixo;
    long atr_dosador = 0;
    int estbt_datador;
    int estbta_datador = SinalBaixo;
    long atr_datador = 0;
  // Programa
    boolean stand_by = Falso;
    boolean maquina_ligada = Falso;
    boolean dosador_ligado = Falso;
    boolean datador_ligado = Falso;


void setup() {
  pinMode(bt_geral, ModoBotoes);
  pinMode(bt_dosador, ModoBotoes);
  pinMode(bt_datador, ModoBotoes);

  pinMode(led_geral, ModoLeds);
  pinMode(led_dosador, ModoLeds);
  pinMode(led_datador, ModoLeds);

  // Define estado inicial das saídas
  digitalWrite(led_geral, SaidaDesligada);
  digitalWrite(led_dosador, SaidaDesligada);
  digitalWrite(led_datador, SaidaDesligada);
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
    stand_by = Verdadeiro;
  }
}

void iniciaTrabalho(){

}

void btUmClique(byte botao, int *estado, int *est_ant, long *atr_ant, boolean *funcao){
  byte leitura = leEntrada(botao);
  if (leitura != *est_ant) {
    *atr_ant = millis();
  }

  if ((millis() - *atr_ant) > atraso) {
    if (leitura != *estado) {
      *estado = leitura;
      if (*estado == SinalAlto) {
        *funcao = !*funcao;
      }
    }
  }
  *est_ant = leitura;
}

byte leEntrada(byte pino){
  return digitalRead(pino);
}

void liga(byte saida){
  digitalWrite(saida, SaidaLigada);
}

void desliga(byte saida){
  digitalWrite(saida, SaidaDesligada);
}


void modoTeste(){
  if (maquina_ligada){
    liga(led_geral);
  } else {
    desliga(led_geral);
  }
  if (dosador_ligado){
    liga(led_dosador);
  } else {
    desliga(led_dosador);
  }
  if (datador_ligado){
    liga(led_datador);
  } else {
    desliga(led_datador);
  }
}
