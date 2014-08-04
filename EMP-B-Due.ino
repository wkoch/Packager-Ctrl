/*
 EMP-B Due
 V0.1 © 03/08/2014 William Koch

 Controlador de Empacotadeira com Arduino Due.
*/

// Constantes de Entradas
const byte botao_geral = 22;
int estado_botao_geral;
int estado_botao_geral_anterior = HIGH;
long atraso_geral_anterior = 0;

const byte indicador_geral = 25;
int estado_indicador_geral = LOW;


const byte botao_dosador = 26;
int estado_botao_dosador;
int estado_botao_dosador_anterior = HIGH;
long atraso_dosador_anterior = 0;

const byte indicador_dosador = 29;
int estado_indicador_dosador = LOW;



long atraso = 50;

void setup() {
  pinMode(botao_geral, INPUT_PULLUP);
  pinMode(botao_dosador, INPUT_PULLUP);

  pinMode(indicador_geral, OUTPUT);
  pinMode(indicador_dosador, OUTPUT);

  // set initial LED state
  digitalWrite(indicador_geral, estado_indicador_geral);
  digitalWrite(indicador_dosador, estado_indicador_dosador);
}


void loop() {
  botao_um_clique(botao_geral, &estado_botao_geral, &estado_botao_geral_anterior, &atraso_geral_anterior, indicador_geral, &estado_indicador_geral);
  botao_um_clique(botao_dosador, &estado_botao_dosador, &estado_botao_dosador_anterior, &atraso_dosador_anterior, indicador_dosador, &estado_indicador_dosador);
}


void botao_um_clique(byte botao, int *estado, int *estado_anterior, long *atraso_anterior, byte saida, int *estado_saida ){
  int reading = digitalRead(botao);
  if (reading != *estado_anterior) {
    *atraso_anterior = millis();
  }

  if ((millis() - *atraso_anterior) > atraso) {
    if (reading != *estado) {
      *estado = reading;
      if (*estado == LOW) {
        *estado_saida = !*estado_saida;
      }
    }
  }
  digitalWrite(saida, *estado_saida);
  *estado_anterior = reading;
}



// void botao_maquina(){
//   // read the state of the switch into a local variable:
//   int reading = digitalRead(buttonPin);

//   // check to see if you just pressed the button
//   // (i.e. the input went from LOW to HIGH),  and you've waited
//   // long enough since the last press to ignore any noise:

//   // If the switch changed, due to noise or pressing:
//   if (reading != lastButtonState) {
//     // reset the debouncing timer
//     lastDebounceTime = millis();
//   }

//   if ((millis() - lastDebounceTime) > debounceDelay) {
//     // whatever the reading is at, it's been there for longer
//     // than the debounce delay, so take it as the actual current state:

//     // if the button state has changed:
//     if (reading != buttonState) {
//       buttonState = reading;

//       // only toggle the LED if the new button state is HIGH
//       if (buttonState == LOW) {
//         ledState = !ledState;
//       }
//     }
//   }

//   // set the LED:
//   digitalWrite(ledPin, ledState);

//   // save the reading.  Next time through the loop,
//   // it'll be the lastButtonState:
//   lastButtonState = reading;
// }
