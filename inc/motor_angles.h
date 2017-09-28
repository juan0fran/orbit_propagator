#ifndef __MOTOR_ANGLES_HPP__
#define __MOTOR_ANGLES_HPP__

#include <vector>
#include "orbit_simulator.h"

typedef struct NewAngles{
  propagation_output_t propagation;
  double motor_az;
  double motor_el;
}NewAngles;

typedef std::vector<NewAngles> NewAnglesVec;

class Motor_Angles {
    public:
        Motor_Angles();
        void computeMotorAngle(SimulatorResultsVec Results);
        void clear();

    private:
        NewAnglesVec _Angles;

};

#endif
