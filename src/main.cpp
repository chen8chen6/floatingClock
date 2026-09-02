#include <QApplication>
#include <QMenu>
#include <QSystemTrayIcon>
#include "dialog.h"
#include "setting.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //悬浮时钟界面
    Dialog w;
    w.setWindowFlags(w.windowFlags()
                     | Qt::FramelessWindowHint  //无边框
                     | Qt::WindowStaysOnTopHint //置顶
                     | Qt::WindowTransparentForInput    //忽略鼠标键盘事件
                     );
    w.setAttribute(Qt::WA_TranslucentBackground);   //窗口透明

    //设置界面
    CSetting setting;

    //系统托盘
    QSystemTrayIcon tray(QIcon(":/fltClk.ico"));    //TODO: 新的ico
    QMenu menu;
    QAction *actSetting = new QAction(QStringLiteral("设置"), &menu);
    QAction *actQuit = new QAction(QStringLiteral("退出"), &menu);
    menu.addAction(actSetting);
    menu.addAction(actQuit);
    tray.setContextMenu(&menu);

    //信号槽
    QObject::connect(&setting, &CSetting::sigGeoChanged, &w, &Dialog::setGeo);
    QObject::connect(&setting, &CSetting::sigClkColorChanged, &w, &Dialog::setClkColor);
    QObject::connect(actSetting, &QAction::triggered, [&](){setting.show();});
    QObject::connect(actQuit, &QAction::triggered, [&](){a.quit();});

    //显示所有界面
    if (0 != setting.loadCfg())
        setting.show();
    w.show();
    tray.show();

    return a.exec();
}
