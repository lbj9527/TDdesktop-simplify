#include "telegram_client.h"

TelegramClient::TelegramClient(QObject *parent)
    : QObject(parent)
    , m_mtprotoClient(new MTProtoClient(this))
    , m_apiId(0)
    , m_isAuthorized(false)
{
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
}

TelegramClient::~TelegramClient()
{
}

void TelegramClient::setApiCredentials(int apiId, const QString& apiHash)
{
    m_apiId = apiId;
    m_apiHash = apiHash;
    m_mtprotoClient->setApiCredentials(apiId, apiHash);
}

QString TelegramClient::phoneNumber() const
{
    return m_phoneNumber;
}

void TelegramClient::setProxy(bool enabled, const QString& host, quint16 port, 
                             const QString& username, const QString& password)
{
    m_mtprotoClient->setProxy(enabled, host, port, username, password);
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