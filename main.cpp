#include "mbed.h"
#include <cstdint>
#include <events/mbed_events.h>
#include "ble/BLE.h"
#include "ble/Gap.h"
#include <AdvertisingDataSimpleBuilder.h>
#include <ble/gatt/GattCharacteristic.h>
#include <ble/gatt/GattService.h>
#include "att_uuid.h"

#include <USBSerial.h>
#define PressureFlag (1UL << 0)
#define TemperatureFlag (1UL << 9)
#define Start_Flag1 (1UL << 1)

DigitalOut ledG(LED3);
InterruptIn button2(p27); // D9

USBSerial ser;
Thread tempthread, pressurethread,VoltCheck;
Ticker guy;
I2C sensor_bus(I2C_SDA0,I2C_SCL0);
Mutex std_mutex;
EventFlags event_flags;

int counter = 0;
int temp = 0;
uint32_t flags_read = 0;

int16_t AC1, AC2, AC3, B1, B2, MB, MC, MD;;
uint16_t AC4, AC5, AC6;
int64_t UP,UT,B5,TOUT = 0,B6,X1,X2,X3,B3,p;
uint64_t B4,B7;


const int readaddr = 0xEF;
const int writeaddr = 0xEE;
uint8_t whoamiaddr[] = {0xD0};
int resp=4;
int16_t oss = 0; 

char readData[] ={0, 0};
char subaddr[] ={0, 0};

uint8_t databuf[2];

void ticker_function(){
    
    if (temp == 1){
        event_flags.set(PressureFlag);
        
    }
    else if (temp == 0) {
        event_flags.set(TemperatureFlag);
    }

}

void flip(){
    ledG = !ledG;
    temp = !temp;
}

void Vibe_VoltCheck(){
    while(true){
        button2.rise(&flip);
        thread_sleep_for(1);
    }
}

void measure_pressure(){

    while(1){
        flags_read = event_flags.wait_any(PressureFlag);
        std_mutex.lock();
        //Read uncompensated temp     
        readData[0] = 0xF4; // Control Reg 2
        readData[1] = 0x2E; // Signal a one shot temp reading.
        resp = sensor_bus.write(readaddr, readData, 2);

		thread_sleep_for(5);
        
        subaddr[0] = 0xF6; // MSB Temperature
        sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
        sensor_bus.read(readaddr, readData, 1);
        databuf[0] = ((uint8_t)readData[0]);

        subaddr[0] = 0xF7; // LSB Temperature
        sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
        sensor_bus.read(readaddr, readData, 1);
        databuf[1] = readData[0];

        UT = (databuf[0]<<8) | databuf[1];
        //Read uncompensated pressure
        readData[0] = 0xF4; 
        readData[1] = 0x34; 
        resp = sensor_bus.write(readaddr, readData, 2);
        thread_sleep_for(5);
        subaddr[0] = 0xF6; // MSB Presure
        sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
        sensor_bus.read(readaddr, readData, 1);
        databuf[0] = ((uint8_t)readData[0]);

        subaddr[0] = 0xF7; // LSB Pressure
        sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
        sensor_bus.read(readaddr, readData, 1);
        databuf[1] = readData[0];
        UP = (databuf[0]<<8) | databuf[1];

        //Calculate true temperature
        X1 = (UT-AC6)*AC5/pow(2,15);
        X2 = MC*pow(2,11)/(X1+MD);
        B5 = X1+X2;

        //Calculate true pressure
        B6 = B5 - 4000;
        X1 = (B2 * (B6*B6/pow(2,12))) / pow(2,11);
        X2 = AC2 * B6 / pow(2,11);
        X3 = X1 + X2;
        B3 = (((AC1 * 4 + X3) << oss)+2)/4;
        X1 = AC3 * B6 / pow(2,13);
        X2 = (B1 * (B6 * B6 / pow(2,12))) / pow(2,16);
        X3 = ((X1 + X2) + 2) / pow(2,2);
        B4 = AC4 * (uint64_t)(X3 + 32768) / pow(2,15);
        B7 = ((uint64_t)UP - B3) * (50000 >> oss);
        if (B7 < 0x80000000){
            p = (B7*2)/B4;
        } else{
            p = (B7/B4) * 2;
        }
        X1 = (p/pow(2,8))*(p/pow(2,8));
        X1 = (X1*3038)/ pow(2,16);
        X2 = (-7357*p)/pow(2,16);
        p = (p + (X1 + X2 +3791) / pow(2,4));
        ser.printf("Calibrated pressure: %d Pa\n\r",p);

        std_mutex.unlock();
        event_flags.clear(PressureFlag);
        thread_sleep_for(500);
    }
}

