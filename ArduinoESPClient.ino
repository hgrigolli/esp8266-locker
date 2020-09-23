#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

SoftwareSerial ESP_Serial(10, 11); // RX, TX

const int RS = 13, EN = 12, D4 = 7, D5 = 6, D6 = 5, D7 = 4;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
const char chars[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
int charindex = 0;
bool apertouBtn9 = false; //Confirmar palavra
bool apertouBtn8 = false; //Confirmar letra
bool apertouBtn2 = false; //Alterar letra
bool resetar = false;
int charsSize = 0;

int cursorX = 0;
int cursorY = 1;

String pass = "";
char cpass = '\0';

String resposta = "";
const int rele = 3;
int estadoRele = 0;

void setup() {
  pinMode(rele, OUTPUT);
  lcd.begin(16, 2);

  lcd.clear();
  pinMode(9, INPUT);
  pinMode(8, INPUT);
  pinMode(2, INPUT);
  lcd.setCursor(0, 0);

  charsSize = sizeof(chars);

  Serial.begin(9600);
  ESP_Serial.begin(9600);

  mostraMensagem("Inicializando...");
  delay(1000);

  sendCommand("AT");
  readResponse(1000);

  sendCommand("AT+CWJAP?");
  readResponse(1000);

  sendCommand("AT+CIFSR");
  readResponse(1000);

  sendCommand("AT+CIPMUX?");
  readResponse(1000);

  sendCommand("AT+CIPMUX=1");
  readResponse(1000);

  sendCommand("AT+CIPSTART=1,\"TCP\",\"192.168.0.117\",41821");
  readResponse(3000);
  
  lcd.clear();
  lcd.print("Digite a senha: ");
  lcd.setCursor(cursorX, cursorY);
  Serial.println("Fim do setup");
}


void loop() {
  
  
  if (digitalRead(9) == HIGH) {
    apertouBtn9 = true;
  }

  if (digitalRead(8) == HIGH) {
    apertouBtn8 = true;
  }

  if (digitalRead(2) == HIGH) {
    apertouBtn2 = true;
  }

  if (apertouBtn2) {
    if(charindex == charsSize){
      charindex = 0;
    }
    cpass = chars[charindex];
    lcd.print(cpass);
    lcd.setCursor(cursorX, cursorY);
    charindex++;
    apertouBtn2 = false;
    Serial.println("Apertou botao 2");
  }

  if (apertouBtn8) {
    pass += cpass;
    charindex = 0;
    cursorX++;
    if(cursorX == 16){
      cursorX = 0;
    }
    lcd.setCursor(cursorX, cursorY);
    apertouBtn8 = false;
    Serial.println("Apertou botao 8");
  }

  if (apertouBtn9) {
    Serial.println("Apertou botao 9");
    apertouBtn9 = false;
    
    if(pass.length() != 4){
      Serial.println("Senha deve conter 4 caracteres");
      mostraMensagem("pass 4 chars");
      delay(1500);
      resetLCD();
      charindex = 0;
      return;
    }

    sendCommand("AT+CIPSEND=1,6");
    readResponse(1000);
    pass += '\r';
    pass += '\n';
    sendCommand(pass);
    mostraMensagem("Verificando...");
    delay(2000);

    if (ESP_Serial.available()) {
      //.find le os dados vindos da serial ate o alvo, neste caso +IPD,
      Serial.println("Encontrou servidor...");
      if (ESP_Serial.find("+IPD,1,2:")) {
        Serial.println("Validando resposta do servidor...");
        delay(500);
        String retorno = ESP_Serial.readStringUntil('\n');
        String ok = "OK";
        retorno.trim();
        ok.trim();
        if (ok.equals(retorno)) {
          if (estadoRele == 0) {
            mostraMensagem("Acesso permitido");
            digitalWrite(rele, HIGH);
            estadoRele = 1;
          } else {
            digitalWrite(rele, LOW);
            estadoRele = 0;
          }
        } else {
          Serial.println("Senha invalida 1");
          mostraMensagem("Senha invalida");
          delay(5000);
          resetLCD();
          digitalWrite(rele, LOW);
        }
      } else {
        Serial.println("Senha invalida 2");
        mostraMensagem("Senha invalida");
        delay(5000);
        resetLCD();
        digitalWrite(rele, LOW);
      }
      
      if (estadoRele == 1) {
        delay(5000);
        digitalWrite(rele, LOW);
        estadoRele = 0;
        resetLCD();
      }
    }
    
    if(resetar) {
      resetar = false;
      sendCommand("AT+CIPSTART=1,\"TCP\",\"192.168.0.117\",41821");
      readResponse(1000);
    }
  }
  
  delay(200);

}

void mostraMensagem(String msg) {
  lcd.clear();
  lcd.print(msg);
  delay(500);
}


void sendCommand(String cmd) {
  ESP_Serial.println(cmd);
}

void readResponse(unsigned int timeout) {
  unsigned long timeIn = millis();
  resposta = "";
  while (timeIn + timeout > millis()) {
    while (ESP_Serial.available()) {
      char c = ESP_Serial.read();
      resposta += c;
    }
  }
  Serial.println(resposta);
}

void resetLCD() {
  resetar = true;
  pass = "";
  lcd.clear();
  lcd.print("Digite a senha: ");
  cursorX = 0;
  cursorY = 1;
  lcd.setCursor(cursorX, cursorY);
}
