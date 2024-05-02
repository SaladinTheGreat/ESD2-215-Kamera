 




  const int PinInSize = 2;
  const int PinIn[PinInSize]={25,26};
  const int PinOutSize = 2;
  const int PinOut[2]={16,17};
    //Pins ind og ud


  const int Asize = 50;
  const int SampleTime = 50;
    //Asize er array'ets størrelse, SampleTime er frekvensen af samples.


  int MikSampleMain[Asize];
  int MainPin;


  int MikSampleSecond[Asize];
  int SecondPin;
    //De to arrays der bliver skrevet til for mik input, Hvilken pin er først skal findes senere.

  int DelayXY;
  int DelayXZ;


  volatile bool FirstTrig = false;


 void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Sneaker("Setting up",__LINE__);


/*for (int i = 0; i < PinInSize; i++) {
    pinMode(PinIn[i], INPUT);
    Serial.println(PinIn[i]);
}*/ 
  pinMode(A0, INPUT);
  pinMode(A2, INPUT);

for (int i = 0; i < PinOutSize; i++) {
    pinMode(PinOut[i], OUTPUT);
    Serial.println(PinOut[i]);
}


  StartInterrupt();
  //Klargøre triggers til første lyd


}


void StartInterrupt() {
  for (int i = 0; i < PinInSize; i++){
  attachInterrupt(digitalPinToInterrupt(PinIn[i]), FirstSound, HIGH);
  }
    Sneaker("setting interrupts 1st time",__LINE__);
    //Nulstiller alle input pin til ny lyd
}


void ResetInterrupt () {
 for (int i = 0; i < PinInSize; i++){
  detachInterrupt(digitalPinToInterrupt(PinIn[i]));
  }
   Sneaker("removing interrupts",__LINE__);
   //Detacher alle interupts på input pins.
}




void DirectionInterrupt () {


  attachInterrupt(digitalPinToInterrupt(MainPin), MikTrig, HIGH);
  attachInterrupt(digitalPinToInterrupt(SecondPin), MikTrig, HIGH);

    Sneaker("setting interrupts 2nd time",__LINE__);
}






void loop() {
 //ikke brugt


}










void FirstSound() {


    if (!FirstTrig) {
        // This block runs the first time an interrupt is triggered
        FirstTrig = true;
        //if (digitalRead(PinIn[0]) == HIGH) {
        if (digitalRead(A0) == HIGH) {
            MainPin = PinIn[0];
        //} else if (digitalRead(PinIn[1]) == HIGH) {
        } else if (digitalRead(A2) == HIGH) {
            MainPin = PinIn[1];
        } else {
            // Handle error: neither pin is HIGH when the interrupt is triggered
            ErrorEx("First Pin Not found",__LINE__); // Call an error handling function
            return;
        }
        detachInterrupt(digitalPinToInterrupt(MainPin)); // Disable further interrupts on the first triggered pin
         Sneaker("setting main pin",__LINE__);
        return; // Exit after handling the first trigger
    }


    if (FirstTrig) {
        // This block attempts to identify the second trigger
        FirstTrig = false; // Reset for potential future reactivation
        //if (digitalRead(PinIn[0]) == HIGH && PinIn[0] != MainPin) {
        if (digitalRead(A0) == HIGH && PinIn[0] != MainPin) {
            SecondPin = PinIn[0];
        //} else if (digitalRead(PinIn[1]) == HIGH && PinIn[1] != MainPin) {
        } else if (digitalRead(A2) == HIGH && PinIn[1] != MainPin) {
            SecondPin = PinIn[1];
        } else {
            // Handle error: second pin not found
            ErrorEx("Second Pin Not Found", __LINE__);
            return;
        }
      Sneaker("setting second pin",__LINE__);

      ResetInterrupt(); // Reset interrupts


      DirectionInterrupt(); // Handle directional logic
    }
}




void MikTrig() {
  SampleToArray(MikSampleMain, MainPin);
  SampleToArray(MikSampleSecond, SecondPin);
  detachInterrupt(digitalPinToInterrupt(MainPin));

  Sneaker("sampling to arrays",__LINE__);

  for (int i=0; i<Asize;i++){
    ;Serial.print(" || ");
    Serial.print(MikSampleMain[i]);
  }
  Serial.print(" || ");


  Serial.print("Mik Second Sample: ");
    for (int i=0; i<Asize;i++){
    Serial.print(" || ");
    Serial.print(MikSampleSecond[i]);
  }
  Serial.print(" || ");
  

  DelayXY = CrossCorelate(MikSampleMain, MikSampleSecond);
}




int CrossCorelate(int* Array1, int* Array2) {
  float CrossArray[Asize];
  int tArray [Asize];
  float dArray [Asize];
  int Max;
  int Maxd;


  for (int n=0; n <= Asize; n++) {
    for (int i=0; i <= Asize; i++) {
      if (i+n <= Asize){
      tArray[i] = Array1[i] * Array2[i+n];
      } else {
        tArray[i] = 0;
      }

    }
      
      int sum = 0;

    for (int i=0; i <= Asize; i++){
      sum = sum + tArray[i];
    }

    dArray[n]  = sum/(Asize-n);

  }

  Max = dArray[0];
  Maxd = 0;
  for (int i=1; i <= Asize; i++){
    if(Max < dArray[i] ){
      Max = dArray[i];
      Maxd = i;

    }

  }

int delay = SampleTime * Maxd;
  return delay;


}










void SampleToArray(int* SamplingArray, int Pin) {
  for (int i = 0; i < Asize; i++) {
    SamplingArray[i] = analogRead(Pin);
    delay(SampleTime);
  }
//Fills array with analog read sampling
}




void Sneaker(const char* SneakerMessage, int line) {
    Serial.println(" ");
    Serial.print("The Program is at line ");
    Serial.println(line);
    
    Serial.print("The program is ");
    
    Serial.println(SneakerMessage);

}



void ErrorEx(const char* errorMessage, int line) {
    Serial.println(errorMessage);
    Serial.print("The unexpected Error occurred at line: ");
    Serial.println(line);
    Serial.println("Program has halted");
    while(1) {
        delay(1000);  // Simple error loop to prevent further execution
    }
}



