#include <QApplication>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用名称和组织信息
    app.setApplicationName("Telegram客户端");
    app.setOrganizationName("TelegramDesktop");
    app.setOrganizationDomain("telegram.org");
    
    // 创建并显示主窗口
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
} 