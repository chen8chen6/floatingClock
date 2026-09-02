#ifndef SETTING_H
#define SETTING_H

#include <QDialog>

namespace Ui {
class CSetting;
}

class CSetting : public QDialog
{
    Q_OBJECT

public:
    explicit CSetting(QWidget *parent = nullptr);
    ~CSetting() override;
    int loadCfg(void);
    int saveCfg(void);
    void init(const QRect &rect);

signals:
    void sigGeoChanged(const QRect);
    void sigClkColorChanged(const QColor);

private slots:
    void onGeoChanged(void);
    void on_btnClockColor_clicked(void);
    virtual void accept() override;
    virtual void reject() override;

private:
    const QRect DEFAULT_CLK_GEO{0,0,400,200};   //时钟黙认位置在左上方
    const QColor DEFAULT_CLK_COLOR{0,0,0};      //时钟黙认为黒色

    Ui::CSetting *ui;
    QString cfgFilePath(void) const;
    QRect m_clkGeo = DEFAULT_CLK_GEO;
    QColor m_clkColor = DEFAULT_CLK_COLOR;
};

#endif // SETTING_H
