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
        connect(viewObject, SIGNAL(needStartPlaying()), this, SLOT(startPlaying()));
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
    int progress(0);
    model->getNextTelemetry(telemetry, progress);
    view->showTelemetry(telemetry);
    view->showProgress(progress);
}

void MainPresenter::loadTelemetry(const QString& filePath)
{
    model->loadTelemetry(filePath);
    view->setEnabledFileLoading(false);
}

void MainPresenter::notifyAboutProgress(int progress)
{
    view->showProgress(progress);
    if (100 == progress)
    {
        view->setEnabledPlayingTelenetry(true);
    }
}

void MainPresenter::startPlaying()
{
    if (0 == timerId)
    {
        timerId = this->startTimer(timerFrequencyMs);
        view->setEnabledPlayingTelenetry(false);
        view->setEnabledStopPlayingTelemetry(true);
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
