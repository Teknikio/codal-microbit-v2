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

#ifndef MICROBIT_MICROPHONE_SERVICE_H
#define MICROBIT_MICROPHONE_SERVICE_H

#include "MicroBitConfig.h"
#include "MicroBit.h"
#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitBLEManager.h"
#include "MicroBitBLEService.h"
#include "EventModel.h"

/**
  * Class definition for the custom MicroBit Microphone Service.
  * Provides a BLE service to remotely monitor mic input
  */
class MicroBitMicrophoneService : public MicroBitBLEService, public DataSink
{
    public:

    /**
      * Constructor.
      * Create a representation of the MicophoneService
      * @param _thermometer An instance of MicroBitThermometer to use as our temperature source.
      */
    MicroBitMicrophoneService( BLEDevice &_ble);
    int             windowSize;         // The number of samples the make up a level detection window.
    int             windowPosition;     // The number of samples used so far in the calculation of a window.
    int             level;              // The current, instantaneous level.
    int             sigma;              // Running total of the samples in the current window.

    /**
     * Callback provided when data is ready.
     */
    virtual int pullRequest();

    private:

    /**
      * Set up or tear down event listers
      */
    void listen( bool yes);

    /**
      * Invoked when BLE connects.
      */
    void onConnect( const microbit_ble_evt_t *p_ble_evt);

    /**
      * Invoked when BLE disconnects.
      */
    void onDisconnect( const microbit_ble_evt_t *p_ble_evt);

    /**
      * Callback. Invoked when any of our attributes are written via BLE.
      */
    void onDataWritten(const microbit_ble_evt_write_t *params);

    /**
     * Temperature update callback
     */
    void microphoneUpdate(MicroBitEvent e);

    // memory for our mic data characteristic.
    int8_t             microphoneDataCharacteristicBuffer;
    
    // Index for each charactersitic in arrays of handles and UUIDs
    typedef enum mbbs_cIdx
    {
        mbbs_cIdxDATA,
        mbbs_cIdxCOUNT
    } mbbs_cIdx;
    
    // UUIDs for our service and characteristics
    static const uint16_t serviceUUID;
    static const uint16_t charUUID[ mbbs_cIdxCOUNT];
    
    // Data for each characteristic when they are held by Soft Device.
    MicroBitBLEChar      chars[ mbbs_cIdxCOUNT];

    public:
    
    int              characteristicCount()          { return mbbs_cIdxCOUNT; };
    MicroBitBLEChar *characteristicPtr( int idx)    { return &chars[ idx]; };
};


#endif
#endif