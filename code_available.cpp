#define LED_BUILTIN 13

#define KIRMIZI_LED A0
#define YESIL_LED A1

#include <EEPROM.h>

struct hizmet{
  int hizmetID;
  char hizmetAd[20];
  int kalanHizmetAdet;
  int hizmetFiyat;
};

 String met = "20,20,10,30,5\n1,kopukleme,30,15 TL\n2,yikama,50,10 TL\n3,kurulama,100,5 TL\n4,cilalama,20,50 TL\n";
  

int bekle = 0;
int randomSayi;

uint8_t tuslar[5];
bool flag_tuslar[5];
uint8_t tuslar2[4];
bool flag_tuslar2[4];

bool bitis_flag = false;
bool bitis_flag2 = false;
bool reset_flag = false;
struct hizmet hizmetler[4];
volatile int kasa[5];
int kasaParasi = 0;
int hizmetParasi = 0;



void setup()
{
  Serial.begin(115200);
  Serial.println("");
  
  pinMode(KIRMIZI_LED, OUTPUT);
  pinMode(YESIL_LED, OUTPUT);
  
  /* Veri okuma */

  String line = "";
  uint8_t counter = 0;
  
  for(char tmp : met){

    //char tmp = met.charAt(i);
    
    if(tmp == '\n'){
        Serial.println(line);
      	
      if(counter == 0){
		sscanf(line.c_str(), "%d,%d,%d,%d,%d", kasa, kasa+1, kasa+2, kasa+3, kasa+4);
      }else{
		sscanf(line.c_str(), "%d,%[^','],%d,%d TL\n", &hizmetler[counter-1].hizmetID, 
                                              &hizmetler[counter-1].hizmetAd, 
                                              &hizmetler[counter-1].kalanHizmetAdet, 
                                              &hizmetler[counter-1].hizmetFiyat);
        
      }
        line = "";
      	counter++;
      
    } else{
        line.concat(tmp);
    }
  }
  randomSeed(analogRead(3));
  
  Serial.println("\nMakine Bilgileri:");
  Serial.println("Kasadaki Paralar : ");
  
  Serial.print("5 TL miktari : ");
  Serial.println(kasa[0]);
  Serial.print("10 TL miktari : ");
  Serial.println(kasa[1]);
  Serial.print("20 TL miktari : ");
  Serial.println(kasa[2]);
  Serial.print("50 TL miktari : ");
  Serial.println(kasa[3]);
  Serial.print("100 TL miktari : ");
  Serial.println(kasa[4]);
  
  delay(2000);
  
}

