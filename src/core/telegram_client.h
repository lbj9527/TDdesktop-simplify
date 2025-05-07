#pragma once

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

#include "mtproto/mtproto_client.h"

class TelegramClient : public QObject
{
    Q_OBJECT

public:
    explicit TelegramClient(QObject *parent = nullptr);
    ~TelegramClient();

    // 设置API凭据
    void setApiCredentials(int apiId, const QString& apiHash);
    
    // 获取电话号码
    QString phoneNumber() const;
    
    // 代理设置方法
    void setProxy(bool enabled, const QString& host = QString(), quint16 port = 0, 
                 const QString& username = QString(), const QString& password = QString());
    bool isProxyEnabled() const;
    QString proxyHost() const;
    quint16 proxyPort() const;
    QString proxyUsername() const;
    QString proxyPassword() const;
    
    // 登录相关方法
    void sendAuthenticationCode(const QString& phoneNumber);
    void signIn(const QString& phoneNumber, const QString& phoneCodeHash, const QString& code);
    void getMe();

signals:
    // 登录状态信号
    void loginSuccess(const QString& username);
    void loginFailed(const QString& error);
    void codeRequested(const QString& phoneCodeHash);
    void authorizationError(const QString& error);
    
    // 用户信息信号
    void userInfoReceived(const QString& username, const QString& firstName, const QString& lastName);

private:
    // 核心MTProto客户端
    MTProtoClient* m_mtprotoClient;
    
    // API凭据
    int m_apiId;
    QString m_apiHash;
    
    // 电话号码
    QString m_phoneNumber;
    
    // 会话状态
    bool m_isAuthorized;
}; 