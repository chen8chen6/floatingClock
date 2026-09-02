#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

public slots:
    void setGeo(const QRect rect);
    void setClkColor(const QColor clkColor);

private slots:
    void showCurTime(void);

private:
    Ui::Dialog *ui;
};
#endif // DIALOG_H
