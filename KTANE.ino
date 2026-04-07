#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>

// LCD
LiquidCrystal_I2C lcd_1(0x27, 16, 2);

// NEOPIXEL
#define PIN 2
#define NUM_LEDS 3
Adafruit_NeoPixel strip(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// BOTÃO
#define BTN_A3 A3
bool ultimoBotao = HIGH;

// FIOS
int fios[5] = {8, 6, 11, 12, 13};
bool estadoFios[5];

// SWITCHES
int switches[4] = {3, 4, 5, 7};
int etapaSwitch = 1;

// POTENCIÔMETROS
#define POT_MASTER A0
#define POT_BLUE   A1
#define POT_RED    A2

#define PIN_BLUE 9
#define PIN_RED  10

int etapaPot = 1;

// TIMER
long tempoRestante = 600;
unsigned long ultimoSegundo = 0;

// ESTADOS
enum Estado {
  IDLE,
  FIOS1,
  FIOS2,
  POT,
  SWITCHES,
  SUCESSO,
  ERRO_TOTAL
};

Estado estadoAtual = IDLE;

// CONTROLE
bool fio2Removido = false;

// LEDS FIXOS
bool ledFiosOK = false;
bool ledPotOK = false;
bool ledSwitchOK = false;

// RAINBOW
uint8_t j = 0;

uint32_t Wheel(byte pos) {
  pos = 255 - pos;
  if (pos < 85) return strip.Color(255 - pos * 3, 0, pos * 3);
  if (pos < 170) {
    pos -= 85;
    return strip.Color(0, pos * 3, 255 - pos * 3);
  }
  pos -= 170;
  return strip.Color(pos * 3, 255 - pos * 3, 0);
}

void rainbow() {
  delay(15);
  for (int i = 0; i < NUM_LEDS; i++)
    strip.setPixelColor(i, Wheel((i * 256 / NUM_LEDS + j) & 255));
  strip.show();
  j++;
}

// BOTÃO
bool botaoPressionado() {
  bool leitura = digitalRead(BTN_A3);

  if (ultimoBotao == HIGH && leitura == LOW) {
    ultimoBotao = leitura;
    delay(20);
    return true;
  }

  ultimoBotao = leitura;
  return false;
}

// RESTAURA LEDs FIXOS
void restaurarLeds() {
  strip.clear();

  if (ledFiosOK)
    strip.setPixelColor(0, strip.Color(0, 255, 0));

  if (ledPotOK)
    strip.setPixelColor(1, strip.Color(0, 255, 0));

  if (ledSwitchOK)
    strip.setPixelColor(2, strip.Color(0, 255, 0));

  strip.show();
}

// EFEITOS
void erro() {
  for (int k = 0; k < 5; k++) {
    for (int i = 0; i < NUM_LEDS; i++)
      strip.setPixelColor(i, strip.Color(255, 0, 0));
    strip.show();
    delay(120);

    strip.clear();
    strip.show();
    delay(120);
  }

  tempoRestante -= 15;
  restaurarLeds();
}

void acertoEtapa(int ledIndex) {
  for (int k = 0; k < 3; k++) {
    strip.setPixelColor(ledIndex, strip.Color(0, 255, 0));
    strip.show();
    delay(120);
    strip.clear();
    strip.show();
    delay(120);
  }

  restaurarLeds();
}

void acertoModulo(int ledIndex) {
  if (ledIndex == 0) ledFiosOK = true;
  if (ledIndex == 1) ledPotOK = true;
  if (ledIndex == 2) ledSwitchOK = true;

  restaurarLeds();
}

// TIMER
void atualizarTimer() {
  if (estadoAtual == SUCESSO) return;

  if (millis() - ultimoSegundo >= 1000) {
    ultimoSegundo = millis();
    tempoRestante--;

    if (tempoRestante <= 0) {
      estadoAtual = ERRO_TOTAL;
    }
  }
}

void mostrarTimer() {
  int min = tempoRestante / 60;
  int seg = tempoRestante % 60;

  lcd_1.setCursor(0, 1);

  if (seg < 10)
    lcd_1.print(String(min) + ":0" + String(seg) + " ");
  else
    lcd_1.print(String(min) + ":" + String(seg) + " ");
}

// POT
void lerPot(int &blue, int &red, int &master) {
  blue   = analogRead(POT_BLUE);
  red    = analogRead(POT_RED);
  master = analogRead(POT_MASTER);
}

void atualizarRGB(int blue, int red, int master) {

  int b = map(blue, 0, 1023, 0, 255);
  int r = map(red,  0, 1023, 0, 255);

  if (master < 50) {
    analogWrite(PIN_BLUE, 255);
    analogWrite(PIN_RED, 255);
    return;
  }

  if (master > 1000) {
    analogWrite(PIN_BLUE, 255 - b);
    analogWrite(PIN_RED, 255 - r);
    return;
  }

  int tempo = map(master, 0, 1023, 2000, 80);

  static unsigned long t = 0;
  static bool estado = false;

  if (millis() - t >= tempo) {
    t = millis();
    estado = !estado;
  }

  if (estado) {
    analogWrite(PIN_BLUE, 255 - b);
    analogWrite(PIN_RED, 255 - r);
  } else {
    analogWrite(PIN_BLUE, 255);
    analogWrite(PIN_RED, 255);
  }
}

bool verificarPot(int etapa, int blue, int red, int master) {
  if (etapa == 1)
    return (master<=20 && blue>=400 && blue<=650 && red>=990); //Desligado/ Meio/ Máximo

  if (etapa == 2)
    return (master>=1000 && red>=1000 && blue<=35); //Máximo/ Máximo/ Mínimo

  if (etapa == 3)
    return (master>=1000 && red>=1000 && blue>=1000); //Máximo/ Máximo/ Máximo

  return false;
}

// SWITCHES
bool verificarSwitches(int etapa) {

  int s1 = digitalRead(3) == LOW ? 1 : 0; // SW1
  int s2 = digitalRead(4) == LOW ? 1 : 0; // SW2
  int s3 = digitalRead(5) == LOW ? 1 : 0; // SW3
  int s4 = digitalRead(7) == LOW ? 1 : 0; // SW4

  if (etapa == 1) return (s1==1 && s2==0 && s3==1 && s4==1); // 1011
  if (etapa == 2) return (s1==0 && s2==0 && s3==0 && s4==1); // 0001
  if (etapa == 3) return (s1==1 && s2==1 && s3==0 && s4==0); // 1100

  return false;
}

// SETUP
void setup() {
  Serial.begin(9600);

  lcd_1.init();
  lcd_1.backlight();

  pinMode(BTN_A3, INPUT_PULLUP);

  for (int i = 0; i < 5; i++) {
    pinMode(fios[i], INPUT_PULLUP);
    estadoFios[i] = digitalRead(fios[i]);
  }

  for (int i = 0; i < 4; i++)
    pinMode(switches[i], INPUT_PULLUP);

  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_RED, OUTPUT);

  strip.begin();
  strip.setBrightness(50);
  strip.show();
}

