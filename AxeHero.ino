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
int musc = 0, dif = 0, pts, ledAtual = 0, Vida, lcdVida;
unsigned long T0 = 0;
unsigned long intervalo;
void verificarHit(int botaoIndex, Adafruit_NeoPixel &faixa);


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
        	musc = 1;
            X0 = 0;}
          //quando o botão seta para baixo, ele muda a musica selecionada, para baixo
   	 	  if(X1>0){
            lcd.setCursor(0, 0);
        	lcd.print(" ");
            lcd.setCursor(0, 1);
        	lcd.print("-");
          	musc = 2;
            X1 = 0;}
          // se o botão X for acionado, volta ao menu
      	  if(X2>0){
            estadoJogo = 0;}
      	  //se o botão v for acionado, seleciona a música
      	  if(X3>0){
            if(musc == 1){ estadoJogo = 5;}
            if(musc == 2){ estadoJogo = 9;}
            
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
      	X1 = 0;
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
        	dif = 1;
            X0 = 0;}
          //quando o botão seta para baixo, ele muda a musica selecionada, para baixo
   	 	  if(X1>0){
            lcd.setCursor(0, 1);
        	lcd.print(" ");
            lcd.setCursor(7, 1);
        	lcd.print("-");
            dif = 2;
            X1 = 0;}
          // se o botão X for acionado, volta a escolha de musica
      	  if(X2>0){
            estadoJogo = 3;}
      	  //se o botão v for acionado, seleciona a música
      	  if(X3>0){
            //Jogo6 = segura o tchan facil
            if(dif == 1){estadoJogo = 7;}
            //Jogo7 = segura o tchan dificil
            if(dif == 2){estadoJogo = 8;}
            
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
            if(dif == 1){estadoJogo = 11;}
            //Jogo11 = arere dificil
            if(dif == 2){estadoJogo = 12;}
            
          }acionouInterrupcao = false;
      
    }
     
// JOGO FUNICONANDO
//--------------------
  
  
  //SEGURA O TCHAN(FÁCIL)
  if(estadoJogo == 7){
    // Se for a primeira vez entrando no estado, desenha a tela fixa
    static bool telaIniciada = false; 
    if(!telaIniciada){
        pts = 0;
      	Vida = 3;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("PONTOS: 0");
        lcd.setCursor(0, 1);
        lcd.print("VIDAS: 3");
        intervalo = 300; // Ajuste a velocidade aqui
        T0 = millis();
        telaIniciada = true;
    }

    faixa(); // Chama a função que move o LED sem travar o código

  	if(acionouInterrupcao) {
    	verificarQualBotao(); // Identifica qual X0..X3 foi apertado
    	verificarHit(0, faixa1); // Checa se o X0 acertou o timing da faixa1
      	verificarHit(1, faixa2);
      	verificarHit(2, faixa3);
      	verificarHit(3, faixa4);
  		}

      
}
  
  
    //SEGURA O TCHAN(DIFÍCIL)
    if(estadoJogo == 8){
    pts = 0;
    lcd.setCursor(0, 0);
    lcd.print("PONTOS:         ");
    lcd.setCursor(0, 1);
    lcd.print("VIDAS:          ");
    lcd.setCursor(8,0);
    lcd.print(pts);
    intervalo = 200;
    faixa(); 
    
    // Verificação de clique de botão (Exemplo)
    if(acionouInterrupcao) {
        if(digitalRead(botoes[0]) == HIGH && ledAtual == (NUM_LEDS - 1)) {
            pts += 10; // Acertou a nota no último LED!
        }
        acionouInterrupcao = false;}
    }
  
  
  
    //ARERE(FÁCIL)
    if(estadoJogo == 11){
    pts = 0;
    lcd.setCursor(0, 0);
    lcd.print("PONTOS:         ");
    lcd.setCursor(0, 1);
    lcd.print("VIDAS: III      ");
    lcd.setCursor(8,0);
    lcd.print(pts);
    intervalo = 200;
    faixa(); 
    
    // Verificação de clique de botão (Exemplo)
    if(acionouInterrupcao) {
        if(digitalRead(botoes[0]) == HIGH && ledAtual == (NUM_LEDS - 1)) {
            pts += 10; // Acertou a nota no último LED!
        }
        acionouInterrupcao = false;}
    }
  
  
    //SEGURA O TCHAN(DIFÍCIL)
    if(estadoJogo == 12){
    pts = 0;
    lcd.setCursor(0, 0);
    lcd.print("PONTOS:         ");
    lcd.setCursor(0, 1);
    lcd.print("VIDAS: III      ");
    lcd.setCursor(8,0);
    lcd.print(pts);
    intervalo = 200;
    faixa(); 
    
    // Verificação de clique de botão (Exemplo)
    if(acionouInterrupcao) {
        if(digitalRead(botoes[0]) == HIGH && ledAtual == (NUM_LEDS - 1)) {
            pts += 10; // Acertou a nota no último LED!
        }
        acionouInterrupcao = false;}
    
  }
  //DERROTA
  if(estadoJogo == 13){
    lcd.setCursor(0, 0);
    lcd.print("  -VOCE PERDEU- ");
    lcd.setCursor(0, 1);
    lcd.print("PONTOS:         ");
    lcd.setCursor(8,1);
    lcd.print(pts);
    delay(3000);
    estadoJogo = 0;
    acionouInterrupcao = false;
  }
}//FECHA O LOOP

