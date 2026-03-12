#include <Keypad.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/* LCD */
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* ESTADOS */

typedef enum {
  ESTADO_AGUARDANDO,
  ESTADO_VALIDANDO,
  ESTADO_ACESSO_LIBERADO,
  ESTADO_BLOQUEADO
} EstadoSistema;

EstadoSistema estadoAtual = ESTADO_AGUARDANDO;

/* PINOS */

#define LED_R 25
#define LED_G 26
#define LED_B 27

#define BUZZER 32
#define SERVO_PIN 13

/* TECLADO */

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};

byte rowPins[ROWS] = {19,18,5,17};
byte colPins[COLS] = {16,4,2,15};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

/* SERVO */

Servo servoMotor;

/* VARIÁVEIS */

char senhaDigitada[5];
int indiceSenha = 0;
int tentativasErradas = 0;

unsigned long tempoBloqueio = 0;
unsigned long tempoAcesso = 0;
unsigned long ultimoTeclaTempo = 0;

#define DURACAO_BLOQUEIO 5000
#define DURACAO_ACESSO 5000
#define DEBOUNCE_TECLA 200
#define MAX_TENTATIVAS 3

/* RGB */

void setColor(bool r, bool g, bool b){

digitalWrite(LED_R,r);
digitalWrite(LED_G,g);
digitalWrite(LED_B,b);

}

/* BUZZER PASSIVO */

void beep(int freq, int tempo){

ledcAttachPin(BUZZER,0);
ledcWriteTone(0,freq);

delay(tempo);

ledcWriteTone(0,0);

}

/* MOSTRAR SENHA */

void atualizarSenha(){

lcd.setCursor(0,1);

for(int i=0;i<indiceSenha;i++)
lcd.print("*");

}

/* CAPTURA TECLA */

void capturarTecla(){

char key = keypad.getKey();

if(key != NO_KEY){

if(millis() - ultimoTeclaTempo < DEBOUNCE_TECLA) return;

ultimoTeclaTempo = millis();

/* BEEP TECLA */

beep(2000,50);

/* LIMITE DIGITOS */

if(indiceSenha >= 4) return;

/* MOSTRA DIGITO TEMPORARIO */

lcd.setCursor(indiceSenha,1);
lcd.print(key);

delay(300);

senhaDigitada[indiceSenha++] = key;
senhaDigitada[indiceSenha] = '\0';

atualizarSenha();

if(indiceSenha == 4){

estadoAtual = ESTADO_VALIDANDO;

}

}

}

/* VALIDAR SENHA */

int senhaValida(char *senha){

if(strcmp(senha,"1234")==0) return 25;
if(strcmp(senha,"5678")==0) return 50;
if(strcmp(senha,"1111")==0) return 75;
if(strcmp(senha,"2222")==0) return 90;

return -1;

}

/* PROCESSAR SENHA */

void validarSenha(){

int angulo = senhaValida(senhaDigitada);

if(angulo != -1){

servoMotor.write(angulo);

tempoAcesso = millis();

tentativasErradas = 0;

setColor(0,1,0);

beep(2500,150);
delay(120);
beep(2500,150);

lcd.clear();
lcd.setCursor(0,0);
lcd.print("Acesso Liberado");
lcd.setCursor(0,1);
lcd.print("Bem-vindo");

estadoAtual = ESTADO_ACESSO_LIBERADO;

}

else{

tentativasErradas++;

setColor(1,0,0);

beep(1200,200);
delay(100);
beep(1200,200);
delay(100);
beep(1200,200);

int restantes = MAX_TENTATIVAS - tentativasErradas;

lcd.clear();
lcd.setCursor(0,0);
lcd.print("Senha incorreta");
lcd.setCursor(0,1);
lcd.print("Restam: ");
lcd.print(restantes);

delay(1500);

if(tentativasErradas >= MAX_TENTATIVAS){

tempoBloqueio = millis();

lcd.clear();
lcd.setCursor(0,0);
lcd.print("Sistema bloqueado");
lcd.setCursor(0,1);
lcd.print("Aguarde 5s");

beep(800,600);

estadoAtual = ESTADO_BLOQUEADO;

}

else{

estadoAtual = ESTADO_AGUARDANDO;

lcd.clear();
lcd.setCursor(0,0);
lcd.print("Digite a senha");
lcd.setCursor(0,1);

}

}

indiceSenha = 0;

}

/* CONTROLA ACESSO */

void controlarAcesso(){

if(millis() - tempoAcesso >= DURACAO_ACESSO){

servoMotor.write(0);

setColor(0,0,1);

lcd.clear();
lcd.setCursor(0,0);
lcd.print("Digite a senha");
lcd.setCursor(0,1);

estadoAtual = ESTADO_AGUARDANDO;

}

}

/* CONTROLA BLOQUEIO */

void controlarBloqueio(){

if(millis() - tempoBloqueio >= DURACAO_BLOQUEIO){

tentativasErradas = 0;

lcd.clear();
lcd.setCursor(0,0);
lcd.print("Sistema ativo");
lcd.setCursor(0,1);
lcd.print("Digite senha");

setColor(0,0,1);

estadoAtual = ESTADO_AGUARDANDO;

}

}

/* SETUP */

void setup(){

Serial.begin(115200);

pinMode(LED_R,OUTPUT);
pinMode(LED_G,OUTPUT);
pinMode(LED_B,OUTPUT);

pinMode(BUZZER,OUTPUT);

servoMotor.attach(SERVO_PIN);

lcd.init();
lcd.backlight();

setColor(0,0,1);

lcd.setCursor(0,0);
lcd.print("Controle Acesso");
lcd.setCursor(0,1);
lcd.print("Digite a senha");

servoMotor.write(0);

}

/* LOOP */

void loop(){

switch(estadoAtual){

case ESTADO_AGUARDANDO:
capturarTecla();
break;

case ESTADO_VALIDANDO:
validarSenha();
break;

case ESTADO_ACESSO_LIBERADO:
controlarAcesso();
break;

case ESTADO_BLOQUEADO:
controlarBloqueio();
break;

}

}