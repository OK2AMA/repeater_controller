// Selektikvní volba do rádiového převáděče Karel 


const int ledPin =  A0;      // the number of the LED pin
const int DTMF_std = 12;     
const int DTMF1 = 8;     
const int DTMF2 = 9;     
const int DTMF3 = 10;     // píp
const int DTMF4 = 11;
     
const int RX_1 = A4;    
const int RX_2 = A6;     // výstup pro vypnutí TX stanice po 60s.
const int RX_3 = 3;     
const int RX_4 = 13;    
const int TX_1 = A3;     // vysila se uzemnenim
const int TX_2 = A5;
const int TX_3 = A7;
const int TX_4 = 7;
const int TX_5 = 6;
const int TX_6 = 5;



const byte beep = 11;    // dtmf3

byte byt ;
byte data ;
byte temp ;
byte i ;
boolean blok_zvuk ;  // blokace zvuku vypbuta po zapnutí
boolean blok_roger ;  // true = zapnut po startu ROGER // ma byt true = zaple po startu
boolean DTMF_byla ;  // roger pouze bez dtmf
boolean poprve ;     // pomocna promena pro specificke chovani bezd DTMF
boolean opadavani_pomalu ;   // zapnuto opadavani
boolean opadavani_predtim ;  //
boolean povoleni_uhf_tx ;  //

unsigned long vse;
int citac; //setreni vysilaci stanice
int napeti ;

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

  vse = 0;
  citac = 0; //setreni vysilaci stanice
  napeti = 0;

  //  Serial.begin(19200);
  analogReference(DEFAULT);
  pinMode(ledPin, OUTPUT);          

  pinMode(DTMF_std, INPUT);  
  pinMode(DTMF1, INPUT);     
  pinMode(DTMF2, INPUT);     
  pinMode(DTMF3, INPUT);     
  pinMode(DTMF4, INPUT);     

  pinMode(RX_1, INPUT);
  pinMode(RX_2, INPUT);
  pinMode(RX_3, INPUT);
  pinMode(RX_4, INPUT);

  pinMode(TX_1, OUTPUT);
  pinMode(TX_2, OUTPUT);
  pinMode(TX_3, OUTPUT);
  pinMode(TX_4, OUTPUT);
    
  //pinMode(beep, OUTPUT); // tone(pin, frequency, duration)     
 
  digitalWrite(TX_1, LOW);   
  digitalWrite(TX_2, LOW); 
  digitalWrite(TX_3, LOW);
  digitalWrite(TX_4, LOW);
  
  //start();  // uvodni znelka s TX
}

void loop(){
  while(digitalRead(RX_1) == 1)
  {   
      while(digitalRead(RX_1) == 1)
  {   
     digitalWrite(TX_1, HIGH);     
  }
  delay(800);
  }
  
  while(digitalRead(RX_2) == 1)
  {   
      while(digitalRead(RX_2) == 1)
  {   
     digitalWrite(TX_2, HIGH);     
  }
  delay(800);
  }

  while(digitalRead(RX_3) == 1)
  {   
      while(digitalRead(RX_3) == 1)
  {   
     digitalWrite(TX_3, HIGH);     
  }
  delay(800);
  }

  while(digitalRead(RX_4) == 1)
  {   
      while(digitalRead(RX_4) == 1)
  {   
     digitalWrite(TX_4, HIGH);     
  }
  delay(800);
  }
  
  
  digitalWrite(TX_1, LOW);   
  digitalWrite(TX_2, LOW); 
  digitalWrite(TX_3, LOW);
  digitalWrite(TX_4, LOW);
}








