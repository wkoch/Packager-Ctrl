/*
 EMP-B Due
 V0.1 03/08/2014 William Koch

 Controlador de Empacotadeira com Arduino Due.
*/

// Constantes
// Entradas
const byte bt_geral = 22;        // Entrada do botão de Liga/Desliga Geral
const byte bt_dosador = 24;      // Entrada do botão de Liga/Desliga do Dosador
const byte bt_datador = 26;      // Entrada do botão de Liga/Desliga do Datador
const byte sensor_reset = 28;    // Entrada do Sensor de Reset de Ciclo
const byte sensor_porta = 32;    // Sensor de Segurança da Porta Frontal
const byte sensor_painel = 34;   // Sensor de Segurança da Porta do Painel
const byte sensor_maquina = 36;  // Sensor de Segurança da Porta da Máquina
const byte pot_solda_vertical = A0;    // Potenciômetro da Solda Vertical
const byte pot_solda_horizontal = A1;  // Potenciômetro da Solda Vertical
const byte pot_solda_datador = A2;     // Potenciômetro da Solda Vertical
                                       // Saídas
const byte geral = 40;                 // Saída Geral
const byte dosador = 42;               // Saída do Dosador
const byte datador = 44;               // Saída do Datador
const byte solda_vertical = 33;        // Saída da Solda Vertical
const byte solda_horizontal = 35;      // Saída da Solda Horizontal
const byte solda_datador = 37;         // Saída da Solda do Datador
const byte led_teste_1 = 23;
const byte led_teste_2 = 25;
const byte led_teste_3 = 27;

// Variáveis
// Configurações
#define BOTAO INPUT_PULLUP  // INPUT ou INPUT_PULLUP
#define SENSOR INPUT        // INPUT ou INPUT_PULLUP
#define SAIDA OUTPUT
#define ALTO LOW    // Deve ser invertido para INPUT_PULLUP
#define BAIXO HIGH  // Deve ser invertido para INPUT_PULLUP
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
int estbt_geral;                // Estado do Botão Geral
int estbta_geral = BAIXO;       // Estado anterior do Botão Geral
unsigned long atr_geral = 0;    // Atraso do Botão Geral
int estbt_dosador;              // Estado do Botão Dosador
int estbta_dosador = BAIXO;     // Estado anterior do Botão Dosador
unsigned long atr_dosador = 0;  // Atraso do Botão Dosador
int estbt_datador;              // Estado do Botão Datador
int estbta_datador = BAIXO;     // Estado anterior do Botão Dosador
unsigned long atr_datador = 0;  // Atraso do Botão Dosador
                                // PWM das Soldas
const unsigned long ciclo_PWM = 1000;
unsigned long tempo_PWM_vertical = 0;    // Tempo do PWM da solda vertical
unsigned long tempo_PWM_horizontal = 0;  // Tempo do PWM da solda horizontal
unsigned long tempo_PWM_datador = 0;     // Tempo do PWM da solda do Datador
                                         // Programa
boolean stand_by = FALSO;
boolean maquina_ligada = FALSO;
boolean dosador_ligado = FALSO;
boolean datador_ligado = FALSO;
boolean alarme_ativo = FALSO;
unsigned long inicio_ciclo = millis();
bool fotocelula_liberada = FALSO;
bool fotocelula_cortou = FALSO;
bool ciclo_resetado = FALSO;
bool reset_falso = FALSO;
unsigned long conta_ciclos = 0;
boolean produzindo = FALSO;
boolean reset_liberado = FALSO;
boolean sensor_resetou = FALSO;
// Tempos do Ciclo
// Ciclo
unsigned long ciclo_padrao = 1500;
unsigned long ciclo_minimo = 900;
unsigned long ciclo_maximo = 2000;
unsigned long soma_ciclos = 0;
// Mandibula
unsigned long inicio_mandibula = 550;
unsigned long fim_mandibula = 1500;
// Fotocélula
unsigned long inicio_fotocelula = 1250;
unsigned long fim_fotocelula = 1400;
// Faca
unsigned long inicio_faca = 600;
unsigned long fim_faca = 800;
// Refrigeração
unsigned long inicio_refrigeracao = 800;
unsigned long fim_refrigeracao = 1450;
// Datador
unsigned long inicio_datador = 0;
unsigned long fim_datador = 400;
// Solda Vertical
unsigned long inicio_soldas = 0;
unsigned long fim_soldas = 400;
// Atraso entre resets consecutivos
unsigned long atraso_resets = ciclo_padrao / 3;