void loop()
{
  for(int i = 0; i < 5; i++){
    
    if(digitalRead(2 + i) == 0 && flag_tuslar[i])
      flag_tuslar[i] = 0;
    
    if(!bitis_flag)
    { 
      if(!flag_tuslar[i] && digitalRead(2 + i)){
  	 	tuslar[i] += 1;
      	flag_tuslar[i] = true;
        }
    }
    
    if(!bitis_flag && bekle%1000 == 0){
     	Serial.print(i);
     	Serial.print(".indis");
     	Serial.println(tuslar[i]);
    }
 
  }
  
  
  if(digitalRead(7) && !bitis_flag){
    
    bitis_flag = true;
  	Serial.println("Bitis tusuna basildi.");
   	kasaParasi += 100 *  tuslar[4];
    kasaParasi += 50  *  tuslar[3];
    kasaParasi += 20  *  tuslar[2];
    kasaParasi += 10  *  tuslar[1];
    kasaParasi += 5   *  tuslar[0]; 
    Serial.print("Kasadaki Para : ");
    Serial.print(kasaParasi);
    Serial.println(" TL");
    Serial.println(kasa[2]);
  }
  
  if((!bitis_flag || !bitis_flag2) && bekle%1000 == 0)
  	Serial.println();
  
  if(bitis_flag){
    
    for(int i = 0; i<4; i++){
      if(digitalRead(8 + i) == 0 && flag_tuslar2[i])
        flag_tuslar2[i] = 0;

      if(!bitis_flag2)
      { 
        if(!flag_tuslar2[i] && digitalRead(8 + i)){
          tuslar2[i] += 1;
          flag_tuslar2[i] = true;
          }
      }

      if(!bitis_flag2 && bekle%1000 == 0){
          Serial.print(i);
          Serial.print(".Hizmetindis");
          Serial.println(tuslar2[i]);
      }
      
    }
    if(!bitis_flag2 && bekle%1000 == 0)
    {
      Serial.print("Makineye Yuklenen Miktar : ");
      Serial.println(kasaParasi);
    }
  }
  
  
  if(digitalRead(12) && !bitis_flag2 )
  {
    bitis_flag2 = true;
    Serial.println("\n2. bitise bastiniz.");
    
    randomSayi = random(1, 5);
    
    Serial.print("Random Sayi:");
    Serial.println(randomSayi);
    
    if(randomSayi == 2)
    {
      digitalWrite(A0, HIGH);
      Serial.println("Para Takilma Durumu...\nParaniz Iade Ediliyor...\nReset tusuna basiniz.");
      //Para Iadesi
      kasaParasi = 0;
    }
    else{
      
      digitalWrite(A1, HIGH);


      //Koyulan paranın yeterliliği kontrol edilir.
      if(kasaParasi >= hizmetParasi)
      {
        Serial.println("Yuklediginiz Para Miktari Yeterlidir.");
        
        int kopukMiktar    = tuslar2[0];
        int yikamaMiktar   = tuslar2[1];
        int kurulamaMiktar = tuslar2[2];
        int cilalamaMiktar = tuslar2[3];

        for(struct hizmet &x : hizmetler){

          if(x.hizmetID == 1){
            kopukMiktar %= x.kalanHizmetAdet;
            hizmetParasi += kopukMiktar * x.hizmetFiyat;
            x.kalanHizmetAdet -= kopukMiktar;
          }else if(x.hizmetID == 2){
            yikamaMiktar %= x.kalanHizmetAdet;
            hizmetParasi += yikamaMiktar * x.hizmetFiyat;
            x.kalanHizmetAdet -= yikamaMiktar;
          }else if(x.hizmetID == 3){
            kurulamaMiktar %= x.kalanHizmetAdet;
            hizmetParasi += kurulamaMiktar * x.hizmetFiyat;
            x.kalanHizmetAdet -= kurulamaMiktar;
          }else if(x.hizmetID == 4){
            cilalamaMiktar %= x.kalanHizmetAdet;
            hizmetParasi += cilalamaMiktar * x.hizmetFiyat;
            x.kalanHizmetAdet -= cilalamaMiktar;
          }
        }
        
        Serial.print("Hizmet Parasi : ");
        Serial.println(hizmetParasi);
        Serial.print("Makineye Yuklenen Para Miktari : ");
        Serial.println(kasaParasi);
        
        // Para üstü hesabı
        int paraUstu = kasaParasi - hizmetParasi;
        Serial.print(paraUstu);
        Serial.println(" TL Iade Edilecek..\n");
        
        while(paraUstu > 0){
        
          // para ustu 100 tl denf azla ve kasada 100 bankot var
          if(paraUstu >= 100 && kasa[4] > 0){
            Serial.println("100 tl bankot veriliyor");
            paraUstu -= 100;
            kasa[4]--;
          }
          
          if(paraUstu >= 50 && kasa[3] > 0){
            Serial.println("50 tl bankot veriliyor");
            paraUstu -= 50;
            kasa[3]--;
          }
          
          if(paraUstu >= 20 && kasa[2] > 0){
            Serial.println("20 tl bankot veriliyor");
            paraUstu -= 20;
            kasa[2]--;
          }
          
          if(paraUstu >= 10 && kasa[1] > 0){
            Serial.println("10 tl bankot veriliyor");
            paraUstu -= 10;
            kasa[1]--;
          }
          
          if(paraUstu >= 5 && kasa[0] > 0){
            Serial.println("5 tl bankot veriliyor");
            paraUstu -= 5;
			kasa[0]--;
          }
          
        }
        Serial.println("Tum para ustu verildi. \nTessekurler.");
        
        
      } else {
        Serial.println("Yuklediginiz Para Miktari Yeterli Degildir.");
        // Para iadesi
        Serial.println("Para iadesi yapiliyor... Reset tusuna basiniz.");
        kasaParasi = 0;
      }
    }
  }
  
  // Reset tuşu
  if(digitalRead(13) && !reset_flag)
  {
    Serial.println("\n\nReset Tusuna Basildi.\nTum Veriler Sifirlaniyor...\n\n");
    for(uint8_t &x : tuslar2)
      x = 0;
    
    for(uint8_t &x : tuslar)
      x = 0;
    
    bitis_flag = false;
    bitis_flag2= false;
    
    // para yapılmadıysa iade
    kasaParasi = 0;
    reset_flag = true;
    
     
  }
  if(reset_flag && !digitalRead(13))
  {
    reset_flag = false;
  }
  
  bekle++;
  bekle %= 15000;
  
  
}







