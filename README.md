# EC244-Design-Project
Introduction
-
The purpose of the lab is to design, build, and test a stepper motor control circuit. In addition to enhancing my design skills, I also learned electronics prototyping. The circuit must use an issued Arduino NANO to control the stepper motor via a dedicated motor controller breakout board. For the initial design portion, a DC power supply will be used, in future expansion, a smaller power supply circuit will be designed.

The basic functions will be performed:
- Clockwise rotation (quarter, half, and one full turn)
- Clockwise rotation (continuous operation)
- Counter-clockwise rotation (quarter, half, and one full turn)
- Counter-clockwise rotation (continuous operation)
- Be able to use a potentiometer to manually control the motor to rotate in both directions. The potentiometer should also      control the speed of the motor

Equipment
-
- Arduino Nano 33 BLE Sense Rev2
- TMC2208 Breakout Board (BOB) Motor Controller IC
- Adafruit Stepper Motor NEMA17 12V 350mA
- 1kΩ Potentiometer

Procedure
-
1) Design the stepper motor control circuit
2) Implement the circuit with basic control to test basic use
3) Implement the circuit with the code for the arduino

Results
-
All functions were performed well. When doing the initial programming, the stepper motor would overheat because there was no kill switch that turned off current to the motors when not in use. So I added a kill switch after 20 seconds so that the motors wouldn't overheat from constant high current.

Conclusion
-
The objective of the lab was met and a circuit was designed, built, and tested that highlighted the capability of a stepper motor. 
