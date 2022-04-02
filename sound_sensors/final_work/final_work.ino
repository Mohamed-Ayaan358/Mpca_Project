/*This codes reads the data from port A0 of 
VMA309, does FFT on polled data. Finds peaks
If the determined peak is within the expected it 
lights up the external LED. This code also incorporated
some trivial background noise avoidance*/


#include "arduinoFFT.h" //Fast fourier transform, to get the peaks from a given set of polled values
#define baseline_raw 30
#define SAMPLES 128             //SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 1024 //Ts = Based on Nyquist, must be 2 times the highest expected frequency.
//Exepected frequencies : 512 or 256

arduinoFFT FFT = arduinoFFT();

unsigned int samplingPeriod;
unsigned long microSeconds;
int Ledgreen=13;
int Ledwhite=12;

double vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
double vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values

int emer_count = 0; //sets a buffer before detecting emergency, done for background avoidance.

void setup()
{
  Serial.begin(115200); //Baud rate for the Serial Monitor
  samplingPeriod = round(1000000 * (1.0 / SAMPLING_FREQUENCY)); //Period in microseconds
  pinMode(A2,INPUT);
  pinMode(A0,INPUT);
//  pinMode(LED_BUILTIN, OUTPUT); //Turns on when high, setting internal led to OUTPUT
  pinMode(Ledgreen,OUTPUT);
  pinMode(Ledwhite,OUTPUT);
  digitalWrite(Ledwhite,HIGH);
  digitalWrite(Ledgreen,LOW);
}

void loop()
{
  
  int i=0;
  int co2avg=0;
  int sum = 0; //taking 10 values for each approximation
  int co2normalised=0;
  int co2mapped=0;
  
  /*Ardiuno uno cant handle doing FFT on more than 128 values at at time.*/
  while(i<127) 
  {
    microSeconds = micros();    //Returns the number of microseconds since the Arduino board began running the current script.

    vReal[i] = analogRead(0); //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
    vImag[i] = 0; //Makes imaginary term 0 always
    i++;
    /*remaining wait time between samples if necessary*/
    while (micros() < (microSeconds + samplingPeriod))
    {
      //do nothing
    }
  }

  /*Perform FFT on samples*/
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

  /*Find peak frequency and print peak*/
  double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
  Serial.println(peak);     //Print out the most dominant frequency.

  
  for(int i=0;i<10;i++){
    sum = sum + analogRead(A2);
    delay(50);
  }
  co2avg = sum/10;
  co2normalised = co2avg-baseline_raw;
  co2mapped = map(co2normalised,0,1023,400,5000);
  Serial.print("CO2 (majority) PPM :");
  Serial.println(co2mapped);
//  Serial.println(co2avg);

  
  if(peak > 245 && peak < 285){
    if(emer_count <=6){
      Serial.print("Emer count:");
      Serial.println(emer_count);
      emer_count+=1;
    }
  }else if(emer_count > 0){
    emer_count--;
  }
  if(emer_count >= 4){
    Serial.println("High");
//    digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(Ledgreen,HIGH);
      digitalWrite(Ledwhite,LOW);
      
  }else{
//    digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(Ledwhite,HIGH);
      digitalWrite(Ledgreen,LOW);
  }

}
