#include "ConvergenceCalculator.h"

ConvergenceCalculator::ConvergenceCalculator() :
    minCount(500)
    ,telemetries()
{
}

void ConvergenceCalculator::add(const Telemetry& telemetry)
{
    telemetries.push_back(telemetry);
    if (telemetries.count() > minCount)
    {
        telemetries.pop_front();
    }
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
