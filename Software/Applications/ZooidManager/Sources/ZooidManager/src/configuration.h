//
//  configuration.h
//
//  Created by Mathieu Le Goc on 29/01/16.
//
//

#ifndef configuration_h
#define configuration_h


#define NB_ROBOTS                           20
#define NB_ROBOTS_PER_RECEIVER              20

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
#define maxSpeed                            0.70f //Robot's max aheivable speed
#define wheelTrack                          0.020f              // Robot's wheeltrack
#define neighborDist                        (robotDiameter * 1.2f)    // max neighbor distance in ORCA algorithm
#define maxNeighbors                        9                  // max neighbors number in ORCA algorithm
#define goalRadius                          (0.4f*robotRadius)       //0.25 * radius

#define updateFrequency				        30.0f
#define updateTimestep						(1000.0f / updateFrequency) // 0.050f// Simulation Time Step32
#define timeToOrientation                   3.5f*(float)(updateTimestep)/1000.0f       //3.5f * timeStep


enum SimulationMode {
    Off,
    On,
    NoPlanning
};

enum AssignmentMode{
    OptimalAssignment = 1,
    NaiveAssignment = 0
};

typedef struct {
    uint8_t assignmentMode;
    unsigned int nbRobots;
}UdpHeader;


#endif /* configuration_h */
    
