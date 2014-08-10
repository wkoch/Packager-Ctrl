/*
 EMP-B Due
 V0.1 03/08/2014 William Koch

 Controlador de Empacotadeira com Arduino Due.
*/

// CONFIGURAÇÕES
// GERAL
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
#define NomeGeral "Geral" // Acionamento da máquina
#define NomeDosador "Dosador" // Acionamento do Dosador
#define NomeDatador "Datador" // Liberação do Datador
#define NomeMandibula "Mandibula" // Ciclo da Mandíbula
#define NomeFaca "Faca" // Ciclo da Faca
#define NomeRefrigeracao "Refrigeracao" // Ciclo da Refrigeração
#define NomeSoldas "Soldas" // Ciclo das Soldas Vertical e Horizontal
#define NomeSoldaVertical "Solda Vertical"
#define NomeSoldaHorizontal "Solda Horizontal"
#define NomeSoldaDatador "Solda Datador"
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
const byte geral = 23; // Saída Geral
const byte dosador = 25; // Saída do Dosador
const byte led_datador = 27; // Saída do Datador
const byte mandibula = 40;
const byte faca = 42;
const byte refrigeracao = 44;
const byte datador = 46;
const byte soldas = 48;
// SAÍDAS PWM
const byte solda_vertical_PWM = 33;        // Saída da Solda Vertical
const byte solda_horizontal_PWM = 35;      // Saída da Solda Horizontal
const byte solda_datador_PWM = 37;         // Saída da Solda do Datador
// TEMPOS DE CICLO
unsigned long ciclo_padrao = 2000;
const unsigned long ciclo_minimo = 900;
const unsigned long ciclo_maximo = 2000;
// MANDÍBULA
const unsigned int inicio_mandibula = 550;
const unsigned int fim_mandibula = 1500;
// FOTOCÉLULA
const unsigned int inicio_fotocelula = 1250;
const unsigned int fim_fotocelula = 1400;
// FACA
const unsigned int inicio_faca = 600;
const unsigned int fim_faca = 800;
// REFRIGERAÇÃO
const unsigned int inicio_refrigeracao = 800;
const unsigned long fim_refrigeracao = 1450;
// DATADOR
const unsigned int inicio_datador = 5;
const unsigned int fim_datador = 400;
// SOLDAS VERTICAL E HORIZONTAL
const unsigned int inicio_soldas = 5;
const unsigned int fim_soldas = 400;

// REMOVER >>
const byte saida_teste = 2;
const byte entrada_teste = 4;
// << REMOVER

// VARIÁVEIS
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
unsigned long ciclo_padrao_anterior = ciclo_padrao;
unsigned long inicio_ciclo = 0;
unsigned long soma_ciclos = 0;
unsigned long conta_ciclos = 0;
unsigned long fim_ciclo = 0;
unsigned long minimo = 0;
unsigned long maximo = 0;
unsigned long ultimo_ciclo = ciclo_padrao;

