//--------------------------------------------------------------
//INICIALIZAÇÕES
//--------------------------------------------------------------

#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#define NUM_LEDS 8
#define MAX_NOTAS 4

// Protótipos — necessário para o compilador reconhecer Adafruit_NeoPixel& nas funções
void moverNota(int id, Adafruit_NeoPixel &f, uint32_t COR);
void moverNotasAtivas(int id, Adafruit_NeoPixel &f, uint32_t COR);
void verificarHit(int id, Adafruit_NeoPixel &f);
void SeguraoTchan();
void Arere();
void atenderCampainha();
void verificarQualBotao();
void executarAcao(int indice);
void atualizarPlacar();
void contarVidas();
void LCDDesligando();
void LCDLigando();

// Criando os objetos para as 4 faixas nas portas 7, 8, 9 e 10
Adafruit_NeoPixel faixa1(NUM_LEDS, 7, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel faixa2(NUM_LEDS, 8, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel faixa3(NUM_LEDS, 9, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel faixa4(NUM_LEDS, 10, NEO_GRB + NEO_KHZ800);

LiquidCrystal_I2C lcd(32, 16, 2);

// Criando botões
const int botoes[] = {3,4,5,6};
volatile bool acionouInterrupcao = false;
// Criando estado, para funcionar a lógica do on/off
int estadoJogo = 0;
int X0 = 0, X1 = 0, X2 = 0, X3 = 0;
int musc = 0, dif = 0, pts, Vida, lcdVida, iNota, record = 0;
unsigned long intervalo, Tnota, Tbase;
bool telaIniciada = false;

// Arrays de notas múltiplas por faixa
int ledsF[4][MAX_NOTAS];
bool notaAtiva[4][MAX_NOTAS];
unsigned long timersF[4][MAX_NOTAS];

unsigned long T0 = 0;
int notasFaixas[] = {293, 261, 246, 221};

int listaSeguraoTchan[] = {0, 0, 1, 5, 0, 0, 2, 5, 2, 2, 1, 3,
                           1, 1, 3, 5, 0, 0, 1, 5, 0, 0, 2, 5,
                           2, 2, 1, 3, 1, 1, 3, 5, 5, 2, 2, 1,
                           5, 0, 2, 5, 2, 2, 1, 3, 0, 0, 2, 5,
                           5, 2, 2, 1, 5, 0, 2, 5, 2, 2, 1, 3,
                           0, 0, 2, 5, 5, 0, 0, 0, 1, 0, 0, 0,
                           2, 0, 0, 0, 1, 1, 0, 0, 3, 5, 5, 0,
                           0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 1, 1,
                           0, 0, 3, 5};

// Lista do Arere — mesmo formato: 0,1,2,3 = faixas, 5 = pausa
int listaArere[] = {0, 5, 1, 5, 0, 2, 5, 1, 5, 3,
                    0, 5, 1, 5, 2, 5, 0, 1, 5, 3,
                    2, 5, 0, 5, 1, 2, 5, 0, 5, 3,
                    1, 5, 2, 5, 0, 5, 1, 3, 5, 2,
                    0, 5, 1, 5, 0, 2, 5, 3, 5, 1,
                    2, 5, 0, 5, 1, 5, 2, 0, 5, 3,
                    0, 5, 2, 5, 1, 5, 0, 3, 5, 2,
                    1, 5, 0, 5, 2, 1, 5, 3, 5, 0};
#define ARERE_LEN 80

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
  for (int i = 0; i < 4; i++) pinMode(botoes[i], INPUT);
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), atenderCampainha, RISING);

  // Inicializa arrays de notas
  for(int f = 0; f < 4; f++)
    for(int n = 0; n < MAX_NOTAS; n++){
      ledsF[f][n] = -1;
      notaAtiva[f][n] = false;
      timersF[f][n] = 0;
    }

  //iniciando as faixas
  faixa1.begin(); faixa2.begin(); faixa3.begin(); faixa4.begin();
  //faixas desligadas
  faixa1.show();  faixa2.show();  faixa3.show();  faixa4.show();
  //brilho de 0 a 255
  faixa1.setBrightness(50); faixa2.setBrightness(50);
  faixa3.setBrightness(50); faixa4.setBrightness(50);
}

