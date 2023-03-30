// ============= INCLUDES =================

#include "mbed.h"

// ============= DEFINITIONS =================

DigitalOut led(LED1);
Thread led_thread;

// MAIN LOOP THREAD
#define MAIN_RATE 2s
#define MAIN_FLAG 0x01
Ticker mainTicker;
EventFlags mainFlag;
int counter = 0;

// BASE CONTROL UPDATE THREAD
#define MAX_SPEED 1.0f // Max speed on pokirobot seems to be 1m/s with no limit on PWM output
#define CONTROL_THREAD_RATE 10ms
#define CONTROL_THREAD_FLAG 0x02
Ticker controlThreadTicker;
EventFlags controlThreadFlag;
Thread controlThread(osPriorityRealtime, OS_STACK_SIZE * 4);
static float dt_pid = 0.0f, hz_pid = 0.0f;
float robot_target_X = 0.0f, robot_target_Y = 0.0f, robot_target_theta = 0.0f;

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

// This will just update the control loop flag
void controlThreadUpdate() {
    controlThreadFlag.set(CONTROL_THREAD_FLAG);
}

// This is the control Thread. Anything for robot control should be here (motors update, robot X/Y/Theta update ...).
void control() {

    // Convert current rate of the loop in seconds (float)
    auto f_secs = std::chrono::duration_cast<std::chrono::duration<float>>(CONTROL_THREAD_RATE);
    dt_pid = f_secs.count();
    hz_pid = 1.0f / dt_pid; // Very important for all PIDs
    float time_passed = 0.0f;

    // create and init encoders object ...

    // Create and init odometry ...

    // Create and init motors ...


    while (true) {
        // Wait for asserv tick, this will ensure to do the loop at a specific frequency (hz_pid)
        controlThreadFlag.wait_any(CONTROL_THREAD_FLAG);


        // Update Odometry ...

        // Update PIDs with target X/Y/Theta and actual odometry X/Y/Theta...

        // Update motors ...


        // Update time passed in control loop
        time_passed += dt_pid;
    }

}

// This will just update the main loop flag
void mainThreadUpdate() {
    mainFlag.set(MAIN_FLAG);
}

// This is the general main thread.
int main() {

    // Setup the led thread
    led_thread.start(led_thread_function);

    // Setup the ticker for main thread
    mainTicker.attach(&mainThreadUpdate, MAIN_RATE);

    // Setup robot control update ("Asservissement")
    controlThread.start(control);
    controlThreadTicker.attach(&controlThreadUpdate, CONTROL_THREAD_RATE);

    while (true) {
        // Wait for flag update ...
        mainFlag.wait_any(MAIN_FLAG);
        counter += 2;

        // Send something though USB using Nucleo STLINK
        printf("Alive for %ds!\n", counter);

    }
}