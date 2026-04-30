//--------------------------------------------------------------
//INICIALIZAÇÕES
//--------------------------------------------------------------

//iniciando o LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(32, 16, 2);
// Criando variáveis
bool animacaoDesliga = false, animacaoLiga = true;
// Criando botões
const int botoes[] = {5,4,3,1};
volatile bool acionouInterrupcao = false;
//Criando estado, para funcionar a lógica do on/off
int estadoJogo = 0;


//--------------------------------------------------------------
//AREA DE SETUP
//--------------------------------------------------------------

void setup() {
  //LEDs vermelho/verde
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  //Switch
  pinMode(10, INPUT);
  //Buzzer
  pinMode(13, OUTPUT);
  //iniciando para deixar o lcd ligado(nada escrito)
  lcd.init();          
  lcd.backlight();  
  // iniciando botões
  for (int i = 0; i < 4; i++) {
   pinMode(botoes[i], INPUT);
  }
   pinMode(2, INPUT);
   attachInterrupt(digitalPinToInterrupt(2), acaoBotao, RISING);
  }

//--------------------------------------------------------------
//MAIN CODE
//--------------------------------------------------------------
  
void loop() {
  //lê o interruptor constantemente, para fazer a funçao certa
  bool onoff = digitalRead(10);
  
  
  // sistema desligado
  if(onoff == LOW){
    if(animacaoDesliga == true){
      //ajustes dos LEDs
      digitalWrite(12, HIGH);
      digitalWrite(11, LOW);
 	  LCDDesligando();
      //validador, para garantir que só vai acontecer 1 vez a animaçao
      animacaoDesliga = false;
      animacaoLiga = true;
    }
    
    
    //sistema ligado
    }else if(onoff == HIGH){
      if(animacaoLiga == true){
      //ajuste dos LEDs
      digitalWrite(12, LOW);
      digitalWrite(11, HIGH);
      LCDLigando();
      //validação para confirmar que ligou
      animacaoDesliga = true;
      animacaoLiga = false;
      } 
    	 if(acionouInterrupcao){
          	VerificarQualBotao();
         	acionouInterrupcao = false;
          }
      }
  		  
}

//--------------------------------------------------------------
//FUNÇÕES INDEPENDENTES
//--------------------------------------------------------------

void VerificarQualBotao(){
  for (int i = 0; i < 4; i++) {
    if (digitalRead(botoes[i]) == HIGH) {
      executarAcao(i);
    }
  }
}

void executarAcao(int indice){
  switch(indice){
    case 0:
    	break;
    case 1:
    	break;
    case 2:
    	break;
    case 3:
    	break;
  }
}

void acaoBotao() {
  acionouInterrupcao = true;
}


void LCDDesligando(){
  //animação de desligar
      lcd.setCursor(0, 0); 
      lcd.print("desligando...   ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delay(1000);
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
}

void LCDLigando(){
  //animação de ligar
      lcd.setCursor(0, 0); 
      lcd.print("-              -");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delay(100);
      lcd.setCursor(0, 0); 
      lcd.print("--            --");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delay(100);
      lcd.setCursor(0, 0); 
      lcd.print("---          ---");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delay(100);
      lcd.setCursor(0, 0); 
      lcd.print("----        ----");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delay(500);
      lcd.setCursor(0, 0);
      lcd.print("----AXE     ----");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delay(500);
      lcd.setCursor(0, 0);
      lcd.print("----AXE HERO----");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delay(1000); 
      lcd.setCursor(0, 0);
      lcd.print("----AXE HERO----");
      lcd.setCursor(0, 1);
      lcd.print("aperte um botao.");
}
  
