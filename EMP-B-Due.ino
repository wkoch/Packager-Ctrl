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
#define NomeVertical "Solda Vertical" // Ciclo da Solda Vertical
#define NomeSoldaDatador "Solda Datador"
// ENTRADAS
const byte bt_geral = 22;        // Entrada do botão de Liga/Desliga Geral
const byte bt_dosador = 24;      // Entrada do botão de Liga/Desliga do Dosador
const byte bt_datador = 26;      // Entrada do botão de Liga/Desliga do Datador
const byte sensor_reset = 28;    // Entrada do Sensor de Reset de Ciclo
const byte sensor_porta = 32;    // Sensor de Segurança da Porta Frontal
const byte sensor_painel = 34;   // Sensor de Segurança da Porta do Painel
const byte sensor_maquina = 36;  // Sensor de Segurança da Porta da Máquina
const byte sensor_mandibula = 2; // Sensor de Segurança de Fechamento da Mandíbula
const byte fotocelula = 6; // Sensor da Fotocélula de Corte
const byte pot_solda_vertical = A0; // Potenciômetro da Solda Vertical
const byte pot_solda_horizontal = A1; // Potenciômetro da Solda Vertical
const byte pot_solda_datador = A2; // Potenciômetro da Solda Vertical
// SAÍDAS
const byte geral = 23; // Saída Geral
const byte dosador = 25; // Saída do Dosador
const byte led_datador = 27; // Saída do Datador
const byte mandibula = 40; // Saída da Mandíbula
const byte faca = 42; // Saída da Faca
const byte refrigeracao = 44; // Saída da Refrigeração
const byte datador = 46; // Saída do Datador
const byte vertical = 48; // Saída da Solda Vertical
// SAÍDAS PWM
const byte solda_vertical_PWM = 33; // Saída da Solda Vertical
const byte solda_horizontal_PWM = 35; // Saída da Solda Horizontal
const byte solda_datador_PWM = 37; // Saída da Solda do Datador
// TEMPOS DE CICLO
const unsigned long ciclo_PWM = 1000; // Tempo de Ciclo do PWM
const unsigned long ciclo_padrao = 1500; // Ciclo padrão, base de cálculo dos ciclos
const unsigned long ciclo_minimo = 900; // Limite mínimo aceito para um ciclo
const unsigned long ciclo_maximo = 2000; // Limite máximo aceito para um ciclo
// MANDÍBULA
const unsigned long inicio_mandibula = 550; // Tempo de entrada da função Mandíbula
const unsigned long fim_mandibula = 1500; // Tempo de saída da função Mandíbula
// FOTOCÉLULA
const unsigned long inicio_fotocelula = 1250; // Tempo de entrada da liberação da Fotocélula
const unsigned long fim_fotocelula = 1400; // Tempo de saída da liberação da Fotocélula
// FACA
const unsigned long inicio_faca = 1200; // Tempo de entrada da função Faca
const unsigned long fim_faca = 1400; // Tempo de saída da função Faca
// REFRIGERAÇÃO
const unsigned long inicio_refrigeracao = 800; // Tempo de entrada da função Refrigeração
const unsigned long fim_refrigeracao = 1450; // Tempo de saída da função Refrigeração
// DATADOR
const boolean datador_contra_mandibula = VERDADEIRO; // FALSO => Temporizado
const unsigned long inicio_datador = 0; // Tempo de entrada da função Datador
const unsigned long fim_datador = 400; // Tempo de saída da função Datador
// SOLDAS VERTICAL E HORIZONTAL
const boolean vertical_contra_mandibula = VERDADEIRO; // FALSO => Temporizado
const unsigned long inicio_vertical = 0; // Tempo de entrada da função Solda Vertical
const unsigned long fim_vertical = 400; // Tempo de saída da função Solda Vertical

// REMOVER >>
const byte saida_teste = 3;
const byte entrada_teste = 4;
// << REMOVER

// FIM DAS CONFIGURAÇÕES


// VARIÁVEIS
// BOTÕES UM-CLIQUE
unsigned long atraso = 50;
int estbt_geral;                // Estado do Botão Geral
int estbta_geral = BAIXO;       // Estado anterior do Botão Geral
unsigned long atr_geral = 0;    // Atraso do Botão Geral
int estbt_dosador;              // Estado do Botão Dosador
int estbta_dosador = BAIXO;     // Estado anterior do Botão Dosador
unsigned long atr_dosador = 0;  // Atraso do Botão Dosador
int estbt_datador;              // Estado do Botão Datador
int estbta_datador = BAIXO;     // Estado anterior do Botão Datador
unsigned long atr_datador = 0;  // Atraso do Botão Datador

// PWM DAS SOLDAS
unsigned long tempo_PWM_vertical = 0;
unsigned long tempo_PWM_horizontal = 0;
unsigned long tempo_PWM_datador = 0;

