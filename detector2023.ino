#include <Wire.h>
#include <math.h>

#include "SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h"

double avg_reading;
double cal_reading;
double mass;
double voltage;

const double preset_reading = -48729;

double offset = 0;

#define RED_PIN 2
#define BLUE_PIN 3
#define GREEN_PIN 4

//LED Stuff
//change this section to match ranges given at the competition
#define RED_MIN 47
#define RED_MAX 621

#define GREEN_MIN 332
#define GREEN_MAX 735

#define BLUE_MIN 451
#define BLUE_MAX 932

//formula to convert voltage to mass
double valToMass(double x)
{
    return (2.2651*pow(10,-12)*x*x)+(.00387193*x)+588.749;
}


//display led code
void displayLed(double mass){
    if (BLUE_MIN < mass && mass < BLUE_MAX)
        digitalWrite(BLUE_PIN, HIGH);
    else
        digitalWrite(BLUE_PIN, LOW);

    if (RED_MIN < mass && mass < RED_MAX)
        digitalWrite(RED_PIN, HIGH);
    else
        digitalWrite(RED_PIN, LOW);

    if (GREEN_MIN < mass && mass < GREEN_MAX)
        digitalWrite(GREEN_PIN, HIGH);
    else
        digitalWrite(GREEN_PIN, LOW);
}


NAU7802 adc; //Create instance of the NAU7802 class
void setup()
{
  
    Serial.begin(9600);
    Serial.println("BHS Detector Building 2023");
    
    Wire.begin();

    //checks to see if adc is detected
    if (adc.begin() == false)
    {
        Serial.println("ADC not detected");
        while (1);
    }
    Serial.println("ADC detected");

    adc.setGain(NAU7802_GAIN_128); //set gain to 1
    adc.setLDO(NAU7802_LDO_4V5); //set vref pin (pin 1) to 4.5V
    adc.setChannel(NAU7802_CHANNEL_1); //use channel 1
    delay(2000);
    adc.calibrateAFE(); //calibrate stuff
}

void loop()
{
    if(Serial.available() != 0){
        Serial.read();
        offset = preset_reading-avg_reading;
        Serial.print("Calibrated with new factor: ");
        Serial.println(offset);
        delay(500);
    }
    
    avg_reading = adc.getAverage(25);
    cal_reading = avg_reading + offset;
    
    //convert reading to voltage
    voltage = cal_reading*4.5/pow(2,22); //voltage between pins 2 and 3
    
    mass = valToMass(cal_reading); //calculate mass



    // janky manual calibration section
//    if(mass > 750){
//      mass -=2;
//    }
//    else if(mass >500){
//      mass-=1;
//    }
//    else if(mass <200){
//      mass+=1;
//    }
//    else if(mass <50){
//      mass+=2;
//    }
    //
//    if(mass > 500){
//      mass *=1;
//    }
//    else if (mass < 100){
//      mass -= 0;
//    }
//    else if (mass < 250){
//      mass -= 0;
//    }
//    
//    if(mass > 1000){
//      mass = 991;
//    }
//    else if (mass < 0){
//      mass = 10;
//    }


    //print values
    Serial.print("uncal Reading: ");
    Serial.print(avg_reading,0);
    Serial.print(" | Reading: ");
    Serial.print(cal_reading,0);
    Serial.print(" | Voltage: ");
    Serial.print(voltage, 10);
    Serial.print(" | Mass (g): ");
    Serial.print(mass, 1);
    Serial.println();

    displayLed(mass);
}
