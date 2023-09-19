# Automated Pollen Instrument

As climate change increases pollen production and changes its dispersal, pollen deposition on lakes also increases. The goal of the automated pollen instrument is to monitor the changes in types of pollen on lakes and their concentration. 

Currently an in-lab prototype passes pollen under a microscope where photos are taken. This photobase will be used to train AI to recognize different pollen species.

# Prototype

The user inputs the desired cadence and direction of the step motor using the stepper.ino program, ran on an Arduino Mega 2560. This is relayed to a microstep motor controller, which controls the stepper motor of the linear actuator.

![alt text](https://github.com/ach12/upi/blob/main/pollen_analyzer_prototype.jpg?raw=true)


Pollen in the syringe is pushed into a flow chamber where photos are taken.
Pieces to hold the syringe and limit switches in place were modelled in Fusion360 and printed. 

![alt text](https://github.com/ach12/upi/blob/main/model_3D_pieces.jpg?raw=true)
