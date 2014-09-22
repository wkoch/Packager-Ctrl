/*
 EMP-B Due
 V0.9 21/09/2014 William Koch

 Controlador de Empacotadeira com Arduino Due.
*/

#include "Arduino.h"
#include "Estruturas.h"
#include "Config.h"
#include "Setup.h"
#include "Auxiliares.h"
#include "Funcoes.h"

void setup() {
  Comunicacao_Serial();
  Configura_Pinos();
}

void loop() {
  Seguranca();
  StandBy();
}
