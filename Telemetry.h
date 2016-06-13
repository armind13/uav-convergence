#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <QtGlobal>
#include <QPair>

typedef QPair<double, double> MinMaxValues;

class Telemetry
{
public:
    Telemetry() :
        magneticYaw(0.0)
        ,yaw(0.0)
        ,gcsDistance(0.0)
        ,airSpeed(0.0)
        ,time(0)
        ,latitude(0.0)
        ,longitude(0.0)
        ,navigationMode(-1)
        ,direction(0.0)
        ,latitudePlain(0.0)
        ,longitudePlain(0.0)
        ,convergenceSpeed(0.0)
        ,convergenceRatio(0.0)
        ,isConvergenceDataExist(false)
        ,packetId(-1)
    {
    }
    double magneticYaw;
    double yaw;
    double gcsDistance;
    double airSpeed;
    qint64 time;
    double latitude;
    double longitude;
    int navigationMode;
    double direction;
    double latitudePlain;
    double longitudePlain;
    double convergenceSpeed;
    double convergenceRatio;
    bool isConvergenceDataExist;
    int packetId;
};

#endif // TELEMETRY_H
