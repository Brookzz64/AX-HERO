//--------------------------------------------------------------
//INICIALIZAÇÕES
//--------------------------------------------------------------

//iniciando o LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(32, 16, 2);
// Criando botões
const int botoes[] = {3,4,5,6};
volatile bool acionouInterrupcao = false;
//Criando estado, para funcionar a lógica do on/off
int estadoJogo = 0;

//--------------------------------------------------------------
//AREA DE SETUP
//--------------------------------------------------------------

void setup() {
  //LEDs vermelho/verde
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  //Switch
  pinMode(A1, INPUT);
  //Buzzer
  pinMode(A0, OUTPUT);
  //iniciando para deixar o lcd ligado(nada escrito)
  lcd.init();          
  lcd.backlight();  
  //Iniciando o serial, para debugs
  Serial.begin(9600);
  //iniciando botões
  for (int i = 0; i < 4; i++) {
    pinMode(botoes[i], INPUT);}
    pinMode(2, INPUT);
    attachInterrupt(digitalPinToInterrupt(2), atenderCampainha, RISING);
  
}

//--------------------------------------------------------------
//MAIN CODE
//--------------------------------------------------------------

void loop() {
  //lê interruptores constantemente, para fazer a função correta
  bool onoff = digitalRead(A1);
  
  if(onoff == LOW){
    if(estadoJogo != 0){
      //ajustes dos LEDs
      digitalWrite(11, HIGH);
      digitalWrite(12, LOW);
 	  LCDDesligando(); 
      
      estadoJogo = 0;
    }
  }
  if(onoff == HIGH && estadoJogo == 0){
      //ajuste dos LEDs
      digitalWrite(11, LOW);
      digitalWrite(12, HIGH);
      LCDLigando();
      estadoJogo = 2;
      }
  if(estadoJogo == 2){
    
    if (acionouInterrupcao) {
      verificarQualBotao();
      Serial.println("ativo");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("iniciando o jogo");
      delay(1000);
      acionouInterrupcao = false;
      estadoJogo = 3;
    }
  }
}

//--------------------------------------------------------------
//FUNÇÕES INDEPENDENTES
//--------------------------------------------------------------


void atenderCampainha() {
  acionouInterrupcao = true; 
}


void verificarQualBotao() {
   delay(50); 

  for (int i = 0; i < 4; i++) {
    if (digitalRead(botoes[i]) == HIGH) {
      Serial.print("Botao pressionado no pino: ");
      Serial.println(botoes[i]);
     
      executarAcao(i);
    }
  }
}

void executarAcao(int indice) {
  switch(indice) {
    case 0: Serial.println("Acao: Abrir Garagem"); break;
    case 1: Serial.println("Acao: Ligar Luz"); break;
    case 2: Serial.println("Acao: Tocar Alarme"); break;
    case 3: Serial.println("Acao: Resetar Sistema"); break;
  }
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
  
  
