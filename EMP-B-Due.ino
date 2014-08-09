/*
 EMP-B Due
 V0.1 03/08/2014 William Koch

 Controlador de Empacotadeira com Arduino Due.
*/

// CONSTANTES
// ENTRADAS
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

// SAÍDAS
const byte geral = 23;                 // Saída Geral
const byte dosador = 25;               // Saída do Dosador
const byte datador = 27;               // Saída do Datador
const byte mandibula = 40;
const byte solda_vertical_PWM = 33;        // Saída da Solda Vertical
const byte solda_horizontal_PWM = 35;      // Saída da Solda Horizontal
const byte solda_datador_PWM = 37;         // Saída da Solda do Datador
// REMOVER
const byte saida_teste = 2;
const byte entrada_teste = 4;

// VARIÁVEIS
// CONFIGURAÇÕES
#define BOTAO INPUT_PULLUP  // INPUT ou INPUT_PULLUP
#define SENSOR INPUT        // INPUT ou INPUT_PULLUP
#define SAIDA OUTPUT
#define ALTO LOW    // Deve ser invertido para INPUT_PULLUP
#define BAIXO HIGH  // Deve ser invertido para INPUT_PULLUP
#define LIGA HIGH
#define DESLIGA LOW
#define VERDADEIRO true
#define FALSO false

// NOMES DE FUNÇÕES
#define NomeGeral "Geral"
#define NomeDosador "Dosador"
#define NomeDatador "Datador"
#define NomeMandibula "Mandibula"
#define NomeSoldaVertical "Solda Vertical"
#define NomeSoldaHorizontal "Solda Horizontal"
#define NomeSoldaDatador "Solda Datador"

// BOTOÕES UM-CLIQUE
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

// PWM DAS SOLDAS
const unsigned long ciclo_PWM = 1000;
unsigned long tempo_PWM_vertical = 0;    // Tempo do PWM da solda vertical
unsigned long tempo_PWM_horizontal = 0;  // Tempo do PWM da solda horizontal
unsigned long tempo_PWM_datador = 0;     // Tempo do PWM da solda do Datador

// MODOS DE TRABALHO
boolean stand_by = FALSO;
boolean maquina_ligada = FALSO;
boolean dosador_ligado = FALSO;
boolean datador_ligado = FALSO;
boolean alarme_ativo = FALSO;
boolean fotocelula_liberada = FALSO;
boolean fotocelula_cortou = FALSO;
boolean produzindo = FALSO;
boolean resetado = FALSO;

// TEMPOS DE CICLO
// CONTROLE DE CICLO
unsigned long tempo_atual = millis();
unsigned long ciclo_padrao = 1500;
unsigned long ciclo_padrao_anterior = ciclo_padrao;
unsigned long inicio_ciclo = 0;
unsigned long ciclo_minimo = 900;
unsigned long ciclo_maximo = 2000;
unsigned long soma_ciclos = 0;
unsigned long conta_ciclos = 0;
unsigned long fim_ciclo = 0;
unsigned long minimo = 0;
unsigned long maximo = 0;

// MANDÍBULA
unsigned long inicio_mandibula = 550;
unsigned long fim_mandibula = 1500;

// FOTOCÉLULA
unsigned long inicio_fotocelula = 1250;
unsigned long fim_fotocelula = 1400;

// FACA
unsigned long inicio_faca = 600;
unsigned long fim_faca = 800;

// REFRIGERAÇÃO
unsigned long inicio_refrigeracao = 800;
unsigned long fim_refrigeracao = 1450;

// DATADOR
unsigned long inicio_datador = 0;
unsigned long fim_datador = 400;

// SOLDAS VERTICAL E HORIZONTAL
unsigned long inicio_soldas = 0;
unsigned long fim_soldas = 400;

