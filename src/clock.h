#ifndef CLOCK_H
#define CLOCK_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class CClock; }
QT_END_NAMESPACE

class CClock : public QDialog
{
    Q_OBJECT

public:
    CClock(QWidget *parent = nullptr);
    ~CClock();

public slots:
    void setGeo(const QRect rect);
    void setClkColor(const QColor clkColor);

private slots:
    void showCurTime(void);

private:
    Ui::CClock *ui;
};
#endif // CLOCK_H
