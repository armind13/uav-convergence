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
    void showTelemetry(const Telemetry& telemetry, const Telemetry& convergenceTelemetries);
    void showProgress(int progress);
    void setMinMaxPositionValues(const MinMaxValues& latitude, const MinMaxValues& longitude);
    ~MainWindow();

signals:
    void fileSelected(const QString& filePath);
    void needStartPlaying(int packetIndex);
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
    QList<Telemetry> convergenceTelemetries;
    // latitude - first, longitude - second
    QList<QPair<double, double> > traceCoordinatesPercentsOffsets;
    MinMaxValues latitudeMinMax, longitudeMinMax;
    QRect mapArea, pfdArea;

    void calculatePfdDrawingArea();
    void drawYawScale(QPainter& painter);
    void drawAirplane(QPainter& painter);
    void drawYaw(QPainter& painter);
    void drawConvergenceSpeed(QPainter& painter);
    void drawTrace(QPainter& painter);
    void printTelemetry();
    static QString getString(double v1, double v2, int presition = 2);
    static QString getString(qint64 v1, qint64 v2);
    static QString getString(int v1, int v2);
    static QString getNavigationModeDescription(int mode);    
    void storePositionParams();
    void calculateMapAreaSize();
    double getLongitudePx(double percent) const;
    double getLatitudePx(double percent) const;
    QPointF getMapPoint(double longitudePercent, double latitudePercent) const;
};

#endif // MAINWINDOW_H
