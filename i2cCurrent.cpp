#include "i2cCurrent.h"
#include <movingAvgFloat.h>

movingAvgFloat _currentAvg(15);

/*
 * Note:
 *   Uses the 'ArduinoINA219' library (not to be confused with AdafruitINA219)
 *   This 'ArduinoINA219' library seems to be slightly un-maintained.
 *   In order to compile, I needed to modify the_ina219.cpp references of
 *   _delay_ms() to simply delay().  This may be necessary only on the Due
 *   but not on the Teensy -- I'm unsure since I don't test/compile for Teensy
 *
*/


i2cCurrent::i2cCurrent() {
  _ina219 = INA219_WE(I2C_ADDRESS);
}

void i2cCurrent::begin() {
  Wire.begin();
  _currentAvg.begin();
  while(!_ina219.init()) {
    Serial.println("INA219 not connected!");
    delay(500);
  }

  /* Set ADC Mode for Bus and ShuntVoltage
  *   * Mode *          * Res / Samples *     * Conversion Time *
    BIT_MODE_9        9 Bit Resolution             84 µs
    BIT_MODE_10       10 Bit Resolution            148 µs  
    BIT_MODE_11       11 Bit Resolution            276 µs
    BIT_MODE_12       12 Bit Resolution            532 µs  (DEFAULT)
    SAMPLE_MODE_2     Mean Value 2 samples         1.06 ms
    SAMPLE_MODE_4     Mean Value 4 samples         2.13 ms
    SAMPLE_MODE_8     Mean Value 8 samples         4.26 ms
    SAMPLE_MODE_16    Mean Value 16 samples        8.51 ms     
    SAMPLE_MODE_32    Mean Value 32 samples        17.02 ms
    SAMPLE_MODE_64    Mean Value 64 samples        34.05 ms
    SAMPLE_MODE_128   Mean Value 128 samples       68.10 ms
  */
  // _ina219.setADCMode(SAMPLE_MODE_128); // choose mode and uncomment for change of default
  
  /* Set measure mode
    POWER_DOWN  - INA219 switched off
    TRIGGERED   - measurement on demand
    ADC_OFF     - Analog/Digital Converter switched off
    CONTINUOUS  - Continuous measurements (DEFAULT)
  */
  // _ina219.setMeasureMode(TRIGGERED); // choose mode and uncomment for change of default
  
 /* Set PGain
  * Gain *  * Shunt Voltage Range *         * Max Current *
    PG_40          40 mV               0.4 A * 0.1 / shuntSizeInOhms 
    PG_80          80 mV               0.8 A * 0.1 / shuntSizeInOhms 
    PG_160        160 mV               1.6 A * 0.1 / shuntSizeInOhms 
    PG_320        320 mV               3.2 A * 0.1 / shuntSizeInOhms (DEFAULT)
  */
  _ina219.setPGain(PG_320); // choose gain and uncomment for change of default
  
  /* Set Bus Voltage Range
    BRNG_16   -> 16 V
    BRNG_32   -> 32 V (DEFAULT)
  */
  // _ina219.setBusRange(BRNG_32); // choose range and uncomment for change of default

  /* If the current values delivered by the INA219 differ by a constant factor
     from values obtained with calibrated equipment you can define a correction factor.
     Correction factor = current delivered from calibrated equipment / current delivered by INA219
  */
  // _ina219.setCorrectionFactor(0.98); // insert your correction factor if necessary

  /* If you experience a shunt voltage offset, that means you detect a shunt voltage which is not 
     zero, although the current should be zero, you can apply a correction. For this, uncomment the 
     following function and apply the offset you have detected.   
  */
  // _ina219.setShuntVoltOffset_mV(0.5); // insert the shunt voltage (millivolts) you detect at zero current

  /* Set shunt size
     If you don't use a module with a shunt of 0.1 ohms (R100) you can change set the shunt size 
     here. 
  */
  _ina219.setShuntSizeInOhms(R_SHUNT); // Insert your shunt size in ohms
  
  Serial.println("INA219 Set Shunt Size"); 
}

void i2cCurrent::loop() {
  unsigned long _currentMillis = millis();  

  if ((unsigned long)(_currentMillis - _previousMillisReadData) >= _intervalReadData) {
    // Serial.print("Tick "); Serial.println(_currentMillis - _previousMillisReadData);
    _previousMillisReadData = millis();
    _readCurrent();
  }
}

void i2cCurrent::_readCurrent() {
  uint32_t _count = 0;
  unsigned long _newtime;

  _shuntvoltage = _ina219.getShuntVoltage_mV() / 1000;
  _busvoltage   = _ina219.getBusVoltage_V();

  _current_A    = _ina219.getCurrent_mA()      / 1000;
  _currentAvg.reading(_current_A);

  _power        = _ina219.getBusPower();
  _loadVoltage  = _busvoltage + (_shuntvoltage);
  _overflow     = _ina219.getOverflow();

  _newtime      = millis();
  _tick         = _newtime - _lastread;
  _Ah          += (_current_A * _tick)/3600000.0;
  _Wh          += (_power * _tick)/3600000.0;
  _lastread     = _newtime;

#ifdef DEBUG_SHUNT
  Serial.print("Shunt Voltage [mV]: "); Serial.println(_shuntvoltage);
  Serial.print("Bus Voltage [V]: "); Serial.println(_busvoltage);
  Serial.print("Load Voltage [V]: "); Serial.println(_loadVoltage);
  Serial.print("Current[mA]: "); Serial.println(_current_A);
  Serial.print("Bus Power [mW]: "); Serial.println(_power);
  if(!_overflow){
    Serial.println("Values OK - no overflow");
  }
  else{
    Serial.println("Overflow! Choose higher PGAIN");
  }
  Serial.println();
#endif

}

float i2cCurrent::getCurrent() {
  return _currentAvg.getAvg();
}
float i2cCurrent::getVoltage() {
  return _loadVoltage;
}
float i2cCurrent::getAh() {
  return _Ah;
}
float i2cCurrent::getWh() {
  return _Wh;
}

bool i2cCurrent::getOverflow() {
  return _overflow;
}
