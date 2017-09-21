#include <cstdio>
#include <iostream>

#include "orbit_simulator.h"

void initializeObject(OrbitSimulator &orb)
{
    /* start orbit object */
    orb.SetMinimumElevation(0.0);
    orb.SetCommsFreq(437e6);
    orb.SetGroundLocation(41, 2, 200);
    orb.SetTimestep(10);
    orb.SetSpaceTLEFile("mine_tle.txt");
}

/* Give me your next pass */
int main (int argc, char **argv)
{
    uint32_t timer;
    int amount_of_orbits = 200;
    OrbitSimulator *orb = new OrbitSimulator [amount_of_orbits];

    for (int i = 0; i < amount_of_orbits; i++) {
        initializeObject(orb[i]);
        if (i == 0) {
            orb[i].findNextPass(time(NULL));
        } else {
            orb[i].findNextPass(orb[i-1].getLastFoundPassEnd());
        }
        printf("ORBIT NUMBER: %d\n\n", i+1);
        for (SimulatorResultsVec::iterator it = orb[i].Results.begin(); it != orb[i].Results.end(); it++) {
            /* mirar si hace 0 crossing */
            /* si hace, sacarlo por pantalla */
            /* decidir cual es la orientacion de antenas */
            std::cout << it->propagation.timestamp << ";";
            std::cout << it->propagation.az << ";";
            std::cout << it->propagation.el << std::flush << std::endl;
        }
    }
}
