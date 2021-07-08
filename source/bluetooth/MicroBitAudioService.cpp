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
  * Class definition for the custom MicroBit Audio Service.
  * Provides a BLE service provides support for playing notes for specified duration
  */

#include "MicroBitConfig.h"
#include "MicroBit.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitAudioService.h"

const uint16_t MicroBitAudioService::serviceUUID               = 0x6300;
const uint16_t MicroBitAudioService::charUUID[ mbbs_cIdxCOUNT] = { 0x9450 };

static Pin *pin = NULL;
static uint8_t pitchVolume = 0xff;
extern  MicroBit uBit;
/**
  * Constructor.
  * Create a representation of the MicrophoneService
  * @param _ble The instance of a BLE device that we're running on.
  */
MicroBitAudioService::MicroBitAudioService( BLEDevice &_ble ) 
{

    pin = &uBit.audio.virtualOutputPin;
    
    // Register the base UUID and create the service.
    RegisterBaseUUID( bs_base_uuid);
    CreateService( serviceUUID);

    // Create the data structures that represent each of our characteristics in Soft Device.
    CreateCharacteristic( mbbs_cIdxDATA, charUUID[ mbbs_cIdxDATA],
                         (uint8_t *)&audioDataCharacteristicBuffer,
                         sizeof(audioDataCharacteristicBuffer), sizeof(audioDataCharacteristicBuffer),
                         microbit_propWRITE);
}

/**
  * Invoked when BLE connects.
  */
void MicroBitAudioService::onConnect( const microbit_ble_evt_t *p_ble_evt)
{
}

/**
  * Invoked when BLE disconnects.
  */
void MicroBitAudioService::onDisconnect( const microbit_ble_evt_t *p_ble_evt)
{

}

// Pin control as per MakeCode.
void MicroBitAudioService::analogPitch(int frequency, int ms) {
    if (frequency <= 0 || pitchVolume == 0) {
        pin->setAnalogValue(0);
    } else {
        // I don't understand the logic of this value.
        // It is much louder on the real pin.
        int v = 1 << (pitchVolume >> 5);
        // If you flip the order of these they crash on the real pin with E030.
        pin->setAnalogValue(v);
        pin->setAnalogPeriodUs(1000000/frequency);
    }
    if (ms > 0) {
        fiber_sleep(ms);
        pin->setAnalogValue(0);
        fiber_sleep(5);
    }
}

/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
void MicroBitAudioService::onDataWritten( const microbit_ble_evt_write_t *params)
{
  
    if (params->handle == valueHandle( mbbs_cIdxDATA) && params->len >= 4)
    {
        analogPitch(440, 50);
        uint16_t freq = (*(params->data) & 0xff00) >> 16;
        uint16_t dur = 300;

        analogPitch(freq, dur);
        release_fiber();
    }
}


#endif