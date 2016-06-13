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
    const int minCount;
    const int preBufferCount;
    QList<Telemetry> telemetries;
    QList<Telemetry> prebuffer;
};

#endif // CONVERGENCECALCULATOR_H
