/*
 Emp-Ctrl-Due
 V1.1 21/09/2014 William Koch

 Funções de Setup.
*/



void comunicacao_serial();
void configura_es(funcoes *funcao);
void configura_pinos();

void comunicacao_serial(){
  if (modo.manutencao){
    Serial.begin(9600);
  }
}

void configura_pinos(){
  comunicacao_serial();
  configura_es(&geral);
  configura_es(&dosador);
  configura_es(&datador);
  configura_es(&mandibula);
  configura_es(&fotocelula);
  configura_es(&faca);
  configura_es(&refrigeracao);
  configura_es(&soldas);
}

void configura_es(funcoes *funcao){
  if (funcao->pino.entrada > 0){
    pinMode(funcao->pino.entrada, ENTRADA);
  }
  if (funcao->pino.saida > 0){
    pinMode(funcao->pino.saida, SAIDA);
  }
  if (funcao->pino.led > 0){
    pinMode(funcao->pino.led, SAIDA);
  }
}
