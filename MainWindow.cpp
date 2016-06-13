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
    ,convergenceTelemetries()
    ,traceCoordinatesPercentsOffsets()
    ,latitudeMinMax()
    ,longitudeMinMax()
    ,mapArea()
    ,pfdArea()
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
    ui->startPositionEdit->setEnabled(enable);
}

void MainWindow::setEnabledStopPlayingTelemetry(bool enable)
{
    ui->stopButton->setEnabled(enable);
}

void MainWindow::storePositionParams()
{
    double latitudePercent = (telemetry.latitudePlain - latitudeMinMax.first)
                              / (latitudeMinMax.second - latitudeMinMax.first);

    double longitudePercent = (telemetry.longitudePlain - longitudeMinMax.first)
                               / (longitudeMinMax.second - longitudeMinMax.first);

    traceCoordinatesPercentsOffsets.push_back( qMakePair(latitudePercent, longitudePercent));
    if (traceCoordinatesPercentsOffsets.size() > 10000)
    {
        traceCoordinatesPercentsOffsets.pop_front();
    }
}

void MainWindow::calculateMapAreaSize()
{
    const int leftBorder = ui->mapSpacer->geometry().left();
    const int width = ui->mapSpacer->geometry().width();
    const int height = geometry().height();
    const int sideLength = qMin(width, height);

    mapArea = QRect(leftBorder, 0, sideLength, sideLength);
}

double MainWindow::getLongitudePx(double percent) const
{
    return mapArea.left() + mapArea.width() * percent;
}

double MainWindow::getLatitudePx(double percent) const
{
    return (1.0 - percent) * mapArea.height();
}

QPointF MainWindow::getMapPoint(double longitudePercent, double latitudePercent) const
{
    return QPointF(getLongitudePx(longitudePercent), getLatitudePx(latitudePercent));
}

void MainWindow::showTelemetry(const Telemetry& telemetry, const Telemetry& convergenceTelemetry)
{
    this->telemetry = telemetry;
    if (convergenceTelemetry.isConvergenceDataExist)
    {
        if (convergenceTelemetry.packetId >= 0)
        {
            convergenceTelemetries.push_back(convergenceTelemetry);
        }
        if (convergenceTelemetries.size() > 150)
        {
            float delta = qAbs(convergenceTelemetries.first().yaw - convergenceTelemetries.last().yaw);
            const float distinguishValue = 15.0f;
            bool minimalDistinguish = delta < distinguishValue;
            if ( ! minimalDistinguish)
            {
                float less = convergenceTelemetries.first().yaw;
                float more = convergenceTelemetries.last().yaw;
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
                convergenceTelemetries.pop_front();
            }
        }
    }

    storePositionParams();
    repaint();
}

void MainWindow::showProgress(int progress)
{
    ui->progressBar->setValue(progress);
}

void MainWindow::setMinMaxPositionValues(const MinMaxValues& latitude, const MinMaxValues& longitude)
{
    latitudeMinMax = latitude;
    longitudeMinMax = longitude;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    printTelemetry();
    if (pfdArea.isNull() || mapArea.isNull())
    {
        calculatePfdDrawingArea();
        calculateMapAreaSize();
    }
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    drawYawScale(painter);
    drawAirplane(painter);
    drawYaw(painter);
    drawConvergenceSpeed(painter);
    drawTrace(painter);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    calculatePfdDrawingArea();
    calculateMapAreaSize();
}

void MainWindow::on_loadButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Выбрать файл"), "E:\\THESIS\\Raw_telemtry",
                                                    tr("Текстовый файл (*.txt)"));
    emit fileSelected(fileName);
}

void MainWindow::on_playButton_clicked()
{
    bool ok(false);
    const int index = ui->startPositionEdit->text().toInt(&ok);
    if (ok)
    {
        emit needStartPlaying(index);
    }
}

void MainWindow::on_stopButton_clicked()
{
    emit needStopPlaying();
}

void MainWindow::calculatePfdDrawingArea()
{
    int height = ui->verticalSpacer->geometry().height();
    int width = this->width();
    qreal sideLength = qMin(width, height);
    qreal halfSide = sideLength / 2.0;
    QPoint center = ui->verticalSpacer->geometry().center();
    QPointF topLeft(center.x() - halfSide, center.y() - halfSide);
    pfdArea = QRect(topLeft.x(), topLeft.y(), sideLength, sideLength);
}

void MainWindow::drawYawScale(QPainter& painter)
{
    QPointF topLeft(pfdArea.center().x() - yawScalePixmap.width() / 2.0,
                    pfdArea.center().y() - yawScalePixmap.height() / 2.0);
    painter.drawPixmap(topLeft, yawScalePixmap);
}

