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

// Nefunguje blokovani, spatne cte DTMF a pipa roger do hovoru(obcas)
//unguje 431 a 430 vypínání VHF
//nefunguje 421 a 420 vypínání MB, celkové vypínání 936 a 937 funguje
// hlášení po čase 871 a 872 nefunguje, delší prodlevy sem nezkoušel
// *******************************************************************
// opraveno zablkováni GP9OO po rychlem zaklicovani
// opraveno blokovani MB ( 421 )

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
const int TX_uhf = 7;

const int TX_1 = A7;
const int TX_2 = 6;
const int TX_3 = 5;

const int beep_pin = 3;    // dtmf3, beep output

boolean blok_zvuk ;  // blokace zvuku vypbuta po zapnutí
boolean blok_roger = false ;  // true = zapnut po startu ROGER // ma byt true = zaple po startu
boolean DTMF_byla ;  // roger pouze bez dtmf
boolean opadavani_pomalu = true;   // zapnuto opadavani
boolean crossband_mode = false;
boolean crossband_extended = false;
boolean hourly = true;

boolean en_TX_mb = true;
boolean en_TX_vhf = true;

unsigned long CurrentMillis = millis();
unsigned long TempMillis;
unsigned long TX_delay_millis;
unsigned long PreviousMillis;
unsigned long how_often_alarm;

long vse = 0;
long data = 0;
int band_activity = 0;
int i = 0;

void f_TX_mb()
{
  if (en_TX_mb == true)
    digitalWrite(TX_mb, HIGH);
  else
    digitalWrite(TX_mb, LOW);
}

void f_TX_vhf()
{
  if (en_TX_vhf == true)
    digitalWrite(TX_vhf, HIGH);
  else
    digitalWrite(TX_vhf, LOW);
}

