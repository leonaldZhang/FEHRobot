 
#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>
#include <FEHServo.h>
// #define CDS_THRESHOLD *put value here*
#define CDS_BLUE_THRESHOLD 1.015
#define CDS_RED_THRESHOLD .5
#define CDS_UPPER_THRESHOLD 2.0
#define COUNTS_PER_INCH 34.0
#define NINETY_DEG_TWENTYSPEED 5.2
#define SERVO_MIN 982
#define SERVO_MAX 2100
 
/* Declarations for encoders, motors, and input pins (all subject to
change) */
DigitalEncoder right_encoder(FEHIO::P2_0);
DigitalEncoder left_encoder(FEHIO::P3_0); // Left stupid
FEHMotor right_motor(FEHMotor::Motor0, 9.0);
FEHMotor left_motor(FEHMotor::Motor3, 9.0); // left taped
AnalogInputPin cds(FEHIO::P1_0);
FEHServo arm_servo(FEHServo::Servo0);
 
/* function for turning right. use encoders */
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
    while ((left_encoder.Counts() + right_encoder.Counts()) / 2. < encoder_counts)
        ;
 
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
void move_straight(int percentage, int encoder_counts)
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
 
    // Set both motors to desired percent
    left_motor.SetPercent(percentage);
    right_motor.SetPercent(percentage);
 
    // Keep running motors while average of left and right encoder is less
    // than encoderCounts
    while ((left_encoder.Counts() + right_encoder.Counts()) / 2.0 < encoder_counts)
        ;
 
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
    while ((left_encoder.Counts() + right_encoder.Counts()) / 2.0 < encoder_counts)
        ;
 
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
 
void toJuke()
{
    move_straight(20, COUNTS_PER_INCH * 13);
    turn_left(20, COUNTS_PER_INCH * 5.1);
    move_straight(20, COUNTS_PER_INCH * 16.8);
    turn_left(20, COUNTS_PER_INCH * 5.3);
    move_straight(20, COUNTS_PER_INCH * 0.75);
    Sleep(1.0);
}
bool pushButton()
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
        move_straight(20, COUNTS_PER_INCH * 0.5);
        turn_left(20, COUNTS_PER_INCH * 5);
        move_straight(20, COUNTS_PER_INCH * 5);
    }
    else
    {
        LCD.Clear(BLUE); // Display what color is read
        LCD.WriteLine("Need to hit BLUE");
        LCD.WriteLine(cds.Value());
        turn_left(20, COUNTS_PER_INCH * 5);
        move_straight(20, COUNTS_PER_INCH * 0.5);
        turn_right(20, COUNTS_PER_INCH * 5);
        move_straight(20, COUNTS_PER_INCH * 5);
        button = true; // hit blue button
    }
    return button;
}
 
void toRamp(bool redBlue)
{
    LCD.Clear();
    LCD.WriteLine("toRamp");
    if (!redBlue)
    {
        move_straight(-20, COUNTS_PER_INCH * 6);
        turn_right(20, COUNTS_PER_INCH * 5);
        move_straight(-20, COUNTS_PER_INCH * 10);
        turn_left(20, COUNTS_PER_INCH * 5);
        move_straight(-20, COUNTS_PER_INCH * 4);
    }
    else
    {
        move_straight(-20, COUNTS_PER_INCH * 6);
        turn_right(20, COUNTS_PER_INCH * 5);
        move_straight(-20, COUNTS_PER_INCH * 9);
        turn_left(20, COUNTS_PER_INCH * 5);
        move_straight(-20, COUNTS_PER_INCH * 4);
    }
}
 
void moveArm()
{
    // Enter the min and max servo valeus from the .TouchCalibrate() fxn
    arm_servo.SetMin(SERVO_MIN);
    arm_servo.SetMax(SERVO_MAX);
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
 
void toTray()
{
    start();
    move_straight(20, COUNTS_PER_INCH * 13);
    turn_left(20, COUNTS_PER_INCH * 5.475);
    move_straight(20, COUNTS_PER_INCH * 19);
    turn_right(20, COUNTS_PER_INCH * 5.1);
    move_straight(20, COUNTS_PER_INCH * 1);
    Sleep(1.0);
}
 
void atTray()
{
    arm_servo.SetMax(SERVO_MAX);
    arm_servo.SetMin(SERVO_MIN);
    for (float degree = 30.0; degree < 165.0; degree += 5)
    {
        arm_servo.SetDegree(degree);
        Sleep(.05);
    }
    Sleep(1.0);
    move_straight(-20, COUNTS_PER_INCH * 2);
    arm_servo.SetDegree(155);
    move_straight(-20, COUNTS_PER_INCH * 2.25);
}
 
void toTicket()
{
    arm_servo.SetDegree(20.0);
    turn_right(20, COUNTS_PER_INCH * 5.8);
    move_straight(20, COUNTS_PER_INCH * 21.8);
    turn_left(20, COUNTS_PER_INCH * 4.55);
}
 
void atTicket()
{
    arm_servo.SetMin(SERVO_MIN);
    arm_servo.SetMax(SERVO_MAX);
    arm_servo.SetDegree(180.0);
    move_straight(20, COUNTS_PER_INCH * 6.85);
    Sleep(1.0);
    angled_turn(5, 20, 50, 2);
}
 
void toHotPlate()
{
    arm_servo.SetMin(SERVO_MIN);
    arm_servo.SetMax(SERVO_MAX);
 
    turn_right(20, COUNTS_PER_INCH * 0.9); // 15 deg turn
    move_straight(-20, COUNTS_PER_INCH * 4);
    arm_servo.SetDegree(140);
    turn_left(20, COUNTS_PER_INCH * NINETY_DEG_TWENTYSPEED);
    move_straight(20, COUNTS_PER_INCH * 11);
    turn_right(20, COUNTS_PER_INCH * NINETY_DEG_TWENTYSPEED);
    move_straight(35, COUNTS_PER_INCH * 20);
    turn_right(20, COUNTS_PER_INCH * 1.5); // 30 degree turn.
    move_straight(20, COUNTS_PER_INCH * 24);
}

void atHotPlate() {
    arm_servo.SetDegree(100); // set to 140 in the toHotPlate() fxn.
    move_straight(20, COUNTS_PER_INCH * 0.75);
    arm_servo.SetDegree(70);
    turn_right(20, COUNTS_PER_INCH * 1.1); // 20 degree turn
}
 
int main(void)
{
 
    // /* Code to go to the jukebox light from the start*/
    // start();
    // Sleep(1.0);
    // bool whichButton = pushButton(); // get which button was pressed.
    // Sleep(1.0);
    // toRamp(whichButton); // move to ramp based on which button was pressed
    // move_straight(40, COUNTS_PER_INCH * 20);
    // Sleep(10.0);
    // move_straight(60, COUNTS_PER_INCH * 20);
    // Sleep(30.0);
    // move_straight(-20, COUNTS_PER_INCH * 10);
    // Sleep(1.0);
    // move_straight(20, COUNTS_PER_INCH * 30);
    // /* Reads light and makes decision for which button to press */
    toTray();
    atTray();
    toTicket();
    atTicket();
    toHotPlate();
 
    return 0;
}
 
 

