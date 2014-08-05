/*
 EMP-B Due
 V0.1 03/08/2014 William Koch

 Controlador de Empacotadeira com Arduino Due.
*/

// Constantes
  // Entradas
    const byte bt_geral = 22; // Entrada do botão de Liga/Desliga Geral
    const byte bt_dosador = 24; // Entrada do botão de Liga/Desliga do Dosador
    const byte bt_datador = 26; // Entrada do botão de Liga/Desliga do Datador
    const byte sensor_porta = 48; // Sensor de Segurança da Porta Frontal
    const byte sensor_painel = 50; // Sensor de Segurança da Porta do Painel
    const byte sensor_maquina = 52; // Sensor de Segurança da Porta da Máquina
    const byte pot_solda_vertical = A0; // Potenciômetro da Solda Vertical
    const byte pot_solda_horizontal = A1; // Potenciômetro da Solda Vertical
    const byte pot_solda_datador = A2; // Potenciômetro da Solda Vertical
  // Saídas
    const byte geral = 23; // Saída Geral
    const byte dosador = 25; // Saída do Dosador
    const byte datador = 27; // Saída do Datador
    const byte solda_vertical = 33; // Saída da Solda Vertical
    const byte solda_horizontal = 35; // Saída da Solda Horizontal
    const byte solda_datador = 37; // Saída da Solda do Datador


// Variáveis
  // Configurações
    #define ModoBotoes INPUT_PULLUP // INPUT ou INPUT_PULLUP
    #define ModoLeds OUTPUT
    #define ALTO LOW // Deve ser invertido para INPUT_PULLUP
    #define BAIXO HIGH // Deve ser invertido para INPUT_PULLUP
    #define LIGA HIGH
    #define DESLIGA LOW
    #define VERDADEIRO true
    #define FALSO false
  // Nomes de Funções
    #define NomeGeral "Geral"
    #define NomeDosador "Dosador"
    #define NomeDatador "Datador"
    #define NomeSoldaVertical "Solda Vertical"
    #define NomeSoldaHorizontal "Solda Horizontal"
    #define NomeSoldaDatador "Solda Datador"
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
  // PWM das Soldas
    const unsigned long ciclo_PWM = 1000;
    unsigned long tempo_PWM_vertical = 0; // Tempo do PWM da solda vertical
    unsigned long tempo_PWM_horizontal = 0; // Tempo do PWM da solda horizontal
    unsigned long tempo_PWM_datador = 0; // Tempo do PWM da solda do Datador
  // Programa
    boolean stand_by = FALSO;
    boolean maquina_ligada = FALSO;
    boolean dosador_ligado = FALSO;
    boolean datador_ligado = FALSO;
    boolean alarme_ativo = FALSO;


void setup() {
  Serial.begin(9600);
  // Botões
  pinMode(bt_geral, ModoBotoes);
  pinMode(bt_dosador, ModoBotoes);
  pinMode(bt_datador, ModoBotoes);
  // Potenciômetros
  pinMode(pot_solda_vertical, INPUT);
  pinMode(pot_solda_horizontal, INPUT);
  pinMode(pot_solda_horizontal, INPUT);
  // PWM
  pinMode(solda_vertical, OUTPUT);
  pinMode(solda_horizontal, OUTPUT);
  pinMode(solda_datador, OUTPUT);
  // Sensores
  pinMode(sensor_porta, ModoBotoes);
  pinMode(sensor_painel, ModoBotoes);
  pinMode(sensor_maquina, ModoBotoes);


  pinMode(geral, ModoLeds);
  pinMode(dosador, ModoLeds);
  pinMode(datador, ModoLeds);

  // Define estado inicial das saídas
  digitalWrite(geral, DESLIGA);
  digitalWrite(dosador, DESLIGA);
  digitalWrite(datador, DESLIGA);
  digitalWrite(solda_vertical, DESLIGA);
  digitalWrite(solda_horizontal, DESLIGA);
  digitalWrite(solda_datador, DESLIGA);
}


void loop() {
  modoAlarme();
  if (!alarme_ativo){
    standBy();
    modoTeste();
  }
}

void geraPWM(byte pot, unsigned long *inicio, byte saida, String nome){
  unsigned long ativo = map(analogRead(pot), 0, 1023, 0, ciclo_PWM);
  unsigned long inativo = ciclo_PWM - ativo;
  if (millis() > *inicio && millis() <= (*inicio + ativo)){
    liga(saida, nome);
  } else if (millis() > (*inicio + ativo) && millis() < (*inicio + ciclo_PWM)){
    desliga(saida, nome);
  } else if (millis() >= (*inicio + ciclo_PWM)) {
    *inicio = millis();
  }
}

void acionaGeral(){
  btUmClique(bt_geral, &estbt_geral, &estbta_geral, &atr_geral, &maquina_ligada);
}

void acionaDosador(){
  btUmClique(bt_dosador, &estbt_dosador, &estbta_dosador, &atr_dosador, &dosador_ligado);
}

void acionaDatador(){
  if (maquina_ligada){
    btUmClique(bt_datador, &estbt_datador, &estbta_datador, &atr_datador, &datador_ligado);
  }
}

void standBy(){
  if (stand_by){
    acionaGeral(); // Checa botão Geral
    acionaDosador(); // Checa botão Dosador
    acionaDatador(); // Checa botão Datador
    if (maquina_ligada){
      iniciaTrabalho();
    } else {
      reiniciaSaidas();
    }
  } else {
    delay(3000); // Espera 3 segundos após energização
    stand_by = VERDADEIRO; // Libera funcionamento
  }
}

void iniciaTrabalho(){
  geraPWM(pot_solda_vertical, &tempo_PWM_vertical, solda_vertical, NomeSoldaVertical);
  geraPWM(pot_solda_horizontal, &tempo_PWM_horizontal, solda_horizontal, NomeSoldaHorizontal);
  geraPWM(pot_solda_datador, &tempo_PWM_datador, solda_datador, NomeSoldaDatador);
}

void modoAlarme(){
  if (!alarme_ativo){
    if (ativo(sensor_porta)){
      bloqueioPorAlarme("Sensor da Porta");
    } else if (ativo(sensor_painel)){
      bloqueioPorAlarme("Sensor do Painel");
    } else if (ativo(sensor_maquina)){
      bloqueioPorAlarme("Sensor da Máquina");
    }
  }
}

void bloqueioPorAlarme(String texto){
  alarme_ativo = VERDADEIRO;
  escreveSerial("\n<< Alarme! >>\n");
  reiniciaSaidas();
  escreveSerial(texto + " detectou uma falha de segurança.");
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

boolean ativo(byte entrada){
  if (digitalRead(entrada) == ALTO){
    return VERDADEIRO;
  } else {
    return FALSO;
  }
}

boolean inativo(byte entrada){
  if (digitalRead(entrada) == BAIXO){
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

void reiniciaSaidas(){
  dosador_ligado = FALSO;
  datador_ligado = FALSO;

  desliga(geral, NomeGeral);
  desliga(dosador, NomeDosador);
  desliga(datador, NomeDatador);
}


void modoTeste(){
  if (maquina_ligada){
    liga(geral, NomeGeral);
  } else {
    desliga(geral, NomeGeral);
  }
  if (dosador_ligado){
    liga(dosador, NomeDosador);
  } else {
    desliga(dosador, NomeDosador);
  }
  if (datador_ligado){
    liga(datador, NomeDatador);
  } else {
    desliga(datador, NomeDatador);
  }
}
