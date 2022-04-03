//Standard Proteus Libraries
#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>
#include <FEHServo.h>
#include <FEHRPS.h>
#include <FEHSD.h>
#include <string.h>
#include <stdlib.h>
 
// #define CDS_THRESHOLD *put value here*
#define CDS_BLUE_THRESHOLD 1.015
#define CDS_RED_THRESHOLD .5
#define CDS_UPPER_THRESHOLD 2.0
#define COUNTS_PER_INCH 34.0
#define NINETY_DEG_TWENTYSPEED 5.2
#define SERVO_MIN 726
#define SERVO_MAX 1427
#define RPS_WAIT_TIME_IN_SEC 0.35
#define PULSE_TIME 0.3
#define PULSE_POWER 20
#define PLUS 0
#define MINUS 1
#define STD_POWER 33
 
/* Declarations for encoders, motors, and input pins (all subject to
change) */
DigitalEncoder right_encoder(FEHIO::P2_0);
DigitalEncoder left_encoder(FEHIO::P3_0); // Left stupid
FEHMotor right_motor(FEHMotor::Motor0, 9.0);
FEHMotor left_motor(FEHMotor::Motor3, 9.0); // left taped
AnalogInputPin cds(FEHIO::P1_0);
FEHServo arm_servo(FEHServo::Servo0);
 
/* function for pulsing forward using encoders */
void pulse_forward(int percent, float seconds)
{
    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);
 
    // Wait for the correct number of seconds
    Sleep(seconds);
 
    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}
 //Function for pulsing left using encoders
void pulse_counterclockwise(int percent, float seconds)
{
    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(-percent);
 
    // Wait for the correct number of seconds
    Sleep(seconds);
 
    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}
 
//Function to check RPS X values and have the robot correct itself to those values
void check_x(float x_coordinate, int orientation, int power)
{
    // Determine the direction of the motors based on the orientation of the QR code
    if (orientation == MINUS)
    {
        power = -PULSE_POWER;
    }
 
    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while (RPS.X() > 0.0 && RPS.Y() > 0.0 && RPS.Heading() > 0.0 && (RPS.X() < x_coordinate - 0.25 || RPS.X() > x_coordinate + 0.25))
    {
        // Forward
        if (RPS.X() < x_coordinate - 0.25)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(power, .25);
 
        } // Backward
        else if (RPS.X() > x_coordinate + 0.25)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(-power, .25);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}
 
//Function to check RPS Y values and have the robot correct itself to those values  
void check_y(float y_coordinate, int orientation, int power)
{
    // Determine the direction of the motors based on the orientation of the QR code
    if (orientation == MINUS)
    {
        power = -PULSE_POWER;
    }
 
    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while (RPS.X() > 0.0 && RPS.Y() > 0.0 && RPS.Heading() > 0.0 && (RPS.Y() < y_coordinate - 0.25 || RPS.Y() > y_coordinate + 0.25))
    {
        // Forward
        if (RPS.Y() < y_coordinate - 0.25)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(power, PULSE_TIME);
 
        } // Backward
        else if (RPS.Y() > y_coordinate + 0.25)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(-power, PULSE_TIME);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}
 
