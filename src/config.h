#pragma once


static const int HW_PWM_RESOLUTION=11; // MUST BE SETUP ONCE AT STARTUP
static const int HW_PWM_MAX_VALUE=(1<<HW_PWM_RESOLUTION)-1; // MAX VALUE ACCORDING TO RESOLUTION
static const int DRIVER_PWM_MAX_VALUE=HW_PWM_MAX_VALUE*0.98; // MAX VALUE ACCORDING TO RESOLUTION
static const float HW_PWM_FREQ=40000.0;

static const int NANO_BAUDRATE=115200;
static const int MASTER_BAUDRATE=2000000;

static const int pin_C1A_DIR1=10;
static const int pin_C1A_DIR2=11; 
static const int pin_C1A_PWM=3;

static const int pin_C1B_DIR1=8; 
static const int pin_C1B_DIR2=9; 
static const int pin_C1B_PWM=2;

static const int pin_C2A_DIR1=24; 
static const int pin_C2A_DIR2=25; 
static const int pin_C2A_PWM=5;

static const int pin_C2B_DIR1=12; 
static const int pin_C2B_DIR2=32; 
static const int pin_C2B_PWM=4;

static const int pin_C3A_DIR1=30; 
static const int pin_C3A_DIR2=31; 
static const int pin_C3A_PWM=7;

static const int pin_C3B_DIR1=26;
static const int pin_C3B_DIR2=27; 
static const int pin_C3B_PWM=6;