void measure_temp(){
    ser.printf("Let us begin in ...\n\r3\n\r");
    thread_sleep_for(1000);
    ser.printf("2\n\r");
    thread_sleep_for(1000);
    ser.printf("1\n\r");
    thread_sleep_for(1000);
    ser.printf("MASH THAT BUTTON!\n\r");


    resp = sensor_bus.write(writeaddr, (const char *) whoamiaddr, 1, true);
    
    if(  resp != 0 ){
        ser.printf("I failed to talk at the temp sensor. (Returned: %d)\n\r", resp);            
    }
              
    if( sensor_bus.read(readaddr, readData, 1)  != 0 ){
        ser.printf("I failed to listen to the temp sensor.\n\r");        
    }
    
    ser.printf("Sensor ON: %d\n\r", readData[0] );
    if( readData[0] != 0x55 ){
        ser.printf("Sensor not detected.\n\r");
    }

    readData[0] = 0x20; // Control Reg 1
    readData[1] = 0x84; // Turn on our temp sensor, and ensure that we read high to low on our values.
    resp = sensor_bus.write(readaddr, readData, 2);    

    //AC1
    subaddr[0] = 0xAA; 
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[0] = ((uint8_t)readData[0]);

    subaddr[0] = 0xAB;
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[1] = readData[0];
    AC1 = (databuf[0]<<8) | databuf[1];

    //AC2
    subaddr[0] = 0xAC; 
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[0] = ((uint8_t)readData[0]);

    subaddr[0] = 0xAD;
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[1] = readData[0];
    AC2 = (databuf[0]<<8) | databuf[1];
    //AC3
    subaddr[0] = 0xAE; 
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[0] = ((uint8_t)readData[0]);

    subaddr[0] = 0xAF;
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[1] = readData[0];
    AC3 = (databuf[0]<<8) | databuf[1];
    //AC4
    subaddr[0] = 0xB0; 
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[0] = ((uint8_t)readData[0]);

    subaddr[0] = 0xB1;
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[1] = readData[0];
    AC4 = (databuf[0]<<8) | databuf[1];
    //AC5
    subaddr[0] = 0xB2; 
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[0] = ((uint8_t)readData[0]);

    subaddr[0] = 0xB3;
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[1] = readData[0];
    AC5 = (databuf[0]<<8) | databuf[1];
    //AC6
    subaddr[0] = 0xB4; 
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[0] = ((uint8_t)readData[0]);

    subaddr[0] = 0xB5;
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[1] = readData[0];
    AC6 = (databuf[0]<<8) | databuf[1];
    //B1
    subaddr[0] = 0xB6; 
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[0] = ((uint8_t)readData[0]);

    subaddr[0] = 0xB7;
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[1] = readData[0];
    B1 = (databuf[0]<<8) | databuf[1];
    //B2
    subaddr[0] = 0xB8; 
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[0] = ((uint8_t)readData[0]);

    subaddr[0] = 0xB9;
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[1] = readData[0];
    B2 = (databuf[0]<<8) | databuf[1];
    //MB
    subaddr[0] = 0xBA; 
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[0] = ((uint8_t)readData[0]);

    subaddr[0] = 0xBB;
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[1] = readData[0];
    MB = (databuf[0]<<8) | databuf[1];
    //MC
    subaddr[0] = 0xBC; 
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[0] = ((uint8_t)readData[0]);

    subaddr[0] = 0xBD;
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[1] = readData[0];
    MC = (databuf[0]<<8) | databuf[1];
    //MD
    subaddr[0] = 0xBE; 
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[0] = ((uint8_t)readData[0]);

    subaddr[0] = 0xBF;
    sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
    sensor_bus.read(readaddr, readData, 1);
    databuf[1] = readData[0];
    MD = (databuf[0]<<8) | databuf[1];


    while(1){   
        flags_read = event_flags.wait_any(TemperatureFlag);
        std_mutex.lock();
        //Read uncompensated temp     
        readData[0] = 0xF4; // Control Reg 2
        readData[1] = 0x2E; // Signal a one shot temp reading.
        resp = sensor_bus.write(readaddr, readData, 2);

		thread_sleep_for(5);
        
        subaddr[0] = 0xF6; // MSB Temperature
        sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
        sensor_bus.read(readaddr, readData, 1);
        databuf[0] = ((uint8_t)readData[0]);

        subaddr[0] = 0xF7; // LSB Temperature
        sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
        sensor_bus.read(readaddr, readData, 1);
        databuf[1] = readData[0];

        UT = (databuf[0]<<8) | databuf[1];
        //Read uncompensated pressure
        readData[0] = 0xF4; 
        readData[1] = 0x34; 
        resp = sensor_bus.write(readaddr, readData, 2);
        thread_sleep_for(5);
        subaddr[0] = 0xF6; // MSB Presure
        sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
        sensor_bus.read(readaddr, readData, 1);
        databuf[0] = ((uint8_t)readData[0]);

        subaddr[0] = 0xF7; // LSB Pressure
        sensor_bus.write(writeaddr, (const char *) subaddr, 1, true);
        sensor_bus.read(readaddr, readData, 1);
        databuf[1] = readData[0];
        UP = (databuf[0]<<8) | databuf[1];

        //Calculate true temperature
        X1 = (UT-AC6)*AC5/pow(2,15);
        X2 = MC*pow(2,11)/(X1+MD);
        B5 = X1+X2;
        TOUT = ((B5 + 8)/pow(2,4));
        ser.printf("Calibrated temperature: %d C (multiplied by 0.1)\n\r",TOUT);

        std_mutex.unlock();
        event_flags.clear(TemperatureFlag);

        thread_sleep_for(500);
    }
        
}




int main(){
    DigitalOut i2cbuspull(P1_0); // Pull up i2C. resistor.
    i2cbuspull.write(1);
    DigitalOut sensor_pwr(P0_3); // Supply power to all of the sensors (VCC)
    sensor_pwr.write(1);

    //first runs temperature before while loop() waits for flag
    tempthread.start(callback(measure_temp));
    //then activates pressure thread on standby for flag
    pressurethread.start(callback(measure_pressure));

    //constantly checks for button press and toggles temp
    VoltCheck.start(callback(Vibe_VoltCheck));

    //ticker check temp variable every 5 seconds
    //if button is toggled to 1 pressure runs, if button is toggled to 0 
    guy.attach(&ticker_function, 2);
    while(true){
        thread_sleep_for(5000);
    }
}

