// its assumed that CO2 levels span from 300 to 5k
// we are not considering values below 350
// from common sense and messy calibration considering room levels to be 40
// raw output is 55
#define baseline_raw 30


void setup(){
  pinMode(A0,INPUT);
  Serial.begin(9600);
  
}
void loop(){
  int co2avg=0;
  int sum = 0; //taking 10 values for each approximation
  int co2normalised=0;
  int co2mapped=0;
  for(int i=0;i<10;i++){
    sum = sum + analogRead(A0);
    delay(100);
  }
  co2avg = sum/10;
  co2normalised = co2avg-baseline_raw;
  co2mapped = map(co2normalised,0,1023,400,5000);
  Serial.print("CO2 (majority) PPM :");
  Serial.println(co2mapped);
  Serial.println(co2avg);
}