//--------------------------------------------------------------
//MAIN CODE
//--------------------------------------------------------------

//LIGANDO OU DESLIGANDO
//-------------------------------
void loop() {
  Serial.println(iNota);
  //lê interruptores constantemente, para fazer a função correta
  bool onoff = digitalRead(A1);

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
  if(acionouInterrupcao && estadoJogo == 2){
    //função externa, verifica qual botão apertado
    verificarQualBotao();
    //menu iniciado
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Bem Vindo!      ");
    delay(500);
    lcd.setCursor(0, 1); lcd.print("Escolha a musica");
    acionouInterrupcao = false;
    //muda o estado do jogo
    estadoJogo = 3;
  }

  if(acionouInterrupcao && estadoJogo == 3){
    //printa o nome das musicas, deixando um caractere no começo, para a seleção
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(" Segura o Tchan ");
    delay(1000);
    lcd.setCursor(0, 1); lcd.print(" Arere          ");
    acionouInterrupcao = false;
    //ajeitando as variáveis, para prevenir erro ao reiniciar o loop
    X0 = 1; X1 = 0; X2 = 0; X3 = 0;
    //muda o estado do jogo
    estadoJogo = 4;
  }

  if(estadoJogo == 4){
    //novamente verifica o botão pressionado
    if(acionouInterrupcao) verificarQualBotao();
    //quando o botão seta pra cima, ele muda a musica selecionada, para cima
    if(X0>0){ lcd.setCursor(0,0); lcd.print("-"); lcd.setCursor(0,1); lcd.print(" "); musc = 1; X0 = 0; }
    //quando o botão seta para baixo, ele muda a musica selecionada, para baixo
    if(X1>0){ lcd.setCursor(0,0); lcd.print(" "); lcd.setCursor(0,1); lcd.print("-"); musc = 2; X1 = 0; }
    // se o botão X for acionado, volta ao menu
    if(X2>0){ estadoJogo = 0; }
    //se o botão v for acionado, seleciona a música
    if(X3>0){
      if(musc == 1) estadoJogo = 5;
      if(musc == 2) estadoJogo = 9;
    }
    acionouInterrupcao = false;
  }

  if(estadoJogo == 5){
    // printa as dificuldades para a escolha do jogador
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Selecione o modo");
    delay(1000);
    lcd.setCursor(0, 1); lcd.print("-Facil  Dificil ");
    acionouInterrupcao = false;
    //ajeitando as variáveis, para evitar erros ao reiniciar
    X0 = 1; X1 = 0; X2 = 0; X3 = 0;
    //mudando o estado do jogo
    estadoJogo = 6;
  }

  if(estadoJogo == 6){
    //novamente verifica o botão pressionado
    if(acionouInterrupcao) verificarQualBotao();
    //quando o botão seta pra cima, seleciona Fácil
    if(X0>0){ lcd.setCursor(0,1); lcd.print("-"); lcd.setCursor(7,1); lcd.print(" "); dif = 1; X0 = 0; }
    //quando o botão seta para baixo, seleciona Difícil
    if(X1>0){ lcd.setCursor(0,1); lcd.print(" "); lcd.setCursor(7,1); lcd.print("-"); dif = 2; X1 = 0; }
    // se o botão X for acionado, volta a escolha de musica
    if(X2>0){ estadoJogo = 3; }
    //se o botão v for acionado, confirma dificuldade
    if(X3>0){
      //estado 7 = segura o tchan facil
      if(dif == 1) estadoJogo = 7;
      //estado 8 = segura o tchan dificil
      if(dif == 2) estadoJogo = 8;
    }
    acionouInterrupcao = false;
  }

  if(estadoJogo == 9){
    // printa as dificuldades para a escolha do jogador
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Selecione o modo");
    delay(1000);
    lcd.setCursor(0, 1); lcd.print("-Facil  Dificil ");
    acionouInterrupcao = false;
    //ajeitando as variáveis, para evitar erros ao reiniciar
    X0 = 0; X1 = 0; dif = 0; X2 = 0; X3 = 0;
    //mudando o estado do jogo
    estadoJogo = 10;
  }

  if(estadoJogo == 10){
    //novamente verifica o botão pressionado
    if(acionouInterrupcao) verificarQualBotao();
    //quando o botão seta pra cima, seleciona Fácil
    if(X0>0){ lcd.setCursor(0,1); lcd.print("-"); lcd.setCursor(7,1); lcd.print(" "); dif = 1; X0 = 0; }
    //quando o botão seta para baixo, seleciona Difícil
    if(X1>0){ lcd.setCursor(0,1); lcd.print(" "); lcd.setCursor(7,1); lcd.print("-"); dif = 2; X1 = 0; }
    // se o botão X for acionado, volta a escolha de musica
    if(X2>0){ estadoJogo = 3; }
    //se o botão v for acionado, confirma dificuldade
    if(X3>0){
      //estado 11 = arere facil
      if(dif == 1) estadoJogo = 11;
      //estado 12 = arere dificil
      if(dif == 2) estadoJogo = 12;
    }
    acionouInterrupcao = false;
  }

// JOGO FUNCIONANDO
//--------------------

  //SEGURA O TCHAN(FÁCIL)
  if(estadoJogo == 7){
    if(!telaIniciada){
      pts = 0;
      Vida = 3;
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("PONTOS: 0");
      lcd.setCursor(0, 1); lcd.print("VIDAS: 3");
      intervalo = 100; // velocidade do pixel caindo
      Tbase = 500;     // velocidade da musica
      T0 = millis();
      iNota = 0;
      telaIniciada = true;
    }
    SeguraoTchan();
    if(acionouInterrupcao){
      verificarQualBotao();
      verificarHit(0, faixa1);
      verificarHit(1, faixa2);
      verificarHit(2, faixa3);
      verificarHit(3, faixa4);
    }
  }

  //SEGURA O TCHAN(DIFÍCIL)
  if(estadoJogo == 8){
    if(!telaIniciada){
      pts = 0;
      Vida = 3;
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("PONTOS: 0");
      lcd.setCursor(0, 1); lcd.print("VIDAS: 3");
      intervalo = 80;  // LEDs mais rápidos
      Tbase = 350;     // notas mais rápidas
      T0 = millis();
      iNota = 0;
      telaIniciada = true;
    }
    SeguraoTchan();
    if(acionouInterrupcao){
      verificarQualBotao();
      verificarHit(0, faixa1);
      verificarHit(1, faixa2);
      verificarHit(2, faixa3);
      verificarHit(3, faixa4);
    }
  }

  //ARERE(FÁCIL)
  if(estadoJogo == 11){
    if(!telaIniciada){
      pts = 0;
      Vida = 3;
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("PONTOS: 0");
      lcd.setCursor(0, 1); lcd.print("VIDAS: 3");
      intervalo = 100;
      Tbase = 520;
      T0 = millis();
      iNota = 0;
      telaIniciada = true;
    }
    Arere();
    if(acionouInterrupcao){
      verificarQualBotao();
      verificarHit(0, faixa1);
      verificarHit(1, faixa2);
      verificarHit(2, faixa3);
      verificarHit(3, faixa4);
    }
  }

  //ARERE(DIFÍCIL)
  if(estadoJogo == 12){
    if(!telaIniciada){
      pts = 0;
      Vida = 3;
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("PONTOS: 0");
      lcd.setCursor(0, 1); lcd.print("VIDAS: 3");
      intervalo = 80;
      Tbase = 370;
      T0 = millis();
      iNota = 0;
      telaIniciada = true;
    }
    Arere();
    if(acionouInterrupcao){
      verificarQualBotao();
      verificarHit(0, faixa1);
      verificarHit(1, faixa2);
      verificarHit(2, faixa3);
      verificarHit(3, faixa4);
    }
  }

  //DERROTA
  if(estadoJogo == 13){
    lcd.setCursor(0, 0); lcd.print("  -VOCE PERDEU- ");
    lcd.setCursor(0, 1); lcd.print("PONTOS:         ");
    lcd.setCursor(8, 1); lcd.print(pts);
    delay(3000);
    telaIniciada = false;
    estadoJogo = 0;
    acionouInterrupcao = false;
  }

  //VITÓRIA
  if(estadoJogo == 14){
    if(pts > record){record = pts;}
    lcd.setCursor(0, 0); lcd.print("  -PARABENS!-   ");
    lcd.setCursor(0, 1); lcd.print("PONTOS:         ");
    lcd.setCursor(8, 1); lcd.print(pts);
    delay(2000);
    lcd.setCursor(0, 0); lcd.print("  -PARABENS!-   ");
    lcd.setCursor(0, 1); lcd.print("RECORDE:        ");
    lcd.setCursor(9, 1); lcd.print(record);
    delay(2000);
    
    telaIniciada = false;
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
  for(int i = 0; i < 4; i++)
    if(digitalRead(botoes[i]) == HIGH) executarAcao(i);
}

//executa uma ação específica dependendo de qual botão foi acionado
void executarAcao(int indice) {
  switch(indice){
    case 0: X0++; break;
    case 1: X1++; break;
    case 2: X2++; break;
    case 3: X3++; break;
  }
}

// Lista do Segura o Tchan — 0,1,2,3 = faixas, 5 = pausa
void SeguraoTchan(){
  if(millis() - Tnota >= Tbase){
    Tnota = millis();
    int f = listaSeguraoTchan[iNota];
    if(f == 0) moverNota(0, faixa1, faixa1.Color(0, 0, 255));   // Azul
    if(f == 1) moverNota(1, faixa2, faixa2.Color(0, 255, 0));   // Verde
    if(f == 2) moverNota(2, faixa3, faixa3.Color(255, 255, 0)); // Amarelo
    if(f == 3) moverNota(3, faixa4, faixa4.Color(255, 0, 0));   // Vermelho
    // 5 = pausa, não lança nada
    iNota++;
    if(iNota >= 100) estadoJogo = 14;
  }
  // Move as notas que já estão nas faixas (chamada contínua)
  moverNotasAtivas(0, faixa1, faixa1.Color(0, 0, 255));
  moverNotasAtivas(1, faixa2, faixa2.Color(0, 255, 0));
  moverNotasAtivas(2, faixa3, faixa3.Color(255, 255, 0));
  moverNotasAtivas(3, faixa4, faixa4.Color(255, 0, 0));
}

void Arere(){
  if(millis() - Tnota >= Tbase){
    Tnota = millis();
    int f = listaArere[iNota];
    if(f == 0) moverNota(0, faixa1, faixa1.Color(0, 0, 255));   // Azul
    if(f == 1) moverNota(1, faixa2, faixa2.Color(0, 255, 0));   // Verde
    if(f == 2) moverNota(2, faixa3, faixa3.Color(255, 255, 0)); // Amarelo
    if(f == 3) moverNota(3, faixa4, faixa4.Color(255, 0, 0));   // Vermelho
    iNota++;
    if(iNota >= ARERE_LEN) estadoJogo = 14;
  }
  moverNotasAtivas(0, faixa1, faixa1.Color(0, 0, 255));
  moverNotasAtivas(1, faixa2, faixa2.Color(0, 255, 0));
  moverNotasAtivas(2, faixa3, faixa3.Color(255, 255, 0));
  moverNotasAtivas(3, faixa4, faixa4.Color(255, 0, 0));
}

// Lança nota nova num slot vazio da faixa
void moverNota(int id, Adafruit_NeoPixel &f, uint32_t COR){
  for(int n = 0; n < MAX_NOTAS; n++){
    if(!notaAtiva[id][n]){
      notaAtiva[id][n] = true;
      ledsF[id][n] = 0;
      timersF[id][n] = millis();
      f.setPixelColor(0, COR);
      f.show();
      break;
    }
  }
}

// Move todas as notas ativas de uma faixa (chamada a cada loop)
void moverNotasAtivas(int id, Adafruit_NeoPixel &f, uint32_t COR){
  bool mudou = false;
  for(int n = 0; n < MAX_NOTAS; n++){
    if(!notaAtiva[id][n]) continue;
    if(millis() - timersF[id][n] >= intervalo){
      timersF[id][n] = millis();
      // Apaga posição atual
      f.setPixelColor(ledsF[id][n], f.Color(0,0,0));
      ledsF[id][n]++;
      if(ledsF[id][n] >= NUM_LEDS){
        // Nota passou sem ser acertada
        notaAtiva[id][n] = false;
        Vida--;
        if(pts > 0) pts -= 5;
        tone(A0, 150, 200);
        atualizarPlacar();
      } else {
        f.setPixelColor(ledsF[id][n], COR);
      }
      mudou = true;
    }
  }
  if(mudou) f.show();
}

//verifica se acertou o botão coincidindo com o final da faixa
void verificarHit(int id, Adafruit_NeoPixel &f){
  int botaoPressionado = 0;
  if(id == 0) botaoPressionado = X0;
  if(id == 1) botaoPressionado = X1;
  if(id == 2) botaoPressionado = X2;
  if(id == 3) botaoPressionado = X3;

  if(botaoPressionado == 0) return;

  // Procura nota na zona de hit (LED 6 ou 7)
  bool acertou = false;
  for(int n = 0; n < MAX_NOTAS; n++){
    if(notaAtiva[id][n] && ledsF[id][n] >= (NUM_LEDS - 2)){
      pts += 10;
      tone(A0, notasFaixas[id], 50);
      f.setPixelColor(ledsF[id][n], f.Color(0,0,0));
      f.show();
      notaAtiva[id][n] = false;
      acertou = true;
      break;
    }
  }

  if(!acertou){
    if(pts > 0) pts -= 5;
    Vida--;
    tone(A0, 150, 200);
  }

  if(id == 0) X0 = 0; if(id == 1) X1 = 0;
  if(id == 2) X2 = 0; if(id == 3) X3 = 0;
  atualizarPlacar();
  acionouInterrupcao = false;
}

void atualizarPlacar(){
  lcd.setCursor(8, 0); lcd.print("      ");
  lcd.setCursor(8, 0); lcd.print(pts);
  lcd.setCursor(7, 1); lcd.print("      ");
  lcd.setCursor(7, 1);
  contarVidas();
  lcd.print(lcdVida);
}

void contarVidas(){
  if(Vida == 3) lcdVida = 3;
  if(Vida == 2) lcdVida = 2;
  if(Vida == 1) lcdVida = 1;
  if(Vida <= 0){ lcdVida = 0; estadoJogo = 13; }
}

void LCDDesligando(){
  //animação de desligar
  lcd.setCursor(0, 0); lcd.print("desligando...   ");
  lcd.setCursor(0, 1); lcd.print("                ");
  delay(1000);
  lcd.setCursor(0, 0); lcd.print("                ");
  lcd.setCursor(0, 1); lcd.print("                ");
}

void LCDLigando(){
  //animação de ligar
  lcd.setCursor(0, 0); lcd.print("-              -"); lcd.setCursor(0,1); lcd.print("                "); delay(50);
  lcd.setCursor(0, 0); lcd.print("--            --"); lcd.setCursor(0,1); lcd.print("                "); delay(50);
  lcd.setCursor(0, 0); lcd.print("---          ---"); lcd.setCursor(0,1); lcd.print("                "); delay(50);
  lcd.setCursor(0, 0); lcd.print("----        ----"); lcd.setCursor(0,1); lcd.print("                "); delay(200);
  lcd.setCursor(0, 0); lcd.print("----AXE     ----"); lcd.setCursor(0,1); lcd.print("                "); delay(200);
  lcd.setCursor(0, 0); lcd.print("----AXE HERO----"); lcd.setCursor(0,1); lcd.print("                "); delay(500);
  lcd.setCursor(0, 0); lcd.print("----AXE HERO----"); lcd.setCursor(0,1); lcd.print("aperte um botao.");
}
