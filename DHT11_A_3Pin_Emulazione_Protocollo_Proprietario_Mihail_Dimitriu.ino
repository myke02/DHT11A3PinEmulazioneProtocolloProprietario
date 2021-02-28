int pin = 7; //Definizione pin utilizzato

unsigned long duration;
unsigned long tmr = 0;
unsigned long tmr_prec = 0;
int impulsi;
int message[41];

long int DataTimeBlock = 0;
bool readBlocked = true;

long int DataTime = 0;
byte Result[41];
byte DataArray = 0;
byte DHTData[4];

void setup()

{
  //init seriale
  Serial.begin(9600);
  //definisco la porta 2 come uscita
  //in questa porta è collegato il sensore dht11
  pinMode(pin, OUTPUT);
}

void loop()
{
   
   delay(1000);

  //definisco la porta 2 come uscita
  //in questa porta è collegato il sensore dht11
  pinMode(pin, OUTPUT);

  digitalWrite(pin,HIGH);
  delay(100); //Wait 100millisec
  
  //sappiamo che sul pin c'è un livello logico alto
  //avvendo inserito la resistenza di pullup
  //metto la linea a livello basso per 20 millisecondi (>18ms)
  digitalWrite(pin, LOW);
  delay(20);
  
  //metto nuovamente la linea a livello logico alto
  digitalWrite(pin, HIGH);

  //imposto la stessa porta come ingresso
  //in modo da ricevere i dati dal sensore dht11
  pinMode(pin, INPUT);
  
  delayMicroseconds(10);
  
  
  //inizializzo la variabile a 0, questa variabile consente di tenere
  //traccia del numero di bit ricevuti
  impulsi = 0;

  //eseguo un ciclo continuo
  while (true)
  {

    long int DataTimeBlock = 0;
    bool readBlocked = true;

    //controllo solo quando è presente un livello alto
    if (digitalRead(pin) == HIGH)
    {
      //memorizzo il tempo in microsecondi
      tmr_prec = micros();

      //eseguo un ciclo fintanto che il livello è alto
      while (digitalRead(pin) == HIGH)
      {
        ;
      }

      //quando il livello logico diventa basso
      //misuro quanto tempo è durato il livello logico alto
      duration = micros() - tmr_prec;
 
      //memorizzo questo tempo nell'array
      message[impulsi] = duration;
      
      //incremento la variabile per tenere traccia dei bit inviati
      //dal sensore dht11
      impulsi++;
    }

    //quando ho raggiunto 41 bit interrompo il ciclo while
    //i bit sono 41, invece che 40, perchè questo codice
    //misura anche il primo livello alto di 80us che indica l'inizio
    //della trasmissione dei 40 bit di dati 
    if (impulsi == 41)
      break;
  }

  //ora che ho un array contenente i microsecondi di tutti i livelli alti che sono stati inviati
  //dal sensore dht11 posso visuallizzarli tramite il monitor seriale per avere un idea del
  //discostamento dei tempi affettivi da quelli previsti, valori che utilizzerò successivamente
  //quando effettuerò la conversione dei bit

   for( int x = 1; x < 25; x++) {

  Serial.print(x); Serial.print(") ");
  Serial.println(message[x]);
  }

  //Riassegnazione dei valori dell'array a seconda dei tempi rilevati
  //E controllo del Checksum
  bool checksum_mistake = false;
  for( int x = 1; x < 41; x++) {
    if(message[x] >= 20 && message[x] <=30) {
       message[x] = 0;
    } else if (message[x] >= 60 && message[x] <= 80){
      message[x] = 1;
    } else {
      checksum_mistake = true;
    }

    Serial.print(message[x]); Serial.print(",");
    if(x%8 == 0) Serial.println("");
  }

   // Assegnazione dei 5 bytes a partire dai 40 bit dell'array "Result"
   //(Si ignora il primo bit perchè riguarda il tempo di MSB)
   for (int j=0; j< 5; j++){ // redo it for the 5 Bytes (40 Databits /8 = 5)
    for (int i=0; i< 8; i++) {bitWrite(DHTData[j], 7-i, message[i+1+(j*8)]);}
  }

  //Controllo Checksum finale
  if (checksum_mistake) {
    for (int i=0; i< 8; i++) {bitWrite(DHTData[4], 7-i, 1);}
  }
  
  int Humidity = 0;
  int Temp = 0;
  int TempComma = 0;

  Serial.println("Humidity: "); Serial.print("BIN: ");
  Serial.println(DHTData[0], BIN); Serial.print("DEC: ");
  Serial.println(DHTData[0], DEC);
  Serial.println("");
  
  Serial.println("Humidity dec: "); Serial.print("BIN: ");
  Serial.println(DHTData[1], BIN); Serial.print("DEC: ");
  Serial.println(DHTData[1], DEC);
  Serial.println("");

  Serial.println("Temperature: "); Serial.print("BIN: ");
  Serial.println(DHTData[2], BIN);  Serial.print("DEC: ");
  Serial.println(DHTData[2], DEC);
  Serial.println("");
  
  Serial.println("Temperature dec: "); Serial.print("BIN: ");
  Serial.println(DHTData[3], BIN);  Serial.print("DEC: ");
  Serial.println(DHTData[3], DEC);
  Serial.println("");

  int ExpectedCheckSum = (DHTData[0]+DHTData[1]+DHTData[2]+DHTData[3]);
  Serial.print("Expected CheckSum: ");Serial.println(ExpectedCheckSum);

  if (DHTData[4] == ExpectedCheckSum){
    Serial.println("CheckSum IS Verified! \n");
    Humidity = DHTData[0];Temp = DHTData[2];TempComma = DHTData[3];
    Serial.print("Humidity = ");Serial.print(Humidity);Serial.print("% ");
    Serial.print(" Temp = ");Serial.print(Temp);
    Serial.print(",");Serial.print(TempComma);Serial.println("°C ");
  } else { Serial.println("Error");
           Serial.print("CheckSum ISN'T Verified!");
  }
}
