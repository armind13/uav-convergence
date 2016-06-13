#include "MainPresenter.h"
#include <QTimerEvent>

MainPresenter::MainPresenter(DataModel* model, ITelemetryViewer* view, QObject* parent) :
    QObject(parent)
    ,timerFrequencyMs(250)
    ,model(model)
    ,view(view)
    ,timerId(0)
{
    view->setEnabledFileLoading(true);
    view->setEnabledPlayingTelenetry(false);
    view->setEnabledStopPlayingTelemetry(false);
    connect(model, SIGNAL(parseProgressChanged(int)), this, SLOT(notifyAboutProgress(int)));

    QObject* viewObject = dynamic_cast<QObject*>(view);
    if (nullptr != viewObject)
    {
        connect(viewObject, SIGNAL(fileSelected(const QString&)), this, SLOT(loadTelemetry(const QString&)));
        connect(viewObject, SIGNAL(needStartPlaying(int)), this, SLOT(startPlaying(int)));
        connect(viewObject, SIGNAL(needStopPlaying()), this, SLOT(stopPlaying()));
    }
}

void MainPresenter::timerEvent(QTimerEvent* event)
{
    if (event->timerId() != timerId)
    {
        QObject::timerEvent(event);
        return;
    }
    Telemetry telemetry;
    Telemetry convergenceTelemetry;
    int progress(0);
    model->getNextTelemetry(telemetry, progress);
    model->getConvergenceTelemetry(convergenceTelemetry);
    // TODO: подумать над тем, как убрать от дублирования полей телеметрии
    view->showTelemetry(telemetry, convergenceTelemetry);
    view->showProgress(progress);
}

void MainPresenter::loadTelemetry(const QString& filePath)
{
    view->setEnabledFileLoading(false);
    model->loadTelemetry(filePath);    
}

void MainPresenter::notifyAboutProgress(int progress)
{
    view->showProgress(progress);
    if (100 == progress)
    {
        view->setEnabledPlayingTelenetry(true);
    }
}

void MainPresenter::startPlaying(int packetIndex)
{
    if (0 == timerId)
    {
        timerId = this->startTimer(timerFrequencyMs);
        view->setEnabledPlayingTelenetry(false);
        view->setEnabledStopPlayingTelemetry(true);
        MinMaxValues lat, lon;
        model->getMinMaxValues(lat, lon);
        model->setPacketIndex(packetIndex);
        view->setMinMaxPositionValues(lat, lon);
    }
}

void MainPresenter::stopPlaying()
{
    if (0 != timerId)
    {
        this->killTimer(timerId);
        timerId = 0;
        view->setEnabledPlayingTelenetry(true);
        view->setEnabledStopPlayingTelemetry(false);
    }
}
