#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>
// #define CDS_THRESHOLD *put value here*
#define CDS_THESHOLD 1.0

/* Declarations for encoders, motors, and input pins (all subject to
change) */
DigitalEncoder right_encoder(FEHIO::P0_0); 
DigitalEncoder left_encoder(FEHIO::P0_1);
FEHMotor right_motor(FEHMotor::Motor0, 9.0);
FEHMotor left_motor(FEHMotor::Motor1, 9.0);
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
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < encoder_counts);

    // turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

float read_cds() {

}

int main(void)
{
    while(cds.Value() > CDS_THESHOLD); // while the CdS reading is greater than threshold, wait
    
    /* Insert code here for starting the robot */
    
    LCD.WriteLine("Hello World!");
	return 0;
}
