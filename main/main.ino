

  const int heartRateSensorPin = A0;
  const int ledPin = D3;
  const int heartRateThreshold = 500;

void setup() {
  

  pinMode(ledPin, OUTPUT);
  pinMode(heartRateSensorPin, INPUT);
  Serial.begin(9600);


}

void loop() {
  
  
  if (analogRead(heartRateSensorPin) < heartRateThreshold){
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  Serial.println(analogRead(heartRateSensorPin));

}
