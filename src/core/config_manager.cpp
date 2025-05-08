#include "config_manager.h"
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>

// 静态实例初始化
ConfigManager* ConfigManager::s_instance = nullptr;

ConfigManager* ConfigManager::instance()
{
    if (!s_instance) {
        s_instance = new ConfigManager();
        s_instance->initConfigPath();
        s_instance->loadConfig();
    }
    return s_instance;
}

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
{
    // 确保使用UTF-8编码
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    #endif
    // Qt6 已默认使用UTF-8编码
}

ConfigManager::~ConfigManager()
{
    saveConfig(); // 确保配置在析构时保存
}

void ConfigManager::initConfigPath()
{
    // 使用可执行文件所在目录保存配置文件
    QString executableDir = QCoreApplication::applicationDirPath();
    QDir dir(executableDir);
    
    // 确保目录存在
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    m_configFilePath = dir.filePath("config.json");
    qDebug() << "配置文件路径:" << m_configFilePath;
}

void ConfigManager::ensureSaveBeforeExit()
{
    // 连接应用程序退出信号
    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
            this, &ConfigManager::onAboutToQuit);
}

void ConfigManager::onAboutToQuit()
{
    qDebug() << "应用程序即将退出，保存配置...";
    saveConfig();
}

QString ConfigManager::configFilePath() const
{
    return m_configFilePath;
}

bool ConfigManager::loadConfig()
{
    QFile configFile(m_configFilePath);
    
    // 如果文件不存在，创建默认配置
    if (!configFile.exists()) {
        qDebug() << "配置文件不存在，创建默认配置:" << m_configFilePath;
        m_config = createDefaultConfig();
        saveConfig();
        emit configLoaded();
        return true;
    }
    
    // 打开文件
    if (!configFile.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开配置文件:" << configFile.errorString();
        m_config = createDefaultConfig();
        emit configLoaded();
        return false;
    }
    
    // 解析JSON
    QByteArray configData = configFile.readAll();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(configData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "解析配置文件错误:" << parseError.errorString();
        m_config = createDefaultConfig();
        emit configLoaded();
        return false;
    }
    
    // 加载配置
    m_config = doc.object();
    qDebug() << "成功加载配置文件:" << m_configFilePath;
    emit configLoaded();
    return true;
}

bool ConfigManager::saveConfig()
{
    QFile configFile(m_configFilePath);
    
    if (!configFile.open(QIODevice::WriteOnly)) {
        qWarning() << "无法写入配置文件:" << configFile.errorString();
        return false;
    }
    
    QJsonDocument doc(m_config);
    configFile.write(doc.toJson(QJsonDocument::Indented));
    qDebug() << "成功保存配置文件:" << m_configFilePath;
    emit configSaved();
    return true;
}

QJsonObject ConfigManager::createDefaultConfig() const
{
    QJsonObject config;
    
    // 代理设置
    QJsonObject proxy;
    proxy["enabled"] = false;
    proxy["host"] = "";
    proxy["port"] = 1080;
    proxy["username"] = "";
    proxy["password"] = "";
    config["proxy"] = proxy;
    
    // API凭据
    QJsonObject api;
    api["apiId"] = 0;
    api["apiHash"] = "";
    api["phoneNumber"] = "";
    config["api"] = api;
    
    return config;
}

// 代理设置访问器
bool ConfigManager::proxyEnabled() const
{
    return m_config["proxy"].toObject()["enabled"].toBool();
}

QString ConfigManager::proxyHost() const
{
    return m_config["proxy"].toObject()["host"].toString();
}

quint16 ConfigManager::proxyPort() const
{
    return m_config["proxy"].toObject()["port"].toInt();
}

QString ConfigManager::proxyUsername() const
{
    return m_config["proxy"].toObject()["username"].toString();
}

QString ConfigManager::proxyPassword() const
{
    return m_config["proxy"].toObject()["password"].toString();
}

void ConfigManager::setProxyEnabled(bool enabled)
{
    QJsonObject proxy = m_config["proxy"].toObject();
    proxy["enabled"] = enabled;
    m_config["proxy"] = proxy;
    emit proxyConfigChanged();
}

void ConfigManager::setProxyHost(const QString& host)
{
    QJsonObject proxy = m_config["proxy"].toObject();
    proxy["host"] = host;
    m_config["proxy"] = proxy;
    emit proxyConfigChanged();
}

void ConfigManager::setProxyPort(quint16 port)
{
    QJsonObject proxy = m_config["proxy"].toObject();
    proxy["port"] = port;
    m_config["proxy"] = proxy;
    emit proxyConfigChanged();
}

void ConfigManager::setProxyUsername(const QString& username)
{
    QJsonObject proxy = m_config["proxy"].toObject();
    proxy["username"] = username;
    m_config["proxy"] = proxy;
    emit proxyConfigChanged();
}

void ConfigManager::setProxyPassword(const QString& password)
{
    QJsonObject proxy = m_config["proxy"].toObject();
    proxy["password"] = password;
    m_config["proxy"] = proxy;
    emit proxyConfigChanged();
}

// API凭据访问器
int ConfigManager::apiId() const
{
    return m_config["api"].toObject()["apiId"].toInt();
}

QString ConfigManager::apiHash() const
{
    return m_config["api"].toObject()["apiHash"].toString();
}

QString ConfigManager::phoneNumber() const
{
    return m_config["api"].toObject()["phoneNumber"].toString();
}

void ConfigManager::setApiId(int apiId)
{
    QJsonObject api = m_config["api"].toObject();
    api["apiId"] = apiId;
    m_config["api"] = api;
}

void ConfigManager::setApiHash(const QString& apiHash)
{
    QJsonObject api = m_config["api"].toObject();
    api["apiHash"] = apiHash;
    m_config["api"] = api;
}

void ConfigManager::setPhoneNumber(const QString& phoneNumber)
{
    QJsonObject api = m_config["api"].toObject();
    api["phoneNumber"] = phoneNumber;
    m_config["api"] = api;
} 