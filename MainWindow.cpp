#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFile>
#include <QFileDialog>
#include <QPainter>
#include <QtMath>

#include "MainPresenter.h"
#include "DataModel.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent)
    ,ui(new Ui::MainWindow)
    ,presenter(nullptr)
    ,model(nullptr)
    ,telemetry()
    ,airplanePixmap()
    ,yawScalePixmap()
    ,isReachedDistinguish(false)
    ,convergenceTelemetry()
{
    ui->setupUi(this);
    model = new DataModel(this);
    presenter = new MainPresenter(model, this, this);
    airplanePixmap.load(":/images/airplane.png");
    yawScalePixmap.load(":/images/yaw_scale.png");
}

void MainWindow::setEnabledFileLoading(bool enable)
{
    ui->loadButton->setEnabled(enable);
}

void MainWindow::setEnabledPlayingTelenetry(bool enable)
{
    ui->playButton->setEnabled(enable);
}

void MainWindow::setEnabledStopPlayingTelemetry(bool enable)
{
    ui->stopButton->setEnabled(enable);
}

void MainWindow::showTelemetry(const Telemetry& telemetry)
{
    this->telemetry = telemetry;
    if (telemetry.isConvergenceDataExist)
    {
        convergenceTelemetry.push_back(telemetry);
        if (convergenceTelemetry.size() > 20)
        {
            float delta = qAbs(convergenceTelemetry.first().magneticYaw
                               - convergenceTelemetry.last().magneticYaw);
            const float distinguishValue = 15.0f;
            bool minimalDistinguish = delta < distinguishValue;
            if ( ! minimalDistinguish)
            {
                float less = convergenceTelemetry.first().magneticYaw   ;
                float more = convergenceTelemetry.last().magneticYaw;
                if (less > more)
                {
                    std::swap(less, more);
                }
                less += 360.0f;
                float newDelta = less - more;
                minimalDistinguish = newDelta < distinguishValue;
            }
            if (minimalDistinguish)
            {
                convergenceTelemetry.pop_front();
            }
        }
    }
    repaint();
}

void MainWindow::showProgress(int progress)
{
    ui->progressBar->setValue(progress);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    printTelemetry();
    QRectF drawingArea = getDrawingArea();
    QPainter painter(this);
    drawYawScale(painter, drawingArea.center());
    drawAirplane(painter, drawingArea.center());
    drawMagneticYaw(painter, drawingArea.center(), yawScalePixmap.width() / 2.0);
}

void MainWindow::on_loadButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Выбрать файл"), "E:\\",
                                                    tr("Текстовый файл (*.txt)"));
    emit fileSelected(fileName);
}

void MainWindow::on_playButton_clicked()
{
    emit needStartPlaying();
}

void MainWindow::on_stopButton_clicked()
{
    emit needStopPlaying();
}

QRectF MainWindow::getDrawingArea() const
{
    int height = ui->verticalSpacer->geometry().height();
    int width = this->width();
    qreal sideLength = height > width ? width : height;
    qreal halfSide = sideLength / 2.0;
    QPoint center = ui->verticalSpacer->geometry().center();
    QPointF topLeft(center.x() - halfSide, center.y() - halfSide);
    return QRectF(topLeft.x(), topLeft.y(), sideLength, sideLength);
}

void MainWindow::drawYawScale(QPainter& painter, const QPointF& center)
{
    QPointF topLeft(center.x() - yawScalePixmap.width() / 2.0,
                    center.y() - yawScalePixmap.height() / 2.0);
    painter.drawPixmap(topLeft, yawScalePixmap);
}

void MainWindow::drawAirplane(QPainter& painter, const QPointF& center)
{
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.translate(center);
    painter.rotate(telemetry.magneticYaw);
    painter.drawPixmap(QPointF(-airplanePixmap.width() / 2.0, -airplanePixmap.height() / 2.0),
                        airplanePixmap);
    painter.resetTransform();
}

void MainWindow::drawMagneticYaw(QPainter& painter, const QPointF& center, qreal radius)
{
    painter.save();
    painter.setPen(QPen(Qt::red, 2));
    float magneticYaw = telemetry.magneticYaw - 90.0f;
    QPointF end(center.x() + cos(qDegreesToRadians(magneticYaw)) * radius,
                center.y() + sin(qDegreesToRadians(magneticYaw)) * radius);
    painter.drawLine(center, end);
    painter.restore();
}

void MainWindow::drawConvergenceSpeed(QPainter& painter, const QPointF& center, qreal radius)
{
    if (convergenceTelemetry.size() < 2)
    {
        return;
    }
    painter.save();

    QPointF previousPoint(center);
    bool isPreviuosPositive(true);
    QPen positivePen(Qt::green, 2);
    QPen negativePen(Qt::red, 2);
    painter.setPen(positivePen);
    foreach (auto telemetry, convergenceTelemetry)
    {
        qreal length = telemetry.convergenceRatio * radius;
        QPointF end(center.x() + cos(qDegreesToRadians(telemetry.magneticYaw)) * length,
                    center.y() + sin(qDegreesToRadians(telemetry.magneticYaw)) * length);
        bool isCurrentPositive = telemetry.convergenceSpeed > 0.0;
        if (isCurrentPositive != isPreviuosPositive)
        {
            if (isCurrentPositive)
            {
                painter.setPen(positivePen);
            }
            else
            {
                painter.setPen(negativePen);
            }
        }
        isPreviuosPositive = isCurrentPositive;
        painter.drawLine(previousPoint, end);
        previousPoint = end;
    }

    painter.restore();
}

void MainWindow::printTelemetry()
{
    ui->yawLabel->setText(QString::number(telemetry.yaw, 'f', 2));
    ui->magneticYawLabel->setText(QString::number(telemetry.magneticYaw, 'f', 2));
    ui->directionLabel->setText(QString::number(telemetry.direction, 'f', 2));
    ui->gscDistanceLabel->setText(QString::number(telemetry.gcsDistance, 'f', 2));
    ui->timeLabel->setText(QString::number(telemetry.time));
    QString navigationMode;
    switch (telemetry.navigationMode)
    {
    case 0:
        navigationMode = "GPS only";
        break;
    case 1:
        navigationMode = "Auto";
        break;
    case 2:
        navigationMode = "IMU only";
        break;
    case 3:
        navigationMode = "Binding";
        break;
    default:
        navigationMode = "Unknown";
        break;
    }
    ui->navigationModeLabel->setText(navigationMode);
    ui->airSpeedLabel->setText(QString::number(telemetry.airSpeed, 'f', 2));
    ui->convergenceSpeedLabel->setText(QString::number(telemetry.convergenceSpeed, 'f', 2));
    ui->ratioSpeedLabel->setText(QString::number(telemetry.convergenceRatio, 'f', 2));
    ui->latitudeLabel->setText(QString::number(telemetry.latitude));
    ui->longitudeLabel->setText(QString::number(telemetry.longitude));
    ui->packetIdLabel->setText(QString::number(telemetry.packetId));
}