//Function to check RPS Heading values and have the robot correct itself to those values  
void check_heading(float heading)
{
 
    float currentHeading = RPS.Heading();
   
    while (currentHeading > heading + 1 || currentHeading < heading - 1)
    {
       
        float pulseTime = PULSE_TIME * 0.2;
 
        if (heading == 0)
        {
            if(currentHeading < 359.9 - currentHeading) {
                pulse_counterclockwise(-PULSE_POWER, pulseTime);
            } else {
                pulse_counterclockwise(PULSE_POWER, pulseTime);
            }
        } else {
            // Pulse longer if the heading is off by more than 5 to decrease time spent pulsing
            if (abs(currentHeading - heading) > 6){
                pulseTime = PULSE_TIME * 0.5;
            }
            if (currentHeading < heading - 1)
            {
            pulse_counterclockwise(PULSE_POWER, pulseTime);
            } else if (currentHeading > heading + 1)
            {
            pulse_counterclockwise(-PULSE_POWER, pulseTime);
            }
        }
       
        currentHeading = RPS.Heading();
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}
//Function to set RPS values before run
//Creates Consistency between courses and locations
void logPoints(){
    FEHFile *RPS_Points = SD.FOpen("RPS.txt", "w");
    float x, y;
    Sleep(.25);
    LCD.WriteLine("X & Y for Juke:");
    while (!LCD.Touch(&x, &y)){
        LCD.WriteLine(RPS.X());
        LCD.WriteLine(RPS.Y());
        Sleep(.25);
    }
    float tmpJukeX = RPS.X();
    float tmpJukeY = RPS.Y();
    SD.FPrintf(RPS_Points, "%s\t%f\t%f\t%f", "JX", tmpJukeX, 0., 0.);
    SD.FPrintf(RPS_Points, "\n%s\t%f\t%f\t%f", "JY", 0., tmpJukeY, 0.);
    LCD.Clear();
    Sleep(.25);
    LCD.WriteLine("X & Y for Burger:"); //Check X & Y Value for Burger
    while (!LCD.Touch(&x, &y)){
        LCD.WriteLine(RPS.X());
        LCD.WriteLine(RPS.Y());
        Sleep(.25);
    }
    float tmpBurgerX = RPS.X();
    float tmpBurgerY = RPS.Y();
    SD.FPrintf(RPS_Points, "\n%s\t%f\t%f\t%f", "BX", tmpBurgerX, 0., 0.);
    SD.FPrintf(RPS_Points, "\n%s\t%f\t%f\t%f", "BY", 0., tmpBurgerY, 0.);
    LCD.Clear();
    Sleep(.25);
    LCD.WriteLine("X for chocolate ice cream:");
    while (!LCD.Touch(&x, &y)){
        LCD.WriteLine(RPS.X());
        Sleep(.25);
    }
    float tmpChocolate = RPS.X();
    SD.FPrintf(RPS_Points, "\n%s\t%f\t%f\t%f", "CH", tmpChocolate, 0., 0.);
    LCD.Clear();
    SD.FClose(RPS_Points);
}
 
//Function to Read Logged RPS Data Files
float readRPSFile(const char *location, const char *whichRPSVal){
    FEHFile *RPS_Points = SD.FOpen("RPS.txt", "r");
    float RPS_DesiredPoint = -1;
    char name[20];
    if (strcmp(whichRPSVal, "X") == 0){
        for (int i = 0; i < 5; i++){
            SD.FScanf(RPS_Points, "%s%f%*s%*s", name, &RPS_DesiredPoint);
            if(strcmp(location, name) == 0)
                break;
        }
    }
    if (strcmp(whichRPSVal, "Y") == 0){
        for (int i = 0; i < 5; i++){
            SD.FScanf(RPS_Points, "%s%*s%f%*s", name, &RPS_DesiredPoint);
            if(strcmp(location, name) == 0)
                break;
        }
    }
    if (strcmp(whichRPSVal, "Heading") == 0){
        for (int i = 0; i < 5; i++){
            SD.FScanf(RPS_Points, "%s%*s%*s%f", name, &RPS_DesiredPoint);
            if(strcmp(location, name) == 0)
                break;
        }
    }
    SD.FClose(RPS_Points);
    return RPS_DesiredPoint;
}
 
//Function To Turn Right using encoders
void turn_right(int percentage, int encoder_counts)
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
 
    // Set both motors to desired percent
    left_motor.SetPercent(percentage);
    right_motor.SetPercent(-percentage);
 
    // Keep running motors while average of left and right encoder is less
    // than encoderCounts
    while ((left_encoder.Counts() + right_encoder.Counts()) / 2. < encoder_counts);
 
    // turn off motors
    right_motor.Stop();
    left_motor.Stop();
}
 
