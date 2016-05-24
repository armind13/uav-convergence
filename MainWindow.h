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
    QList<Telemetry> convergenceTelemetries;

    QRectF getDrawingArea() const;
    void drawYawScale(QPainter& painter, const QPointF& center);
    void drawAirplane(QPainter& painter, const QPointF& center);
    void drawYaw(QPainter& painter, const QPointF& center, qreal radius);
    void drawConvergenceSpeed(QPainter& painter, const QPointF& center, qreal radius);
    void printTelemetry();
    void calculateRatios();
    static QString getString(double v1, double v2, int presition = 2);
    static QString getString(qint64 v1, qint64 v2);
    static QString getString(int v1, int v2);
    static QString getNavigationModeDescription(int mode);
};

#endif // MAINWINDOW_H
