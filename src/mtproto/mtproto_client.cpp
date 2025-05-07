#include "mtproto_client.h"

// 这里使用简化的实现 - 真实的MTProto实现会更复杂
// 实际的Telegram tdesktop使用完整的MTProto协议实现

// Telegram API URL (使用公共测试API)
const QString API_URL = "https://api.telegram.org";

MTProtoClient::MTProtoClient(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_apiId(0)
    , m_proxyEnabled(false)
    , m_proxyPort(0)
    , m_lastMessageId(0)
{
    // 连接网络响应信号
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &MTProtoClient::onNetworkReply);
}

MTProtoClient::~MTProtoClient()
{
}

void MTProtoClient::setApiCredentials(int apiId, const QString& apiHash)
{
    m_apiId = apiId;
    m_apiHash = apiHash;
}

void MTProtoClient::setProxy(bool enabled, const QString& host, quint16 port, 
                           const QString& username, const QString& password)
{
    m_proxyEnabled = enabled;
    m_proxyHost = host;
    m_proxyPort = port;
    m_proxyUsername = username;
    m_proxyPassword = password;
    
    // 应用代理设置
    applyProxySettings();
}

bool MTProtoClient::isProxyEnabled() const
{
    return m_proxyEnabled;
}

QString MTProtoClient::proxyHost() const
{
    return m_proxyHost;
}

quint16 MTProtoClient::proxyPort() const
{
    return m_proxyPort;
}

QString MTProtoClient::proxyUsername() const
{
    return m_proxyUsername;
}

QString MTProtoClient::proxyPassword() const
{
    return m_proxyPassword;
}

void MTProtoClient::applyProxySettings()
{
    if (m_proxyEnabled && !m_proxyHost.isEmpty() && m_proxyPort > 0) {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::Socks5Proxy);
        proxy.setHostName(m_proxyHost);
        proxy.setPort(m_proxyPort);
        
        if (!m_proxyUsername.isEmpty()) {
            proxy.setUser(m_proxyUsername);
            proxy.setPassword(m_proxyPassword);
        }
        
        m_networkManager->setProxy(proxy);
    } else {
        // 禁用代理
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::NoProxy);
        m_networkManager->setProxy(proxy);
    }
}

void MTProtoClient::sendAuthCode(const QString& phoneNumber)
{
    QJsonObject parameters;
    parameters["phone_number"] = phoneNumber;
    parameters["api_id"] = m_apiId;
    parameters["api_hash"] = m_apiHash;
    parameters["settings"] = QJsonObject{{"allow_flashcall", false}, {"current_number", true}};
    
    makeApiRequest("auth.sendCode", parameters);
}

void MTProtoClient::signIn(const QString& phoneNumber, const QString& phoneCodeHash, const QString& code)
{
    QJsonObject parameters;
    parameters["phone_number"] = phoneNumber;
    parameters["phone_code_hash"] = phoneCodeHash;
    parameters["phone_code"] = code;
    
    makeApiRequest("auth.signIn", parameters);
}

void MTProtoClient::getMe()
{
    QJsonObject parameters;
    makeApiRequest("users.getFullUser", parameters);
}

void MTProtoClient::makeApiRequest(const QString& method, const QJsonObject& parameters)
{
    // 简化的API请求实现 - 实际的MTProto更复杂
    
    // 在真实项目中应该使用MTProto协议
    // 这里为了演示，我们使用一个模拟的API响应
    
    QUrl url(API_URL + "/bot" + m_apiHash + "/" + method);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    // 创建请求数据
    QJsonDocument doc(parameters);
    QByteArray data = doc.toJson();
    
    // 发送请求
    QNetworkReply* reply = m_networkManager->post(request, data);
    
    // 为请求添加标识
    reply->setProperty("method", method);
}

void MTProtoClient::onNetworkReply(QNetworkReply* reply)
{
    // 获取方法名
    QString method = reply->property("method").toString();
    
    if (reply->error() != QNetworkReply::NoError) {
        // 网络错误处理
        if (method == "auth.sendCode") {
            emit authError("发送验证码失败: " + reply->errorString());
        } else if (method == "auth.signIn") {
            emit authError("登录失败: " + reply->errorString());
        } else if (method == "users.getFullUser") {
            emit authError("获取用户信息失败: " + reply->errorString());
        }
        reply->deleteLater();
        return;
    }
    
    // 读取响应数据
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject response = doc.object();
    
    // 模拟不同API的响应
    if (method == "auth.sendCode") {
        // 模拟验证码已发送
        QString phoneCodeHash = QString::number(QRandomGenerator::global()->generate() % 10000000);
        emit authCodeRequested(phoneCodeHash);
    } else if (method == "auth.signIn") {
        // 模拟登录成功
        emit authSuccess("user" + QString::number(QRandomGenerator::global()->generate() % 10000));
    } else if (method == "users.getFullUser") {
        // 模拟获取用户信息
        QString username = "user" + QString::number(QRandomGenerator::global()->generate() % 10000);
        QString firstName = "测试";
        QString lastName = "用户";
        emit userDataReceived(username, firstName, lastName);
    }
    
    reply->deleteLater();
} 