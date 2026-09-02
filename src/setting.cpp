#include "setting.h"
#include "ui_dlgSetting.h"
#include <QColorDialog> //getColor()
#include <QFile>
#include <QRegularExpression>
//#include <QDebug>

CSetting::CSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSetting)
{
    ui->setupUi(this);
    connect(ui->xEditor, &QLineEdit::textChanged, this, &CSetting::onGeoChanged);
    connect(ui->yEditor, &QLineEdit::textChanged, this, &CSetting::onGeoChanged);
    connect(ui->widthEditor, &QLineEdit::textChanged, this, &CSetting::onGeoChanged);
    connect(ui->heightEditor, &QLineEdit::textChanged, this, &CSetting::onGeoChanged);
}

CSetting::~CSetting()
{
    //saveCfg();
    delete ui;
}

int CSetting::loadCfg()
{
    //读配置文件, 失败则按黙认值配置
    QFile cfgFile(cfgFilePath());
    bool isSucc = false;
    do
    {
        if (!cfgFile.exists() || !cfgFile.open(QIODevice::ReadOnly))
            break;

        QString cfgStr = cfgFile.readAll();
        const QRegularExpression rex(R"(\((?<x>\d+),(?<y>\d+),(?<width>\d+),(?<height>\d+)\);\((?<red>\d+),(?<green>\d+),(?<blue>\d+)\))");
        auto match = rex.match(cfgStr);
        //qDebug() << match;

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

        m_clkGeo = std::move(geoLoaded);
        m_clkColor = std::move(colorLoaded);
        isSucc = true;
    } while(0);

    //设置当前界面
    ui->xEditor->setText(QString::number(m_clkGeo.x()));
    ui->yEditor->setText(QString::number(m_clkGeo.y()));
    ui->widthEditor->setText(QString::number(m_clkGeo.width()));
    ui->heightEditor->setText(QString::number(m_clkGeo.height()));
    QString styleSheet = QStringLiteral("background:rgb(%1,%2,%3)")
            .arg(m_clkColor.red())
            .arg(m_clkColor.green())
            .arg(m_clkColor.blue());
    ui->btnClockColor->setStyleSheet(styleSheet);

    //设置时钟界面
    emit sigGeoChanged(m_clkGeo);
    emit sigClkColorChanged(m_clkColor);
    return isSucc ? 0 : -1;
}

int CSetting::saveCfg()
{
    QFile cfgFile(cfgFilePath());
    if (!cfgFile.open(QIODevice::WriteOnly))
        return -1;

    QString cfgStr = QStringLiteral("(%1,%2,%3,%4);(%5,%6,%7)")
            .arg(m_clkGeo.x()).arg(m_clkGeo.y()).arg(m_clkGeo.width()).arg(m_clkGeo.height())
            .arg(m_clkColor.red()).arg(m_clkColor.green()).arg(m_clkColor.blue());
    cfgFile.write(cfgStr.toStdString().c_str());

    return 0;
}

void CSetting::init(const QRect &rect)
{
    ui->xEditor->setText(QString::number(rect.x()));
    ui->yEditor->setText(QString::number(rect.y()));
    ui->widthEditor->setText(QString::number(rect.width()));
    ui->heightEditor->setText(QString::number(rect.height()));
    return;
}

void CSetting::onGeoChanged()
{
    QRect newGeo(ui->xEditor->text().toInt(),
                 ui->yEditor->text().toInt(),
                 ui->widthEditor->text().toInt(),
                 ui->heightEditor->text().toInt());
    if (!newGeo.isValid())
        return;

    m_clkGeo = newGeo;
    emit sigGeoChanged(newGeo);
    return;
}

void CSetting::on_btnClockColor_clicked()
{
    QColor clkColor = QColorDialog::getColor();
    if (!clkColor.isValid())
        return;

    m_clkColor = clkColor;
    emit sigClkColorChanged(clkColor);

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