/* function for turning left. use encoders */
void turn_left(int percentage, int encoder_counts)
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
 
    // Set both motors to desired percent
    left_motor.SetPercent(-percentage);
    right_motor.SetPercent(percentage);
 
    // Keep running motors while average of left and right encoder is less
    // than encoderCounts
    while ((left_encoder.Counts() + right_encoder.Counts()) / 2. < encoder_counts)
        ;
 
    // turn off motors
    right_motor.Stop();
    left_motor.Stop();
}
 
/* function for moving forward. use encoders */
void move_straight(int percentage, int encoder_counts, float time)
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
 
    // Set both motors to desired percent
    left_motor.SetPercent(percentage);
    right_motor.SetPercent(percentage);
 
    // Keep running motors while average of left and right encoder is less
    // than encoderCounts
    double startTime = TimeNow();
    while ((left_encoder.Counts() + right_encoder.Counts()) / 2.0 < encoder_counts && TimeNow() - startTime < time);
 
    // turn off motors
    right_motor.Stop();
    left_motor.Stop();
}
 
/* function for moving at an angle
    direction is based off of quadrants (1 is forward right, 2 forward left, etc.)
    percentage is motor percentage
    angle is the desired angle of turning as a decimal
    encoder counts for how much to go*/
void angled_turn(int direction, int percentage, float angle, float inches)
{
    // Initializing distance and angle
    int encoder_counts = inches * COUNTS_PER_INCH;
    float turn_radius = angle / 100.0;
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    // Set both motors to desired percent based on direction
    int adjusted_angle = percentage * (1 - turn_radius);
    switch (direction)
    {
    case 1:
        left_motor.SetPercent(percentage);
        right_motor.SetPercent(adjusted_angle);
        break;
    case 2:
        left_motor.SetPercent(adjusted_angle);
        right_motor.SetPercent(percentage);
        break;
    case 3:
        left_motor.SetPercent(-adjusted_angle);
        right_motor.SetPercent(-percentage);
        break;
    case 4:
        left_motor.SetPercent(-percentage);
        right_motor.SetPercent(-adjusted_angle);
        break;
    case 5:
        left_motor.SetPercent(-percentage / (1 - turn_radius));
        right_motor.SetPercent(percentage / (1 - turn_radius));
        break;
    }
 
    // Keep running motors while average of left and right encoder is less
    // than encoderCounts
    while ((left_encoder.Counts() + right_encoder.Counts()) / 2.0 < encoder_counts);
 
    // turn off motors
    right_motor.Stop();
    left_motor.Stop();
    Sleep(.25);
}
 
 
/* Function for moving based off sensing the start light */
 
void start()
{
    // Display what function is currently running
    LCD.Clear();
    float x, y;
    while (!LCD.Touch(&x, &y));
    LCD.WriteLine("start");
 
    // Starting servo at 0 degrees
    arm_servo.SetMin(SERVO_MIN);
    arm_servo.SetMax(SERVO_MAX);
    arm_servo.SetDegree(30.0);
 
    // While elapsed time is less than 30 seconds.
    float time_now = TimeNow();
    bool started = false;
    while (TimeNow() - time_now < 30 && !started)
    {
        // Wait until light goes on
        while (cds.Value() > CDS_BLUE_THRESHOLD)
            ;
        if ((cds.Value() > CDS_RED_THRESHOLD) && cds.Value() < CDS_BLUE_THRESHOLD)
        {
            LCD.WriteLine("The start line was blue"); // Display what color is read
        }
        else
        {
            LCD.WriteLine("The start line was red"); // Display what color is read
        }
        started = true;
    }
    // Once 30 seconds elapsed, start anyway
    LCD.WriteLine("Did not read a light within 30 secs. Starting anyway.");
    // Call any function
}
 
