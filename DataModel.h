#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QObject>
#include <QList>

#include "Telemetry.h"
#include "ConvergenceCalculator.h"

class DataModel : public QObject
{
    Q_OBJECT
public:
    DataModel(QObject* parent = 0);
    void loadTelemetry(const QString& pathToFile);
    void getNextTelemetry(Telemetry& telemetry, int& progress);
    void reseTelemetryIndex();
    bool getConvergenceTelemetry(Telemetry& telemetry) const;
signals:
    void parseProgressChanged(int);
public slots:
private:
    int index;
    bool dataFound;
    QList<Telemetry> rawTelemetry;
    QList<Telemetry> closingTelemetry;
    ConvergenceCalculator convergenceCalculator;

    static Telemetry parseStringToTelemetry(const QString& in);
    static float getFloatParam(const QStringList& in, int index);
};

#endif // DATAMODEL_H
