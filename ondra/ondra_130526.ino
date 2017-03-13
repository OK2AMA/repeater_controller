// Selektikvní volba do rádiového převáděče Ondra III. 151,4 169,0375 170,875 

//#include <Narcoleptic.h>
const int ledPin =  A0;      // the number of the LED pin
const int DTMF_std = 9;     
const int DTMF1 = 13;     
const int DTMF2 = 12;     
const int DTMF3 = 11;     // píp
const int DTMF4 = 10;     
const int D2 = 2;     // libovolný vstup 1/0 na dotaz 5 řekne jeho stav
const int D3 = 3;     // výstup pro vypnutí TX stanice po 60s.
const int D4 = 4;     
const int RX = A4;    
const int TX = 7;     // vysila se uzemnenim
const int pwrsw = 8; // dtmf, nefunguje
const int vystup2 = 6;
const int vystup1 = 5;
const int blok1 = A1;  // VHF stanice
const int blok2 = A2;
const int teplota = A3;
const byte baterie = A5; // max 25 V delic je 1/10 (1k + 9k odpor přesně !!) , hodnota ve voltech se vypočte vynásobením přijatého čísla * 0,025
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

  pinMode(D2, INPUT);     
  pinMode(D3, OUTPUT);  // setreni TX    
  pinMode(D4, OUTPUT);  // audio vystup     
  digitalWrite(D2, HIGH);
  digitalWrite(D3, LOW); // 
  digitalWrite(D4, LOW);

  pinMode(RX, INPUT);     
  pinMode(TX, OUTPUT);   
  pinMode(vystup1, OUTPUT);  
  pinMode(vystup2, OUTPUT); 
  pinMode(blok1, OUTPUT);   
  pinMode(blok2, OUTPUT);   
  pinMode(baterie, INPUT);   
  //pinMode(beep, OUTPUT); // tone(pin, frequency, duration)     
  pinMode(pwrsw, OUTPUT); 

  digitalWrite(RX, HIGH);   // zde je připojen signál oznamující přítomnost nosné vlny, aktivní na vysoké úrovni
  digitalWrite(pwrsw, LOW); // musi byt porad na zemi, PWRSW nefunguje u daného čipu, pin je spojen se zemí a kladným napětím zde stoupne odběr na 60mA
    digitalWrite(blok1, LOW);
  digitalWrite(blok2, LOW);
     digitalWrite(vystup1, LOW);
  digitalWrite(vystup2, LOW);
  start();  // uvodni znelka s TX
}

