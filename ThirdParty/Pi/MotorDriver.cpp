#include "MotorDriver.h"


MotorDriver::MotorDriver(uint8_t addr) : RaspiI2C() {
    _i2caddr = addr;
}

void MotorDriver::begin() {
    RaspiI2C::init(_i2caddr);
    this->fd = RaspiI2C::fd;
    reset();
}


void MotorDriver::reset() {
    write8(PCA9685_MODE1, 0x00);
}

void MotorDriver::setPWMFreq(float freq) {
    //printf("Attempting to set freq :%lf \n",freq);

    freq *= 0.9;  // Correct for overshoot in the frequency setting (see issue #11).

    float prescaleval = 25000000.f;
    prescaleval /= 4096;
    prescaleval /= freq;
    prescaleval -= 1;
    uint8_t prescale = floor(prescaleval + 0.5);

    uint8_t oldmode = read8(PCA9685_MODE1);
    uint8_t newmode = (oldmode & 0x7F) | 0x10; // sleep
    write8(PCA9685_MODE1, newmode); // go to sleep
    write8(PCA9685_PRESCALE, prescale); // set the prescaler
    write8(PCA9685_MODE1, oldmode);
    delay(5);
    write8(PCA9685_MODE1, oldmode | 0xa1);  //  This sets the MODE1 register to turn on auto increment.
    // This is why the beginTransmission below was not working.
    //  Serial.print("Mode now 0x"); Serial.println(read8(PCA9685_MODE1), HEX);
}

void MotorDriver::setPWM(uint8_t num, uint16_t on, uint16_t off) {
    //Serial.print("Setting PWM "); Serial.print(num); Serial.print(": "); Serial.print(on); Serial.print("->"); Serial.println(off);

    write8(LED0_ON_L + 4 * num, on & 0xFF);
    write8(LED0_ON_H + 4 * num, on >> 8);
    write8(LED0_OFF_L + 4 * num, off & 0xFF);
    write8(LED0_OFF_H + 4 * num, off >> 8);
}

uint8_t MotorDriver::read8(uint8_t addr) {
    return ReadReg8(addr);
}

void MotorDriver::write8(uint8_t addr, uint8_t d) {
    WriteReg8(addr, d);
}
