//Plays a happy tune using a buzzer and flashes a green LED.
void accessGranted(){
  digitalWrite(greenLed, HIGH); 
  ledcWriteTone(channel, 500);
  delay(200);
  ledcWriteTone(channel, 1000);
  delay(200);
  ledcWriteTone(channel, 2000);
  delay(400);
  ledcWriteTone(channel, 0);
  digitalWrite(greenLed, LOW);
}

//Plays one deep note using a buzzer and flashes a red LED.
void accessDenied(){
  digitalWrite(redLed, HIGH);
  ledcWriteTone(channel, 1000);
  delay(200);
  ledcWriteTone(channel, 500);
  delay(800);
  ledcWriteTone(channel, 0);
  digitalWrite(redLed, LOW);
}
