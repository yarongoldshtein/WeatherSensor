uint8_t step =0;
#define RED 0
#define GREEN 4
#define BLUE 5

#define ON LOW
#define OFF HIGH

void setup() {
  // put your setup code here, to run once:
pinMode(RED,OUTPUT);
pinMode(GREEN,OUTPUT);
pinMode(BLUE,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(RED,OFF);
digitalWrite(GREEN,OFF);
digitalWrite(BLUE,OFF);
switch(step % 3){
  case 0:
  digitalWrite(RED,ON);//RED
  break;
  case 1:
  digitalWrite(GREEN,ON);//GREEN
  break;
  case 2:
  digitalWrite(BLUE,ON);//BLUE
  break;
}
delay (1000);
step++;
}
