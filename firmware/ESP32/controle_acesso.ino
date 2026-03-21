#include <Keypad.h>               // Biblioteca para leitura de teclado matricial
#include <ESP32Servo.h>           // Biblioteca específica para controle de servo no ESP32 (usa PWM via LEDC)
#include <Wire.h>                 // Biblioteca para comunicação I2C (usada pelo LCD)
#include <LiquidCrystal_I2C.h>    // Biblioteca para controle de LCD via I2C
#include <string.h>               // Biblioteca padrão C para manipulação de strings (strcmp, memset)

/* LCD */
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* ESTADOS DO SISTEMA*/
typedef enum {                    // Definição de um tipo enumerado para máquina de estados
  ESTADO_AGUARDANDO,
  ESTADO_VALIDANDO,
  ESTADO_ACESSO_LIBERADO,
  ESTADO_BLOQUEADO
} EstadoSistema;

EstadoSistema estadoAtual = ESTADO_AGUARDANDO;  // Inicializa sistema no estado "aguardando"

/* PINOS */
#define LED_R 25
#define LED_G 26
#define LED_B 27

#define SERVO_PIN 23

/* TECLADO */
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {         // Mapeamento das teclas do teclado matricial
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};

byte rowPins[ROWS] = {19,18,5,17};
byte colPins[COLS] = {16,4,2,15};

// Inicializa o teclado com o mapa de teclas e pinos
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

/* SERVO */
Servo servoMotor;

/* VARIÁVEIS */
char senhaDigitada[5] = "";
int indiceSenha = 0;
int tentativasErradas = 0;

unsigned long tempoBloqueio = 0;
unsigned long tempoAcesso = 0;
unsigned long ultimoTeclaTempo = 0;
unsigned long tempoMensagem = 0;

#define DURACAO_BLOQUEIO 5000
#define DURACAO_ACESSO 3000
#define DEBOUNCE_TECLA 200
#define MAX_TENTATIVAS 3

/* CONTROLE DE MENSAGENS */
bool aguardandoMensagem = false;

/* FUNÇÕES AUXILIARES */

void limparSenha() {
  memset(senhaDigitada, 0, sizeof(senhaDigitada));
  indiceSenha = 0;
}

void mostrarTelaInicial() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Digite a senha");
  lcd.setCursor(0, 1);
}

void setColor(bool r, bool g, bool b) {
  digitalWrite(LED_R, r);
  digitalWrite(LED_G, g);
  digitalWrite(LED_B, b);
}

void atualizarSenha() {

  lcd.setCursor(0, 1);
  lcd.print("                ");        // limpa a linha
  lcd.setCursor(0, 1);

  for (int i = 0; i < indiceSenha; i++) {     
    lcd.print("*");                     // Mostra senha mascarada com '*'
  }
}

/* CAPTURAR TECLA */
void capturarTecla() {

  char key = keypad.getKey();

  if (key != NO_KEY) {

    if (millis() - ultimoTeclaTempo < DEBOUNCE_TECLA) {
      return;
    }

    ultimoTeclaTempo = millis();

    if (indiceSenha >= 4) {
      return;
    }

    /* Mostra o dígito temporariamente */
    lcd.setCursor(indiceSenha, 1);
    lcd.print(key);

    senhaDigitada[indiceSenha] = key;
    indiceSenha++;

    senhaDigitada[indiceSenha] = '\0';

    atualizarSenha();

    if (indiceSenha == 4) {
      estadoAtual = ESTADO_VALIDANDO;
    }
  }
}

/* VALIDAR SENHA */
int senhaValida(const char *senha) {

  if (strcmp(senha, "1234") == 0) return 45;
  if (strcmp(senha, "5678") == 0) return 90;
  if (strcmp(senha, "1111") == 0) return 135;
  if (strcmp(senha, "2222") == 0) return 180;

  return -1;
}

/* PROCESSAR SENHA */
void validarSenha() {

  int angulo = senhaValida(senhaDigitada);

  if (angulo != -1) {

    Serial.print("Movendo servo para: ");
    Serial.println(angulo);

    servoMotor.write(angulo);
    
    tempoAcesso = millis();
    tentativasErradas = 0;

    setColor(0, 1, 0);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Acesso liberado");
    lcd.setCursor(0, 1);
    lcd.print("Bem-vindo");

    limparSenha();

    estadoAtual = ESTADO_ACESSO_LIBERADO;

  } else {

    tentativasErradas++;

    setColor(1, 0, 0);

    int restantes = MAX_TENTATIVAS - tentativasErradas;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Senha incorreta");
    lcd.setCursor(0, 1);
    lcd.print("Restam: ");
    lcd.print(restantes);

    tempoMensagem = millis();
    aguardandoMensagem = true;
  }
}

/* CONTROLE ACESSO */
void controlarAcesso() {

  if (millis() - tempoAcesso >= DURACAO_ACESSO) {

    servoMotor.write(0);

    setColor(0, 0, 1);

    mostrarTelaInicial();

    estadoAtual = ESTADO_AGUARDANDO;
  }
}


/* CONTROLE BLOQUEIO */
void controlarBloqueio() {

  if (millis() - tempoBloqueio >= DURACAO_BLOQUEIO) {

    tentativasErradas = 0;

    limparSenha();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sistema ativo");
    lcd.setCursor(0, 1);
    lcd.print("Digite a senha");

    setColor(0, 0, 1);

    estadoAtual = ESTADO_AGUARDANDO;
  }
}

/* SETUP */
void setup() {

  Serial.begin(115200);                     // Inicializa comunicação serial

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
    
  /* SERVO */
  servoMotor.setPeriodHertz(50);            // Define frequência PWM (50Hz padrão servo)
  servoMotor.attach(SERVO_PIN, 500, 2400);  // Associa servo ao pino com limites de pulso
  servoMotor.write(0);                      // Posição inicial

  /* LCD */
  lcd.init();
  lcd.backlight();

  /* ESTADO INICIAL */
  setColor(0, 0, 1);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Controle Acesso");
  lcd.setCursor(0, 1);
  lcd.print("Digite a senha");

  mostrarTelaInicial();
}

/* LOOP */
void loop() {

  if(aguardandoMensagem){

    if(millis()-tempoMensagem>1500){

      limparSenha();

      if(tentativasErradas>=MAX_TENTATIVAS){

        tempoBloqueio = millis();

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Sistema bloqueado");
        lcd.setCursor(0,1);
        lcd.print("Aguarde 5s");

        estadoAtual = ESTADO_BLOQUEADO;

      }else{

        mostrarTelaInicial();
        estadoAtual = ESTADO_AGUARDANDO;
      }

      aguardandoMensagem = false;
    }

    return;                                 // Evita executar resto do loop
  }

  switch (estadoAtual) {

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