void loop(){

  while ( digitalRead(RX) == 0)
  {
    citac = citac++ ;

    if (citac > 18000) // pri 30 ms prodleve je to 60s
    {
      citac = 20000;
      digitalWrite(D3,HIGH); //       
      // Narcoleptic.delay(20); 
    }
    else
    {
      digitalWrite(D3, LOW); // 
    }

    digitalWrite(ledPin, LOW);  // setreni zaclo, 0V na "D3"
    //Narcoleptic.delay(25); 
    delay(2);
    DTMF_byla = false; // 1 = ma by RB
    vse = 0;
    noTone(D4);              
    digitalWrite(TX, HIGH);   
  }

  //-----------------------------------------------------
  while ( digitalRead(RX) == 1) { // RX  
    if(DTMF_byla == true)  // pokud nebyla DTMF - vysila TX a bude roger == true
    {
      digitalWrite(TX, HIGH);  // nebyla DTMF, vysila stale
    }
    else
    {
      digitalWrite(TX, LOW);  // dtmf již byla, nevysila a ceka na dalsi, nebo konec RX
    }

    digitalWrite(D3, LOW); // zapnuti TX stanice
    citac = 0;
    digitalWrite(ledPin, HIGH);

    if (digitalRead(DTMF_std) == 1)
    {
      digitalWrite(TX, HIGH); // při první DTMF okamžitě vypne TX a v průběhu zadávíní DTMF již nevysílá // ještě je jednoduchá možnost při každé chvilkové přítomnosti DTMF potlačit TX, nicmene vysilac by vysial moc napadne s prestavkama, ale clovek by vedel že to na něj zareagovalo 
      DTMF_byla = true ;
      cteni_bytu();
      digitalWrite(ledPin, LOW);
      while(digitalRead(DTMF_std) == 1 )
      {
        //   zde cekam na vypnuti DTMF
      }
    }// cyklus DTMF skončil

  }//--- zde projde program po odklíčování------------------------------- 

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

    /*  case 0x3731 :
     digitalWrite(D3, HIGH);
     break;
     case 0x3730 :
     digitalWrite(D3,HIGH);
     break;´
     case 0x3741 : // na radiu 3741 , zapnutí D4
     digitalWrite(D4, HIGH);
     break;
     case 0x374A :  // na radiu 3740, vypnutí D4
     digitalWrite(D4, LOW);
     break;  */

  case 0x2811 : 
    digitalWrite(vystup1, HIGH);
    break;
  case 0x281A :  // na radiu 2810
    digitalWrite(vystup1, LOW);
    break;

  case 0x2821 : 
    digitalWrite(vystup2, HIGH);
    break;
  case 0x282A :  // 2820
    digitalWrite(vystup2, LOW);
    break;


  case 0x461 :  // zap. 2m linku
    digitalWrite(blok1, HIGH);
    break;
  case 0x46A : // 4610 
    digitalWrite(blok1, LOW);
    break;

  case 0x8721 : 
    digitalWrite(blok2, HIGH);
    break;
  case 0x872A :  
    digitalWrite(blok2, LOW);
    break;


  case 0x936 : // kompletní zapovězení TX ze strany selektivky !! pro naprostou rádiovou neviditelnost ;-)
    pinMode(TX, INPUT);     // pin TX je nutno stahovat k zemi odporem cca 2 - 10 k ohm, pri zablok TX je na nem vyosoká impendace (spojená s + vnitřním odporem 100k ohm) 
    break;
  case 0x937 : // v základu je vždy TX povoleno, a tímto se opět povoluje 
    pinMode(TX, OUTPUT);    
    break;16

  case 0x926 : // kompletní zapovězení akustického pípání, rogery, identifikace ...
    pinMode(D4, INPUT);     // pinD4 je nutno stahovat k zemi odporem cca 2 - 10 k ohm, pri zablok TX je na nem vyosoká impendace (spojená s + vnitřním odporem 100k ohm) 
    break;
  case 0x927 : // v základu je povoleno 
    pinMode(D4, OUTPUT);    
    break;
    // po relaci ovladaci prvky  * * * * * * * * * * * * * * * * 
  case 0x1621 : // blokovani rogeru
    blok_roger = false;      
    break;
  case 0x162A :  
    blok_roger = true;    
    break;

  case 0x738A : // rychlost opadavani
    opadavani_pomalu = false;      
    break;
  case 0x7381 :  
    opadavani_pomalu = true;    
    break;

  case 0x487A : // rychlost PRED opadavani 
    opadavani_predtim = false;      
    break;
  case 0x4871 :  
    opadavani_predtim = true;    
    break;


  case 0x274A : // vypnout vsechnu identifikaci po relaci
    opadavani_predtim = false;  
    opadavani_pomalu = false;    
    blok_roger = true;       
    break;

  case 0x2741 :   // zapnout vsechnu identifikaci po relaci
    opadavani_predtim = true; 
    opadavani_pomalu = true;    
    blok_roger = false;     
    break;

  case 0x146 :  
    setup(); // reset !!  
    break;

  case 0x48 : 
    analog_cteni(baterie);   
    break;
  case 0x49 :  
    analog_cteni(teplota);   
    break;

  case 0x5 : // telegrafická identifikace
    digitalWrite(TX, LOW);
    delay(300);
    //  telegraf(B01001000);// 010 =2 / 01--- == .- == A
    telegraf(B10010100);// C -.-.  delka 4 : 100/1010-
    telegraf(B10011010);// q
    delay(50);
    digitalWrite(TX, LOW);
    break;

  case 0x4 : // telegrafická identifikace
    digitalWrite(TX, LOW);
    delay(300);
    //  telegraf(B01001000);// 010 =2 / 01--- == .- == A
    telegraf(B01001000);// a -.-.  delka 4 : 100/1010-
    telegraf(B10011010);// h
    telegraf(B01111100);// o
    telegraf(B10001110);// j

    delay(50);
    digitalWrite(TX, LOW);
    break;
    
  default:     
    digitalWrite(TX, LOW);
    delay(500);
    //  telegraf(B01001000);// 010 =2 / 01--- == .- == A
    telegraf(B01100000);// C -.-.  delka 4 : 100/1010-
    delay(50);
    digitalWrite(TX, HIGH);
    break;

  }// konec case ****

  if((DTMF_byla == false) &&( opadavani_predtim == true))  // pokud nebyla DTMF tak podrží
  {    
    digitalWrite(TX, LOW);
    delay(380);  // 
    digitalWrite(TX, HIGH);
  }

  if((DTMF_byla == false )&&(blok_roger == false ))  // pokud nebyla DTMF tak roger
  {
    digitalWrite(TX, LOW);  // tato prodleva zajišťuje bezpečnostní čas pro poslech vlastního rogeru    
    tone(D4,1000 ); // (pin(D4), frekvence v Hz)
    delay(95);
    noTone(D4);  
    delay(10);
    digitalWrite(TX, HIGH);
  }
  if((DTMF_byla == false) &&( opadavani_pomalu == true))  // pokud nebyla DTMF tak podrží
  {
    digitalWrite(TX, LOW);    
    delay(1200); // delka podrzeni TX  
    digitalWrite(TX, HIGH);
  }

}// konec smycky loop

