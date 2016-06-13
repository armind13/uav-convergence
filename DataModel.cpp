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
    ,latitudeMinMax(0.0, 0.0)
    ,longitudeMinMax(0.0, 0.0)
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
        if (rawTelemetry.isEmpty())
        {
            latitudeMinMax.first = latitudeMinMax.second = telemetry.latitudePlain;
            longitudeMinMax.first = longitudeMinMax.second = telemetry.longitudePlain;
        }
        else
        {
            latitudeMinMax.first = qMin(latitudeMinMax.first, telemetry.latitudePlain);
            latitudeMinMax.second = qMax(latitudeMinMax.second, telemetry.latitudePlain);

            longitudeMinMax.first = qMin(longitudeMinMax.first, telemetry.longitudePlain);
            longitudeMinMax.second = qMax(longitudeMinMax.second, telemetry.longitudePlain);
        }

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

void DataModel::getMinMaxValues(MinMaxValues& latitude, MinMaxValues& longitude) const
{
    latitude = latitudeMinMax;
    longitude = longitudeMinMax;
}

void DataModel::setPacketIndex(int index)
{
    if (index >= 0 && index < rawTelemetry.count())
    {
        this->index = index;
    }
    else
    {
        this->index = 0;
    }
}

Telemetry DataModel::parseStringToTelemetry(const QString& in)
{
    QString simplifiedStr = in.simplified();
    QStringList telemetryParams = simplifiedStr.split(QChar(' '));
    Telemetry result;
    result.magneticYaw = getDoubleParam(telemetryParams, 0);
    result.yaw = getDoubleParam(telemetryParams, 1);
    result.gcsDistance = getDoubleParam(telemetryParams, 2);
    result.airSpeed = getDoubleParam(telemetryParams, 3);
    if (telemetryParams.count() > 4)
    {
        result.time = telemetryParams[4].toULongLong();
    }
    result.latitude = getDoubleParam(telemetryParams, 5);
    result.longitude = getDoubleParam(telemetryParams, 6);
    if (telemetryParams.count() > 7)
    {
        result.navigationMode = telemetryParams[7].toInt();
    }
    result.latitudePlain = getDoubleParam(telemetryParams, 8);
    result.longitudePlain = getDoubleParam(telemetryParams, 9);

    return result;
}

double DataModel::getDoubleParam(const QStringList& in, int index)
{
    double result(0.0);
    if (in.count() > index)
    {
        result = in[index].toDouble();
    }
    return result;
}