//Original Function to go to Jukebox from Start Button
void toJuke(int lever)
{
    move_straight(-STD_POWER, COUNTS_PER_INCH * 5., 6.0);
    if (lever != 0) {
        move_straight(-STD_POWER, COUNTS_PER_INCH * 1.5, 6.0);
    }
    Sleep(0.35);
    check_heading(90.);
    turn_left(20, COUNTS_PER_INCH * NINETY_DEG_TWENTYSPEED);
   
    switch (lever) {
        case 0:
            move_straight(-STD_POWER, COUNTS_PER_INCH * 8, 6.0);
            break;
        case 1:
            move_straight(-STD_POWER, COUNTS_PER_INCH * 7, 6.0);
            break;
        case 2:
            move_straight(-STD_POWER, COUNTS_PER_INCH * 6, 6.0);
            break;
    }
 
    turn_left(20, COUNTS_PER_INCH * (NINETY_DEG_TWENTYSPEED + 0.125));
    Sleep(0.35);
    check_heading(270.);
    move_straight(STD_POWER, COUNTS_PER_INCH * 27., 6.0);
    arm_servo.SetDegree(0.);
    turn_right(20, COUNTS_PER_INCH * NINETY_DEG_TWENTYSPEED);
    move_straight(STD_POWER, COUNTS_PER_INCH * 7., 6.0);
    Sleep(.35);
    float JukeX = readRPSFile("JX", "X");
    check_x(JukeX, MINUS, 12);
    turn_left(20, COUNTS_PER_INCH * NINETY_DEG_TWENTYSPEED);
    Sleep(0.35);
    check_heading(270.);
    move_straight(20, COUNTS_PER_INCH * 0.33, 6.0);
    Sleep(0.35);
    float JukeY = readRPSFile("JY", "Y");
    check_y(JukeY, MINUS, 14);
   
 
}
 
//Function to push a jukebox button after reading the light
bool atJuke()
{
    // Set boolean to know which button was hit. False is red, true is blue
    bool button = false;
    // Display what function is currently running
    LCD.Clear();
    LCD.WriteLine("pushButton");
 
    if (cds.Value() < CDS_RED_THRESHOLD)
    {
        LCD.Clear(RED); // Display what color is read
        LCD.WriteLine("Need to hit RED");
        LCD.WriteLine(cds.Value());
        turn_right(20, COUNTS_PER_INCH * 5);
        move_straight(STD_POWER, COUNTS_PER_INCH * 0.75, 6.0);
        turn_left(20, COUNTS_PER_INCH * 5);
        move_straight(STD_POWER, COUNTS_PER_INCH * 5.25, 6.0);
    }
    else
    {
        LCD.Clear(BLUE); // Display what color is read
        LCD.WriteLine("Need to hit BLUE");
        LCD.WriteLine(cds.Value());
        turn_left(20, COUNTS_PER_INCH * 5);
        move_straight(STD_POWER, COUNTS_PER_INCH * 0.5, 6.0);
        turn_right(20, COUNTS_PER_INCH * 5);
        move_straight(STD_POWER, COUNTS_PER_INCH * 5.25, 6.0);
        button = true; // hit blue button
    }
    return button;//Returns which button was hit so robot knows approximately where it is
}
 
//Original function to move from jukebox to ramp
void toRamp(bool redBlue)
{
    LCD.Clear();
    LCD.WriteLine("toRamp");
    if (!redBlue)
    {
        move_straight(-STD_POWER, COUNTS_PER_INCH * 6, 6.0);
        turn_right(20, COUNTS_PER_INCH * 5);
        move_straight(-STD_POWER, COUNTS_PER_INCH * 10, 6.0);
        turn_left(20, COUNTS_PER_INCH * 5);
        move_straight(-STD_POWER, COUNTS_PER_INCH * 4, 6.0);
    }
    else
    {
        move_straight(-STD_POWER, COUNTS_PER_INCH * 6, 6.0);
        turn_right(20, COUNTS_PER_INCH * 5);
        move_straight(-STD_POWER, COUNTS_PER_INCH * 9, 6.0);
        turn_left(20, COUNTS_PER_INCH * 5);
        move_straight(-STD_POWER, COUNTS_PER_INCH * 4, 6.0);
    }
}
 
