/*
 Emp-Ctrl-Due
 V1.1 21/09/2014 William Koch

 Estruturas de dados.
*/


// ESTRUTURAS
typedef struct pinos {
  byte entrada, saida, led;
};

typedef struct tempos {
  unsigned long liga, desliga;
  boolean contrario;
};

typedef struct atrasos {
  unsigned long atual, anterior;
};

typedef struct estados {
  boolean liberado, ativo;
  byte atual, anterior;
};

typedef struct pwm {
  unsigned long ciclo;
  struct pinos pino;
};

typedef struct funcoes {
  char nome;
  struct pinos pino;
  struct tempos tempo;
  struct atrasos atraso;
  struct estados estado;
  boolean proteger;
};

typedef struct modos {
  boolean standby, producao, desligamento, alarme, manutencao;
};
