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
#include "MicroBit.h"
#include "StreamNormalizer.h"
#include "LevelDetector.h"
#include "LevelDetectorSPL.h"

static NRF52ADCChannel *mic = NULL;
static StreamNormalizer *processor = NULL;
static LevelDetectorSPL *levelSPL = NULL;
static int claps = 0;
static volatile int sample;
extern  MicroBit uBit;

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitMicrophoneService.h"

const uint16_t MicroBitMicrophoneService::serviceUUID               = 0x6200;
const uint16_t MicroBitMicrophoneService::charUUID[ mbbs_cIdxCOUNT] = { 0x9350 };


/**
  * Constructor.
  * Create a representation of the MicrophoneService
  * @param _ble The instance of a BLE device that we're running on.
  */
MicroBitMicrophoneService::MicroBitMicrophoneService( BLEDevice &_ble ) 
{
    this->level = 0;
    this->sigma = 0;
    this->windowPosition = 0;
    this->windowSize = LEVEL_DETECTOR_DEFAULT_WINDOW_SIZE;
    // Initialise our characteristic values.
    microphoneDataCharacteristicBuffer   = 0;
    
    // Register the base UUID and create the service.
    RegisterBaseUUID( bs_base_uuid);
    CreateService( serviceUUID);

    // Create the data structures that represent each of our characteristics in Soft Device.
    CreateCharacteristic( mbbs_cIdxDATA, charUUID[ mbbs_cIdxDATA],
                         (uint8_t *)&microphoneDataCharacteristicBuffer,
                         sizeof(microphoneDataCharacteristicBuffer), sizeof(microphoneDataCharacteristicBuffer),
                         microbit_propREAD | microbit_propNOTIFY);

    if (processor == NULL)
        processor = new StreamNormalizer(mic->output, 0.05f, true, DATASTREAM_FORMAT_8BIT_SIGNED);
        
        processor->output.connect(*this);

    if ( getConnected())
        listen( true);
}

/**
 * Callback provided when data is ready.
 */
int MicroBitMicrophoneService::pullRequest()
{
    ManagedBuffer b = processor->output.pull();
    int16_t *data = (int16_t *) &b[0];

    int samples = b.length() / 2;

    for (int i=0; i < samples; i++)
    {
        sigma += abs(*data);
        windowPosition++;

        if (windowPosition == windowSize)
        {
            level = sigma / windowSize;
            sigma = 0;
            windowPosition = 0;
        }

        data++;
    }

    notifyChrValue( mbbs_cIdxDATA, (uint8_t *)&level, 1);
    return DEVICE_OK;
}

/**
  * Set up or tear down event listers
  */
void MicroBitMicrophoneService::listen( bool yes)
{
    if ( yes)
    {
        if (mic == NULL){
            mic = uBit.adc.getChannel(uBit.io.microphone);
            mic->setGain(7,0);          // Uncomment for v1.47.2
            //mic->setGain(7,1);        // Uncomment for v1.46.2
        }
        uBit.io.runmic.setDigitalValue(1);
        uBit.io.runmic.setHighDrive(true);
    }
    else
    {
        uBit.io.runmic.setDigitalValue(0);
        uBit.io.runmic.setHighDrive(false);
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
