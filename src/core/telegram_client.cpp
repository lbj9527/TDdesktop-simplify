#include "telegram_client.h"
#include "config_manager.h"
#include <QDebug>
#include <QSslSocket>
#include <QDir>
#include <QCoreApplication>

TelegramClient::TelegramClient(QObject *parent)
    : QObject(parent)
    , m_mtprotoClient(new MTProtoClient(this))
    , m_configManager(ConfigManager::instance())
    , m_apiId(0)
    , m_isAuthorized(false)
{
    // 初始化MTProto客户端
    m_mtprotoClient->init();
    
    // 检查TLS支持
    checkTlsSupport();
    
    // 连接MTProto客户端信号
    connect(m_mtprotoClient, &MTProtoClient::authCodeRequested, this, [this](const QString& phoneCodeHash) {
        emit codeRequested(phoneCodeHash);
    });
    
    connect(m_mtprotoClient, &MTProtoClient::authSuccess, this, [this](const QString& username) {
        m_isAuthorized = true;
        emit loginSuccess(username);
    });
    
    connect(m_mtprotoClient, &MTProtoClient::authError, this, [this](const QString& error) {
        emit loginFailed(error);
    });
    
    connect(m_mtprotoClient, &MTProtoClient::userDataReceived, this, 
        [this](const QString& username, const QString& firstName, const QString& lastName) {
            emit userInfoReceived(username, firstName, lastName);
        }
    );
    
    // 连接配置管理器信号
    connect(m_configManager, &ConfigManager::proxyConfigChanged, this, &TelegramClient::onProxyConfigChanged);
    
    // 加载配置
    loadSettings();
}

TelegramClient::~TelegramClient()
{
    // 保存设置
    saveSettings();
}

void TelegramClient::loadSettings()
{
    // 从配置管理器加载API凭据
    m_apiId = m_configManager->apiId();
    m_apiHash = m_configManager->apiHash();
    m_phoneNumber = m_configManager->phoneNumber();
    
    // 设置MTProto客户端的API凭据
    if (m_apiId > 0 && !m_apiHash.isEmpty()) {
        m_mtprotoClient->setApiCredentials(m_apiId, m_apiHash);
    }
    
    // 应用代理设置
    applyProxySettings();
}

void TelegramClient::saveSettings()
{
    // 保存API凭据
    m_configManager->setApiId(m_apiId);
    m_configManager->setApiHash(m_apiHash);
    m_configManager->setPhoneNumber(m_phoneNumber);
    
    // 保存配置
    m_configManager->saveConfig();
}

void TelegramClient::onProxyConfigChanged()
{
    // 当代理配置改变时，重新应用代理设置
    applyProxySettings();
}

void TelegramClient::applyProxySettings()
{
    // 从配置管理器获取代理设置并应用
    bool enabled = m_configManager->proxyEnabled();
    QString host = m_configManager->proxyHost();
    quint16 port = m_configManager->proxyPort();
    QString username = m_configManager->proxyUsername();
    QString password = m_configManager->proxyPassword();
    
    // 设置到MTProto客户端
    m_mtprotoClient->setProxy(enabled, host, port, username, password);
}

void TelegramClient::setApiCredentials(int apiId, const QString& apiHash)
{
    m_apiId = apiId;
    m_apiHash = apiHash;
    m_mtprotoClient->setApiCredentials(apiId, apiHash);
    
    // 保存到配置
    m_configManager->setApiId(apiId);
    m_configManager->setApiHash(apiHash);
    m_configManager->saveConfig();
}

QString TelegramClient::phoneNumber() const
{
    return m_phoneNumber;
}

void TelegramClient::setProxy(bool enabled, const QString& host, quint16 port, 
                             const QString& username, const QString& password)
{
    // 设置代理
    m_mtprotoClient->setProxy(enabled, host, port, username, password);
    
    // 保存到配置
    m_configManager->setProxyEnabled(enabled);
    m_configManager->setProxyHost(host);
    m_configManager->setProxyPort(port);
    m_configManager->setProxyUsername(username);
    m_configManager->setProxyPassword(password);
    m_configManager->saveConfig();
}

bool TelegramClient::isProxyEnabled() const
{
    return m_mtprotoClient->isProxyEnabled();
}

QString TelegramClient::proxyHost() const
{
    return m_mtprotoClient->proxyHost();
}

quint16 TelegramClient::proxyPort() const
{
    return m_mtprotoClient->proxyPort();
}

QString TelegramClient::proxyUsername() const
{
    return m_mtprotoClient->proxyUsername();
}

QString TelegramClient::proxyPassword() const
{
    return m_mtprotoClient->proxyPassword();
}

void TelegramClient::sendAuthenticationCode(const QString& phoneNumber)
{
    m_phoneNumber = phoneNumber;
    m_mtprotoClient->sendAuthCode(phoneNumber);
    
    // 保存电话号码到配置
    m_configManager->setPhoneNumber(phoneNumber);
    m_configManager->saveConfig();
}

void TelegramClient::signIn(const QString& phoneNumber, const QString& phoneCodeHash, const QString& code)
{
    m_mtprotoClient->signIn(phoneNumber, phoneCodeHash, code);
}

void TelegramClient::getMe()
{
    if (m_isAuthorized) {
        m_mtprotoClient->getMe();
    } else {
        emit authorizationError("未授权，请先登录");
    }
}

void TelegramClient::checkTlsSupport()
{
    if (!QSslSocket::supportsSsl()) {
        qCritical() << "TLS不可用! OpenSSL可能未找到或未正确配置";
        qCritical() << "查找以下文件: libssl 和 libcrypto";
        qCritical() << "库路径: " << QDir::currentPath();
        qCritical() << "Qt SSL库版本: " << QSslSocket::sslLibraryBuildVersionString();
        
        // 检查TLS插件是否存在
        QDir pluginsDir(QCoreApplication::applicationDirPath() + "/tls");
        QStringList tlsFiles = pluginsDir.entryList(QStringList() << "*.dll", QDir::Files);
        qDebug() << "TLS插件目录: " << pluginsDir.absolutePath();
        qDebug() << "找到的TLS插件: " << tlsFiles;
        
        // 检查OpenSSL库是否存在
        QDir appDir(QCoreApplication::applicationDirPath());
        QStringList sslFiles = appDir.entryList(QStringList() << "libssl*.dll" << "libcrypto*.dll", QDir::Files);
        qDebug() << "应用目录: " << appDir.absolutePath();
        qDebug() << "找到的OpenSSL库: " << sslFiles;
    } else {
        qDebug() << "TLS支持正常, 使用版本: " << QSslSocket::sslLibraryVersionString();
    }
} 