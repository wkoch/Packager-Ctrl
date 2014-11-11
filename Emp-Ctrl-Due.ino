/*
 Emp-Ctrl-Due
 V1.1 21/09/2014 William Koch

 Controlador de Empacotadeira com Arduino Due.
*/

#include <SlowPWM.h>
#include "Estruturas.h"
#include "Config.h"
#include "Setup.h"
#include "Auxiliares.h"
#include "Funcoes.h"


void setup() {
  configurar();
  configura_pinos();
}

void loop() {
  Seguranca();
  StandBy();
}
