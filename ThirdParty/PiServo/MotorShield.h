/******************************************************************

 It will only work with http://www.7gp.cn
 
 ******************************************************************/

#ifndef _Emakefun_MotorShield_h_
#define _Emakefun_MotorShield_h_

#include "MotorDriver.h"

//#define MOTORDEBUG

#define MICROSTEPS 16         // 8 or 16

#define MOTOR1_A 2
#define MOTOR1_B 3
#define MOTOR2_A 1
#define MOTOR2_B 4
#define MOTOR4_A 0
#define MOTOR4_B 6
#define MOTOR3_A 5
#define MOTOR3_B 7

#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3
#define RELEASE 4

#define SINGLE 1
#define DOUBLE 2
#define INTERLEAVE 3
#define MICROSTEP 4

class MotorShield;

class DCMotor {
public:
    DCMotor();

    friend class Emakefun_MotorShield;

    void run(uint8_t);

    void setSpeed(uint8_t);

    uint8_t _speed, IN1pin, IN2pin, MDIR{};
    MotorShield *MC;
    uint8_t motornum;
};

class StepperMotor {
public:
    StepperMotor();

    friend class Emakefun_MotorShield;

    void step(uint16_t steps, uint8_t dir, uint8_t style = SINGLE);

    void setSpeed(uint16_t speed);

    uint8_t oneStep(uint8_t dir, uint8_t style);

    void release();

    uint32_t usperstep{};

    uint8_t PWMApin{}, AIN1pin{}, AIN2pin{};
    uint8_t PWMBpin{}, BIN1pin{}, BIN2pin{};
    uint16_t revsteps; // # steps per revolution
    uint8_t currentstep;
    MotorShield *MC{};
    uint8_t steppernum;
};

class Servo {
public:
    Servo();

    friend class Emakefun_MotorShield;

    void setServoPulse(double pulse);

    void writeServo(uint8_t angle);

    uint8_t readDegrees();

    uint8_t PWMpin;
    uint16_t PWMfreq{};
    MotorShield *MC;
    uint8_t servonum, currentAngle;
};

class MotorShield {
public:
    explicit MotorShield(uint8_t addr = 0x60);

    friend class Emakefun_DCMotor;

    void begin(uint16_t freq = 50);

    void setPWM(uint8_t pin, uint16_t val);

    void setPin(uint8_t pin, uint8_t val);

    DCMotor *getMotor(uint8_t n);

    StepperMotor *getStepper(uint16_t steps, uint8_t n);

    Servo *getServo(uint8_t n);

private:
    uint8_t _addr;
    uint16_t _freq{};
    Servo servos[8];
    DCMotor dcmotors[4];
    StepperMotor steppers[2];
    MotorDriver _pwm;
};

#endif
