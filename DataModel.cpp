#include "DataModel.h"
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>

DataModel::DataModel(QObject* parent) :
    QObject(parent)
    ,index(0)
    ,dataFound(false)
    ,rawTelemetry()
    ,closingTelemetry()
    ,convergenceCalculator()
{    
}

void DataModel::loadTelemetry(const QString& pathToFile)
{
    QFile input(pathToFile);
    rawTelemetry.clear();
    if ( ! (input.exists() && input.open(QFile::ReadOnly)))
    {
        dataFound = false;
        return;
    }

    auto fileSize = input.size();
    if (0 == fileSize)
    {
        dataFound = false;
        return;
    }

    qint64 readBytesCount = 0;
    QTextStream in(&input);

    const int maxProgress = 100;
    while (!in.atEnd())
    {
        auto line = in.readLine();
        Telemetry telemetry = parseStringToTelemetry(line);

        telemetry.packetId = rawTelemetry.size() + 1;
        rawTelemetry.push_back(telemetry);
        readBytesCount += line.length();
        int progress = readBytesCount / (double)fileSize * 100.0;
        if (progress > maxProgress)
        {
            progress = maxProgress;
        }
        emit parseProgressChanged(progress);
    }
    input.close();
    emit parseProgressChanged(maxProgress);
}

void DataModel::getNextTelemetry(Telemetry& telemetry, int& progress)
{
    if (rawTelemetry.empty())
    {
        telemetry = Telemetry();
        progress = 0;
        return;
    }

    if (index <= (rawTelemetry.size() - 1))
    {
        telemetry = rawTelemetry[index];
        progress = (index / (float)rawTelemetry.size()) * 100.0f;
        convergenceCalculator.add(telemetry);
        ++index;
    }
    else
    {
        telemetry = rawTelemetry.last();
        progress = 100;
    }
}

void DataModel::reseTelemetryIndex()
{
    index = 0;
}

bool DataModel::getConvergenceTelemetry(Telemetry& telemetry) const
{
    return convergenceCalculator.getConvergence(telemetry);
}

Telemetry DataModel::parseStringToTelemetry(const QString& in)
{
    QStringList telemetryParams = in.split(QChar(' '));
    Telemetry result;
    result.magneticYaw = getFloatParam(telemetryParams, 0);
    result.yaw = getFloatParam(telemetryParams, 1);
    result.gcsDistance = getFloatParam(telemetryParams, 2);
    result.airSpeed = getFloatParam(telemetryParams, 3);
    if (telemetryParams.count() > 4)
    {
        result.time = telemetryParams[4].toULongLong();
    }
    result.latitude = getFloatParam(telemetryParams, 5);
    result.longitude = getFloatParam(telemetryParams, 6);
    if (telemetryParams.count() > 7)
    {
        result.navigationMode = telemetryParams[7].toInt();
    }
    result.direction = getFloatParam(telemetryParams, 8);

    return result;
}

float DataModel::getFloatParam(const QStringList& in, int index)
{
    float result(0.0f);
    if (in.count() > index)
    {
        result = in[index].toFloat();
    }
    return result;
}
