#ifndef __LINK_HPP__
#define __LINK_HPP__

#include <itpp/comm/channel.h>
#include <itpp/base/vec.h>
#include <itpp/itbase.h>

typedef struct LinkResults{
    double snr;
    double ber;
    double per;
    double rfer;
    double bfer;
}LinkResults;

class Link {
    public:
        Link();
        Link(double frequency);

        double getBurstFrameErrorRate();
        double getRandomFrameErrorRate();
        double getPacketErrorRate();
        double getSNR();

        void setPoutdBm(double pout);
        void setGains(double gs_gain, double sat_gain);
        void setLosses(double polarization, double atmospheric);
        void setNoiseTemperature(double antenna_temp, double system_temp, double signal_bandwidth);
        void setFrequency(double central_freq);
        void setDistance(double distance);
        void setFadings(double s4);
        void setAntennaBeamFromD(double directivity);
        void setAntennaBeam(double dbbeam);
        void setRandomPointingSigma(double sigma);
        //setAttitude(); TODO Implement attitude based link budget
        double computeCodedBER();
        double computeUncodedBER();
        double computeSNR(double distance);

        double computeRandomFading();
        double computeRandomPointing();

        double computeCodedBER(double snr);
        double computeUncodedBER(double snr);

        void fillStructure(LinkResults *link);

    private:
        double  _p_out_dbm;
        double  _gain_db;
        double  _losses_db;
        double  _atmospheric;
        double  _pointing;
        double  _3db_beam;
        double  _random_sigma;
        double  _s4_sigma;
        double  _s4_v;
        double  _distance;
        double  _snr;
        double  _n_floor_dbm;
        double  _rf_freq_hz;
        double  _bw_hz;
        int     _packet_size;

};

#endif
