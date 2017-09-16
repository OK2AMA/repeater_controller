// Selektikvní volba do rádiového převáděče Karel
// https://www.itnetwork.cz/hardware-pc/arduino/arduino-stavba-jazyka
// http://docs.uart.cz/docs/knihovny/
// https://uart.cz/1466/sifrovani-v-arduinu/
// -12V                       70cm TX4 D7
// GND
// 4m TX1 A3
// 2m RX1 A4                   D13 4m RX4
// 2m TX2 A5
// -
// -
// 70cm RX3 D3

const int ledPin =  A0;      // the number of the LED pin
const int DTMF_std = 12;
const int DTMF1 = 8;
const int DTMF2 = 9;
const int DTMF3 = 10;
const int DTMF4 = 11;

const int RX_vhf = A4;
const int RX_1 = A6;
const int RX_uhf = 3;
const int RX_mb = 13;

const int TX_mb = A3;
const int TX_vhf = A5;
const int TX_1 = A7;
const int TX_uhf = 7;
const int TX_2 = 6;
const int TX_3 = 5;

const int beep_pin = 11;    // dtmf3, beep output

byte byt = 0;
byte data = 0;
byte temp = 0;
byte i = 0;


boolean blok_zvuk ;  // blokace zvuku vypbuta po zapnutí
boolean blok_roger ;  // true = zapnut po startu ROGER // ma byt true = zaple po startu
boolean DTMF_byla ;  // roger pouze bez dtmf
boolean poprve ;     // pomocna promena pro specificke chovani bezd DTMF
boolean opadavani_pomalu ;   // zapnuto opadavani
boolean opadavani_predtim ;  //
boolean povoleni_uhf_tx ;  //

long vse = 0;

int napeti ;
int band_activity = 0;

void tx_quiet() {
  digitalWrite(TX_mb, LOW);
  digitalWrite(TX_vhf, LOW);
  digitalWrite(TX_uhf, LOW);
  digitalWrite(TX_1, LOW);
}

void cteni_bytu()
{
  vse = vse << 4;
  data = 0x00;
  data = ((digitalRead(DTMF4) | data) << 1);
  data = ((digitalRead(DTMF3) | data) << 1);
  data = ((digitalRead(DTMF2) | data) << 1);
  data = ((digitalRead(DTMF1) | data));
  data = data & B00001111;
  vse = (vse | data);
}

void telegraf(byte zdroj) // zdroj: 0Bdddzzzzz 3x delka a nasledne 5x znak
{ // pouze tisk petiznakovych zancek
  byte i = 0;
  byte delka = (zdroj & B11100000) >> 5;
  byte znaky = (zdroj & B00011111);
  delay(100);

  for  ( i = 0 ; i < delka ; i++)
  {
    if (((znaky & B00010000 )) == B00010000)
    {
      tone(beep_pin, 1200);
      delay(210);
      noTone(beep_pin);
    }
    else
    {
      tone(beep_pin, 1200);
      delay(70);
      noTone(beep_pin);
    }
    delay(70);
    znaky = znaky << 1;
  }
  delay(200);
}

void start_TX_dtmf() {
  if (band_activity = 1)
    digitalWrite(TX_mb, HIGH);
  if (band_activity = 2)
    digitalWrite(TX_vhf, HIGH);
  if (band_activity = 2)
    digitalWrite(TX_uhf, HIGH);
}

void stop_TX_dtmf() {
  if (band_activity = 1)
    digitalWrite(TX_mb, LOW);
  if (band_activity = 2)
    digitalWrite(TX_vhf, LOW);
  if (band_activity = 2)
    digitalWrite(TX_uhf, LOW);
}

