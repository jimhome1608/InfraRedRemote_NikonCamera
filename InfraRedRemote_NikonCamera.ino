/**
 * 	 * arduino Nikon IR remote
 * 	 * @license	Creative commons: Attribution-Noncommercial-Share Alike 3.0 (http://creativecommons.org/licenses/by-nc-sa/3.0/)
 * 	 * @author Aurelien ANTOINE
 * 	 * version 1
 * 	 * date 20081217
**/

#define PIN_STATUS 13
#define PIN_IR_LED 12
#define FREQ 38400  // IR frequence

//shutter sequence (on,off,on,off ... in microsecond)
unsigned long sequence[] = {
  2000,27830,390,1580,410,3580,400,63200,2000,27830,390,1580,410,3580,400,0};
int seq_l;

//oscd is a delay in microsecond used at each oscillation.
int oscd;

void oscillate(int pin, unsigned long n, int shine){
  int ir_status=0;
  while(n>0){
    n--;
    delayMicroseconds(oscd);
    ir_status  =  !ir_status; 
    digitalWrite(pin,  ir_status && shine);  
  }
}

void snap(){
  int i;
  digitalWrite(PIN_STATUS,  1);  
  for(i=0;i<seq_l;i++){
    oscillate(PIN_IR_LED, sequence[i], i%2==0);
  }
  digitalWrite(PIN_STATUS,  0);
}

void setup() {
  int min=1, max=100, i;
  int last_oscd=0;
  unsigned long before, intervalle;
  oscd=max;

  seq_l = sizeof(sequence)/sizeof(unsigned long);

  pinMode(PIN_STATUS, OUTPUT);
  pinMode(PIN_IR_LED, OUTPUT);
  Serial.begin(28800);

  //this "while" will process the best "oscd"
  Serial.println("Ready");
  while(last_oscd!=oscd){
    last_oscd=oscd;
    oscd=(min+max)>>1;

    before=millis();
    oscillate(PIN_STATUS, FREQ, 1);
    intervalle=millis()-before;

    if(intervalle >= 1000) max=oscd;
    else min=oscd;

    Serial.print(intervalle);
    Serial.print(" : ");
    Serial.print(min);
    Serial.print("<->");
    Serial.println(max);
  }

  Serial.print("oscd: ");
  Serial.println(oscd);
  //rewrite the sequence array, we replace all values in microsecond by the number of oscillation
  for(i=0;i<seq_l;i++){
    Serial.print(sequence[i]);
    Serial.print("->");
    sequence[i] = (sequence[i] * FREQ) / (intervalle * 1000);
    Serial.println(sequence[i]);
  }
}


void loop() {	
  //make a photo, wait 2 seconds
  snap();
  delay(2000);

}

