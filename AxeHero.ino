//--------------------------------------------------------------
//INICIALIZAÇÕES
//--------------------------------------------------------------

//iniciando o LCD
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#define NUM_LEDS 8 // Quantidade de LEDs por faixa

// Criando os objetos para as 4 faixas nas portas 7, 8, 9 e 10
Adafruit_NeoPixel faixa1(NUM_LEDS, 7, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel faixa2(NUM_LEDS, 8, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel faixa3(NUM_LEDS, 9, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel faixa4(NUM_LEDS, 10, NEO_GRB + NEO_KHZ800);




LiquidCrystal_I2C lcd(32, 16, 2);
// Criando botões
const int botoes[] = {3,4,5,6};
volatile bool acionouInterrupcao = false;
//Criando estado, para funcionar a lógica do on/off
int estadoJogo = 0;
int X0 = 0, X1 = 0, X2 = 0, X3 = 0;
int musc = 0, dif = 0, pts = 100;

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
  //iniciando as faixas
  faixa1.begin();
  faixa2.begin();
  faixa3.begin();
  faixa4.begin();
  //faixas desligadas
  faixa1.show();
  faixa2.show();
  faixa3.show();
  faixa4.show();
  //brilho de 0 255
  faixa1.setBrightness(50);
  faixa2.setBrightness(50);
  faixa3.setBrightness(50);
  faixa4.setBrightness(50);
}

//--------------------------------------------------------------
//MAIN CODE
//--------------------------------------------------------------


//LIGANDO OU DESLIGANDO
//-------------------------------
void loop() {
  //lê interruptores constantemente, para fazer a função correta
  bool onoff = digitalRead(A1);
  Serial.println(estadoJogo);
  
  if(onoff == LOW){
    if(estadoJogo != 0){
      //ajustes dos LEDs
      digitalWrite(11, HIGH);
      digitalWrite(12, LOW);
 	  LCDDesligando(); 
      //muda o estado do jogo
      estadoJogo = 0;
    }
  }
  if(onoff == HIGH && estadoJogo == 0){
      //ajuste dos LEDs
      digitalWrite(11, LOW);
      digitalWrite(12, HIGH);
      //animação de ligar
      LCDLigando();
      //muda o estado do jogo
      estadoJogo = 2;
      }
  
// MENU DO JOGO
//-----------------------
    if (acionouInterrupcao && estadoJogo == 2) {
      //função externa, verifica qual botão apertado
      verificarQualBotao();
      //menu iniciado
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bem Vindo!      ");
      delay(500);
      lcd.setCursor(0, 1);
      lcd.print("Escolha a musica");
      acionouInterrupcao = false;
      //muda o estado do jogo
      estadoJogo = 3;
        
    }if(acionouInterrupcao && estadoJogo == 3){
        //printa o nome das musicas, deixando um caractere no começo, para a seleção
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" Segura o Tchan ");
        delay(1000);
        lcd.setCursor(0, 1);
        lcd.print(" Arere          ");
        acionouInterrupcao = false;
        //ajeitando as variáveis, para previnir erro ao reiniciar o loop
      	X0 = 1;
      	X1 = 0;
      	X2 = 0;
      	X3 = 0;
        //muda o estado do jogo
        estadoJogo = 4;
      
     
        
        
    }if(estadoJogo == 4){
      //novamente verifica o botão pressionado
      if(acionouInterrupcao){	
        verificarQualBotao();}
      	  //quando o botão seta pra cima, ele muda a musica selecionada, para cima
          if(X0>0){
          	lcd.setCursor(0, 0);
        	lcd.print("-");
            lcd.setCursor(0, 1);
        	lcd.print(" ");
        	X0 = 0;
          	musc = 1;}
          //quando o botão seta para baixo, ele muda a musica selecionada, para baixo
   	 	  if(X1>0){
            lcd.setCursor(0, 0);
        	lcd.print(" ");
            lcd.setCursor(0, 1);
        	lcd.print("-");
            X1 = 0;
          	musc = 2;}
          // se o botão X for acionado, volta ao menu
      	  if(X2>0){
            estadoJogo = 0;}
      	  //se o botão v for acionado, seleciona a música
      	  if(X3>0){
            if(musc == 1){ estadoJogo = 5;}
            if(musc == 2){ estadoJogo = 8;}
            
          }acionouInterrupcao = false;
      
      
     
    }if(estadoJogo == 5){
      	// printa as dificuldades para a escolha do jogador
      	lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Selecione o modo de jogo");
        delay(1000);
        lcd.setCursor(0, 1);
        lcd.print("-Facil  Dificil ");
      	acionouInterrupcao = false;
      	//ajeitando as variáveis, para evitar erros ao reiniciar
      	X0 = 1;
      	dif = 1;
      	X2 = 0;
        X3 = 0;
      	//mudando o estado do jogo
      	estadoJogo = 6;
      
      
    }if(estadoJogo == 6){
      //novamente verifica o botão pressionado
      if(acionouInterrupcao){	
        verificarQualBotao();}
      	  //quando o botão seta pra cima, ele muda a musica selecionada, para cima
          if(X0>0){
          	lcd.setCursor(0, 1);
        	lcd.print("-");
            lcd.setCursor(7, 1);
        	lcd.print(" ");
        	X0 = 0;
          	dif = 1;}
          //quando o botão seta para baixo, ele muda a musica selecionada, para baixo
   	 	  if(X1>0){
            lcd.setCursor(0, 1);
        	lcd.print(" ");
            lcd.setCursor(7, 1);
        	lcd.print("-");
            X1 = 0;
          	dif = 2;}
          // se o botão X for acionado, volta a escolha de musica
      	  if(X2>0){
            estadoJogo = 3;}
      	  //se o botão v for acionado, seleciona a música
      	  if(X3>0){
            //Jogo6 = segura o tchan facil
            if(dif = 1){estadoJogo = 7;}
            //Jogo7 = segura o tchan dificil
            if(dif = 2){estadoJogo = 8;}
            
          }acionouInterrupcao = false;
      
    }if(estadoJogo == 9){
      // printa as dificuldades para a escolha do jogador
      	lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Selecione o modo de jogo");
        delay(1000);
        lcd.setCursor(0, 1);
        lcd.print("-Facil  Dificil ");
      	acionouInterrupcao = false;
      	//ajeitando as variáveis, para evitar erros ao reiniciar
      	X0 = 0;
      	X1 = 0;
      	dif = 0;
      	X2 = 0;
        X3 = 0;
      	//mudando o estado do jogo
      	estadoJogo = 10;
            
    }if(estadoJogo == 10){
      //novamente verifica o botão pressionado
      if(acionouInterrupcao){	
        verificarQualBotao();}
      	  //quando o botão seta pra cima, ele muda a musica selecionada, para cima
          if(X0>0){
          	lcd.setCursor(0, 1);
        	lcd.print("-");
            lcd.setCursor(7, 1);
        	lcd.print(" ");
        	X0 = 0;
          	dif = 1;}
          //quando o botão seta para baixo, ele muda a musica selecionada, para baixo
   	 	  if(X1>0){
            lcd.setCursor(0, 1);
        	lcd.print(" ");
            lcd.setCursor(7, 1);
        	lcd.print("-");
            X1 = 0;
          	dif = 2;}
          // se o botão X for acionado, volta a escolha de musica
      	  if(X2>0){
            estadoJogo = 3;}
      	  //se o botão v for acionado, seleciona a música
      	  if(X3>0){
            //Jogo10 = arere facil
            if(dif = 1){estadoJogo = 11;}
            //Jogo11 = arere dificil
            if(dif = 2){estadoJogo = 12;}
            
          }acionouInterrupcao = false;
      
    }
     
// JOGO FUNICONANDO
//--------------------
  //SEGURA O TCHAN(FÁCIL)
  if(estadoJogo == 7){
    lcd.setCursor(0, 0);
    lcd.print("PONTOS:         ");
    lcd.setCursor(0, 1);
    lcd.print("VIDAS: III      ");
    lcd.setCursor(8,0);
    lcd.print(pts);
  }
 
}//FECHA O LOOP

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
      executarAcao(i);
    }
  }
}

void executarAcao(int indice) {
  switch(indice) {
    case 0: X0++; break;
    case 1: X1++; break;
    case 2: X2++; break;
    case 3: X3++; break;
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
      delay(50);
      lcd.setCursor(0, 0); 
      lcd.print("--            --");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delay(50);
      lcd.setCursor(0, 0); 
      lcd.print("---          ---");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delay(50);
      lcd.setCursor(0, 0); 
      lcd.print("----        ----");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delay(200);
      lcd.setCursor(0, 0);
      lcd.print("----AXE     ----");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delay(200);
      lcd.setCursor(0, 0);
      lcd.print("----AXE HERO----");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delay(500); 
      lcd.setCursor(0, 0);
      lcd.print("----AXE HERO----");
      lcd.setCursor(0, 1);
      lcd.print("aperte um botao.");
}
  
  
