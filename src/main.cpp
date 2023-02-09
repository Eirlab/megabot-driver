// ============= INCLUDES =================

#include "mbed.h"

// ============= DEFINITIONS =================

DigitalOut led(LED1);
Thread led_thread;

#define MAIN_RATE 2s
#define MAIN_FLAG 0x01
Ticker mainTicker;
EventFlags mainFlag;
int counter = 0;

// ============= FUNCTION =================

// This is the main function for led thread.
// So this will be considered as thread for Mbed.
void led_thread_function() {

    // init led at 0
    led = 0;

    while (true) {

        // Invert led
        led = !led;

        // Wait for 1 second
        ThisThread::sleep_for(1s);
    }
}

void mainThreadUpdate() {
    mainFlag.set(MAIN_FLAG);
}

// This is the general main thread.
int main() {

    // Setup the led thread
    led_thread.start(led_thread_function);

    // Setup the ticker for main thread
    mainTicker.attach(&mainThreadUpdate, MAIN_RATE);

    while (true) {
        // Wait for flag update ...
        mainFlag.wait_any(MAIN_FLAG);
        counter += 2;

        // Send something though USB using Nucleo STLINK
        printf("Alive for %ds!\n", counter);

    }
}