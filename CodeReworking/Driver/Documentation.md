\mainpage Documentation for Motor driver of OmniWheel robot

\dotfile project_hierarchy.gv "Pic.1 - Files hierarchy of the project"

# Brief
This driver controls 2 motors, read data from their encoders and communicate with the main controler.

# Comunication
## Recieve message format
Format in which Controler sends data through I2C to this driver - sent from up to bottom.

### Mode:
- Stop:
    - 0 (Value used for state determination)
- Wait:
    - 1 (Value used for state determination)
- SpeedPWM:
    - 2 (Value used for state determination)
    - 16 bit int (normal int) = Speed of first motor in PWM (0 - 255)
    - 16 bit int (normal int) = Speed of second motor in PWM (0 - 255)
- SpeedReal:
    - 3 (Value used for state determination)
    - float = Speed of first motor in rad/s
    - float = Speed of second motor in rad/s
# To do:
- Self meassure deadband - pwm sensitivity of regulator

# Be aware of:
- When arduino is reading Serial port from computer (computer is sending to Arduino) set Line end settings to "No line ending" or "Line feed" (don't set it to "Carriage return" - will read blank or "Both CR & LF" - will freeze)
- 