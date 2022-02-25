#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>
// #define CDS_THRESHOLD *put value here*
#define CDS_BLUE_THRESHOLD 1.015
#define CDS_RED_THRESHOLD .5
#define CDS_UPPER_THRESHOLD 2.0
#define COUNTS_PER_INCH 34.0

/* Declarations for encoders, motors, and input pins (all subject to
change) */
DigitalEncoder right_encoder(FEHIO::P2_0); 
DigitalEncoder left_encoder(FEHIO::P3_0);
FEHMotor right_motor(FEHMotor::Motor0, 9.0);
FEHMotor left_motor(FEHMotor::Motor3, 9.0);
AnalogInputPin cds(FEHIO::P1_0);

/* function for turning right. use encoders */
void turn_right(int percentage, int encoder_counts) { 
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    left_motor.SetPercent(percentage);
    right_motor.SetPercent(-percentage);

    // Keep running motors while average of left and right encoder is less
    // than encoderCounts
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < encoder_counts);

    // turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

/* function for turning left. use encoders */
void turn_left(int percentage, int encoder_counts) { 
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    left_motor.SetPercent(-percentage);
    right_motor.SetPercent(percentage);

    // Keep running motors while average of left and right encoder is less
    // than encoderCounts
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < encoder_counts);

    // turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

/* function for moving forward. use encoders */
void move_straight(int percentage, int encoder_counts) { 
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    left_motor.SetPercent(percentage);
    right_motor.SetPercent(percentage);

    // Keep running motors while average of left and right encoder is less
    // than encoderCounts
    while((left_encoder.Counts() + right_encoder.Counts()) / 2.0 < encoder_counts);
    
    
    // turn off motors
    right_motor.Stop();
    left_motor.Stop();

    
}

/* function for moving at an angle 
    direction is based off of quadrants (1 is forward right, 2 forward left, etc.) 
    percentage is motor percentage
    angle is the desired angle of turning as a decimal
    encoder counts for how much to go*/
void angled_turn(int direction, int percentage, float angle, int inches){
    // Initializing distance and angle
    int encoder_counts = inches*COUNTS_PER_INCH;
    float turn_radius = angle/100.0;
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    // Set both motors to desired percent based on direction
    int adjusted_angle = percentage*(1-turn_radius);
    switch (direction){
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
    }


    // Keep running motors while average of left and right encoder is less
    // than encoderCounts
    while((left_encoder.Counts() + right_encoder.Counts()) / 2.0 < encoder_counts);

    // turn off motors
    right_motor.Stop();
    left_motor.Stop();
    Sleep(.25);
}
/* Function for moving based off sensing the start light */
void start(){
    // Display what function is currently running
    LCD.Clear();
    LCD.WriteLine("start");

    // Wait until light goes on
    while (cds.Value() > CDS_BLUE_THRESHOLD);
    // Calling function to make angled turn to the front left 20 inches at a 5 degree angle
    /*angled_turn(2, 20, 10.0, 20);
    // Calling function to make backward right angled 5 inches at a 3 degree angle turn to re align 
    angled_turn(4, 20, 55.0, 12);
    // Calling function to go to the front right 8 inches at a 5 degree angle to the light
    angled_turn(1, 20, 5.0, 5); */
    if((cds.Value() > CDS_RED_THRESHOLD) && cds.Value() < CDS_BLUE_THRESHOLD) {
        LCD.WriteLine("The start line was blue"); // Display what color is read
    } else {
        LCD.WriteLine("The start line was red"); // Display what color is read
    }
    move_straight(20, COUNTS_PER_INCH*13);
    turn_left(20, COUNTS_PER_INCH*5);
    move_straight(20, COUNTS_PER_INCH*16.8);
    turn_left(20, COUNTS_PER_INCH*5.5);
    move_straight(20, COUNTS_PER_INCH * 1.5);

    Sleep(1.0);
    
}

void pushButton() {
    // Display what function is currently running
    LCD.Clear();
    LCD.WriteLine("pushButton"); 

    if (cds.Value() < CDS_RED_THRESHOLD) {
        LCD.Clear(RED); // Display what color is read
        LCD.WriteLine("Need to hit RED");
        LCD.WriteLine(cds.Value());
        turn_right(20, COUNTS_PER_INCH * 5);
        move_straight(20, COUNTS_PER_INCH * 0.5);
        turn_left(20, COUNTS_PER_INCH * 5);
        move_straight(20, COUNTS_PER_INCH * 6);
    } else {
        LCD.Clear(BLUE); // Display what color is read
        LCD.WriteLine("Need to hit BLUE");
        LCD.WriteLine(cds.Value());
        turn_left(20, COUNTS_PER_INCH * 5);
        move_straight(20, COUNTS_PER_INCH * 0.5);
        turn_right(20, COUNTS_PER_INCH * 5);
        move_straight(20, COUNTS_PER_INCH * 6);
    }
}

void toRamp() {
    move_straight(-20, COUNTS_PER_INCH * 6);
    turn_right(20, COUNTS_PER_INCH * 5);
    move_straight(-20, COUNTS_PER_INCH * 6);
    turn_left(20, COUNTS_PER_INCH * 5);
    move_straight(-20, COUNTS_PER_INCH * 4);
}
int main(void)
{
    /* Code to go to the jukebox light from the start*/
    start();
    Sleep(1.0);
    pushButton();
    Sleep(1.0);
    toRamp();
    move_straight(-35, COUNTS_PER_INCH * 15);
    move_straight(20, COUNTS_PER_INCH * 15);
    /* Reads light and makes decision for which button to press */
	return 0;
}