// LOOP
void loop() {

  if (estadoAtual == IDLE) {
    rainbow();
    if (botaoPressionado()) {
      estadoAtual = FIOS1;
      lcd_1.clear();
    }
  }
  // Fios ---------------
  else if (estadoAtual == FIOS1 || estadoAtual == FIOS2) {

    atualizarTimer();

    lcd_1.setCursor(0, 0);
    lcd_1.print(estadoAtual == FIOS1 ? "Amore" : "Casa");
    mostrarTimer();

    for (int i = 0; i < 5; i++) {

      bool leitura = digitalRead(fios[i]);

      if (leitura == HIGH && estadoFios[i] == LOW) {

        if (i == 1 && estadoAtual == FIOS1) {
          acertoEtapa(0);
          estadoAtual = FIOS2;
          fio2Removido = true;
        }

        else if (i == 4 && estadoAtual == FIOS2 && fio2Removido) {
          acertoModulo(0);
          estadoAtual = POT;
          lcd_1.clear();
        }

        else {
          erro();
        }
      }

      estadoFios[i] = leitura;
    }
  }
  // Potenciometros ------------
  else if (estadoAtual == POT) {

    atualizarTimer();

    lcd_1.setCursor(0, 0);

    if (etapaPot == 1) lcd_1.print("Libro");
    if (etapaPot == 2) lcd_1.print("Cielo");
    if (etapaPot == 3) lcd_1.print("Acqua");

    mostrarTimer();

    int blue, red, master;
    lerPot(blue, red, master);
    atualizarRGB(blue, red, master);

    if (botaoPressionado()) {

      if (verificarPot(etapaPot, blue, red, master)) {

        acertoEtapa(1);
        etapaPot++;

        if (etapaPot > 3) {
          acertoModulo(1);
          estadoAtual = SWITCHES;
          lcd_1.clear();
        }
      } else {
        erro();
      }
    }
  }

  // Interruptores -----------------------
  else if (estadoAtual == SWITCHES) {

    atualizarTimer();

    lcd_1.setCursor(0, 0);

    if (etapaSwitch == 1) lcd_1.print("Sole");
    if (etapaSwitch == 2) lcd_1.print("Strada");
    if (etapaSwitch == 3) lcd_1.print("Tempo");

    mostrarTimer();

    // DEBUG SWITCHES
    int s1 = digitalRead(3) == LOW ? 1 : 0;
    int s2 = digitalRead(4) == LOW ? 1 : 0;
    int s3 = digitalRead(5) == LOW ? 1 : 0;
    int s4 = digitalRead(7) == LOW ? 1 : 0;

    Serial.print("SW: ");
    Serial.print(s1);
    Serial.print(s2);
    Serial.print(s3);
    Serial.println(s4);

    delay(200);

    if (verificarSwitches(etapaSwitch)) {

      etapaSwitch++;
      delay(300);

      if (etapaSwitch > 3) {
        acertoModulo(2);
        estadoAtual = SUCESSO;
        lcd_1.clear();
      }
    }
  }

  else if (estadoAtual == SUCESSO) {

    lcd_1.setCursor(0, 0);
    lcd_1.print("Successo");

    restaurarLeds();
  }

  else if (estadoAtual == ERRO_TOTAL) {

    for (int i = 0; i < NUM_LEDS; i++)
      strip.setPixelColor(i, strip.Color(255, 0, 0));

    strip.show();
  }
}