void setup() {
  Serial.begin(9600);
  // Botões
  pinMode(bt_geral, BOTAO);
  pinMode(bt_dosador, BOTAO);
  pinMode(bt_datador, BOTAO);
  pinMode(sensor_reset, BOTAO);  // Mudar para Sensor após os testes.
  // Potenciômetros das Soldas
  pinMode(pot_solda_vertical, SENSOR);
  pinMode(pot_solda_horizontal, SENSOR);
  pinMode(pot_solda_horizontal, SENSOR);
  // Saídas PWM das Soldas
  pinMode(solda_vertical, SAIDA);
  pinMode(solda_horizontal, SAIDA);
  pinMode(solda_datador, SAIDA);
  // Sensores
  pinMode(sensor_porta, BOTAO);
  pinMode(sensor_painel, BOTAO);
  pinMode(sensor_maquina, BOTAO);

  pinMode(geral, SAIDA);
  pinMode(dosador, SAIDA);
  pinMode(datador, SAIDA);
  pinMode(led_teste_1, SAIDA);
  pinMode(led_teste_2, SAIDA);
  pinMode(led_teste_3, SAIDA);

  // Define estado inicial das saídas
  resetCompleto();
}

void loop() {
  modoAlarme();

  geraPWM(pot_solda_vertical, &tempo_PWM_vertical, solda_vertical,
          NomeSoldaVertical);
  geraPWM(pot_solda_horizontal, &tempo_PWM_horizontal, solda_horizontal,
          NomeSoldaHorizontal);
  geraPWM(pot_solda_datador, &tempo_PWM_datador, solda_datador,
          NomeSoldaDatador);

  if (!alarme_ativo) {
    standBy();
    modoTeste();
  }
}

// MODOS DE TRABALHO

void standBy() {
  if (stand_by && !alarme_ativo) {
    acionaGeral();    // Monitora botão Geral
    acionaDosador();  // Monitora botão Dosador
    acionaDatador();  // Monitora botão Datador
    if (maquina_ligada) {
      iniciaTrabalho();
    } else {
      reiniciaSaidas();
    }
  } else if (!alarme_ativo) {
    escreveSerial("Preparando para iniciar trabalho.");
    delay(3000);            // Espera 3 segundos após energização
    stand_by = VERDADEIRO;  // Libera funcionamento
    escreveSerial("Tudo pronto!");
  }
}

void iniciaTrabalho() {
  if (!produzindo) {
    produzindo = VERDADEIRO;
    reiniciaCiclo();
  }
  if (conta_ciclos >= 3) {
    ligaFuncao(led_teste_1, "Led Teste 1");  // Exemplo para teste.
                                             // ciclo de trabalho
  }
  funcaoReset();
}

void modoAlarme() {
  if (!alarme_ativo) {
    if (ativo(sensor_porta)) {
      bloqueioPorAlarme("Sensor da Porta");
    } else if (ativo(sensor_painel)) {
      bloqueioPorAlarme("Sensor do Painel");
    } else if (ativo(sensor_maquina)) {
      bloqueioPorAlarme("Sensor da Máquina");
    }
  }
}

// FUNÇÕES ADICIONAIS

void bloqueioPorAlarme(String texto) {
  alarme_ativo = VERDADEIRO;
  stand_by = FALSO;
  escreveSerial("\n<< Alarme! >>\n");
  reiniciaSaidas();
  escreveSerial(texto + " detectou uma falha!");
}

void acionaGeral() {
  btUmClique(bt_geral, &estbt_geral, &estbta_geral, &atr_geral,
             &maquina_ligada);
}

void acionaDosador() {
  btUmClique(bt_dosador, &estbt_dosador, &estbta_dosador, &atr_dosador,
             &dosador_ligado);
}

void acionaDatador() {
  if (maquina_ligada && !alarme_ativo) {
    btUmClique(bt_datador, &estbt_datador, &estbta_datador, &atr_datador,
               &datador_ligado);
  }
}

void funcaoReset() {
  unsigned long tempo_atual = millis();
  unsigned long fim_ciclo = inicio_ciclo + ciclo_padrao;
  unsigned long minimo = inicio_ciclo + ciclo_minimo;
  unsigned long maximo = inicio_ciclo + ciclo_maximo;
  sensorReset();
  if (tempo_atual >= minimo){
    reset_liberado = VERDADEIRO;
    if (sensor_resetou || tempo_atual >= maximo) {
      conta_ciclos++;
      escreveSerial((String)conta_ciclos + ": " +
                      (String)(tempo_atual - inicio_ciclo) + " / R:" +
                      (String)ciclo_padrao);
      soma_ciclos += tempo_atual - inicio_ciclo;
      reiniciaCiclo();
    }
  }
}