void MainWindow::drawAirplane(QPainter& painter)
{
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.translate(pfdArea.center());
    painter.rotate(telemetry.yaw);
    painter.drawPixmap(QPointF(-airplanePixmap.width() / 2.0, -airplanePixmap.height() / 2.0),
                        airplanePixmap);
    painter.resetTransform();
}

void MainWindow::drawYaw(QPainter& painter)
{
    painter.save();    
    const qreal radius = yawScalePixmap.width() / 2.0;
    painter.setPen(QPen(Qt::blue, 2));
    float yaw = telemetry.yaw - 90.0f;
    QPointF end(pfdArea.center().x() + cos(qDegreesToRadians(yaw)) * radius,
                pfdArea.center().y() + sin(qDegreesToRadians(yaw)) * radius);
    painter.drawLine(pfdArea.center(), end);
    painter.restore();
}

void MainWindow::drawConvergenceSpeed(QPainter& painter)
{    
    if (convergenceTelemetries.size() < 2)
    {
        return;
    }
    const qreal radius = yawScalePixmap.width() / 2.0;
    painter.save();

    QPointF previousPoint(pfdArea.center());
    bool isPreviuosPositive(true);
    QPen positivePen(Qt::green, 2);
    QPen negativePen(Qt::red, 2);
    painter.setPen(positivePen);
    foreach (auto telemetry, convergenceTelemetries)
    {
        qreal length = telemetry.convergenceRatio * radius;
        double angle = telemetry.yaw - 90.0;
        QPointF end(pfdArea.center().x() + cos(qDegreesToRadians(angle)) * length,
                    pfdArea.center().y() + sin(qDegreesToRadians(angle)) * length);
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

void MainWindow::drawTrace(QPainter& painter)
{
    painter.save();
    painter.drawRect(mapArea);

    painter.setPen(QPen(Qt::blue, 2));

    QPointF previous;
    if (traceCoordinatesPercentsOffsets.size() > 1)
    {
        previous = getMapPoint(traceCoordinatesPercentsOffsets[0].second,
                               traceCoordinatesPercentsOffsets[0].first);
    }
    for (int i = 1; i < traceCoordinatesPercentsOffsets.size(); ++i)
    {
        QPointF currcent = getMapPoint(traceCoordinatesPercentsOffsets[i].second,
                                       traceCoordinatesPercentsOffsets[i].first);
        painter.drawLine(previous, currcent);
        previous = currcent;
    }

    painter.restore();
}

void MainWindow::printTelemetry()
{
    Telemetry convergenceTelem = convergenceTelemetries.count() > 0 ? convergenceTelemetries.last() : Telemetry();
    ui->yawLabel->setText(getString(telemetry.yaw, convergenceTelem.yaw));
    ui->magneticYawLabel->setText(getString(telemetry.magneticYaw, convergenceTelem.magneticYaw));
    ui->directionLabel->setText(getString(telemetry.direction, convergenceTelem.direction));
    ui->gscDistanceLabel->setText(getString(telemetry.gcsDistance, convergenceTelem.gcsDistance));
    ui->timeLabel->setText(getString(telemetry.time, convergenceTelem.time));
    ui->navigationModeLabel->setText(getNavigationModeDescription(telemetry.navigationMode)
                                     + " | "
                                     + getNavigationModeDescription(convergenceTelem.navigationMode));
    ui->airSpeedLabel->setText(getString(telemetry.airSpeed, convergenceTelem.airSpeed));
    ui->convergenceSpeedLabel->setText(getString(telemetry.convergenceSpeed, convergenceTelem.convergenceSpeed));
    ui->ratioSpeedLabel->setText(getString(telemetry.convergenceRatio, convergenceTelem.convergenceRatio));
    ui->latitudeLabel->setText(getString(telemetry.latitude, convergenceTelem.latitude, 4));
    ui->longitudeLabel->setText(getString(telemetry.longitude, convergenceTelem.longitude, 4));
    ui->packetIdLabel->setText(getString(telemetry.packetId, convergenceTelem.packetId));
    ui->plainLatitudeLabel->setText(QString::number(telemetry.latitudePlain, 'f', 2));
    ui->plainLongitudeLabel->setText(QString::number(telemetry.longitudePlain, 'f', 2));
}

QString MainWindow::getString(double v1, double v2, int presition)
{
    return QString("%1 | %2").arg(v1, 5, 'f', presition, QChar('0')).arg(v2, 5, 'f', presition, QChar('0'));
}

QString MainWindow::getString(qint64 v1, qint64 v2)
{
    return QString("%1 | %2").arg(v1).arg(v2);
}

QString MainWindow::getString(int v1, int v2)
{
    return QString("%1 | %2").arg(v1).arg(v2);
}

QString MainWindow::getNavigationModeDescription(int mode)
{
    QString navigationMode;
    switch (mode)
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
    return navigationMode;
}
