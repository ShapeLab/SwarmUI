# Zooids: Building Blocks for Swarm User Interfaces \\  Extended Material
![Teaser](/Images/Teaser3.png)
This paper introduces swarm user interfaces, a new class of human-computer interfaces comprised of many autonomous
robots that handle both display and interaction. We describe the design of Zooids, an open-source open-hardware platform
for developing tabletop swarm interfaces. The platform consists of a collection of custom-designed wheeled micro
robots each 2.6 cm in diameter, a radio base-station, a highspeed DLP structured light projector for optical tracking, and
a software framework for application development and control. We illustrate the potential of tabletop swarm user interfaces
through a set of application scenarios developed with Zooids, and discuss general design considerations unique to swarm user interfaces.

[![IMAGE ALT TEXT](http://img.youtube.com/vi/ZVdAfDMP3m0/0.jpg)](http://www.youtube.com/watch?v=ZVdAfDMP3m0 "Zooids: Building Blocks for Swarm User Interfaces")


## Hardware
<p align="center">
<img src="Images/exploded.PNG" alt="exploded" width="400">
</p>

Zooids are small custom-made robots as shown above; their dimensions are 26 mm in diameter, 21 mm in height
and they weight about 12 g. Each robot is powered by a 100 mAh LiPo battery and uses motor driven wheels. It contains a flexible elctrode for capacitive touch sensing. It communicates with the main computer through a NRF24L01+ chip. 

### Tracking
Our system uses a projector-based tracking system for robot position tracking as shown below. Using a high frame rate (3000Hz) projector (DLP LightCrafter) from Texas Instruments Inc., a sequence of gray-coded patterns are projected onto a flat surface. Then, the photodiodes on the robot independently decodes into a location within the projected area. 
The instruction for setting up this projector-based tracking system is included in the repository. 

## Software
<p align="center">
<img src="Images/architecture.PNG" alt="architecture" width="700">
</p>

The communication structure consists of four main layers from highest to lowest level: Application, Simulation, Server, and Hardware. 

At the application level, the desired positions of the robots are computed. These desired positions are transmitted to
the simulation layer through a network socket. The application programmer can choose between two control strategies:
Proportional-Integral-Derivative (PID) position control or Hybrid Reciprocal Velocity Obstacles (HRVO) combined
with PID (these options are explained in the next paragraphs). Based on the chosen control strategy, the simulation layer
computes the goal positions of the robots, either final positions for PID or intermediate points for HRVO, and sends them to
the server. Finally, the server layer dispatches commands to the individual zooids, while at the same time monitoring their status and position.

Each robot independently controls its motion through a PID controller based on the state machine shown below. Given a final goal, the robot initially turns itself in the right direction and, once aligned, accelerates to its user-defined preferred speed. When it reaches the speed, it maintains it with a PID control on the orientation to ensure its direction towards the final goal. When a new incremental goal is given, it will still move at same speed but the PID control on orientation will direct the robot towards the new intermediate goal. When the robot arrives within 5 cm of the final goal, it slows down to its minimum velocity and once within 1 cm
of the final goal, it stops and orients itself as commanded by the application programmer. To enable smooth transitions between the incremental goal positions, robots are given their next position at 60 Hz.

<p align="center">
<img src="Images/local_control.PNG" alt="control" width="700">
</p>

## Acknowledgments 

This is a joint work between the Shape Lab at Stanford University (USA) and the Aviz team at Inria (France).
It was partially funded by the Région Ile de France, DIM ISC-PIF. We would also like to thank Alexa Siu, Shenli Yuan, Ernesto Ramirez and Pham Minh Hieu for investing so much time and efforts in making this work possible. 

<p align="center">
<img src="Images/logos.png" alt="logos" width="700">
</p>

<a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/">Creative Commons Attribution-ShareAlike 4.0 International License</a>.
