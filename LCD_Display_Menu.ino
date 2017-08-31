// 

#define encoder0PinSW 4
#define encoder0PinA  2
#define encoder0PinB  3

volatile unsigned int encoder0Pos = 0;
unsigned int enc0LastPos = 1;
static boolean rotating = false;

// interrupt service routine vars
boolean A_set = false;
boolean B_set = false;

void setup() {
  pinMode(encoder0PinSW, INPUT_PULLUP);
  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);

  // encoder pin A on interrupt
  attachInterrupt(digitalPinToInterrupt(encoder0PinA), doEncoderA, CHANGE);

  // encoder pin B on interrupt 
  attachInterrupt(digitalPinToInterrupt(encoder0PinB), doEncoderB, CHANGE);

  Serial.begin (9600); //initializes serial communication
}

void loop() {

  rotating = true; //reset the debouncer

  if (enc0LastPos != encoder0Pos){
    Serial.print("Index:");
    Serial.println(encoder0Pos, DEC);
    enc0LastPos = encoder0Pos;
  }
  
  if(digitalRead(encoder0PinSW) == LOW){
    while(digitalRead(encoder0PinSW)==0); //wait till switch is released
    encoder0Pos = 0;
    Serial.println("Button");
  }
}

void doEncoderA() {
  //debounce
  if (rotating) delay (1);//wait until bouncing is done
  
  // look for a low-to-high on channel A
  if (digitalRead(encoder0PinA) != A_set ) {
    A_set = !A_set;

    // adjust counter + if A leads B
    if ( A_set && !B_set )
      encoder0Pos += 1;

    rotating = false;  // no more debouncing until loop() hits again
  }
}

void doEncoderB() {

  if ( rotating ) delay (1);
  
  if ( digitalRead(encoder0PinB) != B_set ) {
    B_set = !B_set;
    
    //  adjust counter - 1 if B leads A
    if ( B_set && !A_set )
      encoder0Pos -= 1;

    rotating = false;
  }
}

