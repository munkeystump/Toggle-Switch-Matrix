/*
 * This software is licensed under the CC0 1.0 Universal License https://creativecommons.org/publicdomain/zero/1.0/
 * Which is in itself ridiculous
 * Do whatever you want with this code, it comes with no warranties and not even a guarantee that it works.
 * I use it, if you want to use it too then great.
 * Got a question, or a feature request try the github https://github.com/munkeystump
*/

/*
 * The following is the code for a puzzle which requires a matrix of 30 toggle switches to be in the correct positions before triggering a relay.
 * There are two combinations to be entered, each of which will trigger a different relay.
 * 
 * It requires an ARDUINO MEGA or similar to have enough pins to scan 30 switches.
 * The same could be achieved much more elegantly for an UNO or NANO using the keypad library using diodes to avoid ghosting but that's not what I had on hand.
*/
 
/*
=================================
BELOW FOLLOWS SETTINGS FOR PUZZLE
=================================
*/

// Pin numbers for maglocks
byte lock1 = 10;
byte lock2 = 11;
// Constants to define the number of rows / columns in matrix
const byte switchRows = 5;
const byte switchCols = 6;

// Defines if the maglock relays require HIGH (true) or LOW(false) to switch ON
bool lockOn = true;

//Defines if the locks should latch open when puzzle solved NOTE: if TRUE the puzzlee needs resetting manually
bool latching = false;

// Set puzzle solution for stage one
int solution1[switchRows][switchCols]={
{1,1,0,0,0,0},  
{0,0,1,1,0,0},
{0,0,0,0,0,0},
{0,0,0,0,0,0},
{0,0,0,0,0,0},
};

// Set puzzle solution for stage two
int solution2[switchRows][switchCols]={
{0,0,0,0,0,1},  
{0,0,0,0,0,1},
{0,0,0,0,0,0},
{0,0,0,0,0,0},
{0,0,0,0,0,0},
};

// Define pins for switch matrix
byte switchPins[switchRows][switchCols] = {
{23,24,25,26,27,28},
{29,30,31,32,33,34},
{35,36,37,38,39,40},
{41,42,43,44,45,46},
{47,48,49,50,51,52}
};

/*
=============
END OF CONFIG
=============
 */

// Array to hold current state of all switches
int currentState[switchRows][switchCols]={
{0,0,0,0,0,0},  
{0,0,0,0,0,0},
{0,0,0,0,0,0},
{0,0,0,0,0,0},
{0,0,0,0,0,0},
};

// Tracks puzzle state for each solution solved
int puzzleState = 0;

void setup() {
  Serial.begin(9600);
  resetme();
}

void loop() {
  switch(puzzleState){
    case 0:
      if(solutionScan(solution1)){
        puzzleState++;
        unlock(lock1);
      }
      break;
    case 1:
      if(solutionScan(solution2)){
        puzzleState++;
        unlock(lock2);
      }
      break;
    default:
      if(!latching){
        resetme();
      }
      break;
  }
}

// scans entire matrix to find switch states and writes to current state on state change
// this could be made WAY more efficient if scans stopped on non-matching value but debugging messages would be incomplete
bool solutionScan(int sol[switchRows][switchCols]){
  bool solved = true;
  for(int i = 0; i<(switchRows); i++){
    for(int j = 0; j<(switchCols); j++){
      int pinRead = !digitalRead(switchPins[i][j]);
      if (!currentState[i][j]== pinRead){
        currentState[i][j]=pinRead;
        printState();
      }
      if(!currentState[i][j]==sol[i][j]){
        solved = false;
      }
    }
  }
  return solved;
}

// Print helper function to print out current state of each switch
void printState(){
  Serial.println("=========STATE CHANGE DETECTED==========");
  for(int i = 0; i<(switchRows); i++){
    for(int j = 0; j<(switchCols); j++){
      Serial.print(currentState[i][j]);
    }
  Serial.println();
  }
}

// Trigger relay for maglock from current puzzle state
void unlock(byte lock){
  Serial.print("LOCK ");
  Serial.print(lock);
  Serial.println(" UNLOCKED");
  digitalWrite(lock,!lockOn);
  delay(1000);
}

// reset puzzle
void resetme(){
  Serial.println("=========PUZZLE RESET==========");
  puzzleState=0;
  for(int i = 0; i<(switchRows); i++){
    for(int j = 0; j<(switchCols); j++){
      pinMode(switchPins[i][j],INPUT_PULLUP);
    }
  }
  digitalWrite(lock1, lockOn);
  digitalWrite(lock2, lockOn);
}
