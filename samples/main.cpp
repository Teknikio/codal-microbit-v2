#include "MicroBit.h"
#include "Tests.h"

MicroBit uBit;

const char * const happy_emoji ="\
    000,255,000,255,000\n\
    000,000,000,000,000\n\
    255,000,000,000,255\n\
    000,255,255,255,000\n\
    000,000,000,000,000\n";

const char * const wink_emoji ="\
    000,255,000,000,000\n\
    000,000,000,000,000\n\
    255,000,000,000,255\n\
    000,255,255,255,000\n\
    000,000,000,000,000\n";

MicroBitUARTService *uart;

// we use events abd the 'connected' variable to keep track of the status of the Bluetooth connection
void onConnected(MicroBitEvent)
{
    uBit.display.print("C");
}

void onDisconnected(MicroBitEvent)
{
    uBit.display.print("D");
}

void onDelim(MicroBitEvent)
{
    ManagedString r = uart->readUntil("\r\n");
    uart->send(r);
}

int
main()
{
    MicroBitImage smile(happy_emoji);
    MicroBitImage wink(wink_emoji);

    uBit.init();
    uBit.display.scroll("BLE ABDILMTU/P");

    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);
    
    uBit.messageBus.listen(MICROBIT_ID_BLE_UART, MICROBIT_UART_S_EVT_DELIM_MATCH, onDelim);

    new MicroBitAccelerometerService(*uBit.ble, uBit.accelerometer);
    new MicroBitButtonService(*uBit.ble);
    new MicroBitIOPinService(*uBit.ble, uBit.io);
    new MicroBitLEDService(*uBit.ble, uBit.display);
    new MicroBitMagnetometerService(*uBit.ble, uBit.compass);
    new MicroBitTemperatureService(*uBit.ble, uBit.thermometer);

    uart = new MicroBitUARTService(*uBit.ble, 32, 32);
    uart->eventOn("\r\n");

    while(1)
    {
        uBit.display.print(smile);
        uBit.sleep(1000);
        uBit.display.print(wink);
        uBit.sleep(500);
        uBit.display.print(smile);
        uBit.sleep(1000);
        uBit.display.clear();

        uBit.display.scroll("BLE ADMIN");
    }
}