#include "setting.h"
#include "ui_dlgSetting.h"
#include <QColorDialog> //getColor()
#include <QFile>
#include <QRegularExpression>
//#include <QDebug>

#define Digits R"(\d+)"
#define Key_ValRex(key, valRex) QStringLiteral("(?<" key ">" valRex ")")  //(?<x>\d+)

CSetting::CSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSetting)
{
    ui->setupUi(this);
    connect(ui->xEditor, &QLineEdit::textEdited, this, &CSetting::onGeoEdited);
    connect(ui->yEditor, &QLineEdit::textEdited, this, &CSetting::onGeoEdited);
    connect(ui->widthEditor, &QLineEdit::textEdited, this, &CSetting::onGeoEdited);
    connect(ui->heightEditor, &QLineEdit::textEdited, this, &CSetting::onGeoEdited);
}

CSetting::~CSetting()
{
    //saveCfg();
    delete ui;
}

int CSetting::loadCfg()
{
    //读配置文件, 失败则按黙认值配置
    QRect clkGeoToSet = DEFAULT_CLK_GEO;
    QColor clkColorToSet = DEFAULT_CLK_COLOR;

    QFile cfgFile(cfgFilePath());
    bool isSucc = false;
    do
    {
        if (!cfgFile.exists() || !cfgFile.open(QIODevice::ReadOnly))
            break;

        QString cfgStr = cfgFile.readAll();
        //example:(0,0,400,200);(255,255,0)
        const QRegularExpression rex(QStringLiteral(R"(\(%1,%2,%3,%4\);\(%5,%6,%7\))")
                                     .arg(Key_ValRex("x", Digits),
                                          Key_ValRex("y", Digits),
                                          Key_ValRex("width", Digits),
                                          Key_ValRex("height", Digits),
                                          Key_ValRex("red", Digits),
                                          Key_ValRex("green", Digits),
                                          Key_ValRex("blue", Digits)));
        auto match = rex.match(cfgStr);
        if (!match.isValid())
            break;  //配置文件损坏

        QRect geoLoaded(match.captured(QStringLiteral("x")).toInt(),
                        match.captured(QStringLiteral("y")).toInt(),
                        match.captured(QStringLiteral("width")).toInt(),
                        match.captured(QStringLiteral("height")).toInt());
        QColor colorLoaded(match.captured(QStringLiteral("red")).toInt(),
                           match.captured(QStringLiteral("green")).toInt(),
                           match.captured(QStringLiteral("blue")).toInt());
        if (!geoLoaded.isValid() || !colorLoaded.isValid())
            break;  //配置值错误

        clkGeoToSet = std::move(geoLoaded);
        clkColorToSet = std::move(colorLoaded);
        isSucc = true;
    } while(0);

    //设置当前界面
    /* QLineEdit::setText()会发射textChanged()信号, 触发onGeoChanged()信号槽引起m_cfg.clkGeo意外变化.
     * 此次改动虽然将onGeoChanged()改为通过textEdited()信号触发, 但为防止类似情况再发生,
     * m_cfg仅用于保存配置项, 不参与计算和传值 */
    ui->xEditor->setText(QString::number(clkGeoToSet.x()));
    ui->yEditor->setText(QString::number(clkGeoToSet.y()));
    ui->widthEditor->setText(QString::number(clkGeoToSet.width()));
    ui->heightEditor->setText(QString::number(clkGeoToSet.height()));
    QString styleSheet = QStringLiteral("background:rgb(%1,%2,%3)")
            .arg(clkColorToSet.red())
            .arg(clkColorToSet.green())
            .arg(clkColorToSet.blue());
    ui->btnClockColor->setStyleSheet(styleSheet);

    //设置时钟界面
    emit sigGeoChanged(clkGeoToSet);
    emit sigClkColorChanged(clkColorToSet);

    //更新配置项
    m_cfg.clkGeo = clkGeoToSet;
    m_cfg.clkColor = clkColorToSet;
    return isSucc ? 0 : -1;
}

int CSetting::saveCfg()
{
    QFile cfgFile(cfgFilePath());
    if (!cfgFile.open(QIODevice::WriteOnly))
        return -1;

    QString cfgStr = QStringLiteral("(%1,%2,%3,%4);(%5,%6,%7)")
            .arg(m_cfg.clkGeo.x())
            .arg(m_cfg.clkGeo.y())
            .arg(m_cfg.clkGeo.width())
            .arg(m_cfg.clkGeo.height())
            .arg(m_cfg.clkColor.red())
            .arg(m_cfg.clkColor.green())
            .arg(m_cfg.clkColor.blue());
    cfgFile.write(cfgStr.toStdString().c_str());

    return 0;
}

void CSetting::onGeoEdited()
{
    QRect newGeo(ui->xEditor->text().toInt(),
                 ui->yEditor->text().toInt(),
                 ui->widthEditor->text().toInt(),
                 ui->heightEditor->text().toInt());
    if (!newGeo.isValid() || newGeo == m_cfg.clkGeo)
        return;

    emit sigGeoChanged(newGeo);
    m_cfg.clkGeo = newGeo;
    return;
}

void CSetting::on_btnClockColor_clicked()
{
    QColor clkColor = QColorDialog::getColor();
    if (!clkColor.isValid())
        return;

    emit sigClkColorChanged(clkColor);
    m_cfg.clkColor = clkColor;

    //按钮变色
    QString styleSheet = QStringLiteral("background:rgb(%1,%2,%3)")
            .arg(clkColor.red())
            .arg(clkColor.green())
            .arg(clkColor.blue());
    ui->btnClockColor->setStyleSheet(styleSheet);
    return;
}

void CSetting::accept()
{
    saveCfg();
    QDialog::accept();
}

void CSetting::reject()
{
    loadCfg();
    QDialog::reject();
}

QString CSetting::cfgFilePath() const
{
    return QCoreApplication::applicationDirPath() + QStringLiteral("/config");
}