// TEMPOS MÉDIOS
// MANDÍBULA
unsigned long ti_mandibula = (unsigned long)inicio_mandibula; // NÃO ALTERE
unsigned long tf_mandibula = (unsigned long)fim_mandibula; // NÃO ALTERE
// FOTOCÉLULA
unsigned long ti_fotocelula = (unsigned long)inicio_fotocelula; // NÃO ALTERE
unsigned long tf_fotocelula = (unsigned long)fim_fotocelula; // NÃO ALTERE
// FACA
unsigned long ti_faca = (unsigned long)inicio_faca; // NÃO ALTERE
unsigned long tf_faca = (unsigned long)fim_faca; // NÃO ALTERE
// REFRIGERAÇÃO
unsigned long ti_refrigeracao = (unsigned long)inicio_refrigeracao; // NÃO ALTERE
unsigned long tf_refrigeracao = (unsigned long)fim_refrigeracao; // NÃO ALTERE
// DATADOR
unsigned long ti_datador = (unsigned long)inicio_datador; // NÃO ALTERE
unsigned long tf_datador = (unsigned long)fim_datador; // NÃO ALTERE
// SOLDAS VERTICAL E HORIZONTAL
unsigned long ti_soldas = (unsigned long)inicio_soldas; // NÃO ALTERE
unsigned long tf_soldas = (unsigned long)fim_soldas; // NÃO ALTERE

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
  pinMode(entrada_teste, SENSOR);


  pinMode(geral, SAIDA);
  pinMode(dosador, SAIDA);
  pinMode(led_datador, SAIDA);
  pinMode(faca, SAIDA);
  pinMode(refrigeracao, SAIDA);
  pinMode(datador, SAIDA);
  pinMode(soldas, SAIDA);

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
    // funcaoSimples(mandibula, NomeMandibula, inicio_mandibula, fim_mandibula);
    // funcaoSimples(faca, NomeFaca, inicio_faca, fim_faca);
    // funcaoSimples(refrigeracao, NomeRefrigeracao, inicio_refrigeracao, fim_refrigeracao);
    funcaoSimples(datador, NomeDatador, ti_datador, tf_datador);
    // funcaoSimples(soldas, NomeSoldas, inicio_soldas, fim_soldas);
    // funcaoSimples(saida_teste, "Teste", 1500, 1600);
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
    if (ativo(sensor_reset) || inativo(entrada_teste) || tempo_atual == maximo) {
      conta_ciclos++;
      ultimo_ciclo = tempo_atual - inicio_ciclo;
      soma_ciclos += (tempo_atual - inicio_ciclo);
      reiniciaCiclo(tempo_atual);
      escreveSerial("\t" + (String)conta_ciclos + " " +
                    (String)ultimo_ciclo + " " +
                    (String)ciclo_padrao + " " + (String)tempo_atual + " " +
                    (String)millis());
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
    ciclo_padrao = ultimo_ciclo; // soma_ciclos / conta_ciclos;
    // AJUSTE DINÂMICO DOS TEMPOS DAS FUNÇÕES
    ajustaCiclo(&ti_mandibula, inicio_mandibula);
    ajustaCiclo(&tf_mandibula, fim_mandibula);
    ajustaCiclo(&ti_faca, inicio_faca);
    ajustaCiclo(&tf_faca, fim_faca);
    ajustaCiclo(&ti_refrigeracao, inicio_refrigeracao);
    ajustaCiclo(&tf_refrigeracao, fim_refrigeracao);
    ajustaCiclo(&ti_datador, inicio_datador);
    ajustaCiclo(&tf_datador, fim_datador);
    ajustaCiclo(&ti_soldas, inicio_soldas);
    ajustaCiclo(&tf_soldas, fim_soldas);
  }
}

void ajustaCiclo(unsigned long *variavel, unsigned int original){
  unsigned long tempo = ((unsigned long)original * ciclo_padrao) / ciclo_padrao_anterior;
  if (tempo <= 0){
    *variavel = 1;
  } else {
    *variavel = tempo;
  }
}

void geraPWM(byte pot, unsigned long *inicio, byte saida) {
  unsigned long ativo = *inicio + map(analogRead(pot), 0, 1023, 0, ciclo_PWM);
  unsigned long inativo = ciclo_PWM - ativo;
  unsigned long pwm = *inicio + ciclo_PWM;
  if (maquina_ligada && !alarme_ativo) {
    if (tempo_atual > *inicio && tempo_atual <= ativo) {
      liga(saida);
    } else if (tempo_atual > ativo && tempo_atual < pwm) {
      desliga(saida);
    } else if (tempo_atual >= pwm) {
      *inicio = tempo_atual;
    }
  } else {
    desliga(saida);
  }
}

// FUNÇÕES DE CICLO

void funcaoSimples(const byte saida, String nome, unsigned long inicio, unsigned long fim) {
  inicio += inicio_ciclo;
  fim += inicio_ciclo;
  if (desligado(saida)){
    if (tempo_atual >= inicio && tempo_atual < fim) {
      ligaFuncao(saida, nome);
    }
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
      escreveSerial(">> " + nome + " Ligado." + (String)ti_datador);
    } else {
      escreveSerial("Ocorreu um erro ao ligar " + nome);
    }
  }
}

void desliga(byte saida) { digitalWrite(saida, DESLIGA); }

void desligaFuncao(byte saida, String nome) {
  if (ligado(saida)) {
    desliga(saida);
    escreveSerial("<< " + nome + " Desligado." + (String)tf_datador);
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
  desligaFuncao(led_datador, NomeDatador);
  desligaFuncao(mandibula, NomeMandibula);
  desligaFuncao(faca, NomeFaca);
  desligaFuncao(refrigeracao, NomeRefrigeracao);
  desligaFuncao(datador, NomeDatador);
  desligaFuncao(soldas, NomeSoldas);
}

void resetCompleto() {
  dosador_ligado = FALSO;
  datador_ligado = FALSO;
  conta_ciclos = 0;
  desliga(geral);
  desliga(dosador);
  desliga(led_datador);
  desliga(mandibula);
  desliga(faca);
  desliga(refrigeracao);
  desliga(datador);
  desliga(soldas);

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
    ligaFuncao(led_datador, NomeDatador);
  } else {
    desligaFuncao(led_datador, NomeDatador);
  }
}
