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
- Speed:
    - 1 (Value used for state determination)
    - 16 bit int = Speed of first motor
    - 16 bit int = Speed of second motor

# To do:
- Self meassure deadband - pwm sensitivity of regulator