//--- Zde jsou již jen podprogramy a rutiny ----------------------
//****************************************************************
//----------------------------------------------------------------

void cteni_bytu()
{
  vse = vse<<4;
  data = 0x00;
  data = ((digitalRead(DTMF4)|data)<<1);
  data = ((digitalRead(DTMF3)|data)<<1);
  data = ((digitalRead(DTMF2)|data)<<1);
  data = ((digitalRead(DTMF1)|data)); 
  data = data & B00001111;
  vse = (vse | data);
}

void analog_cteni(byte cislo_portu)
{
  digitalWrite(TX, LOW);
  napeti=analogRead(cislo_portu);
  delay(70) ;  // prodleva pro cekani nabehnuti vysilace naplno
  for(int i = 0; i < 10; i++)
  {
    if ((napeti & 1) == 1){
      tone(D4, 1200); 
      delay(70);
      noTone(D4);
      delay(50);
      tone(D4, 120); 
      delay(70);
      noTone(D4);    
    }
    else{
      tone(D4, 1000, 600);
      delay(500); 
      noTone(D4);
    }
    napeti = napeti >> 1;
    delay(500);
  }   
  noTone(D4);
  delay(150);
  digitalWrite(TX, HIGH);
  delay(150);
}

void start()
{  
  digitalWrite(TX, LOW); // -
  delay(400); 
  tone(D4, 1500); 
  delay(300);      
  noTone(D4);
  delay(50);

  tone(D4, 1500);  // .
  delay(70);      
  noTone(D4);
  delay(50);

  tone(D4, 1500); // - 
  delay(300);      
  noTone(D4);
  delay(50);

  digitalWrite(TX, HIGH);
  delay(200);
}

void telegraf(byte zdroj) // zdroj: 0Bdddzzzzz 3x delka a nasledne 5x znak
{                       // pouze tisk petiznakovych zancek
  byte i = 0;
  byte delka = (zdroj & B11100000)>>5;
  byte znaky = (zdroj & B00011111);
  delay(100);      

  for  ( i=0 ; i < delka ; i++)
  {
    if(((znaky & B00010000 )) == B00010000)
    {  
      tone(D4, 1200); 
      delay(210);      
      noTone(D4); 
    }
    else
    {         
      tone(D4, 1200); 
      delay(70);      
      noTone(D4); 
    }
    delay(70);
    znaky = znaky << 1;
  }
  delay(200);
}










