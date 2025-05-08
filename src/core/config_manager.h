#pragma once

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDir>

/**
 * @brief 配置管理类，负责应用配置的加载和保存
 * 
 * 这个类使用JSON格式存储配置信息，包括代理设置、API凭据等
 */
class ConfigManager : public QObject
{
    Q_OBJECT

public:
    static ConfigManager* instance();
    ~ConfigManager();

    // 禁用拷贝构造和赋值操作
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    // 加载和保存配置
    bool loadConfig();
    bool saveConfig();
    
    // 确保退出前保存配置
    void ensureSaveBeforeExit();

    // 代理设置
    bool proxyEnabled() const;
    QString proxyHost() const;
    quint16 proxyPort() const;
    QString proxyUsername() const;
    QString proxyPassword() const;

    void setProxyEnabled(bool enabled);
    void setProxyHost(const QString& host);
    void setProxyPort(quint16 port);
    void setProxyUsername(const QString& username);
    void setProxyPassword(const QString& password);

    // API凭据
    int apiId() const;
    QString apiHash() const;
    QString phoneNumber() const;

    void setApiId(int apiId);
    void setApiHash(const QString& apiHash);
    void setPhoneNumber(const QString& phoneNumber);

    // 配置文件路径
    QString configFilePath() const;

public slots:
    // 应用程序退出时保存配置
    void onAboutToQuit();

signals:
    void configLoaded();
    void configSaved();
    void proxyConfigChanged();

private:
    ConfigManager(QObject* parent = nullptr);
    
    static ConfigManager* s_instance;
    
    // 配置数据
    QJsonObject m_config;
    
    // 配置文件路径
    QString m_configFilePath;
    
    // 初始化配置文件路径
    void initConfigPath();
    
    // 创建默认配置
    QJsonObject createDefaultConfig() const;
}; 