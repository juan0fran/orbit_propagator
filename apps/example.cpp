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
    orb.setNoiseTemperature(5000.0, 750.0, 12000.0);
    orb.setLosses(0.0, 1.0);
    orb.setGains(15.3, 1.76);
    orb.setPoutdBm(30.0);
    orb.setFadings(0.3);
//    orb.setAntennaBeamFromD(1.76);
    orb.setAntennaBeam(135);
    orb.setRandomPointingSigma(45);
    /* start orbit object */
    orb.SetMinimumElevation(10.0);
    orb.SetCommsFreq(437e6);
    orb.SetGroundLocation(41, 2, 200);
    orb.SetSpaceTLEFile("funcube_tle.txt");
    orb.SetTimestep(10);
}

void initializeObject(DataSimulator &data, unsigned int duration, unsigned int interval, unsigned int size)
{
    data.setPassDuration(duration);  /* 360 second */
    data.setFrameInterval(interval); /* 10 seconds between frames */
    data.setFrameSize(size);
    data.setMaxDataDutyCycle(97);
    data.setScientifcDLTime(225);
    data.setScientificSize(0*1024); /* 18 kB of scientific data to be downloaded per pass */
    data.setTelemetryMessageSize(300); /* 300 bytes is the telemetry */
    //data.addScientificRedundacy(5.0/8.0);
    data.addMinimumLogInfo(20*1024);
    data.calculatePassBudget();
}

int main (void)
{
    //std::ofstream ofs("output_file.csv");
    std::string results;
    unsigned int size, interval;
    DataSimulator data;
    int amount_of_orbits = 200;
    OrbitSimulator *orb = new OrbitSimulator [amount_of_orbits];
    size = 876;
    interval = 20;
    //ofs << "Timestamp;Elevation;Distance;SNR;BER" << std::flush << std::endl;
    for (int i = 0; i < amount_of_orbits; i++) {
        //std::cout << "Orbit number: " << i+1 << std::endl;
        initializeObject(orb[i]);
        if (i == 0) {
            orb[i].findNextPass(time(NULL));
        } else {
            orb[i].findNextPass(orb[i-1].getLastFoundPassEnd());
        }
        results = orb[i].GetResults();
        std::cout << results;
        //ofs << results;
        //if ( (((orb[i].getLastFoundPassEnd() - orb[i].getLastFoundPassStart()) * orb[i].getPassAvailability()))/100 >= 180) {
            //std::cout << (orb[i].getLastFoundPassStart()) << "; ";
            //std::cout << (orb[i].getLastFoundPassEnd()) << "; ";
            //std::cout << orb[i].getPassAvailability() << std::endl;
        //}
        /*
        if ( (((orb[i].getLastFoundPassEnd() - orb[i].getLastFoundPassStart()) * orb[i].getPassAvailability()))/100 >= 360) {
            initializeObject(data,
                ((((orb[i].getLastFoundPassEnd() - orb[i].getLastFoundPassStart()) * orb[i].getPassAvailability()))/100),
                interval, size);
            data.plotCurrentBudget();
            if (data.getFrameSize() < size) {
                size = data.getFrameSize();
            }
            if (data.getFrameInterval() < interval) {
                interval = data.getFrameInterval();
            }
        }
        */
        //printProgBar(((i+1)*100) / amount_of_orbits);
    }
    //ofs.close();
    //std::cout << std::endl;

}
