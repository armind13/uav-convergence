#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include "ITelemetryViewer.h"

class DataModel;
class MainPresenter;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public ITelemetryViewer
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    void setEnabledFileLoading(bool enable);
    void setEnabledPlayingTelenetry(bool enable);
    void setEnabledStopPlayingTelemetry(bool enable);
    void showTelemetry(const Telemetry& telemetry);
    void showProgress(int progress);
    void setMapLimits(double minLatitude, double maxLatitude, double minLongitude, double maxLongitude);
    ~MainWindow();

signals:
    void fileSelected(const QString& filePath);
    void needStartPlaying();
    void needStopPlaying();

protected:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);

private slots:
    void on_loadButton_clicked();
    void on_playButton_clicked();
    void on_stopButton_clicked();

private:
    Ui::MainWindow* ui;
    MainPresenter* presenter;
    DataModel* model;
    Telemetry telemetry;
    QPixmap airplanePixmap;
    QPixmap yawScalePixmap;
    bool isReachedDistinguish;
    QList<Telemetry> convergenceTelemetry;
    QList<QPointF> uavCoords;
    double minLatitude, maxLatitude, minLongitude, maxLongitude, ratioHeight, ratioWidth;

    QRectF getDrawingArea() const;
    void drawYawScale(QPainter& painter, const QPointF& center);
    void drawAirplane(QPainter& painter, const QPointF& center);
    void drawYaw(QPainter& painter, const QPointF& center, qreal radius);
    void drawConvergenceSpeed(QPainter& painter, const QPointF& center, qreal radius);
    void drawTrack(QPainter& painter);
    void printTelemetry();
    void calculateRatios();
};

#endif // MAINWINDOW_H