void setup() {
  Serial.begin(9600);
  // BOTÕES
  pinMode(bt_geral, BOTAO);
  pinMode(bt_dosador, BOTAO);
  pinMode(bt_datador, BOTAO);
  pinMode(sensor_reset, BOTAO);  // Mudar para Sensor após os testes.
  // POTENCIÔMETROS DAS SOLDAS
  pinMode(pot_solda_vertical, SENSOR);
  pinMode(pot_solda_horizontal, SENSOR);
  pinMode(pot_solda_datador, SENSOR);
  // SAÍDAS PWM DAS SOLDAS
  pinMode(mandibula, SAIDA);
  pinMode(solda_vertical_PWM, SAIDA);
  pinMode(solda_horizontal_PWM, SAIDA);
  pinMode(solda_datador_PWM, SAIDA);
  // SENSORES
  pinMode(sensor_porta, BOTAO);
  pinMode(sensor_painel, BOTAO);
  pinMode(sensor_maquina, BOTAO);

  // REMOVER
  pinMode(saida_teste, SAIDA);


  pinMode(geral, SAIDA);
  pinMode(dosador, SAIDA);
  pinMode(datador, SAIDA);

  // ESTADO INICIAL DAS SAÍDAS
  resetCompleto();
}

void loop() {
  modoAlarme();

  geraPWM(pot_solda_vertical, &tempo_PWM_vertical, solda_vertical_PWM);
  geraPWM(pot_solda_horizontal, &tempo_PWM_horizontal, solda_horizontal_PWM);
  geraPWM(pot_solda_datador, &tempo_PWM_datador, solda_datador_PWM);

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
    reiniciaCiclo(millis());
  }
  if (conta_ciclos >= 3) {
    funcaoSimples(mandibula, NomeMandibula, inicio_mandibula, fim_mandibula);
    funcaoSimples(saida_teste, "Teste", 1500, 1501);
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
  tempo_atual = millis();
  if (resetado){
    fim_ciclo = inicio_ciclo + ciclo_padrao;
    minimo = inicio_ciclo + ciclo_minimo;
    maximo = inicio_ciclo + ciclo_maximo;
    resetado = FALSO;
    reiniciaSaidas();
  }

  if (tempo_atual >= minimo) {
    if (ativo(sensor_reset) || tempo_atual == maximo) {
      conta_ciclos++;
      escreveSerial((String)conta_ciclos + " " +
                    (String)(tempo_atual - inicio_ciclo) + " " +
                    (String)ciclo_padrao + " " + (String)tempo_atual + " " +
                    (String)millis());
      soma_ciclos += (tempo_atual - inicio_ciclo);
      reiniciaCiclo(tempo_atual);
    }
  }
}

void reiniciaCiclo(unsigned long tempo) {
  inicio_ciclo = tempo;
  cicloMedio();
  resetado = VERDADEIRO;
  fotocelula_liberada = FALSO;
  fotocelula_cortou = FALSO;
}

void cicloMedio() {
  if (conta_ciclos > 0) {
    ciclo_padrao_anterior = ciclo_padrao;
    ciclo_padrao = soma_ciclos / conta_ciclos;
    inicio_mandibula *= (long)((float)ciclo_padrao_anterior / (float)ciclo_padrao);
  }
}

void geraPWM(byte pot, unsigned long *inicio, byte saida) {
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

// FUNÇÕES DE CICLO

void funcaoSimples(const byte saida, String nome, unsigned long inicio, unsigned long fim) {
  inicio += inicio_ciclo;
  fim += inicio_ciclo;
  if ((tempo_atual >= inicio) && (tempo_atual < fim) && desligado(saida)) {
    ligaFuncao(saida, nome);
  } else if (tempo_atual >= fim) {
    desligaFuncao(saida, nome);
  }
}

// FERRAMENTAS AUXILIARES

byte leEntrada(byte pino) { return digitalRead(pino); }

void liga(byte saida) { digitalWrite(saida, LIGA); }

void ligaFuncao(byte saida, String nome) {
  if (desligado(saida)) {
    liga(saida);
    if (ligado(saida)){
      escreveSerial(nome + " Ligado.");
    } else {
      escreveSerial("Ocorreu um erro ao ligar " + nome);
    }
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
  desligaFuncao(dosador, NomeDosador);
  desligaFuncao(datador, NomeDatador);
  desligaFuncao(mandibula, NomeMandibula);
}

void resetCompleto() {
  dosador_ligado = FALSO;
  datador_ligado = FALSO;
  conta_ciclos = 0;
  desliga(geral);
  desliga(dosador);
  desliga(datador);
  desliga(mandibula);

  desliga(solda_vertical_PWM);
  desliga(solda_horizontal_PWM);
  desliga(solda_datador_PWM);
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
