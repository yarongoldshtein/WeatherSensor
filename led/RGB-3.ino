// Robo India Tutorial 
// Digital Input and Output on LED 
// Hardware: NodeMCU

const int RED = 5;
const int GREEN = 4;
const int BLUE = 0;

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
}

void loop() { 
  analogWrite(RED, 50);
  analogWrite(GREEN, 50);
  analogWrite(BLUE, 200);  
  delay (1000);
  analogWrite(RED, 100);
  analogWrite(GREEN, 100);
  analogWrite(BLUE, 100); 
  delay (1000);
}
