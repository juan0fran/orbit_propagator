#include "link.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include <chrono>
#include <random>
#include <stdio.h>

Link::Link()
{
    _p_out_dbm = 0;
    _gain_db = 0;
    _losses_db = 0;
    _n_floor_dbm = 0;
    _rf_freq_hz = 0;
    _bw_hz = 0;
    _packet_size = 255 * 8;
}
Link::Link(double frequency)
{
    _p_out_dbm = 0;
    _gain_db = 0;
    _losses_db = 0;
    _n_floor_dbm = 0;
    _bw_hz = 0;
    _rf_freq_hz = frequency;
    _packet_size = 255 * 8;
}

/* this is not a class method */
double C(double n, double k)
{
    if (n == k || k == 0) {
        return 1; // There's exactly one way to select n or 0 objects out of n
    }
    return C(n - 1, k - 1) * n / k;
}

/* this is not a class method */
double RandomErrCode(int N, int T, double symb_err)
{
    double sum = 0.0;
    for (double i = T+1.0; i < N; i = i + 1.0) {
        sum += (double) (i * C(N,i) * pow(symb_err, i) * pow((1.0 - symb_err), N-i));
    }
    sum /= N;
    if (sum >= 0.5) {
        return 0.5;
    }else {
        return sum;
    }
}

/* this is not a class method */
double BurstErrCode(int N, int T, double ber)
{
    double sum = 0.0;
    for (int i = 1; i < T; i++) {
        sum += i*RandomErrCode(N, i, ber);
    }
    sum /= T;
    if (sum >= 0.5) {
        return 0.5;
    }else {
        return sum;
    }
}

void Link::setRandomPointingSigma(double sigma)
{
    _random_sigma = sigma;
}

double Link::computeRandomPointing()
{
    /* rand() */
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::normal_distribution<double> distribution(0.0, _random_sigma);
    double offset_angle = distribution(generator);
    double value = 12.0 * pow(offset_angle/_3db_beam, 2.0);
    return value;
}

double Link::computeRandomFading()
{
    /* rice(sigma, v) */
    itpp::Rice_RNG rice(_s4_sigma, _s4_v);
    double value = -10.0 * std::log10(rice());
    return value;
}

/* this is private */
double Link::computeSNR(double distance)
{
    double s;
    double path_loss = (20.0 * log10(distance) + 20.0 * log10(_rf_freq_hz) - 147.55);
    s = _p_out_dbm + _gain_db - ( _losses_db + path_loss + _atmospheric + computeRandomFading() + computeRandomPointing());
    _snr = (s - _n_floor_dbm);
    return _snr;
}

double Link::getRandomFrameErrorRate()
{
    /* probability of having 7 packets correct from 11 events */
    double per = getPacketErrorRate();
    if (per >= 0.63) {
        return 1;
    }else {
        return (RandomErrCode(11, 4, per));
    }
}

double Link::getBurstFrameErrorRate()
{
    /* probability of having 7 packets correct from 11 events */
    double per = getPacketErrorRate();
    if (per >= 0.63) {
        return 1;
    }else {
        return (BurstErrCode(11, 4, per));
    }
}

double Link::getPacketErrorRate()
{
    /* Frame Error Rate is computed as 1 - (1 - BER) ^ (packet_size * 8) */
    return (1.0 - pow((1.0 - computeCodedBER()), _packet_size));
}

double Link::computeCodedBER()
{
    /* coded solution -> we do have RS(255,223) into account */
    double ebno = pow(10.0, ((_snr+1.5)/10.0));
    return (0.5 * erfc(sqrt(0.76 * ebno)));
}

double Link::computeUncodedBER()
{
    /* coded solution -> we do not have RS(255,223) into account */
    double ebno = pow(10.0, (_snr/10.0));
    return (0.5 * erfc(sqrt(0.76 * ebno)));
}

double Link::computeCodedBER(double snr)
{
    /* coded solution -> we do have RS(255,223) into account */
    //double ebno = pow(10.0, ((snr+1.5)/10.0));
    double ebno = pow(10.0, (snr/10.0));
    double uncoded = (0.5 * erfc(sqrt(0.76 * ebno)));
    return BurstErrCode(255, 16, uncoded);
}

double Link::computeUncodedBER(double snr)
{
    /* coded solution -> we do not have RS(255,223) into account */
    double ebno = pow(10.0, (snr/10.0));
    return (0.5 * erfc(sqrt(0.76 * ebno)));
}


void Link::setPoutdBm(double pout)
{
    _p_out_dbm = pout;
}

void Link::setGains(double gs_gain, double sat_gain)
{
    _gain_db = gs_gain + sat_gain;
}

void Link::setLosses(double polarization, double atmospheric)
{
    _losses_db = polarization;
    _atmospheric = atmospheric;
}

void Link::setFadings(double s4)
{
    /*
        K = (1-S4^2)^0.5
        sigma is (1-(1-S4^2)^0.5) / 2
        v is ((1-S4^2)^0.5) / (1-(1-S4^2)^0.5)
    */
    double K = std::sqrt(1.0 - pow(s4, 2.0))/(1.0 - std::sqrt(1.0 - pow(s4, 2.0)));
    _s4_sigma = std::sqrt((1.0 - std::sqrt(1.0 - pow(s4, 2.0))));
    _s4_v = std::sqrt(_s4_sigma*_s4_sigma*2.0*K);
}

void Link::setAntennaBeamFromD(double directivity)
{
    double directivity_lin = pow(10.0, (directivity/10.0));
    _3db_beam = 41253.0 / (directivity_lin * 360.0);
}

void Link::setAntennaBeam(double dbbeam)
{
    _3db_beam = dbbeam;
}

void Link::setDistance(double distance)
{
    _distance = distance;
}

void Link::setNoiseTemperature(double antenna_temp, double system_temp, double signal_bandwidth)
{
    _n_floor_dbm = (10.0 * log10(1.38e-23 * (antenna_temp + system_temp) * signal_bandwidth)) + 30.0;
}

void Link::setFrequency(double central_freq)
{
    _rf_freq_hz = central_freq;
}

double Link::getSNR()
{
    return _snr;
}

void Link::fillStructure(LinkResults *link)
{
    link->snr = computeSNR(_distance);
    link->ber = computeCodedBER();
    link->per = getPacketErrorRate();
    link->rfer = getRandomFrameErrorRate();
    link->bfer = getBurstFrameErrorRate();
}
