#include<LiquidCrystal.h>

#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)

/* Useful Macros for getting elapsed time */
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays(_time_) ( _time_ / SECS_PER_DAY)

#define maxItemSize 16
#define encoder0PinSW 4
#define encoder0PinA  2
#define encoder0PinB  3

volatile unsigned int encoder0Pos = 0;
unsigned int enc0LastPos = 1;
static boolean rotating = false;

const int itemsPerScreen = 4;

// interrupt service routine vars
boolean A_set = false;
boolean B_set = false;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

//Declare the Menus you need.
char menu[][maxItemSize] = {"Date", "Time", "Alarm", "Format", "Zone", "Daylight", "BACK"};
// Only 2 sub-menus are shown. You can add as many as you wish.
char subMenu0[][maxItemSize] = {"Date", "Month", "Year", "BACK"};
char subMenu1[][maxItemSize] = {"Hours", "Min", "Secs", "BACK"};

int cnt = 0,i;
int itemSelected, subMenuSelected;
int itemsToDisplay = 0;


void setup() {
  lcd.begin(20, 4);

  pinMode(encoder0PinSW, INPUT_PULLUP);
  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);

  // encoder interrupts
  attachInterrupt(digitalPinToInterrupt(encoder0PinA), doEncoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoder0PinB), doEncoderB, CHANGE);

  Serial.begin (9600); //initializes serial communication
}


void loop() {
  lcd.setCursor(8, 1);
  time(millis() / 1000);

  rotating = true; //reset the debouncer
  
  //Enter menu
  if (digitalRead(encoder0PinSW) == LOW) {
    while (digitalRead(encoder0PinSW) == LOW); //wait till switch is released
    lcd.clear();
    itemSelected = displayMenu(menu, sizeof(menu)/maxItemSize);
  }
}


int displayMenu(char menuInput[][maxItemSize], int menuLength){
  int curPos, startPos, endPos;
  do{
    startPos=encoder0Pos%menuLength;
    Serial.print("startPos:");
    Serial.println(startPos);

    endPos=itemsPerScreen;

    if (menuLength < itemsPerScreen){
      endPos = menuLength - startPos;
    }

    if ((menuLength-startPos)<itemsPerScreen){
      endPos = menuLength-startPos;
    }

    for (cnt = 0; cnt<=(endPos-1); cnt ++){
      if (cnt == 0){
        lcd.setCursor(0,0);
        lcd.print("->");
      }

      lcd.setCursor(2, cnt);
      lcd.print(menuInput[cnt+startPos]);
      //clear trailing remains
    }

    cnt=0;

    if (enc0LastPos != encoder0Pos) {
      enc0LastPos = encoder0Pos;
    }
  }while(digitalRead(encoder0PinSW));
  while (digitalRead(encoder0PinSW) == LOW);
  lcd.clear();
  return startPos;
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

void time(long val) {
  int days = elapsedDays(val);
  int hours = numberOfHours(val);
  int minutes = numberOfMinutes(val);
  int seconds = numberOfSeconds(val);

  // digital clock display of current time
  // display.print(days,DEC);
  // printDigits(hours);
  lcd.print(minutes);
  printDigits(seconds);
}

void printDigits(byte digits) {
  // utility function for digital clock display: prints colon and leading 0
  lcd.print(":");
  if (digits < 10)
    lcd.print('0');
  lcd.print(digits, DEC);
}

