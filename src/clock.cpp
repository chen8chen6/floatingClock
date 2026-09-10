#include "clock.h"
#include "ui_dlgClock.h"

#include <chrono>
#include <QLCDNumber>
#include <QTimer>
#include <QTime>
//#include <QDebug>

CClock::CClock(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CClock)
{
    ui->setupUi(this);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CClock::showCurTime);
    timer->start(std::chrono::milliseconds(300));
    showCurTime();
}

CClock::~CClock()
{
    delete ui;
}

void CClock::setGeo(const QRect rect)
{
    setGeometry(rect);
}

void CClock::setClkColor(const QColor clkColor)
{
    QString styleSheet = QStringLiteral("color:rgb(%1,%2,%3)")
            .arg(clkColor.red())
            .arg(clkColor.green())
            .arg(clkColor.blue());
    ui->lcdClock->setStyleSheet(styleSheet);
}

void CClock::showCurTime()
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

