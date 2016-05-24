#ifndef CONVERGENCECALCULATOR_H
#define CONVERGENCECALCULATOR_H

#include <Telemetry.h>
#include <QList>

class ConvergenceCalculator
{
public:
    ConvergenceCalculator();
    void add(const Telemetry& telemetry);
    bool getConvergence(Telemetry& telemetry) const;

private:
    int minCount;
    QList<Telemetry> telemetries;
};

#endif // CONVERGENCECALCULATOR_H
