//
//  configuration.h
//
//  Created by Mathieu Le Goc on 29/01/16.
//
//

#ifndef configuration_h
#define configuration_h




//wheel diameter ~11.8mm
#define robotDiameter                       0.026f                          // diameter of robot
#define robotRadius                         (robotDiameter / 2.0f)
#define maxAccel                            0.0001f                         //0.90f * 0.060f// max acceleration rate
#define maxSpeed                            0.70f                           //Robot's max aheivable speed
#define wheelTrack                          0.020f                          // Robot's wheeltrack
#define neighborDist                        (robotDiameter * 1.2f)          // max neighbor distance in ORCA algorithm
#define maxNeighbors                        9                               // max neighbors number in ORCA algorithm
#define goalRadius                          (0.5f*robotRadius)              //0.25 * radius
    

#define NUM_ZOOIDS_PER_RECEIVER             10

#define coordinatesMinX                     50.0f
#define coordinatesMaxX                     980.0f
#define coordinatesMinY                     220.0f
#define coordinatesMaxY                     800.0f
#define robotFieldMargin                    robotDiameter * 1.0f

#define SYSTEM_UPDATE_FREQUENCY		         60.0f
#define UPDATE_PERIOD						(1000.0f / SYSTEM_UPDATE_FREQUENCY)     // 0.050f// Simulation Time Step32
#define TIME_TO_ORIENTATION                  3.5f*(float)(UPDATE_PERIOD)/1000.0f       //3.5f * timeStep
#define ZOOID_WATCHDOG_TIMEOUT               1000

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
    