boolean read_debounc(int debounce_pin)
{
  byte temp = 0;
  for (byte i = 0; i <= 200; i++ )
  {
    if (digitalRead(debounce_pin) == 1)
      temp = temp + 1 ;
    delayMicroseconds(2);
  }
  if (temp >= 100)
    return true;
  else
    return false;
}

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
    f_TX_mb();
  if (band_activity == 2)
    f_TX_mb();
  if (band_activity == 3)
    f_TX_mb();
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
  delay(10);
  if (read_debounc(RX_mb) == 1)
    band_activity = 1;
  if (read_debounc(RX_vhf) == 1)
    band_activity = 2;
  if (read_debounc(RX_uhf) == 1)
    band_activity = 3;

  vse = 0;
  while ((read_debounc(RX_mb) == 1) or (read_debounc(RX_vhf) == 1))
  { // RX
    if (read_debounc(DTMF_std) == 1 )
    {
      delay(5);
      cteni_bytu();
      while (digitalRead(DTMF_std) == 1 )
      {
        delay(5);
      }
    }
    delay(5);
  }// waiting for stop transmitting

  //--- zde projde program po odklíčování-------------------------------
  switch (vse) {
    /*
      DTMF code is different compare to BCD
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
    case 0x87A :
      hourly = false;
      break;
    case 0x871 :// 1 minut
      hourly = true;
      how_often_alarm = 1 * 60 * 1000;
      break;
    case 0x872 :
      hourly = true;
      how_often_alarm = 2 * 60 * 1000;
      break;
    case 0x873 :
      hourly = true;
      how_often_alarm = 5 * 60 * 1000;
      break;
    case 0x874 :
      hourly = true;
      how_often_alarm = 15 * 60 * 1000;

    case 0x875 :
      hourly = true;
      how_often_alarm = 30 * 60 * 1000;
      break;
    case 0x876 :
      hourly = true;
      how_often_alarm = 60 * 60 * 1000;
      break;

    case 0x877 :
      hourly = true;
      how_often_alarm = 120 * 60 * 1000;
      break;

    case 0x936 : // disable all TX
      en_TX_mb = false;
      en_TX_vhf = false;
      break;
    case 0x937 : // enable all TX
      en_TX_mb = true;
      en_TX_vhf = true;
      break;

    case 0x42A : // disable MB tx
      en_TX_mb = false;
      break;
    case 0x421 : // enable MB tx
      en_TX_mb = true;
      break;
    case 0x43A : // disable VHF tx
      en_TX_vhf = false;
      break;
    case 0x431 : // enable VHF tx
      en_TX_vhf = true;
      break;

    case 0x146 :
      setup(); // soft reset !!
      break;


    case 0x51 : // kompletní zapovězení akustického pípání, rogery, identifikace ...
      pinMode(beep_pin, INPUT);     // pinD4 je nutno stahovat k zemi odporem cca 2 - 10 k ohm, pri zablok TX je na nem vyosoká impendace (spojená s + vnitřním odporem 100k ohm)
      break;
    case 0x5A : // v základu je povoleno
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
    case 0x9A : // zapnout crossband
      crossband_mode = false;
      crossband_extended = false;
      break;
    case 0x91 :
      crossband_mode = true;
      crossband_extended = true;
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
      telegraf(B10000000);// h
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
  DTMF_byla = false;  // roger pouze bez dtmf
  opadavani_pomalu = true;   // zapnuto opadavani
  blok_zvuk ;  // blokace zvuku vypbuta po zapnutí
  blok_roger = false ;  // true = zapnut po startu ROGER // ma byt true = zaple po startu
  DTMF_byla ;  // roger pouze bez dtmf
  opadavani_pomalu = true;   // zapnuto opadavani
  crossband_mode = false;
  crossband_extended = false;
  hourly = true;
  how_often_alarm = 60 * 60 * 1000;

  long vse = 0;
  long data = 0;
  int band_activity = 0;
  int i = 0;


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

  digitalWrite(TX_vhf, HIGH);

  //start();  // uvodni znelka s TX
  telegraf(B01000000);// a -.-.  delka 4 : 100/1010-
  //  telegraf(B10011010);// h
  //  telegraf(B01111100);// o
  //  telegraf(B10001110);// j

  delay(1000);
  digitalWrite(TX_mb, LOW);
  digitalWrite(TX_vhf, LOW);
  digitalWrite(TX_uhf, LOW);
  digitalWrite(TX_1, LOW);
}


void loop() {
  CurrentMillis = millis();

  if ((unsigned long) CurrentMillis < 15000)//
  {
    // co bude delat 15s po zapnuti
    //crossband_mode = true;
    //crossband_extended = true;
  }

  if (hourly == true) // pravidelne hlaseni
  {
    if ((unsigned long) CurrentMillis > (TX_delay_millis + 10000)) // aby neklicovalo v prubehu hovoru
    {
      if ((unsigned long) CurrentMillis > ( how_often_alarm + TempMillis))
      {
        TempMillis = CurrentMillis;
        f_TX_mb();
        delay(300);
        telegraf(B10001000);// L
        digitalWrite(TX_mb, LOW);
        delay(300);

        f_TX_vhf();
        delay(300);
        telegraf(B10001000);// L
        digitalWrite(TX_vhf, LOW);
        delay(300);
      }
    }
  }

  while (read_debounc(RX_mb) == 1)
  {

    if (crossband_mode == false)
      f_TX_mb();
    else
      f_TX_vhf();

    if (crossband_extended == true)
      f_TX_mb();

    while (read_debounc(RX_mb) == 1)
    {
      if (read_debounc(DTMF_std) == 1)
        dtmf_service();
    }

    delay(270);// z duvodu skrnuti po odklicovani aby byl cisty roger
    tone(beep_pin, 700); // -
    if (1)
      delay(200);
    noTone(beep_pin);
    if (opadavani_pomalu == true)
      delay(2200);
    TX_delay_millis = CurrentMillis;
  }





  while (read_debounc(RX_vhf) == 1)
  {
    if (crossband_mode == false)
      f_TX_vhf();
    else
      f_TX_mb();

    if (crossband_extended == true)
      f_TX_vhf();

    while (read_debounc(RX_vhf) == 1)
    {
      if (read_debounc(DTMF_std) == 1)
        dtmf_service();
    }

    delay(270);
    tone(beep_pin, 700);
    if (blok_roger == false)
      delay(200);
    noTone(beep_pin);
    if (opadavani_pomalu == true)
      delay(2200);
    TX_delay_millis = CurrentMillis;
  }


  if (TX_vhf == HIGH) // GP900 nesnasi rychle zaklicovani po sobě
  {
    digitalWrite(TX_vhf, LOW);
    delay(350); // jak dlouho ceka
  }
  tx_quiet();
}








