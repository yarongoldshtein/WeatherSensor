/*
Adafruit Arduino - Lesson 3. RGB LED
*/

#define ON LOW
#define OFF HIGH

#define RED 0
#define GREEN 4
#define BLUE 5

uint8_t step =0;
//uncomment this line if using a Common Anode LED
//#define COMMON_ANODE
 
void setup()
{
  pinMode(RED,OUTPUT);
  pinMode(GREEN,OUTPUT);
  pinMode(BLUE,OUTPUT);
}
 
void loop()
{
  digitalWrite(0,OFF);
  digitalWrite(4,OFF);
  digitalWrite(5,OFF);
  switch(step % 3){
    case 0:
    digitalWrite(RED,ON);//RED
    break;
    case 1:
    digitalWrite(orange(),ON);//orange
    break;
    case 2:
    digitalWrite(GREEN,ON);//GREEN
    break;
  }
  delay (1000);
  step++;
}
 
int orange()
{
 digitalWrite(RED,ON);//RED
 digitalWrite(GREEN,ON);//GREEN  
 digitalWrite(BLUE,ON);//BLUE
}
