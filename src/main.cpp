#include <QApplication>
#include <QDir>
#include <QLoggingCategory>
#include <QDebug>
#include <QSslSocket>
#include "ui/mainwindow.h"
#include "core/config_manager.h"
#include <iostream>

// 自定义消息处理程序，将日志输出到控制台
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    
    switch (type) {
    case QtDebugMsg:
        std::cout << "[Debug] " << localMsg.constData() << std::endl;
        break;
    case QtInfoMsg:
        std::cout << "[Info] " << localMsg.constData() << std::endl;
        break;
    case QtWarningMsg:
        std::cout << "[Warning] " << localMsg.constData() << std::endl;
        break;
    case QtCriticalMsg:
        std::cout << "[Critical] " << localMsg.constData() << std::endl;
        break;
    case QtFatalMsg:
        std::cout << "[Fatal] " << localMsg.constData() << std::endl;
        abort();
    }
}

int main(int argc, char *argv[])
{
    // 安装自定义消息处理程序
    qInstallMessageHandler(messageHandler);
    
    // 设置高DPI缩放
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    // 创建应用程序实例
    QApplication app(argc, argv);
    
    // Qt6 已默认使用UTF-8编码
    
    // 设置应用名称和组织信息
    app.setApplicationName("Telegram客户端");
    app.setOrganizationName("TelegramDesktop");
    app.setOrganizationDomain("telegram.org");
    
    // 设置日志输出，但仍然在控制台显示警告
    QLoggingCategory::setFilterRules("qt.network.ssl.warning=true");
    
    // 检查OpenSSL支持
    qDebug() << "OpenSSL支持: " << QSslSocket::supportsSsl()
             << " 版本: " << QSslSocket::sslLibraryVersionString()
             << " 构建版本: " << QSslSocket::sslLibraryBuildVersionString();
    
    // 确保当前工作目录设置正确
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    qDebug() << "当前工作目录: " << QDir::currentPath();
    
    // 初始化配置管理器并确保退出前保存配置
    ConfigManager* configManager = ConfigManager::instance();
    configManager->ensureSaveBeforeExit();
    
    // 创建并显示主窗口
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
} 