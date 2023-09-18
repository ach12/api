// Based on the code from 'Curious Scientist'
// accelStepper: https://curiousscientist.tech/blog/arduino-accelstepper-tb6600-stepper-motor
// limit switches: https://curiousscientist.tech/blog/ping-pong-with-the-accelstepper-library-and-two-limit-switches-polling-and-interrupts

//Transforming the motor's rotary motion into linear motion by using a threaded rod:
//Threaded rod's pitch = 5 mm. This means that one revolution will move the nut 5 mm.
//Default stepping = 200 step/revolution.
// 200 step = 1 revolution = 5 mm linear motion. (single start 5 mm pitch screw)
// 1 cm = 10 mm =>> 10/5 * 200 = 400 steps are needed to move the nut by 1 cm.

//character for commands
/*
     'C' : Prints all the commands and their functions.
     'P' : Rotates the motor in positive (CW) direction, relative.
     'N' : Rotates the motor in negative (CCW) direction, relative.
     'F' : Rotates motor in positive direction at a given speed.
     'R' : Rotates motor in negative direction at a given speed.
     'S' : Stops the motor immediately.
     'A' : Sets an acceleration value.

 */
// We will set the positive direction as moving away from the motor.
// Negative direction moving towards the motor.

#include <AccelStepper.h>
 
#define FRONTLIMITSWITCH 2
#define BACKLIMITSWITCH 3

bool switchFlipped = false;
//User-defined values
long receivedSteps = 0; //Number of steps
long receivedSpeed = 0; //Steps / second
long receivedAcceleration = 0; //Steps / second^2
char receivedCommand;
//-------------------------------------------------------------------------------
int directionMultiplier = 1; // = -1: positive direction, = 1: negative direction
bool newData, runallowed = false; // booleans for new data from serial, and runallowed flag
bool runInfinite = false; // when runInfinite is true the motor rotates in one direction until 'S' command is entered or a limit switch is activated
AccelStepper stepper(1, 8, 9);// direction Digital 9 (CCW), pulses Digital 8 (CLK)
 
void setup()
{
    Serial.begin(9600); //define baud rate
    Serial.println("Demonstration of AccelStepper Library"); //print a messages
    Serial.println("Send 'C' for printing the commands.");
 
    //Limit Switches
    pinMode(FRONTLIMITSWITCH, INPUT_PULLUP);
    pinMode(BACKLIMITSWITCH, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FRONTLIMITSWITCH), LimitSwitchStopFront, FALLING);  
    attachInterrupt(digitalPinToInterrupt(BACKLIMITSWITCH), LimitSwitchStopBack, FALLING);  

    //setting up some default values for maximum speed and maximum acceleration
    Serial.println("Default speed: 400 steps/s, default acceleration: 800 steps/s^2.");
    stepper.setMaxSpeed(400); //SPEED = Steps / second
    stepper.setAcceleration(800); //ACCELERATION = Steps /(second)^2
 
    stepper.disableOutputs(); //disable outputs
}
 
void loop()
{
    //Constantly looping through these 2 functions.
    //We only use non-blocking commands, so something else (should also be non-blocking) can be done during the movement of the motor
    checkSerial(); //check serial port for new commands
    RunTheMotor(); //function to handle the motor  

}
 
void LimitSwitchStopFront()
{
    Serial.println("Limit switch front");
    stepper.stop(); //stop motor
    stepper.disableOutputs(); //disable power
    runallowed = false; //disable running
}

void LimitSwitchStopBack()
{
    Serial.println("Limit switch back");
    stepper.stop(); //stop motor
    stepper.disableOutputs(); //disable power
    runallowed = false; //disable running
}

void RunTheMotor() //function for the motor
{
    if (runallowed == true)
    {
      if (runInfinite == true)
      {
        stepper.enableOutputs(); //enable pins
        stepper.runSpeed(); // Poll the motor and step it if a step is due, implementing a constant speed as set by the most recent call to setSpeed()
      }
      else 
      {
        stepper.enableOutputs(); //enable pins
        stepper.run(); //implements accelerations and decelerations to achieve the target position
      }
    }
    else //program enters this part if the runallowed is FALSE, we do not do anything
    {
        stepper.disableOutputs(); //disable outputs
        return;
    }
}
 
 
 
