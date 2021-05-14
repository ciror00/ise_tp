//=====[Libraries]=============================================================

#include "arm_book_lib.h"
#include "display.h"

//=====[Declaration of private defines]======================================

#define DISPLAY_REFRESH_TIME_MS 1000

//=====[Declaration and initialization of public global objects]===============

Serial uartUsb(USBTX, USBRX);
AnalogIn ligthSensor(A0);

DigitalIn blueButton(BUTTON1);
DigitalOut verminLed(LED1);
DigitalOut alarmLed(LED3);
DigitalOut otherLed(LED2);

DigitalOut sirenPin(PE_10); // Buzzer

// Interrupciones
InterruptIn pir(PG_0); // HC-SR501
InterruptIn shock(PG_1); // EM8343

Ticker preventiveCheck;

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

float ligth = 0.0;
int shaking = 0;

bool check = OFF;
bool pirStatus = ON;
bool paused = OFF;
bool maintenance = OFF;
bool verminDetectorState = OFF;

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of public functions]=========================

static void maintenanceMode();

static void userInterfaceDisplayInit();
static void userInterfaceDisplayUpdate(float ligth, bool vermin);

static void displayClear();
static void alarm();

void verminDetected();
void insideStatusIn();
void insideStatusOut();

//=====[Main function, the program entry point after power on or reset]========

int main(){
    userInterfaceDisplayInit();
    
    shock.mode(PullDown);
    //pir.mode(PullDown);

    shock.fall(&verminDetected);
    pir.fall(&insideStatusIn);
    preventiveCheck.attach([](){otherLed = !otherLed;}, 10.00);
    
    otherLed = OFF;
    
    while(1) {
        maintenance = (!blueButton) ? ON : OFF;

        if(maintenance){
            if(paused == ON){
                paused = OFF;
                userInterfaceDisplayInit();
            }
            ligth = ligthSensor.read() * 100;
            userInterfaceDisplayUpdate(ligth, check);
            alarm();
        }else{
            maintenanceMode();
        }
    }
}

//=====[Implementations of public functions]===================================

static void userInterfaceDisplayInit(){
    displayInit( DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER );
    displayClear();
    displayCharPositionWrite ( 0,1 );
    displayStringWrite( "Luz: " );
    displayCharPositionWrite ( 0,2 );
    displayStringWrite( "DET: " );
}

static void userInterfaceDisplayUpdate(float ligth, bool vermin){
    static int accumulatedDisplayTime = 0;
    char ligthString[2];
    
    if( accumulatedDisplayTime >= DISPLAY_REFRESH_TIME_MS ) {
        accumulatedDisplayTime = 0;

        sprintf(ligthString, "%.2f", ligth);
        displayCharPositionWrite ( 7,1 );
        displayStringWrite( ligthString );
        displayCharPositionWrite ( 12,1 );
        displayStringWrite( "lux" );
        uartUsb.printf( "Luz: %f lux\r\n", ligth); 
        
        displayCharPositionWrite ( 7,2 );
        if(!vermin || pir){
            displayStringWrite("DETECCION");
        }else{
            displayStringWrite("VACIO    ");
        }
        
        if(verminDetectorState){
            displayCharPositionWrite ( 7,0 );
            displayStringWrite("ALERTA!");
            displayCharPositionWrite ( 7,3 );
            displayStringWrite("ALERTA!");
        }else{
            displayCharPositionWrite ( 7,0 );
            displayStringWrite("       ");
            displayCharPositionWrite ( 7,3 );
            displayStringWrite("       ");
        }
    } else {
        accumulatedDisplayTime = accumulatedDisplayTime + 1;        
    } 
}

static void maintenanceMode(){
    displayClear();
    displayCharPositionWrite(3,1);
    displayStringWrite("MANTENIMIENTO");
    verminDetectorState = OFF;
    paused = ON;
}

void verminDetected(){
    if(!verminDetectorState){
        verminDetectorState = ON;
    }
}

void insideStatusIn(){
    check = ON;
    verminLed = ON;
    pir.fall(NULL);
    pir.rise(&insideStatusOut);
}

void insideStatusOut(){
    check = OFF;
    verminLed = OFF;
    pir.rise(NULL);
    if(pirStatus){
        pir.fall(&insideStatusIn);
    }
}

static void displayClear(){
    displayCharPositionWrite(0,0);
    displayStringWrite("                    ");
    displayCharPositionWrite(0,1);
    displayStringWrite("                    ");
    displayCharPositionWrite(0,2);
    displayStringWrite("                    ");
    displayCharPositionWrite(0,3);
    displayStringWrite("                    ");
}

static void alarm(){
    if(verminDetectorState){
        sirenPin = !sirenPin;
        alarmLed = sirenPin;
    }else{
        sirenPin = ON;
        alarmLed = !sirenPin;
    }
}