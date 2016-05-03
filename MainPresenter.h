#ifndef MAINPRESENTER_H
#define MAINPRESENTER_H

#include <QObject>
#include "DataModel.h"
#include "ITelemetryViewer.h"

class MainPresenter : public QObject
{
    Q_OBJECT
public:
    MainPresenter(DataModel* model, ITelemetryViewer* view, QObject *parent = 0);

protected:
    void timerEvent(QTimerEvent* event);

private slots:
    void loadTelemetry(const QString& filePath);
    void notifyAboutProgress(int progress);
    void startPlaying();
    void stopPlaying();

private:
    const int timerFrequencyMs;
    DataModel* model;
    ITelemetryViewer* view;
    int timerId;
};

#endif // MAINPRESENTER_H
