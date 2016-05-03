#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <QtGlobal>

class Telemetry
{
public:
    Telemetry() :
        magneticYaw(0.0f)
        ,yaw(0.0f)
        ,gcsDistance(0.0f)
        ,airSpeed(0.0f)
        ,time(0)
        ,latitude(0.0f)
        ,longitude(0.0f)
        ,navigationMode(-1)
        ,direction(0.0f)
        ,convergenceSpeed(0.0f)
        ,convergenceRatio(0.0f)
        ,isConvergenceDataExist(false)
        ,packetId(0)
    {
    }
    float magneticYaw;
    float yaw;
    float gcsDistance;
    float airSpeed;
    qint64 time;
    float latitude;
    float longitude;
    int navigationMode;
    float direction;
    float convergenceSpeed;
    float convergenceRatio;
    bool isConvergenceDataExist;
    int packetId;

    Telemetry operator=(const Telemetry& right)
    {
        if (this == &right)
        {
            return *this;
        }

        magneticYaw = right.magneticYaw;
        yaw = right.yaw;
        gcsDistance = right.gcsDistance;
        airSpeed = right.airSpeed;
        time = right.time;
        latitude = right.latitude;
        longitude = right.longitude;
        navigationMode = right.navigationMode;
        direction = right.direction;
        convergenceSpeed = right.convergenceSpeed;
        convergenceRatio = right.convergenceRatio;
        isConvergenceDataExist = right.isConvergenceDataExist;
        packetId = right.packetId;
        return *this;
    }

};

#endif // TELEMETRY_H