// MODOS DE TRABALHO
boolean stand_by = FALSO;
boolean maquina_ligada = FALSO;
boolean dosador_ligado = FALSO;
boolean datador_ligado = FALSO;
boolean desligamento_suave = FALSO;
unsigned long desligarEm = 0;
boolean alarme_ativo = FALSO;
boolean fotocelula_liberada = FALSO;
boolean fotocelula_cortou = FALSO;
boolean produzindo = FALSO;
boolean resetado = FALSO;

// TEMPOS DE CICLO
// CONTROLE DE CICLO
unsigned long tempo_atual = millis();
unsigned long novo_ciclo_padrao = ciclo_padrao;
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
unsigned long ti_mandibula = inicio_mandibula;
unsigned long tf_mandibula = fim_mandibula;
// FOTOCÉLULA
unsigned long ti_fotocelula = inicio_fotocelula;
unsigned long tf_fotocelula = fim_fotocelula;
// FACA
unsigned long ti_faca = inicio_faca;
unsigned long tf_faca = fim_faca;
// REFRIGERAÇÃO
unsigned long ti_refrigeracao = inicio_refrigeracao;
unsigned long tf_refrigeracao = fim_refrigeracao;
// DATADOR
unsigned long ti_datador = inicio_datador;
unsigned long tf_datador = fim_datador;
// SOLDA VERTICAL
unsigned long ti_vertical = inicio_vertical;
unsigned long tf_vertical = fim_vertical;

void setup() {
  Serial.begin(9600);
  // BOTÕES
  pinMode(bt_geral, BOTAO);
  pinMode(bt_dosador, BOTAO);
  pinMode(bt_datador, BOTAO);
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
  pinMode(sensor_reset, BOTAO); // Mudar para Sensor após os testes.
  pinMode(sensor_mandibula, BOTAO); // Mudar para Sensor após os testes.
  pinMode(fotocelula, BOTAO); // Mudar para Sensor após os testes.
  pinMode(sensor_porta, BOTAO); // Mudar para Sensor após os testes.
  pinMode(sensor_painel, BOTAO); // Mudar para Sensor após os testes.
  pinMode(sensor_maquina, BOTAO); // Mudar para Sensor após os testes.
  // SAÍDAS
  pinMode(geral, SAIDA);
  pinMode(dosador, SAIDA);
  pinMode(led_datador, SAIDA);
  pinMode(faca, SAIDA);
  pinMode(refrigeracao, SAIDA);
  pinMode(datador, SAIDA);
  pinMode(vertical, SAIDA);
  // ESTADO INICIAL DAS SAÍDAS
  resetCompleto();


  // REMOVER >>
  pinMode(saida_teste, SAIDA);
  pinMode(entrada_teste, SENSOR);
  // << REMOVER
}

void loop() {
  modoAlarme();
  standBy();
}

// MODOS DE TRABALHO

void standBy() {
  if (!alarme_ativo){
    if (stand_by) {
      acionaGeral();    // Monitora botão Geral
      acionaDosador();  // Monitora botão Dosador
      acionaDatador();  // Monitora botão Datador
      geraPWM(pot_solda_vertical, &tempo_PWM_vertical, solda_vertical_PWM);
      geraPWM(pot_solda_horizontal, &tempo_PWM_horizontal, solda_horizontal_PWM);
      geraPWM(pot_solda_datador, &tempo_PWM_datador, solda_datador_PWM);
      if (maquina_ligada || desligamento_suave) {
        iniciaTrabalho(); // Inicia Ciclos de Produção
      }
    } else {
      escreveSerial("Preparando para iniciar trabalho.");
      delay(3000);            // Espera 3 segundos após energização
      stand_by = VERDADEIRO;  // Libera funcionamento
      escreveSerial("Tudo pronto!");
    }
  }
}

void iniciaTrabalho() {
  if (!produzindo) {
    produzindo = VERDADEIRO;
    reiniciaCiclo(millis());
  }
  if (conta_ciclos >= 3) {
    liberaFotocelula();
    leFotocelula();
    funcaoSimples(mandibula, NomeMandibula, ti_mandibula, tf_mandibula);
    funcaoSegura(faca, NomeFaca, ti_faca, tf_faca, sensor_mandibula);
    funcaoSimples(refrigeracao, NomeRefrigeracao, ti_refrigeracao, tf_refrigeracao);
    funcaoComLiberacao(datador, NomeDatador, ti_datador, tf_datador, datador_ligado, datador_contra_mandibula);
    funcaoBloqueante(vertical, NomeVertical, ti_vertical, tf_vertical, vertical_contra_mandibula);
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
    } else if (ligado(faca) && inativo(sensor_mandibula)){
      bloqueioPorAlarme("Sensor da Mandíbula2");
    }
  }
}