//Function used to test servo
void moveArm()
{
    // Enter the min and max servo valeus from the .TouchCalibrate() fxn
 
    float x, y;
    while (!LCD.Touch(&x, &y))
    {
        arm_servo.SetDegree(180.0);
    }
    Sleep(.1);
    while (LCD.Touch(&x, &y))
    {
        arm_servo.SetDegree(1.0 / cds.Value() * 180.0);
    }
}
 
//Function to have robot move to the trash can and set up to deposit tray
void toTray()
{
    move_straight(STD_POWER, COUNTS_PER_INCH * 14, 6.0);
    turn_left(20, COUNTS_PER_INCH * 5.6);
    move_straight(STD_POWER, COUNTS_PER_INCH * 18.5, 6.0);
    turn_right(20, COUNTS_PER_INCH * 5.15);
    check_heading(93.);
    Sleep(1.0);
}
 
//Moves arm down to deposit tray into trash can
void atTray()
{
 
    for (float degree = 30.0; degree < 130.0; degree += 5)
    {
        arm_servo.SetDegree(degree);
        Sleep(.05);
    }
    Sleep(1.0);
    move_straight(-20, COUNTS_PER_INCH * .5, 1.0); //The next few lines ensure that the arm doesn't keep holding the tray while its moving back
    arm_servo.SetDegree(125);
    move_straight(-20, COUNTS_PER_INCH * 1.5, 1.0);
    arm_servo.SetDegree(120);
    move_straight(-20, COUNTS_PER_INCH * 2.25, 1.5);
}
 
//Moves robot to the ticket from the trash can
void toTicket()
{
 
    arm_servo.SetDegree(20.0);
    turn_right(20, COUNTS_PER_INCH * 5.6);
    move_straight(STD_POWER, COUNTS_PER_INCH * 21., 6.0);
    // check_heading(0.);
    // turn_left(20, COUNTS_PER_INCH * 4.25); // DO NOT CHANGE JUST CHARGE PROTEUS
    turn_left(20, COUNTS_PER_INCH * NINETY_DEG_TWENTYSPEED);
    Sleep(0.35);
    check_heading(90.);
    turn_right(20, COUNTS_PER_INCH * 1.25);
 
 
}
 
//places arm in the correct place to quickly rotate and throw the ticket to the ready side
void atTicket()
{
    for (int i = 20.; i < 160; i+=5){
        arm_servo.SetDegree(i);
        Sleep(.05);
    }
    arm_servo.SetDegree(158);
    move_straight(STD_POWER, COUNTS_PER_INCH * 6.5, 2.5);
    Sleep(1.0);
    angled_turn(5, 20, 50, 2);
}
 
//Moves back from ticket and approaches the hotplate
void toBurger()
{
    turn_right(20, COUNTS_PER_INCH * 1.1); // 15 deg turn
    move_straight(-STD_POWER, COUNTS_PER_INCH * 6.5, 6.0);
    arm_servo.SetDegree(140);
    turn_left(20, COUNTS_PER_INCH * NINETY_DEG_TWENTYSPEED);
    move_straight(STD_POWER, COUNTS_PER_INCH * 10.5, 6.0);
    turn_right(20, COUNTS_PER_INCH * NINETY_DEG_TWENTYSPEED);
    move_straight(50, COUNTS_PER_INCH * 26., 6.0);
    turn_right(20, COUNTS_PER_INCH * 5.475);
    arm_servo.SetDegree(20);
    move_straight(STD_POWER, COUNTS_PER_INCH * 4.75, 6.0);
    float burgerX = readRPSFile("BX", "X");
    Sleep(.35);
    check_x(burgerX, PLUS,14);
    turn_left(20, COUNTS_PER_INCH * 5.13);
    Sleep(.35);
    check_heading(88.);
    arm_servo.SetDegree(180);
    move_straight(STD_POWER, COUNTS_PER_INCH * 7.0, 3.0);
}
 
