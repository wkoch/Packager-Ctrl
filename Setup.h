/*
 EMP-B Due - Setup
 V0.1 21/09/2014 William Koch

 Funções de setup.
*/


void Comunicacao_Serial();
void configura_es(funcoes *funcao);
void Configura_Pinos();

void Comunicacao_Serial(){
  if (VERDADEIRO == modo.manutencao){
    Serial.begin(9600);
  }
}

void Configura_Pinos(){
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