void checkSerial() //function for receiving the commands
{  
    if (Serial.available() > 0) //if something comes from the computer
    {
        receivedCommand = Serial.read(); // pass the value to the receivedCommad variable
        newData = true; //indicate that there is a new data by setting this bool to true
 
        if (newData == true) //we only enter this long switch-case statement if there is a new command from the computer
        {
            switch (receivedCommand) //we check what is the command
            {
 
            case 'P': //P uses the move() function of the AccelStepper library, which means that it moves relatively to the current position.              
               
                receivedSteps = Serial.parseFloat(); //value for the steps (mm)
                receivedSpeed = Serial.parseFloat(); //value for the speed (mm)
                directionMultiplier = -1; //We define the direction
                Serial.println("Positive direction."); //print the action
                RotateRelative(); //Run the function

                //example: P2000 400 - 2000 steps (5 revolution with 400 step/rev microstepping) and 400 steps/s speed
                //In theory, this movement should take 5 seconds
                break;         
 
            case 'N': //N uses the move() function of the AccelStepper library, which means that it moves relatively to the current position.      
               
                receivedSteps = Serial.parseFloat(); //value for the steps
                receivedSpeed = Serial.parseFloat(); //value for the speed 
                directionMultiplier = 1; //We define the direction
                Serial.println("Negative direction."); //print action
                RotateRelative(); //Run the function
                //example: N2000 400 - 2000 steps (5 revolution with 400 step/rev microstepping) and 500 steps/s speed; will rotate in the other direction
                //In theory, this movement should take 5 seconds
                break;

            case 'F':

                receivedSpeed = Serial.parseFloat(); //value for the speed 
                directionMultiplier = -1; //We define the direction
                Serial.println("Forward direction."); //print action
                RotateInfinite();
                break;

            case 'R':

                receivedSpeed = Serial.parseFloat(); //value for the speed 
                directionMultiplier = 1; //We define the direction
                Serial.println("Reverse direction."); //print action
                RotateInfinite();
                break;

            case 'S': // Stops the motor
               
                stepper.stop(); //stop motor
                stepper.disableOutputs(); //disable power
                Serial.println("Stopped."); //print action
                runallowed = false; //disable running
                break;
 
            case 'A': // Updates acceleration
 
                runallowed = false; //we still keep running disabled, since we just update a variable
                stepper.disableOutputs(); //disable power
                receivedAcceleration = Serial.parseFloat(); //receive the acceleration from serial
                stepper.setAcceleration(receivedAcceleration); //update the value of the variable
                Serial.print("New acceleration value: "); //confirm update by message
                Serial.println(receivedAcceleration); //confirm update by message
                break;
 
            case 'C':
 
                PrintCommands(); //Print the commands for controlling the motor
                break;
 
            default:  

                break;
            }
        }
        //after we went through the above tasks, newData is set to false again, so we are ready to receive new commands again.
        newData = false;       
    }
}
 
void RotateRelative()
{
    //We move X steps from the current position of the stepper motor in a given direction.
    //The direction is determined by the multiplier (+1 or -1)
   
    runallowed = true; //allow running - this allows entering the RunTheMotor() function.
    runInfinite = false;
    stepper.setMaxSpeed(receivedSpeed); //set speed
    stepper.move(directionMultiplier * receivedSteps); //set relative distance and direction
}
 
void RotateAbsolute()
{
    //We move to an absolute position.
    //The AccelStepper library keeps track of the position.
    //The direction is determined by the multiplier (+1 or -1)
    //Why do we need negative numbers? - If you drive a threaded rod and the zero position is in the middle of the rod...
 
    runallowed = true; //allow running - this allows entering the RunTheMotor() function.
    runInfinite = false;
    stepper.setMaxSpeed(receivedSpeed); //set speed
    stepper.moveTo(directionMultiplier * receivedSteps); //set relative distance   
}

void RotateInfinite()
{
    // Move at a constant speed.
    //The direction is determined by the multiplier (+1 or -1)
    runallowed = true;
    runInfinite = true;
    //mmToStep();
    stepper.setSpeed(directionMultiplier * receivedSpeed);

}

void PrintCommands()
{  
    //Printing the commands
    Serial.println(" 'C' : Prints all the commands and their functions.");
    Serial.println(" 'P' : Rotates the motor in positive (CW) direction, relative.");
    Serial.println(" 'N' : Rotates the motor in negative (CCW) direction, relative.");
    Serial.println(" 'F' : Rotates motor in positive direction at a given speed.");
    Serial.println(" 'R' : Rotates motor in negative direction at a given speed");
    Serial.println(" 'S' : Stops the motor immediately."); 
    Serial.println(" 'A' : Sets an acceleration value.");

}