void reiniciaCiclo() {
  inicio_ciclo = millis();
  cicloMedio();
  ciclo_resetado = VERDADEIRO;
  reset_liberado = FALSO;
  sensor_resetou = FALSO;
  fotocelula_liberada = FALSO;
  fotocelula_cortou = FALSO;
}

void sensorReset(){
  if (ativo(sensor_reset) && reset_liberado){
    sensor_resetou = VERDADEIRO;
  }
}

void cicloMedio(){
  if (conta_ciclos > 0){
    ciclo_padrao = soma_ciclos / conta_ciclos;
  }
}

void geraPWM(byte pot, unsigned long *inicio, byte saida, String nome) {
  unsigned long ativo = map(analogRead(pot), 0, 1023, 0, ciclo_PWM);
  unsigned long inativo = ciclo_PWM - ativo;
  if (maquina_ligada && !alarme_ativo) {
    if (millis() > *inicio && millis() <= (*inicio + ativo)) {
      liga(saida);
    } else if (millis() > (*inicio + ativo) &&
               millis() < (*inicio + ciclo_PWM)) {
      desliga(saida);
    } else if (millis() >= (*inicio + ciclo_PWM)) {
      *inicio = millis();
    }
  } else {
    desliga(saida);
  }
}

// FERRAMENTAS AUXILIARES

byte leEntrada(byte pino) { return digitalRead(pino); }

void liga(byte saida) { digitalWrite(saida, LIGA); }

void ligaFuncao(byte saida, String texto) {
  if (desligado(saida)) {
    liga(saida);
    escreveSerial(texto + " Ligado.");
  }
}

void desliga(byte saida) { digitalWrite(saida, DESLIGA); }

void desligaFuncao(byte saida, String texto) {
  if (ligado(saida)) {
    desliga(saida);
    escreveSerial(texto + " Desligado.");
  }
}

boolean ligado(byte saida) {
  if (digitalRead(saida) == LIGA) {
    return VERDADEIRO;
  } else {
    return FALSO;
  }
}

boolean desligado(byte saida) {
  if (digitalRead(saida) == DESLIGA) {
    return VERDADEIRO;
  } else {
    return FALSO;
  }
}

boolean ativo(byte entrada) {
  if (digitalRead(entrada) == ALTO) {
    return VERDADEIRO;
  } else {
    return FALSO;
  }
}

boolean inativo(byte entrada) {
  if (digitalRead(entrada) == BAIXO) {
    return VERDADEIRO;
  } else {
    return FALSO;
  }
}

void btUmClique(byte botao, int *estado, int *est_ant, unsigned long *atr_ant,
                boolean *funcao) {
  byte estado_atual = leEntrada(botao);
  if (estado_atual != *est_ant) {
    *atr_ant = millis();
  }

  if ((millis() - *atr_ant) > atraso) {
    if (estado_atual != *estado) {
      *estado = estado_atual;
      if (*estado == ALTO) {
        *funcao = !*funcao;
      }
    }
  }
  *est_ant = estado_atual;
}

void escreveSerial(String texto) {
  if (Serial) {
    Serial.println(texto);
  }
}

void reiniciaSaidas() {
  dosador_ligado = FALSO;
  datador_ligado = FALSO;
  conta_ciclos = 0;

  desligaFuncao(geral, NomeGeral);
  desligaFuncao(dosador, NomeDosador);
  desligaFuncao(datador, NomeDatador);

  desliga(led_teste_1);  // Para testes
  desliga(led_teste_2);  // Para testes
  desliga(led_teste_3);  // Para testes
}

void resetCompleto() {
  desliga(geral);
  desliga(dosador);
  desliga(datador);

  desliga(solda_vertical);
  desliga(solda_horizontal);
  desliga(solda_datador);

  desliga(led_teste_1);  // Para testes
  desliga(led_teste_2);  // Para testes
  desliga(led_teste_3);  // Para testes
}

void modoTeste() {
  if (maquina_ligada) {
    ligaFuncao(geral, NomeGeral);
  } else {
    desligaFuncao(geral, NomeGeral);
  }
  if (dosador_ligado) {
    ligaFuncao(dosador, NomeDosador);
  } else {
    desligaFuncao(dosador, NomeDosador);
  }
  if (datador_ligado) {
    ligaFuncao(datador, NomeDatador);
  } else {
    desligaFuncao(datador, NomeDatador);
  }
}
