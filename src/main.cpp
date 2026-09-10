#include <QApplication>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTranslator>
#include "clock.h"
#include "setting.h"

void installTranslator(QApplication *app)
{
    QTranslator *translator = new QTranslator(app);
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const auto &locale : uiLanguages)
    {
        const QString baseName = "fltClk_" + QLocale(locale).name();
        if (translator->load(":/i18n/"+baseName))
        {
            app->installTranslator(translator);
            break;
        }
    }
    return;
}

void showSettingOnClick(CSetting *setting, QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::ActivationReason::Trigger)
        setting->show();
    return;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    installTranslator(&a);  //安装翻译器

    //悬浮时钟界面
    CClock clock;
    clock.setWindowFlags(clock.windowFlags()
                     | Qt::FramelessWindowHint  //无边框
                     | Qt::WindowStaysOnTopHint //置顶
                     | Qt::WindowTransparentForInput);  //忽略鼠标键盘事件
    clock.setAttribute(Qt::WA_TranslucentBackground);   //窗口透明

    //设置界面
    CSetting setting;

    //系统托盘
    QSystemTrayIcon tray(QIcon(":/fltClk.ico"));
    QMenu menu;
    QAction *actSetting = new QAction(QObject::tr("Setting"), &menu);
    QAction *actQuit = new QAction(QObject::tr("Quit"), &menu);
    menu.addAction(actSetting);
    menu.addAction(actQuit);
    tray.setContextMenu(&menu);

    //信号槽
    QObject::connect(&setting, &CSetting::sigGeoChanged, &clock, &CClock::setGeo);
    QObject::connect(&setting, &CSetting::sigClkColorChanged, &clock, &CClock::setClkColor);
    QObject::connect(actSetting, &QAction::triggered, [&](){setting.show();});
    QObject::connect(actQuit, &QAction::triggered, [&](){a.quit();});
    QObject::connect(&tray, &QSystemTrayIcon::activated,
                     [&](QSystemTrayIcon::ActivationReason reason){showSettingOnClick(&setting, reason);});

    //显示所有界面
    if (0 != setting.loadCfg())
        setting.show();
    clock.show();
    tray.show();

    return a.exec();
}
