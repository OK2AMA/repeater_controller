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

const int RX_vhf = 13;
const int RX_1 = A6;
const int RX_uhf = 3;
const int RX_mb = A4;

const int TX_mb = A5;
const int TX_vhf = A3;
const int TX_1 = A7;
const int TX_uhf = 7;
const int TX_2 = 6;
const int TX_3 = 5;

const int beep_pin = 3;    // dtmf3, beep output


boolean blok_zvuk ;  // blokace zvuku vypbuta po zapnutí
boolean blok_roger = false ;  // true = zapnut po startu ROGER // ma byt true = zaple po startu
boolean DTMF_byla ;  // roger pouze bez dtmf
boolean opadavani_pomalu = true;   // zapnuto opadavani
boolean crossband_mode = false;
boolean crossband_extended = false;

long vse = 0;
long data = 0;
int band_activity = 0;
int i = 0;

void tx_quiet() {
  digitalWrite(TX_mb, LOW);
  digitalWrite(TX_vhf, LOW);
  digitalWrite(TX_uhf, LOW);
}

void cteni_bytu()
{
  vse = vse << 4;
  data = 0x00;
  data = ((digitalRead(DTMF4) | data) << 1); // prohozeno
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
  if (band_activity == 1)
    digitalWrite(TX_mb, HIGH);
  if (band_activity == 2)
    digitalWrite(TX_vhf, HIGH);
  if (band_activity == 3)
    digitalWrite(TX_uhf, HIGH);
}

void stop_TX_dtmf() {
  if (band_activity == 1)
    digitalWrite(TX_mb, LOW);
  if (band_activity == 2)
    digitalWrite(TX_vhf, LOW);
  if (band_activity == 3)
    digitalWrite(TX_uhf, LOW);
}

void dtmf_service() {
  tx_quiet();
  delay(70);
  if (digitalRead(RX_mb) == 1)
    band_activity = 1;

  if (digitalRead(RX_vhf) == 1)
    band_activity = 2;

  if (digitalRead(RX_uhf) == 1)
    band_activity = 3;

  //digitalWrite(ledPin, HIGH);
  vse = 0;
  while ((digitalRead(RX_mb) == 1) or (digitalRead(RX_vhf) == 1)) { // RX
    if (digitalRead(DTMF_std) == 1 )
    {
      delay(10);
      cteni_bytu();
      while (digitalRead(DTMF_std) == 1 )
      {
        //   now waiting for interrupt DTMF
      }
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
    case 0x61 : // blokovani rogeru
      blok_roger = false;
      break;
    case 0x6A :
      blok_roger = true;
      break;
    case 0x7A : // rychlost opadavani
      opadavani_pomalu = false;
      break;
    case 0x71 :
      opadavani_pomalu = true;
      break;
    case 0x8A : // zapnout crossband
      crossband_mode = false;
      break;
    case 0x81 : // zapnout crossband
      crossband_mode = true;
      break;
    case 0x9A :
    case 0x80 : // zapnout crossband
      crossband_mode = false;
      crossband_extended = false;
      break;
    case 0x91 :
      crossband_mode = true;
      crossband_extended = true;
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
      //tx_quiet();
      break;

  }// konec case ****

}




void setup() {



  i = 0;
  blok_zvuk = false;  // blokace zvuku vypbuta po zapnutí
  // true = zapnut po startu ROGER // ma byt true = zaple po startu
  DTMF_byla = false;  // roger pouze bez dtmf
  opadavani_pomalu = true;   // zapnuto opadavani


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
  digitalWrite(TX_vhf, HIGH);
  digitalWrite(TX_uhf, LOW);
  digitalWrite(TX_1, LOW);

  //start();  // uvodni znelka s TX
  telegraf(B01000000);// a -.-.  delka 4 : 100/1010-
  //  telegraf(B10011010);// h
  //  telegraf(B01111100);// o
  //  telegraf(B10001110);// j


  delay(1500);
  digitalWrite(TX_mb, LOW);
  digitalWrite(TX_vhf, LOW);
  digitalWrite(TX_uhf, LOW);
  digitalWrite(TX_1, LOW);
  unsigned long PreviousMillis = 0;
}


void loop() {
  unsigned long CurrentMillis = millis();

  // unsigned long
  // if((unsigned long)(currentMillis - PreviousMillis) >= 3600000)
  //   {
  //   PreviosMillis = currentMillis;
  //   casovac = 0
  //   }

  //if((unsigned long) CurrentMillis < 120000)
  //  crossband_mode == true;
  //  else
  //  crossband_mode == false;



  if ((unsigned long) CurrentMillis <= 1500 )
  {
    crossband_mode = true;
    crossband_extended = true;
  }



  if (digitalRead(RX_mb) == 1)
  {
    while (digitalRead(RX_mb) == 1)
    {
      if (crossband_mode == false)
        digitalWrite(TX_mb, HIGH);
      else
        digitalWrite(TX_vhf, HIGH);
      if (crossband_extended == true)
        digitalWrite(TX_mb, HIGH);
      if (digitalRead(DTMF_std) == 1)
        dtmf_service();
      while (digitalRead(RX_mb) == 1)
      {
        if (digitalRead(DTMF_std) == 1)
          dtmf_service();  
        delay(30);
      }
      delay(30);
    }
    delay(270);// z duvodu skrnuti po odklicovani aby byl cisty roger
    tone(beep_pin, 700); // -
    if (1)
      delay(200);
    noTone(beep_pin);
    if (opadavani_pomalu == true)
      delay(1700);
  }

  if (digitalRead(RX_vhf) == 1)
  {
    while (digitalRead(RX_vhf) == 1)
    {
      if (crossband_mode == false)
        digitalWrite(TX_vhf, HIGH);
      else
        digitalWrite(TX_mb, HIGH);
      if (crossband_extended == true)
        digitalWrite(TX_vhf, HIGH);

      while (digitalRead(RX_vhf) == 1)
      {
        if (digitalRead(DTMF_std) == 1)
          dtmf_service();
        delay(30);
      }
      delay(30);
    }
    delay(270);
    tone(beep_pin, 700); // -
    if (blok_roger == false)
      delay(200);
    noTone(beep_pin);
    if (opadavani_pomalu == true)
      delay(1700);
  }


  while (0) //  digitalRead(RX_uhf) == 1
  {
    while (digitalRead(RX_uhf) == 1)
    {
      digitalWrite(TX_3, HIGH);
    }
    tone(beep_pin, 400); // -
    delay(120);
    noTone(beep_pin);
    if (opadavani_pomalu == true)
      delay(1500);
  }

  while (0) // digitalRead(RX_1) == 1
  {
    while (digitalRead(RX_1) == 1)
    {
      digitalWrite(TX_1, HIGH);
    }
    if (opadavani_pomalu == true)
      delay(1500);
  }



  digitalWrite(TX_mb, LOW);
  digitalWrite(TX_vhf, LOW);
  digitalWrite(TX_uhf, LOW);
  digitalWrite(TX_1, LOW);
}








