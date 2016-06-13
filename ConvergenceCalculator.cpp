#include "ConvergenceCalculator.h"

ConvergenceCalculator::ConvergenceCalculator() :
    minCount(100)
    ,preBufferCount(23)
    ,telemetries()
    ,prebuffer()
{
}

void ConvergenceCalculator::add(const Telemetry& telemetry)
{
    if (prebuffer.count() == preBufferCount)
    {
        const int middleIndex = preBufferCount / 2;
        Telemetry result = prebuffer[middleIndex];

        QList<double> distances;
        for (auto tel : prebuffer)
        {
            distances.push_back(tel.gcsDistance);
        }
        qSort(distances);

        result.gcsDistance = distances[middleIndex];
        telemetries.push_back(result);
        if (telemetries.count() > minCount)
        {
            telemetries.pop_front();
        }
        prebuffer.pop_front();
    }
    prebuffer.push_back(telemetry);
}

bool ConvergenceCalculator::getConvergence(Telemetry& telemetry) const
{
    if (minCount <= telemetries.count())
    {
        Telemetry last = telemetries.last();
        Telemetry first = telemetries.first();

        double distance = last.gcsDistance - first.gcsDistance;
        double time = (last.time - first.time) / 1000.0;
        double speed = -(distance / time) * 3.6;
        double ratio = qAbs(speed / last.airSpeed);
        ratio = qMin(ratio, 1.0);
        telemetry = last;
        telemetry.convergenceRatio = ratio;
        telemetry.convergenceSpeed = speed;
        telemetry.isConvergenceDataExist = true;
        return true;
    }
    return false;
}
