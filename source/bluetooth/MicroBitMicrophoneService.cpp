/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

/**
  * Class definition for the custom MicroBit Microphone Service.
  * Provides a BLE service provides streaming mic data, and magnitude
  */
#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitMicrophoneService.h"

const uint16_t MicroBitMicrophoneService::serviceUUID               = 0x6200;
const uint16_t MicroBitMicrophoneService::charUUID[ mbbs_cIdxCOUNT] = { 0x9350, 0x1c25 };


/**
  * Constructor.
  * Create a representation of the MicrophoneService
  * @param _ble The instance of a BLE device that we're running on.
  */
MicroBitMicrophoneService::MicroBitMicrophoneService( BLEDevice &_ble ) :
{
    // Initialise our characteristic values.
    microphoneDataCharacteristicBuffer   = 0;
    microphonePeriodCharacteristicBuffer = 0;
    
    // Register the base UUID and create the service.
    RegisterBaseUUID( bs_base_uuid);
    CreateService( serviceUUID);

    // Create the data structures that represent each of our characteristics in Soft Device.
    CreateCharacteristic( mbbs_cIdxDATA, charUUID[ mbbs_cIdxDATA],
                         (uint8_t *)&microphoneDataCharacteristicBuffer,
                         sizeof(microphoneDataCharacteristicBuffer), sizeof(microphoneDataCharacteristicBuffer),
                         microbit_propREAD | microbit_propNOTIFY);

    CreateCharacteristic( mbbs_cIdxPERIOD, charUUID[ mbbs_cIdxPERIOD],
                         (uint8_t *)&microphonePeriodCharacteristicBuffer,
                         sizeof(microphonePeriodCharacteristicBuffer), sizeof(microphonePeriodCharacteristicBuffer),
                         microbit_propREAD | microbit_propWRITE);

    if ( getConnected())
        listen( true);
}


/**
  * Set up or tear down event listers
  */
void MicroBitMicrophoneService::listen( bool yes)
{
    if (EventModel::defaultEventBus)
    {
        if ( yes)
        {
            // Ensure thermometer is being updated
            //microphoneDataCharacteristicBuffer   = 1;//thermometer.getTemperature();
            //microphonePeriodCharacteristicBuffer = 1;//thermometer.getPeriod();
            //EventModel::defaultEventBus->listen(MICROBIT_ID_THERMOMETER, MICROBIT_THERMOMETER_EVT_UPDATE, this, &MicroBitMicrophoneService::microphoneUpdate, MESSAGE_BUS_LISTENER_IMMEDIATE);
        }
        else
        {
            //EventModel::defaultEventBus->ignore(MICROBIT_ID_THERMOMETER, MICROBIT_THERMOMETER_EVT_UPDATE, this, &MicroBitMicrophoneService::microphoneUpdate);
        }
    }
}


/**
  * Invoked when BLE connects.
  */
void MicroBitMicrophoneService::onConnect( const microbit_ble_evt_t *p_ble_evt)
{
    listen( true);
}


/**
  * Invoked when BLE disconnects.
  */
void MicroBitMicrophoneService::onDisconnect( const microbit_ble_evt_t *p_ble_evt)
{
    listen( false);
}


/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
void MicroBitMicrophoneService::onDataWritten(const microbit_ble_evt_write_t *params)
{
    if (params->handle == valueHandle( mbbs_cIdxPERIOD) && params->len >= sizeof(microphonePeriodCharacteristicBuffer))
    {
        //memcpy(&microphonePeriodCharacteristicBuffer, params->data, sizeof(microphonePeriodCharacteristicBuffer));
        //thermometer.setPeriod(microphonePeriodCharacteristicBuffer);

        // The accelerometer will choose the nearest period to that requested that it can support
        // Read back the ACTUAL period it is using, and report this back.
        //microphonePeriodCharacteristicBuffer = thermometer.getPeriod();
        //setChrValue( mbbs_cIdxPERIOD, (const uint8_t *)&microphonePeriodCharacteristicBuffer, sizeof(microphonePeriodCharacteristicBuffer));
    }
}


/**
  * Temperature update callback
  */
void MicroBitMicrophoneService::microphoneUpdate(MicroBitEvent)
{
    if ( getConnected())
    {
        microphoneDataCharacteristicBuffer = 1;//thermometer.getTemperature();
        notifyChrValue( mbbs_cIdxDATA, (uint8_t *)&microphoneDataCharacteristicBuffer, sizeof(microphoneDataCharacteristicBuffer));
    }
}

#endif
