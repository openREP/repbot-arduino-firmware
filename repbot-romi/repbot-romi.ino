#include <Servo.h>
#include <Romi32U4.h>
#include <PololuRPiSlave.h>

struct Data
{
  bool yellow;
  bool red;
  bool green;

  bool buttonA;
  bool buttonB;
  bool buttonC;

  uint16_t batteryMillivolts;

  uint16_t analog[6];

  int16_t leftMotor;
  int16_t rightMotor;

  bool leftEncoderReset;
  bool rightEncoderReset;

  int16_t leftEncoder;
  int16_t rightEncoder;
};

PololuRPiSlave<struct Data,5> slave;
PololuBuzzer buzzer;
Romi32U4Motors motors;
Romi32U4ButtonA buttonA;
Romi32U4ButtonB buttonB;
Romi32U4ButtonC buttonC;
Romi32U4Encoders encoders;

void setup()
{
  // Set up the slave at I2C address 20.
  slave.init(20);

  // Play startup sound.
  buzzer.play("v10>>g16>>>c16");
}

void loop()
{
  // Call updateBuffer() before using the buffer, to get the latest
  // data including recent master writes.
  slave.updateBuffer();

  // Write various values into the data structure.
  slave.buffer.buttonA = buttonA.isPressed();
  slave.buffer.buttonB = buttonB.isPressed();
  slave.buffer.buttonC = buttonC.isPressed();

  // Change this to readBatteryMillivoltsLV() for the LV model.
  slave.buffer.batteryMillivolts = readBatteryMillivolts();

  for(uint8_t i=0; i<6; i++)
  {
    slave.buffer.analog[i] = analogRead(i);
  }

  // READING the buffer is allowed before or after finalizeWrites().
  ledYellow(slave.buffer.yellow);
  ledGreen(slave.buffer.green);
  ledRed(slave.buffer.red);
  motors.setSpeeds(slave.buffer.leftMotor, slave.buffer.rightMotor);

  if (slave.buffer.leftEncoderReset) {
    encoders.getCountsAndResetLeft();
    slave.buffer.leftEncoderReset = false;
  }

  if (slave.buffer.rightEncoderReset) {
    encoders.getCountsAndResetRight();
    slave.buffer.rightEncoderReset = false;
  }

  slave.buffer.leftEncoder = encoders.getCountsLeft();
  slave.buffer.rightEncoder = encoders.getCountsRight();

  // When you are done WRITING, call finalizeWrites() to make modified
  // data available to I2C master.
  slave.finalizeWrites();
}