//Throws the hotplate up and returns the hotplate to original position
void atBurger()
{
 
    arm_servo.SetDegree(180);
    move_straight(STD_POWER, COUNTS_PER_INCH * 1.5, 6.0);
    float burgerY = readRPSFile("BY", "Y");
    Sleep(.35);
    check_y(burgerY, PLUS, 14);
    Sleep(1.0);
    arm_servo.SetDegree(170);
    Sleep(1.0);
    for (int i = 170; i > 130; i = i - 10)
    {
        arm_servo.SetDegree(i);
        Sleep(0.05);
    }
    // arm_servo.SetDegree(100); // set to 140 in the toHotPlate() fxn.
    move_straight(STD_POWER, COUNTS_PER_INCH * .4, 6.0);
    arm_servo.SetDegree(65);
    // left_motor.SetPercent(20);
    turn_right(40, COUNTS_PER_INCH * 2); // 20 degree turn
    Sleep(1.0);
    arm_servo.SetDegree(90);
    move_straight(-25, COUNTS_PER_INCH * 3, 6.0);
    turn_left(20, COUNTS_PER_INCH * 2.);
    Sleep(0.35);
    check_heading(90.);
    Sleep(0.35);
}
 
void toIceCream(int whichIceCream)
{
    move_straight(-STD_POWER, COUNTS_PER_INCH * 4, 6.0);
    turn_left(20, COUNTS_PER_INCH * NINETY_DEG_TWENTYSPEED);
    Sleep(0.35);
    check_heading(180.);
    move_straight(STD_POWER, COUNTS_PER_INCH * 4.5, 6.0);
    float chocolateX = readRPSFile("CH", "X");
    Sleep(.35);
    check_x(chocolateX, MINUS, 14);
    arm_servo.SetDegree(30);
    switch(whichIceCream){
        case 2:
        move_straight(25, COUNTS_PER_INCH * 5., 6.0);
        turn_right(20, COUNTS_PER_INCH * NINETY_DEG_TWENTYSPEED);
        move_straight(25, COUNTS_PER_INCH * 7., 6.0);
        break;
        case 1:
        move_straight(25, COUNTS_PER_INCH * 8.5, 6.0);
        turn_right(20, COUNTS_PER_INCH * NINETY_DEG_TWENTYSPEED);
        move_straight(25, COUNTS_PER_INCH * 4.75, 6.0);
        break;
        case 0:
        move_straight(25, COUNTS_PER_INCH * 9., 6.0);
        turn_right(20, COUNTS_PER_INCH * 4.4);
        move_straight(25, COUNTS_PER_INCH * 2.5, 6.0);
        break;
    }
}
 
void atIceCream(int lever){
    Sleep(.5);
    for (int degree = 30; degree < 140; degree+=10)
    {
        arm_servo.SetDegree(degree);
        Sleep(.05);
    }
    arm_servo.SetDegree(120);
    move_straight(-STD_POWER, COUNTS_PER_INCH * 3, 6.0);
    Sleep(1.0);
    arm_servo.SetDegree(160);
    move_straight(STD_POWER, COUNTS_PER_INCH * 3, 6.0);
    for (int degree = 160; degree > 80; degree-=10)
    {
        arm_servo.SetDegree(degree);
        Sleep(.05);
    }
 
}
 
void toLever(float IRPSY, float IRPSH)
{
    move_straight(STD_POWER, COUNTS_PER_INCH * 13, 6.0);
    turn_left(20, COUNTS_PER_INCH * 5.475);
    move_straight(STD_POWER, COUNTS_PER_INCH * 7.25, 6.0);
    turn_right(20, COUNTS_PER_INCH * 5.1);
    move_straight(40, COUNTS_PER_INCH * 23, 6.0);
 
    Sleep(.5);
    check_heading(90);
    move_straight(STD_POWER, COUNTS_PER_INCH * 9.5, 6.0);
    check_y(IRPSY + 46, PLUS, 15);
    turn_left(20, COUNTS_PER_INCH * 3);
    Sleep(1.0);
    check_heading(IRPSH + 41);
    move_straight(STD_POWER, COUNTS_PER_INCH * 1.5, 6.0);
    Sleep(1.0);
}
 
