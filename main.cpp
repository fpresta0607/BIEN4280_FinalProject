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

static events::EventQueue event_queue(16 * EVENTS_EVENT_SIZE);

BLE &bleinit= BLE::Instance();
Gap& gap = bleinit.gap();
GattServer& gattServe = bleinit.gattServer();
GattClient& gattClient = bleinit.gattClient();

int16_t TOUT =0;

using namespace ble;

/**
 * Event handler struct
 */
struct GapEventHandler : Gap::EventHandler{
    /* 
	 * Implement the functions here that you think you'll need. These are defined in the GAP EventHandler:
     * https://os.mbed.com/docs/mbed-os/v6.6/mbed-os-api-doxy/structble_1_1_gap_1_1_event_handler.html
     */
    
    void onAdvertisingStart(const AdvertisingStartEvent &event){
        ser.printf("Searching for connection...\r\n");
    } 
    void onConnectionComplete(const ConnectionCompleteEvent &event){
        ser.printf("Connection established.\r\n");
    }
    void onDisconnectionComplete(const DisconnectionCompleteEvent &event){
        ser.printf("Device disconnected.");
        gap.startAdvertising(LEGACY_ADVERTISING_HANDLE);
    }
    virtual void onScanTimeout(const ScanTimeoutEvent &event){
        ser.printf("Searching timed out.");
    }
        

};


GapEventHandler THE_gap_EvtHandler;
GattCharacteristic *characteristics[1];
UUID tempUUID("211e2e01-8f43-4ef7-9958-765fe2518da9");
ReadOnlyGattCharacteristic<int16_t>* read_value;
GattService * francos;

void measure_temp(){
    I2C sensor_bus(I2C_SDA0,I2C_SCL0);

    const int readaddr = 0xEF;
    const int writeaddr = 0xEE;
    uint8_t whoamiaddr[] = {0xD0};
    int resp=4;

    char readData[] ={0, 0};
    char subaddr[] ={0, 0};
    resp = sensor_bus.write(writeaddr, (const char *) whoamiaddr, 1, true);
    
    if(  resp != 0 ){
        ser.printf("I failed to talk at the temp sensor. (Returned: %d)\n\r", resp);            
    }
              
    if( sensor_bus.read(readaddr, readData, 1)  != 0 ){
        ser.printf("I failed to listen to the temp sensor.\n\r");        
    }
    
    ser.printf("Who Am I? %d\n", readData[0] );
    if( readData[0] != 0x55 ){
        ser.printf("Who are are you?\n\r");
    }

    readData[0] = 0x20; // Control Reg 1
    readData[1] = 0x84; // Turn on our temp sensor, and ensure that we read high to low on our values.
    resp = sensor_bus.write(readaddr, readData, 2);    


    uint8_t databuf[2];
    while(1){        
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

        TOUT = (databuf[0]<<8) | databuf[1];
        ser.printf("Uncalibrated temperature: %d\n\r",TOUT);
        //write from temp from server back to client
        gattServe.write(read_value->getValueHandle(),databuf,sizeof(databuf)/sizeof(databuf[0]),false);
        // Sleep for a while.
        thread_sleep_for(5000);
    }
}
void ads(){
    //Setup advertising payload
    gap.setAdvertisingPayload(
            LEGACY_ADVERTISING_HANDLE,
            AdvertisingDataSimpleBuilder<LEGACY_ADVERTISING_MAX_SIZE>()
                .setFlags()
                .setName("May contain nuts", true)
                .setAppearance(adv_data_appearance_t::GENERIC_PHONE)
                //.setAdvertisingInterval(interval) //EXTRA CREDIT
                .getAdvertisingData()
    );
    
    //start advertising
    gap.startAdvertising(LEGACY_ADVERTISING_HANDLE);

}
void on_init_complete(BLE::InitializationCompleteCallbackContext *params){
    //Notify the terminal if initialization was succesful using callback parameter
    if (params->error == BLE_ERROR_NONE){
        ser.printf("Successfully initialized.\r\n");
    }else{
        ser.printf("No success.\r\n");
    }
    ser.printf("%i\r\n",params->error);

    
    //set the event handler
    gap.setEventHandler(&THE_gap_EvtHandler);

    // Set Up custom Characteristics
    //steps
    // read only gatt
    // gatt characteristics good
    // gatt service ()

    //define new ReadOnlyGatt Char
    read_value = new ReadOnlyGattCharacteristic<int16_t>(tempUUID, &TOUT, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ|GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE);
    characteristics[0] = read_value;
    //create service (uuid, charctertistics pointer, numCharacteristics) //find size in bytes of characteristics
    francos = new GattService(GattService::UUID_HEALTH_THERMOMETER_SERVICE, characteristics, sizeof(characteristics)/sizeof(characteristics[0]));

    
    //add gatt serivice
    gattServe.addService(*francos); //should be pointer arg

    //linking to temp thread to get correct temp
    tempthread.start(callback(measure_temp));
    event_queue.call(&ads);
    

}

/* Schedule processing of events from the BLE middleware in the event queue. */
void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context){
    event_queue.call(mbed::Callback<void()>(&context->ble, &BLE::processEvents));
}



int main(){
    DigitalOut i2cbuspull(P1_0); // Pull up i2C. resistor.
    i2cbuspull.write(1);
    DigitalOut sensor_pwr(P0_3); // Supply power to all of the sensors (VCC)
    sensor_pwr.write(1);

    bleinit.onEventsToProcess(schedule_ble_events);
    

    // Initialize bleinit
    bleinit.init(on_init_complete);
    

    // This will never return...
    event_queue.dispatch_forever();
}