// FUNÇÕES ADICIONAIS

void bloqueioPorAlarme(String texto) {
  alarme_ativo = VERDADEIRO;
  stand_by = FALSO;
  escreveSerial("\n<< Alarme! >>\n");
  resetCompleto();
  escreveSerial(texto + " detectou uma falha!");
}

void acionaGeral() {
  btUmClique(bt_geral, &estbt_geral, &estbta_geral, &atr_geral,
             &maquina_ligada);
  if (maquina_ligada) {
    ligaFuncao(geral, NomeGeral);
  } else if (ligado(geral) && !desligamento_suave){
    desligamento_suave = VERDADEIRO;
    escreveSerial("Fazendo desligamento suave.");
  } else if (desligamento_suave && conta_ciclos == desligarEm) {
    desligaFuncao(geral, NomeGeral);
    resetCompleto();
  }
}

void acionaDosador() {
  btUmClique(bt_dosador, &estbt_dosador, &estbta_dosador, &atr_dosador,
             &dosador_ligado);
  if (dosador_ligado && !desligamento_suave && (!maquina_ligada || conta_ciclos >= 3)) {
    ligaFuncao(dosador, NomeDosador);
  } else if (!dosador_ligado) {
    desligaFuncao(dosador, NomeDosador);
  }
}

void acionaDatador() {
  if (maquina_ligada) {
    btUmClique(bt_datador, &estbt_datador, &estbta_datador, &atr_datador,
               &datador_ligado);
    if (datador_ligado) {
      ligaFuncao(led_datador, NomeDatador);
    } else {
      desligaFuncao(led_datador, NomeDatador);
    }
  }
}

