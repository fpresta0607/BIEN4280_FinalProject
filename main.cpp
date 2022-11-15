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

USBSerial ser;
Thread tempthread;
int16_t AC1, AC2, AC3, B1, B2, MB, MC, MD;;
uint16_t AC4, AC5, AC6;
int32_t UP,UT,B5,TOUT = 0,B6,X1,X2,X3,B3,p;
uint32_t B4,B7;

void measure_temp(){
    I2C sensor_bus(I2C_SDA0,I2C_SCL0);
    ser.printf("Let us begin in ...\n\r3\n\r");
    thread_sleep_for(1000);
    ser.printf("2\n\r");
    thread_sleep_for(1000);
    ser.printf("1\n\r");
    thread_sleep_for(1000);

    const int readaddr = 0xEF;
    const int writeaddr = 0xEE;
    uint8_t whoamiaddr[] = {0xD0};
    int resp=4;
    int16_t oss = 0; 

    char readData[] ={0, 0};
    char subaddr[] ={0, 0};
    resp = sensor_bus.write(writeaddr, (const char *) whoamiaddr, 1, true);
    
    if(  resp != 0 ){
        ser.printf("I failed to talk at the temp sensor. (Returned: %d)\n\r", resp);            
    }
              
    if( sensor_bus.read(readaddr, readData, 1)  != 0 ){
        ser.printf("I failed to listen to the temp sensor.\n\r");        
    }
    
    ser.printf("Who Am I? %d\n\r", readData[0] );
    if( readData[0] != 0x55 ){
        ser.printf("Who are are you?\n\r");
    }

    readData[0] = 0x20; // Control Reg 1
    readData[1] = 0x84; // Turn on our temp sensor, and ensure that we read high to low on our values.
    resp = sensor_bus.write(readaddr, readData, 2);    


    //READ in calibration data
    uint8_t databuf[2];
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

        //Read uncalibrated temp     
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
        TOUT = (B5 + 8)/pow(2,4);
        ser.printf("Calibrated temperature: %d C\n\r",TOUT);

        //Calculate true pressure
        
        thread_sleep_for(500);
    }
        
}




int main(){
    DigitalOut i2cbuspull(P1_0); // Pull up i2C. resistor.
    i2cbuspull.write(1);
    DigitalOut sensor_pwr(P0_3); // Supply power to all of the sensors (VCC)
    sensor_pwr.write(1);

    tempthread.start(callback(measure_temp));
    while(true){
        thread_sleep_for(5000);
    }
}

