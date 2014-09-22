/*
 Emp-Ctrl-Due
 V1.1 21/09/2014 William Koch

 Funções auxiliares gerais.
*/



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
