#include "dialog.h"
#include "ui_dialog.h"

#include <chrono>

#include <QLCDNumber>
#include <QTimer>
#include <QTime>
//#include <QDebug>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Dialog::showCurTime);
    timer->start(std::chrono::milliseconds(300));
    showCurTime();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::setGeo(const QRect rect)
{
    setGeometry(rect);
}

void Dialog::setClkColor(const QColor clkColor)
{
    QString styleSheet = QStringLiteral("color:rgb(%1,%2,%3)")
            .arg(clkColor.red())
            .arg(clkColor.green())
            .arg(clkColor.blue());
    ui->lcdClock->setStyleSheet(styleSheet);
}

void Dialog::showCurTime()
{
    static QTime lastTime = QTime();
    const QTime curTime = QTime::currentTime();
    if (curTime == lastTime)
        return;

    lastTime = curTime;
    QString hhmm = curTime.toString("hh:mm");
    if (0 == curTime.second() % 2)
        hhmm[2] = ' ';

    ui->lcdClock->display(hhmm);
    return;
}

