//
//  configuration.h
//  SwarmSimulator
//
//  Created by Mathieu Le Goc on 29/01/16.
//
//

#ifndef configuration_h
#define configuration_h


#define TYPE_ROBOT_POSITION                 0x06
#define TYPE_MOTORS_VELOCITY                0x05
#define TYPE_STATUS                         0x04


#define NB_ROBOTS                           20
#define NB_ROBOTS_PER_RECEIVER				10

#define coordinatesMinX                     70.0f
#define coordinatesMaxX                     953.0f
#define coordinatesMinY                     232.0f
#define coordinatesMaxY                     790.0f

#define dimensionX                          0.8128f
#define dimensionY                          0.508f

//wheel diameter ~11.8mm
#define robotDiameter                       0.026f              // diameter of robot
#define robotRadius                         (robotDiameter / 2.0f)
#define maxAccel                            0.0001f             //0.90f * 0.060f// max acceleration rate
#define maxSpeed                            (2.0f * PI * ( 1400.0f / 60.0f )* (0.0118/2.0f))//0.040f// Robot's max aheivable speed
#define wheelTrack                          0.020f              // Robot's wheeltrack
#define neighborDist                        (robotDiameter * 1.6f)    // max neighbor distance in ORCA algorithm
#define maxNeighbors                        9                  // max neighbors number in ORCA algorithm
#define goalRadius                          (0.75f*robotRadius)       //0.25 * radius

#define simulationFrequency                 30.0
#define timeStep                            (1.0f / simulationFrequency) // 0.050f// Simulation Time Step32
#define timeToOrientation                   3.5f*timeStep       //3.5f * timeStep


#endif /* configuration_h */
