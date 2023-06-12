\mainpage Documentation for Motor driver of OmniWheel robot



# Brief
This driver controls 2 motors, read data from their encoders and communicate with the main controler.

# Comunication
## Recieve message format
Format in which Controler sends data through I2C to this driver - sent from up to bottom.

### Mode:
- Stop: (Stops motors)
    - 0 (Value used for state determination)
- Wait: (Idle)
    - 1 (Value used for state determination)
- SpeedPWM: (Set required PWM spd to motors)
    - 2 (Value used for state determination)
    - 16 bit int (normal int) = Speed of first motor in PWM (0 - 255)
    - 16 bit int (normal int) = Speed of second motor in PWM (0 - 255)
- SpeedReal: (Set required spd [ENC] to regulator - to calculate regulator action)
    - 3 (Value used for state determination)
    - int = Speed of first motor [ENC]
    - int = Speed of second motor [ENC]
- ChangeConstPID: (Change PID constants)
    - 4 (Value used for state determination)
    - float = value to increment Kp
    - float = value to increment Ki
    - float = value to increment Kd
- CalibDeadBand: (Turn on deadband calibration)
    - 5 (Value used for state determination)
- Meas: (Turn on meassurement)
    - 10 (Value used for state determination)
    - int = which motor (0/1)
    - int = which meassurement


# To do:
## Currently
- Add docummentation to state machines (Serial, Meas)
## In the future
- Self meassure deadband - pwm sensitivity of regulator - Almost done - use CalibDeadBand

# Be aware of:
- When arduino is reading Serial port from computer (computer is sending to Arduino) set Line end settings to "No line ending" or "Line feed" (don't set it to "Carriage return" - will read blank or "Both CR & LF" - will freeze)
- 