void atLever(int IceCreamLever, float IRPSH)
{
    switch (IceCreamLever)
    {
    case 0:
        check_heading(IRPSH + 55);
        move_straight(STD_POWER, COUNTS_PER_INCH * 6, 6.0);
        break;
    case 1:
        move_straight(STD_POWER, COUNTS_PER_INCH * 5, 6.0);
        break;
    case 2:
        check_heading(IRPSH + 28);
        move_straight(STD_POWER, COUNTS_PER_INCH * 6, 6.0);
        break;
    }
    for (float degree = 30.0; degree < 140.0; degree += 10)
    {
        arm_servo.SetDegree(degree);
        Sleep(.05);
    }
    Sleep(1.0);
        for (float degree = 140.0; degree > 90.0; degree -= 5)
    {
        arm_servo.SetDegree(degree);
        Sleep(.05);
    }
 
    if (IceCreamLever == 0)
    {
        move_straight(-STD_POWER, COUNTS_PER_INCH * 2, 6.0);
    }
 
    move_straight(-STD_POWER, COUNTS_PER_INCH * 4, 6.0);
    Sleep(1.0);
    for (float degree = 90.0; degree < 140.0; degree += 5)
    {
        arm_servo.SetDegree(degree);
        Sleep(.05);
    }
   
    Sleep(8.0);
    if (IceCreamLever == 2)
    {
        move_straight(STD_POWER, COUNTS_PER_INCH * .5, 6.0);
    }
    move_straight(STD_POWER, COUNTS_PER_INCH * 2.5, 6.0);
    for (float degree = 140.0; degree > 70.0; degree -= 10)
    {
        arm_servo.SetDegree(degree);
        Sleep(.05);
    }
}
 
// void toFinishOld(int Lever)
// {
//     Sleep(2.0);
//     switch (Lever)
//     {
//     case 0:
//         move_straight(-STD_POWER, COUNTS_PER_INCH * 6);
//         turn_right(20, COUNTS_PER_INCH * 3.0);
//         break;
//     case 1:
//         move_straight(-STD_POWER, COUNTS_PER_INCH * 5);
//         turn_right(20, COUNTS_PER_INCH * 3.0);
//         break;
//     case 2:
//         move_straight(-STD_POWER, COUNTS_PER_INCH * 5);
//         break;
//     }
//     arm_servo.SetDegree(70);
 
//     Sleep(1.0);
//     check_heading(86);
//     move_straight(-STD_POWER, COUNTS_PER_INCH * 32.5);
//     turn_left(20, COUNTS_PER_INCH * 2.2);
//     move_straight(-40, COUNTS_PER_INCH * 25.);
// }
 
void toFinish(bool button)
{
    if(!button) // Red button
    {
        move_straight(-STD_POWER,COUNTS_PER_INCH*10.5, 6.0);
        turn_left(20,NINETY_DEG_TWENTYSPEED+COUNTS_PER_INCH*3);
        move_straight(45,COUNTS_PER_INCH*30, 6.0);
    } else
    {
        move_straight(-STD_POWER,COUNTS_PER_INCH*7.5, 6.0);
        turn_left(20,NINETY_DEG_TWENTYSPEED+COUNTS_PER_INCH*3);
        move_straight(45,COUNTS_PER_INCH*30, 6.0);
    }
}
 
 
int main(void)
{
    RPS.InitializeTouchMenu();
    logPoints();
    arm_servo.SetMin(SERVO_MIN);
    arm_servo.SetMax(SERVO_MAX);
 
    // float IRPSX = RPS.X();
    // float IRPSY = RPS.Y();
    start();
    int lever = RPS.GetIceCream();
    toTray();
    atTray();
    toTicket();
    atTicket();
    toBurger();
    atBurger();
    toIceCream(lever);
    atIceCream(lever);
 
    toJuke(lever);
    bool button = atJuke();
    toFinish(button);
    return 0;
}
 
 
 