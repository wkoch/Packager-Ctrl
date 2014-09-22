/*
 EMP-B Due - Estrutura de Dados
 V0.1 21/09/2014 William Koch

 Estruturas de Dados.
*/

// ESTRUTURAS
struct pinos {
  byte entrada;
  byte saida;
  byte led;
};

struct tempos {
  unsigned long liga;
  unsigned long desliga;
  boolean contrario;
};

struct atrasos {
  unsigned long atual;
  unsigned long anterior;
};

struct estados {
  boolean liberado;
  boolean ativo;
  byte atual;
  byte anterior;
};

struct pwm {
  unsigned long ciclo;
  struct pinos pino;
};

struct funcoes {
  char nome;
  struct pinos pino;
  struct tempos tempo;
  struct atrasos atraso;
  struct estados estado;
  boolean proteger;
};

struct modos {
  boolean standby;
  boolean producao;
  boolean desligamento;
  boolean alarme;
  boolean manutencao;
};
