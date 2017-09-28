#include <cstdio>
#include <iostream>

#include "orbit_simulator.h"
#include "motor_angles.h"

void initializeObject(OrbitSimulator &orb, char *tle_filename)
{
    /* start orbit object */
    orb.SetMinimumElevation(0.0);
    orb.SetCommsFreq(437e6);
    orb.SetGroundLocation(41, 2, 200);
    orb.SetTimestep(10);
    orb.SetSpaceTLEFile(tle_filename);
}

/* Give me your next pass */
int main (int argc, char **argv)
{
    uint32_t timer;
    int amount_of_orbits = 200;

    if (argc != 2) {
      printf("Bad call sintax\n");
      exit(1);
    }else {
      FILE *fp;
      fp = fopen(argv[1], "r");
      if (fp != NULL) {
        fclose(fp);
      }else {
        printf("Bad filename\n");
        exit(1);
      }
    }

    OrbitSimulator *orb = new OrbitSimulator [amount_of_orbits];
    Motor_Angles *ang = new Motor_Angles;

    for (int i = 0; i < amount_of_orbits; i++) {
        initializeObject(orb[i], argv[1]);
        if (i == 0) {
            orb[i].findNextPass(time(NULL));
        } else {
            orb[i].findNextPass(orb[i-1].getLastFoundPassEnd());
        }

        printf("ORBIT NUMBER: %d\n\n", i+1);
        ang->computeMotorAngle(orb[i].Results);

        //for (SimulatorResultsVec::iterator it = orb[i].Results.begin(); it != orb[i].Results.end(); it++) {
            /* mirar si hace 0 crossing */
            /* si hace, sacarlo por pantalla */
            /* decidir cual es la orientacion de antenas */
            /*std::cout << it->propagation.timestamp << ";";
            std::cout << it->propagation.az << ";";
            std::cout << it->propagation.el << std::flush << std::endl;*/
        //}
    }
}