void dtmf_service() {
  tx_quiet();
  DTMF_byla = true ;

  digitalWrite(ledPin, HIGH);

  while ((digitalRead(RX_mb) == 1) or (digitalRead(RX_vhf) == 1) or (digitalRead(RX_uhf) == 1)) { // RX
    if (digitalRead(RX_mb) == 1)
      band_activity = 1;
    if (digitalRead(RX_mb) == 1)
      band_activity = 2;
    if (digitalRead(RX_uhf) == 1)
      band_activity = 3;

    delay(10);
    cteni_bytu();
    while (digitalRead(DTMF_std) == 1 )
    {
      //   now waiting for interrupt DTMF
    }
    delay(10);
  }// waiting for stop transmitting

  //--- zde projde program po odklíčování-------------------------------
  switch (vse) {
    /*
      DTMF kod odlisnost od BCD,  cisla nastesti odpovidaji
      DTMF  BCD
      0 =   A
      S =   B
      # =   C
      A =   D
      B =   E
      C =   F
      D =   0
    */
    case 0x00 :
      break;
    case 0x01 :

      break;
    case 0x2811 :
      break;
    case 0x281A :  // na radiu 2810
      break;
    case 0x2821 :
      break;
    case 0x282A :  // 2820
      break;
    case 0x461 :  // zap. 2m linku
      break;
    case 0x46A : // 4610
      break;
    case 0x8721 :
      break;
    case 0x872A :
      break;
    case 0x936 : // kompletní zapovězení TX ze strany selektivky !! pro naprostou rádiovou neviditelnost ;-)
      pinMode(TX_mb,  INPUT);     // pin TX je nutno stahovat k zemi odporem cca 2 - 10 k ohm, pri zablok TX je na nem vyosoká impendace (spojená s + vnitřním odporem 100k ohm)
      pinMode(TX_vhf, INPUT);
      pinMode(TX_uhf, INPUT);
      break;
    case 0x937 : // v základu je vždy TX povoleno, a tímto se opět povoluje
      pinMode(TX_mb,  OUTPUT);
      pinMode(TX_vhf, OUTPUT);
      pinMode(TX_uhf, OUTPUT);
      break;
    case 0x926 : // kompletní zapovězení akustického pípání, rogery, identifikace ...
      pinMode(beep_pin, INPUT);     // pinD4 je nutno stahovat k zemi odporem cca 2 - 10 k ohm, pri zablok TX je na nem vyosoká impendace (spojená s + vnitřním odporem 100k ohm)
      break;
    case 0x927 : // v základu je povoleno
      pinMode(beep_pin, OUTPUT);
      break;
    // po relaci ovladaci prvky  * * * * * * * * * * * * * * * *
    case 0x1621 : // blokovani rogeru
      //blok_roger = false;
      break;
    case 0x162A :
      //blok_roger = true;
      break;
    case 0x738A : // rychlost opadavani
      //opadavani_pomalu = false;
      break;
    case 0x7381 :
      //opadavani_pomalu = true;
      break;
    case 0x487A : // rychlost PRED opadavani
      //opadavani_predtim = false;
      break;
    case 0x4871 :
      //opadavani_predtim = true;
      break;
    case 0x274A : // vypnout vsechnu identifikaci po relaci
      //opadavani_predtim = false;
      //opadavani_pomalu = false;
      //blok_roger = true;
      break;

    case 0x2741 :   // zapnout vsechnu identifikaci po relaci
      //opadavani_predtim = true;
      //opadavani_pomalu = true;
      //blok_roger = false;
      break;

    case 0x146 :
      setup(); // soft reset !!
      break;

    case 0x48 :
      //analog_cteni(baterie);
      break;
    case 0x49 :
      //analog_cteni(teplota);
      break;

    case 0x5 : // telegrafická identifikace
      start_TX_dtmf();
      delay(300);
      //  telegraf(B01001000);// 010 =2 / 01--- == .- == A
      telegraf(B10010100);// C -.-.  delka 4 : 100/1010-
      telegraf(B10011010);// q
      delay(50);
      stop_TX_dtmf();
      break;

    case 0x4 : // telegrafická identifikace
      start_TX_dtmf();
      delay(300);
      //  telegraf(B01001000);// 010 =2 / 01--- == .- == A
      telegraf(B01001000);// a -.-.  delka 4 : 100/1010-
      telegraf(B10011010);// h
      telegraf(B01111100);// o
      telegraf(B10001110);// j
      delay(50);
      stop_TX_dtmf();
      break;

    default: // unknown received DTMF code
      start_TX_dtmf();
      delay(500);
      //  telegraf(B01001000);// 010 =2 / 01000 == .- == A
      telegraf(B01100000);
      delay(50);
      stop_TX_dtmf();
      break;

  }// konec case ****

  if ((DTMF_byla == false) && ( opadavani_predtim == true)) // pokud nebyla DTMF tak podrží
  {
    start_TX_dtmf();
    delay(380);  //
    stop_TX_dtmf();
  }

  if ((DTMF_byla == false ) && (blok_roger == false )) // pokud nebyla DTMF tak roger
  {
    start_TX_dtmf();  // tato prodleva zajišťuje bezpečnostní čas pro poslech vlastního rogeru
    tone(beep_pin, 1000 ); // (pin(D4), frekvence v Hz)
    delay(95);
    noTone(beep_pin);
    delay(10);
    stop_TX_dtmf();
  }
  if ((DTMF_byla == false) && ( opadavani_pomalu == true)) // pokud nebyla DTMF tak podrží
  {
    start_TX_dtmf();
    delay(1200); // delka podrzeni TX
    stop_TX_dtmf();
  }
}




