#include <cstdio>
#include <iostream>

#include "orbit_simulator.h"

void initializeObject(OrbitSimulator &orb)
{
    /* start orbit object */
    orb.SetMinimumElevation(0.0);
    orb.SetCommsFreq(437e6);
    orb.SetGroundLocation(41, 2, 200);
    orb.SetTimestep(5);
    orb.SetSpaceTLEFile("mine_tle.txt");
}



/* Give me your next pass */
int main (int argc, char **argv)
{
    OrbitSimulator orb;
    if (argc != 2) {
        initializeObject(orb);
        orb.findNextPass(time(NULL));
    }else {
        initializeObject(orb);
        orb.findNextPass(atoi(argv[1]));
    }
    for (SimulatorResultsVec::iterator it = orb.Results.begin(); it != orb.Results.end(); it++) {
        std::cout << it->propagation.timestamp << ";";
        std::cout << it->propagation.az << ";";
        std::cout << it->propagation.el << ";";
        std::cout << it->propagation.ul_doppler << ";";
        std::cout << it->propagation.dl_doppler << std::flush << std::endl;
    }
}