//--------------------------------------------------------------
//FUNÇÕES INDEPENDENTES
//--------------------------------------------------------------

//condição que acontece quando algum botão é pressionado
void atenderCampainha() {
  acionouInterrupcao = true; 
}

//função utilizada para identificar qual dos 4 botões estão ligados
void verificarQualBotao() {
   delay(50); 

  for (int i = 0; i < 4; i++) {
    if (digitalRead(botoes[i]) == HIGH) {
      executarAcao(i);
    }
  }
}

//executa uma acção especifica dependendo de qual botão foi acionado
void executarAcao(int indice) {
  switch(indice) {
    case 0: X0++; break;
    case 1: X1++; break;
    case 2: X2++; break;
    case 3: X3++; break;
  }
}

//tmeporaria, tme a função de pulsar um led decresente no neopixel
void faixa() {
  // Verifica se o tempo decorrido é maior que o intervalo definido
  if (millis() - T0 >= intervalo) {
    T0 = millis(); // Reinicia o cronômetro

    // Apaga o LED anterior (se ele existir)
    if (ledAtual > 0) {
      faixa1.setPixelColor(ledAtual - 1, faixa1.Color(0, 0, 0));
    } else {
      // Se estamos no começo (led 0), apaga o último LED do ciclo anterior
      faixa1.setPixelColor(NUM_LEDS - 1, faixa1.Color(0, 0, 0));
    }

    // Acende o LED atual
    if (ledAtual < NUM_LEDS) {
      faixa1.setPixelColor(ledAtual, faixa1.Color(0, 0, 255)); // Verde
      faixa1.show();
      ledAtual++;
    } else {
      // Quando chega no fim da faixa, volta para o início
      ledAtual = 0;
    }
  }
}

//verifica se acertou o botao coincidindo com o final da faixa

void verificarHit(int botaoIndex, Adafruit_NeoPixel &faixa) {
  int botaoPressionado = 0;
  
  if(botaoIndex == 0) botaoPressionado = X0;
  if(botaoIndex == 1) botaoPressionado = X1;
  if(botaoIndex == 2) botaoPressionado = X2;
  if(botaoIndex == 3) botaoPressionado = X3;

  if (botaoPressionado > 0) {
    // Se o ledAtual for global, usamos ele aqui. 
    // Se for usar o array que sugeri antes, use ledAtual[botaoIndex]
    if (ledAtual >= (NUM_LEDS - 1)) {
      pts += 10;
      faixa.fill(faixa.Color(255, 255, 255), 0, NUM_LEDS);
      faixa.show();
      tone(A0, 1000, 50); 
      atualizarPlacar();
    } else {
      if(pts > 0) pts -= 5;
      Vida--;
      atualizarPlacar();
    }
    
    // Reseta apenas o botão que foi testado para não interferir nas outras faixas
    if(botaoIndex == 0) X0 = 0;
    if(botaoIndex == 1) X1 = 0;
    if(botaoIndex == 2) X2 = 0;
    if(botaoIndex == 3) X3 = 0;
    
    acionouInterrupcao = false;
  }
}

void atualizarPlacar() {
  lcd.setCursor(8, 0);
  lcd.print("      "); // Limpa o número anterior
  lcd.setCursor(8, 0);
  lcd.print(pts);
  lcd.setCursor(7, 1);
  lcd.print("      "); // Limpa o número anterior
  lcd.setCursor(7, 1);
  contarVidas();
  lcd.print(lcdVida);
}

void contarVidas(){
  if(Vida == 3){ lcdVida = 3;}
  if(Vida == 2){ lcdVida = 2;}
  if(Vida == 1){ lcdVida = 1;}
  if(Vida == 0){ lcdVida = 3;
                estadoJogo = 13;
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
