#ifndef ITELEMETRYVIEWER_H
#define ITELEMETRYVIEWER_H

#include "Telemetry.h"

class ITelemetryViewer
{
public:
    ~ITelemetryViewer() {}
    virtual void setEnabledFileLoading(bool enable) = 0;
    virtual void setEnabledPlayingTelenetry(bool enable) = 0;
    virtual void setEnabledStopPlayingTelemetry(bool enable) = 0;
    virtual void showTelemetry(const Telemetry& telemetry) = 0;
    virtual void showProgress(int progress) = 0;
    virtual void setMapLimits(double minLatitude, double maxLatitude,
                              double minLongitude, double maxLongitude) = 0;

//signals:
    virtual void fileSelected(const QString& filePath) = 0;
    virtual void needStartPlaying() = 0;
    virtual void needStopPlaying() = 0;
};

#endif // ITELEMETRYVIEWER_H