void setup() {
  byt = B00000000;
  data = B00000000;
  temp = B00000000;
  i = 0;
  blok_zvuk = false;  // blokace zvuku vypbuta po zapnutí
  blok_roger = true;  // true = zapnut po startu ROGER // ma byt true = zaple po startu
  DTMF_byla = false;  // roger pouze bez dtmf
  poprve = false;     // pomocna promena pro specificke chovani bezd DTMF
  opadavani_pomalu = true;   // zapnuto opadavani
  opadavani_predtim = true;   // zapnuto opadavani predtim
  opadavani_predtim = true;   // zapnuto opadavani predtim
  povoleni_uhf_tx = true;   // zapnuto opadavani predtim

  napeti = 0;

  //  Serial.begin(19200);
  analogReference(DEFAULT);
  pinMode(ledPin, OUTPUT);

  pinMode(DTMF_std, INPUT);
  pinMode(DTMF1, INPUT);
  pinMode(DTMF2, INPUT);
  pinMode(DTMF3, INPUT);
  pinMode(DTMF4, INPUT);

  pinMode(RX_mb, INPUT);
  pinMode(RX_vhf, INPUT);
  pinMode(RX_uhf, INPUT);
  pinMode(RX_1, INPUT);

  pinMode(TX_mb, OUTPUT);
  pinMode(TX_vhf, OUTPUT);
  pinMode(TX_uhf, OUTPUT);
  pinMode(TX_1, OUTPUT);

  //pinMode(beep_pin, OUTPUT); // tone(pin, frequency, duration)

  digitalWrite(TX_mb, LOW);
  digitalWrite(TX_vhf, LOW);
  digitalWrite(TX_uhf, LOW);
  digitalWrite(TX_1, LOW);

  //start();  // uvodni znelka s TX
}


void loop() {
  while (digitalRead(RX_mb) == 1)
  {
    while (digitalRead(RX_mb) == 1)
    {
      digitalWrite(TX_mb, HIGH);
    }
    tone(beep_pin, 400); // -
    delay(300);
    noTone(beep_pin);
    delay(800);
  }

  while (digitalRead(RX_vhf) == 1)
  {
    while (digitalRead(RX_vhf) == 1)
    {
      digitalWrite(TX_vhf, HIGH);
    }
    delay(800);
    tone(beep_pin, 400); // -
    delay(300);
    noTone(beep_pin);
    delay(800);
  }


  while (digitalRead(RX_uhf) == 1)
  {
    while (digitalRead(RX_uhf) == 1)
    {
      digitalWrite(TX_3, HIGH);
    }
    delay(800);
  }

  while (digitalRead(RX_1) == 1)
  {
    while (digitalRead(RX_1) == 1)
    {
      digitalWrite(TX_1, HIGH);
    }
    delay(800);
  }

  if (digitalRead(DTMF_std) == 1)
    dtmf_service();
    
  digitalWrite(TX_mb, LOW);
  digitalWrite(TX_vhf, LOW);
  digitalWrite(TX_uhf, LOW);
  digitalWrite(TX_1, LOW);
}