void funcaoReset() {
  tempo_atual = millis();
  if (resetado){
    fim_ciclo = inicio_ciclo + novo_ciclo_padrao;
    minimo = inicio_ciclo + ciclo_minimo;
    maximo = inicio_ciclo + ciclo_maximo;
    reiniciaSaidas();
    resetado = FALSO;
  }

  if (tempo_atual >= minimo) {
    if (ativo(sensor_reset) || inativo(entrada_teste) || tempo_atual == maximo) {
      conta_ciclos++;
      ultimo_ciclo = tempo_atual - inicio_ciclo;
      soma_ciclos += (tempo_atual - inicio_ciclo);
      reiniciaCiclo(tempo_atual);
      reiniciaSaidas();
      escreveSerial("\t" + (String)conta_ciclos + " " +
                    (String)ultimo_ciclo + " " +
                    (String)novo_ciclo_padrao + " " + (String)tempo_atual + " " +
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
  if (desligamento_suave){
    if (dosador_ligado){
      dosador_ligado = FALSO;
      desligaFuncao(dosador, NomeDosador);
      desligarEm = conta_ciclos + 1;
    } else {
      desligarEm = conta_ciclos;
    }
  }
}

void cicloMedio() {
  if (conta_ciclos > 0) {
    ciclo_padrao_anterior = novo_ciclo_padrao;
    novo_ciclo_padrao = ultimo_ciclo; // soma_ciclos / conta_ciclos;
    // AJUSTE DINÂMICO DOS TEMPOS DAS FUNÇÕES
    ajustaCiclo(&ti_mandibula, inicio_mandibula);
    ajustaCiclo(&tf_mandibula, fim_mandibula);
    ajustaCiclo(&ti_faca, inicio_faca);
    ajustaCiclo(&tf_faca, fim_faca);
    ajustaCiclo(&ti_refrigeracao, inicio_refrigeracao);
    ajustaCiclo(&tf_refrigeracao, fim_refrigeracao);
    ajustaCiclo(&ti_datador, inicio_datador);
    ajustaCiclo(&tf_datador, fim_datador);
    ajustaCiclo(&ti_vertical, inicio_vertical);
    ajustaCiclo(&tf_vertical, fim_vertical);
  }
}

void ajustaCiclo(unsigned long *variavel, unsigned long original){
  unsigned long tempo = (original * ciclo_padrao_anterior) / ciclo_padrao;
  if (tempo <= 0){ // Evita um problema com acionamentos em tempo 0
    *variavel = 1;
  } else {
    *variavel = tempo;
  }
}

void geraPWM(byte pot, unsigned long *inicio, byte saida) {
  unsigned long ativo = *inicio + map(analogRead(pot), 0, 1023, 0, ciclo_PWM);
  unsigned long inativo = ciclo_PWM - ativo;
  unsigned long pwm = *inicio + ciclo_PWM;
  if (maquina_ligada) {
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
  if (desligado(saida) && !fotocelula_cortou){
    if (tempo_atual >= inicio && tempo_atual < fim) {
      ligaFuncao(saida, nome);
    }
  } else if (tempo_atual >= fim) {
    desligaFuncao(saida, nome);
  }
}

void funcaoSegura(const byte saida, String nome, unsigned long inicio, unsigned long fim, byte seguranca){
  inicio += inicio_ciclo;
  fim += inicio_ciclo;
  if (desligado(saida)){
    if (tempo_atual >= inicio && tempo_atual < fim) {
      if (ativo(seguranca)){
        ligaFuncao(saida, nome);
      } else {
        bloqueioPorAlarme("Sensor da Mandibula");
      }
    }
  } else if (tempo_atual >= fim || inativo(seguranca)) {
    desligaFuncao(saida, nome);
  }
}

void funcaoBloqueante(const byte saida, String nome, unsigned long inicio, unsigned long fim, boolean contra_mandibula){
  if (contra_mandibula){
    if (ativo(sensor_mandibula)){
      desligaFuncao(saida, nome);
    } else {
      ligaFuncao(saida, nome);
    }
  } else {
    if (ativo(sensor_mandibula)){
      desligaFuncao(saida, nome);
    } else {
      funcaoSimples(saida, nome, inicio, fim);
    }
  }
}

void funcaoComLiberacao(const byte saida, String nome, unsigned long inicio, unsigned long fim, byte liberacao, boolean contra_mandibula){
  if (liberacao){
    funcaoBloqueante(saida, nome, inicio, fim, contra_mandibula);
  } else {
    desligaFuncao(saida, nome);
  }
}

void liberaFotocelula() {
  unsigned long inicio = ti_fotocelula + inicio_ciclo;
  unsigned long fim = tf_fotocelula + inicio_ciclo;
  if (!fotocelula_cortou){
    if (tempo_atual >= inicio && tempo_atual < fim) {
      fotocelula_liberada = VERDADEIRO;
    }
  } else if (tempo_atual >= fim) {
    fotocelula_liberada = FALSO;
  }
}

void leFotocelula(){
  if (fotocelula_liberada && !fotocelula_cortou){
    if (ativo(fotocelula)){
      fotocelula_cortou = VERDADEIRO;
      escreveSerial("Fotocelula fez o corte.");
      reiniciaSaidas();
    }
  }
}

// FERRAMENTAS AUXILIARES

byte lePino(byte pino) { return digitalRead(pino); }

void liga(byte saida) { digitalWrite(saida, LIGA); }

void ligaFuncao(byte saida, String nome) {
  if (desligado(saida) && !alarme_ativo) {
    liga(saida);
    if (ligado(saida)){
      escreveSerial(">> " + nome + " Ligado.");
    } else {
      escreveSerial("Ocorreu um erro ao ligar " + nome);
    }
  }
}

void desliga(byte saida) { digitalWrite(saida, DESLIGA); }

void desligaFuncao(byte saida, String nome) {
  if (ligado(saida)) {
    desliga(saida);
    escreveSerial("<< " + nome + " Desligado.");
  }
}

boolean ligado(byte saida) {
  if (lePino(saida) == LIGA) {
    return VERDADEIRO;
  } else {
    return FALSO;
  }
}

boolean desligado(byte saida) {
  if (lePino(saida) == DESLIGA) {
    return VERDADEIRO;
  } else {
    return FALSO;
  }
}

boolean ativo(byte entrada) {
  if (lePino(entrada) == ALTO) {
    return VERDADEIRO;
  } else {
    return FALSO;
  }
}

boolean inativo(byte entrada) {
  if (lePino(entrada) == BAIXO) {
    return VERDADEIRO;
  } else {
    return FALSO;
  }
}

void btUmClique(byte botao, int *estado, int *est_ant, unsigned long *atr_ant,
                boolean *funcao) {
  byte estado_atual = lePino(botao);
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
  desligaFuncao(mandibula, NomeMandibula);
  desligaFuncao(faca, NomeFaca);
  desligaFuncao(refrigeracao, NomeRefrigeracao);
  if (!datador_contra_mandibula){
    desligaFuncao(datador, NomeDatador);
  }
  if (!vertical_contra_mandibula){
    desligaFuncao(vertical, NomeVertical);
  }
}

void resetCompleto() {
  conta_ciclos = 0;
  maquina_ligada = FALSO;
  dosador_ligado = FALSO;
  datador_ligado = FALSO;
  desligamento_suave = FALSO;
  desligarEm = 0;
  fotocelula_liberada = FALSO;
  fotocelula_cortou = FALSO;
  produzindo = FALSO;
  resetado = FALSO;
  desliga(geral);
  desliga(dosador);
  desliga(led_datador);
  desliga(mandibula);
  desliga(faca);
  desliga(refrigeracao);
  desliga(datador);
  desliga(vertical);

  desliga(solda_vertical_PWM);
  desliga(solda_horizontal_PWM);
  desliga(solda_datador_PWM);
}
