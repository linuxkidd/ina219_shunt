#ifndef i2cCurrent_h
#define i2cCurrent_h

#include <Wire.h>
#include <INA219_WE.h>

// #define DEBUG_SHUNT

#define I2C_ADDRESS 0x40
// Setup ina219 Current measuring shunt:

#define R_SHUNT 0.0001
// #define R_SHUNT 0.1



class i2cCurrent
{
  public:
    i2cCurrent();
    void begin();
    void loop();
    float getCurrent();
    float getVoltage();
    float getAh();
    float getWh();
    bool  getOverflow();
  
  private:
    // current and voltage readings
    INA219_WE _ina219;
    void  _readCurrent();
    float _shuntvoltage = 0.0;
    float _busvoltage = 0.0;
    float _loadVoltage = 0.0;
    float _current_A = 0.0;
    float _power = 0.0;
    bool  _overflow = false;

    float _Ah = 0.0;
    float _Wh = 0.0;
    unsigned long _lastread = 0; // used to calculate Ah, Wh
    unsigned long _tick;         // current read time - last read
    unsigned long _currentMillis;
  
    // different intervals for each Task
    int _intervalReadData = 100;
  
    // last Task call
    unsigned long _previousMillisReadData = 0;

};

#endif
