/*
 EMP-B Due - Configurações
 V0.1 21/09/2014 William Koch

 Todas as opções de configurações do programa.
*/


// CONFIGURAÇÕES

// GERAL
#define ENTRADA INPUT_PULLUP  // INPUT ou INPUT_PULLUP
// #define BOTAO INPUT_PULLUP  // INPUT ou INPUT_PULLUP
// #define SENSOR INPUT        // INPUT ou INPUT_PULLUP
#define SAIDA OUTPUT
#define ALTO LOW    // Deve ser invertido para INPUT_PULLUP
#define BAIXO HIGH  // Deve ser invertido para INPUT_PULLUP
#define LIGA HIGH
#define DESLIGA LOW
#define VERDADEIRO true
#define FALSO false

// PWM SOLDA VERTICAL
struct pwm solda_vertical {
      .ciclo = 1000,
      .pino.entrada = A0,
      .pino.saida = 33};

// PWM SOLDA HORIZONTAL
struct pwm solda_horizontal {
      .ciclo = 1000,
      .pino.entrada = A1,
      .pino.saida = 35};

// PWM SOLDA DATADOR
struct pwm solda_datador {
      .ciclo = 1000,
      .pino.entrada = A2,
      .pino.saida = 37};

// FUNÇÕES DE PRODUÇÃO

// GERAL
struct funcoes geral {
      .nome = "Geral",
      .pino.entrada = 22,
      .pino.saida = 23,
      .pino.led = 0,
      .tempo.liga = 0,
      .tempo.desliga = 0,
      .tempo.contrario = FALSO,
      .atraso.atual = 0,
      .atraso.anterior = 0,
      .estado.liberado = FALSO,
      .estado.ativo = FALSO,
      .estado.atual = BAIXO,
      .estado.anterior = BAIXO,
      .proteger = FALSO};

// DOSADOR
struct funcoes dosador {
      .nome = "Dosador",
      .pino.entrada = 24,
      .pino.saida = 25,
      .pino.led = 0,
      .tempo.liga = 0,
      .tempo.desliga = 0,
      .tempo.contrario = FALSO,
      .atraso.atual = 0,
      .atraso.anterior = 0,
      .estado.liberado = FALSO,
      .estado.ativo = FALSO,
      .estado.atual = BAIXO,
      .estado.anterior = BAIXO,
      .proteger = FALSO};

// DATADOR
struct funcoes datador {
      .nome = "Datador",
      .pino.entrada = 26,
      .pino.saida = 46,
      .pino.led = 27,
      .tempo.liga = 0,
      .tempo.desliga = 400,
      .tempo.contrario = FALSO,
      .atraso.atual = 0,
      .atraso.anterior = 0,
      .estado.liberado = FALSO,
      .estado.ativo = FALSO,
      .estado.atual = BAIXO,
      .estado.anterior = BAIXO,
      .proteger = FALSO};

// MANDÍBULA
struct funcoes mandibula {
      .nome = "Mandíbula",
      .pino.entrada = 0,
      .pino.saida = 40,
      .pino.led =41,
      .tempo.liga = 550,
      .tempo.desliga = 1500,
      .tempo.contrario = FALSO,
      .atraso.atual = 0,
      .atraso.anterior = 0,
      .estado.liberado = FALSO,
      .estado.ativo = FALSO,
      .estado.atual = BAIXO,
      .estado.anterior = BAIXO,
      .proteger = FALSO};

// FOTOCÉLULA
struct funcoes fotocelula {
      .nome = "Fotocélula",
      .pino.entrada = 6,
      .pino.saida = 0,
      .pino.led = 7,
      .tempo.liga = 1250,
      .tempo.desliga = 1400,
      .tempo.contrario = FALSO,
      .atraso.atual = 0,
      .atraso.anterior = 0,
      .estado.liberado = FALSO,
      .estado.ativo = FALSO,
      .estado.atual = BAIXO,
      .estado.anterior = BAIXO,
      .proteger = FALSO};

// FACA
struct funcoes faca {
      .nome = "Faca",
      .pino.entrada = 2,
      .pino.saida = 42,
      .pino.led = 43,
      .tempo.liga = 600,
      .tempo.desliga = 800,
      .tempo.contrario = FALSO,
      .atraso.atual = 0,
      .atraso.anterior = 0,
      .estado.liberado = FALSO,
      .estado.ativo = FALSO,
      .estado.atual = BAIXO,
      .estado.anterior = BAIXO,
      .proteger = VERDADEIRO};

// REFRIGERAÇÃO
struct funcoes refrigeracao {
      .nome = "Refrigeração",
      .pino.entrada = 0,
      .pino.saida = 44,
      .pino.led = 45,
      .tempo.liga = 800,
      .tempo.desliga = 1450,
      .tempo.contrario = FALSO,
      .atraso.atual = 0,
      .atraso.anterior = 0,
      .estado.liberado = FALSO,
      .estado.ativo = FALSO,
      .estado.atual = BAIXO,
      .estado.anterior = BAIXO,
      .proteger = VERDADEIRO};

// SOLDAS HORIZONTAL E VERTICAL
struct funcoes soldas {
      .nome = "Soldas",
      .pino.entrada = 0,
      .pino.saida = 48,
      .pino.led = 49,
      .tempo.liga = 0,
      .tempo.desliga = 400,
      .tempo.contrario = FALSO,
      .atraso.atual = 0,
      .atraso.anterior = 0,
      .estado.liberado = FALSO,
      .estado.ativo = FALSO,
      .estado.atual = BAIXO,
      .estado.anterior = BAIXO,
      .proteger = FALSO};

// MODOS DE FUNCIONAMENTO
struct modos modo {
      .standby = FALSO,
      .producao = FALSO,
      .desligamento = FALSO,
      .alarme = FALSO,
      .manutencao = VERDADEIRO};
