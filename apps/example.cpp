#include <cstdio>
#include <iostream>

#include "orbit_simulator.h"
#include "link.h"
#include "data_simulator.h"

void printProgBar(int percent)
{
    std::string bar;
    for(int i = 0; i < 50; i++) {
        if(i < (percent/2)) {
            bar.replace(i,1,"=");
        }else if(i == (percent / 2)) {
            bar.replace(i,1,">");
        }else{
            bar.replace(i,1," ");
        }
    }
    std::cout<< "\r" "[" << bar << "] ";
    std::cout.width( 3 );
    std::cout<< percent << "%     " << std::flush;
}

void initializeObject(OrbitSimulator &orb)
{
    /* start link object */
    orb.setNoiseTemperature(750.0, 500.0, 12000.0);
    orb.setLosses(3.0, 0.5, 2.0);
    orb.setGains(12.8, 3.0);
    orb.setPoutdBm(27.0);

    /* start orbit object */
    orb.SetMinimumElevation(5.0);
    orb.SetCommsFreq(438e6);
    orb.SetGroundLocation(41, 2, 200);
    orb.SetSpaceTLEFile("mine_tle.txt");
    orb.SetTimestep(5);
}

void initializeObject(DataSimulator &data, unsigned int duration)
{
    data.setPassDuration(duration);  /* 360 second */
    data.setFrameInterval(11.5); /* 10 seconds between frames */
    data.setFrameSize(1750);
    data.setMaxDataDutyCycle(150);
    data.setScientifcDLTime(220);
    data.setScientificSize(18*1024); /* 18 kB of scientific data to be downloaded per pass */
    data.setTelemetryMessageSize(300); /* 300 bytes is the telemetry */
    data.addScientificRedundacy(5.0/8.0);
    data.addMinimumLogInfo(24*1024);
    data.calculatePassBudget();
}

int main (void)
{
    DataSimulator data;
    OrbitSimulator *orb = new OrbitSimulator [200];
    //std::cout << "Starting Simulation..." << std::endl;
    std::cout << "Pass Duration, Frame Interval, Frame Size, Payload DL Time, ";
    std::cout << "Total Pass Budget, SCI Block Size, Total Log Budget, Log Block Size" << std::endl;
    for (int i = 0; i < 200; i++) {
        //std::cout << "Orbit number: " << i+1 << std::endl;
        initializeObject(orb[i]);
        if (i == 0) {
            orb[i].findNextPass(time(NULL));
        }else {
            orb[i].findNextPass(orb[i-1].getLastFoundPassEnd());
        }
        //orb[i].ShowResults();
        initializeObject(data, (orb[i].getLastFoundPassEnd() - orb[i].getLastFoundPassStart()));
        data.plotCurrentBudget();
    }
}
