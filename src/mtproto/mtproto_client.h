#pragma once

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QRandomGenerator>

class MTProtoClient : public QObject
{
    Q_OBJECT

public:
    explicit MTProtoClient(QObject *parent = nullptr);
    ~MTProtoClient();

    // 设置API凭据
    void setApiCredentials(int apiId, const QString& apiHash);
    
    // 代理设置
    void setProxy(bool enabled, const QString& host = QString(), quint16 port = 0, 
                  const QString& username = QString(), const QString& password = QString());
    bool isProxyEnabled() const;
    QString proxyHost() const;
    quint16 proxyPort() const;
    QString proxyUsername() const;
    QString proxyPassword() const;
    
    // 认证方法
    void sendAuthCode(const QString& phoneNumber);
    void signIn(const QString& phoneNumber, const QString& phoneCodeHash, const QString& code);
    
    // 用户数据方法
    void getMe();

signals:
    // 认证信号
    void authCodeRequested(const QString& phoneCodeHash);
    void authSuccess(const QString& username);
    void authError(const QString& error);
    
    // 用户数据信号
    void userDataReceived(const QString& username, const QString& firstName, const QString& lastName);

private slots:
    void onNetworkReply(QNetworkReply* reply);

private:
    // API 调用帮助方法
    void makeApiRequest(const QString& method, const QJsonObject& parameters);
    
    // 应用代理设置
    void applyProxySettings();
    
    // 网络管理
    QNetworkAccessManager* m_networkManager;
    
    // API凭据
    int m_apiId;
    QString m_apiHash;
    
    // 代理配置
    bool m_proxyEnabled;
    QString m_proxyHost;
    quint16 m_proxyPort;
    QString m_proxyUsername;
    QString m_proxyPassword;
    
    // 会话数据
    QString m_dcId;
    QString m_authKey;
    QString m_serverSalt;
    QString m_sessionId;
    qint64 m_lastMessageId;
    
    // 认证数据
    QString m_authToken;
}; 