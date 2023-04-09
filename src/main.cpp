/* ************************************************
 *                   Includes
 * ************************************************/

#include "mbed.h"





int main() {


    // init led at 0
    led = 0;

    while (true) {

        // Invert led
        led = !led;

        // Wait for 1 second
        ThisThread::sleep_for(1s);
    }
}