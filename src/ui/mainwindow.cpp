#include "mainwindow.h"
#include <QDialog>
#include <QFormLayout>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_client(new TelegramClient(this))
    , m_proxyDialog(nullptr)
{
    // 设置窗口标题和大小
    setWindowTitle("Telegram 客户端");
    resize(400, 300);
    
    // 设置UI
    setupUi();
    
    // 创建菜单栏
    createMenuBar();
    
    // 加载代理设置
    loadProxySettings();
    
    // 连接信号和槽
    connect(m_loginButton, &QPushButton::clicked, this, &MainWindow::onLoginButtonClicked);
    connect(m_verifyButton, &QPushButton::clicked, this, &MainWindow::onVerificationCodeEntered);
    connect(m_getMeButton, &QPushButton::clicked, this, &MainWindow::onGetMeClicked);
    
    // 连接客户端信号
    connect(m_client, &TelegramClient::loginSuccess, this, &MainWindow::onLoginSuccess);
    connect(m_client, &TelegramClient::loginFailed, this, &MainWindow::onLoginFailed);
    connect(m_client, &TelegramClient::codeRequested, this, &MainWindow::onCodeRequested);
    connect(m_client, &TelegramClient::authorizationError, this, &MainWindow::onAuthorizationError);
    connect(m_client, &TelegramClient::userInfoReceived, this, &MainWindow::onUserInfoReceived);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    // 创建中央窗口部件
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 创建布局
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // 创建堆叠页面部件
    m_stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(m_stackedWidget);
    
    // 创建各个页面
    createLoginPage();
    createVerificationPage();
    createMainPage();
    
    // 默认显示登录页面
    m_stackedWidget->setCurrentWidget(m_loginPage);
}

void MainWindow::createMenuBar()
{
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    QMenu* settingsMenu = menuBar->addMenu("设置");
    QAction* proxyAction = settingsMenu->addAction("代理设置...");
    
    connect(proxyAction, &QAction::triggered, this, &MainWindow::onProxySettingsAction);
}

void MainWindow::createLoginPage()
{
    m_loginPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_loginPage);
    
    // API ID 输入
    QHBoxLayout* apiIdLayout = new QHBoxLayout();
    QLabel* apiIdLabel = new QLabel("API ID:", m_loginPage);
    m_apiIdEdit = new QLineEdit(m_loginPage);
    m_apiIdEdit->setPlaceholderText("输入您的API ID");
    apiIdLayout->addWidget(apiIdLabel);
    apiIdLayout->addWidget(m_apiIdEdit);
    layout->addLayout(apiIdLayout);
    
    // API Hash 输入
    QHBoxLayout* apiHashLayout = new QHBoxLayout();
    QLabel* apiHashLabel = new QLabel("API Hash:", m_loginPage);
    m_apiHashEdit = new QLineEdit(m_loginPage);
    m_apiHashEdit->setPlaceholderText("输入您的API Hash");
    apiHashLayout->addWidget(apiHashLabel);
    apiHashLayout->addWidget(m_apiHashEdit);
    layout->addLayout(apiHashLayout);
    
    // 电话号码输入
    QHBoxLayout* phoneLayout = new QHBoxLayout();
    QLabel* phoneLabel = new QLabel("电话号码:", m_loginPage);
    m_phoneNumberEdit = new QLineEdit(m_loginPage);
    m_phoneNumberEdit->setPlaceholderText("如: +86123456789");
    phoneLayout->addWidget(phoneLabel);
    phoneLayout->addWidget(m_phoneNumberEdit);
    layout->addLayout(phoneLayout);
    
    // 登录按钮
    m_loginButton = new QPushButton("登录", m_loginPage);
    layout->addWidget(m_loginButton);
    
    // 添加到堆叠部件
    m_stackedWidget->addWidget(m_loginPage);
}

void MainWindow::createVerificationPage()
{
    m_verificationPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_verificationPage);
    
    // 提示标签
    QLabel* infoLabel = new QLabel("请输入发送到您手机上的验证码:", m_verificationPage);
    layout->addWidget(infoLabel);
    
    // 验证码输入
    m_codeEdit = new QLineEdit(m_verificationPage);
    m_codeEdit->setPlaceholderText("验证码");
    layout->addWidget(m_codeEdit);
    
    // 验证按钮
    m_verifyButton = new QPushButton("验证", m_verificationPage);
    layout->addWidget(m_verifyButton);
    
    // 添加到堆叠部件
    m_stackedWidget->addWidget(m_verificationPage);
}

void MainWindow::createMainPage()
{
    m_mainPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_mainPage);
    
    // 用户名显示
    QLabel* welcomeLabel = new QLabel("欢迎使用Telegram客户端!", m_mainPage);
    layout->addWidget(welcomeLabel);
    
    m_usernameLabel = new QLabel("", m_mainPage);
    layout->addWidget(m_usernameLabel);
    
    // Get Me 按钮
    m_getMeButton = new QPushButton("获取账户信息", m_mainPage);
    layout->addWidget(m_getMeButton);
    
    // 添加到堆叠部件
    m_stackedWidget->addWidget(m_mainPage);
}

void MainWindow::createProxySettingsDialog()
{
    // 如果对话框已存在，直接返回
    if (m_proxyDialog) {
        return;
    }
    
    // 创建对话框
    m_proxyDialog = new QDialog(this);
    m_proxyDialog->setWindowTitle("SOCKS5代理设置");
    m_proxyDialog->setMinimumWidth(400);
    
    // 创建布局
    QVBoxLayout* mainLayout = new QVBoxLayout(m_proxyDialog);
    
    // 启用代理复选框
    m_proxyEnabledCheckBox = new QCheckBox("启用SOCKS5代理", m_proxyDialog);
    mainLayout->addWidget(m_proxyEnabledCheckBox);
    
    // 代理设置组
    QGroupBox* proxyGroupBox = new QGroupBox("代理服务器设置", m_proxyDialog);
    QFormLayout* formLayout = new QFormLayout(proxyGroupBox);
    
    // 主机和端口
    m_proxyHostEdit = new QLineEdit(m_proxyDialog);
    formLayout->addRow("服务器地址:", m_proxyHostEdit);
    
    m_proxyPortSpinBox = new QSpinBox(m_proxyDialog);
    m_proxyPortSpinBox->setRange(1, 65535);
    m_proxyPortSpinBox->setValue(1080); // 默认SOCKS5端口
    formLayout->addRow("端口:", m_proxyPortSpinBox);
    
    // 认证（可选）
    m_proxyUsernameEdit = new QLineEdit(m_proxyDialog);
    formLayout->addRow("用户名(可选):", m_proxyUsernameEdit);
    
    m_proxyPasswordEdit = new QLineEdit(m_proxyDialog);
    m_proxyPasswordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("密码(可选):", m_proxyPasswordEdit);
    
    mainLayout->addWidget(proxyGroupBox);
    
    // 按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_applyProxyButton = new QPushButton("应用", m_proxyDialog);
    QPushButton* cancelButton = new QPushButton("取消", m_proxyDialog);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_applyProxyButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);
    
    // 连接信号
    connect(m_proxyEnabledCheckBox, &QCheckBox::toggled, this, &MainWindow::onProxyCheckBoxToggled);
    connect(m_applyProxyButton, &QPushButton::clicked, this, &MainWindow::onApplyProxyButtonClicked);
    connect(cancelButton, &QPushButton::clicked, m_proxyDialog, &QDialog::reject);
    
    // 根据当前设置更新UI状态
    m_proxyEnabledCheckBox->setChecked(m_client->isProxyEnabled());
    m_proxyHostEdit->setText(m_client->proxyHost());
    m_proxyPortSpinBox->setValue(m_client->proxyPort() > 0 ? m_client->proxyPort() : 1080);
    m_proxyUsernameEdit->setText(m_client->proxyUsername());
    m_proxyPasswordEdit->setText(m_client->proxyPassword());
    
    // 初始启用/禁用输入框
    onProxyCheckBoxToggled(m_proxyEnabledCheckBox->isChecked());
}

void MainWindow::loadProxySettings()
{
    QSettings settings("TelegramDesktop", "TelegramClient");
    settings.beginGroup("Proxy");
    
    bool enabled = settings.value("Enabled", false).toBool();
    QString host = settings.value("Host", "").toString();
    quint16 port = settings.value("Port", 1080).toUInt();
    QString username = settings.value("Username", "").toString();
    QString password = settings.value("Password", "").toString();
    
    settings.endGroup();
    
    // 应用设置到客户端
    m_client->setProxy(enabled, host, port, username, password);
}

void MainWindow::saveProxySettings()
{
    QSettings settings("TelegramDesktop", "TelegramClient");
    settings.beginGroup("Proxy");
    
    settings.setValue("Enabled", m_client->isProxyEnabled());
    settings.setValue("Host", m_client->proxyHost());
    settings.setValue("Port", m_client->proxyPort());
    settings.setValue("Username", m_client->proxyUsername());
    settings.setValue("Password", m_client->proxyPassword());
    
    settings.endGroup();
}

void MainWindow::onLoginButtonClicked()
{
    QString apiId = m_apiIdEdit->text().trimmed();
    QString apiHash = m_apiHashEdit->text().trimmed();
    QString phoneNumber = m_phoneNumberEdit->text().trimmed();
    
    // 简单验证
    if (apiId.isEmpty() || apiHash.isEmpty() || phoneNumber.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请填写所有必填字段");
        return;
    }
    
    // 请求验证码
    m_client->setApiCredentials(apiId.toInt(), apiHash);
    m_client->sendAuthenticationCode(phoneNumber);
}

void MainWindow::onVerificationCodeEntered()
{
    QString code = m_codeEdit->text().trimmed();
    
    if (code.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入验证码");
        return;
    }
    
    // 验证码登录
    m_client->signIn(m_client->phoneNumber(), m_phoneCodeHash, code);
}

void MainWindow::onGetMeClicked()
{
    m_client->getMe();
}

void MainWindow::onLoginSuccess(const QString& username)
{
    m_usernameLabel->setText("用户名: " + username);
    m_stackedWidget->setCurrentWidget(m_mainPage);
}

void MainWindow::onLoginFailed(const QString& error)
{
    QMessageBox::critical(this, "登录失败", "错误: " + error);
}

void MainWindow::onCodeRequested(const QString& phoneCodeHash)
{
    m_phoneCodeHash = phoneCodeHash;
    m_stackedWidget->setCurrentWidget(m_verificationPage);
}

void MainWindow::onAuthorizationError(const QString& error)
{
    QMessageBox::critical(this, "授权错误", "错误: " + error);
    m_stackedWidget->setCurrentWidget(m_loginPage);
}

void MainWindow::onUserInfoReceived(const QString& username, const QString& firstName, const QString& lastName)
{
    QString displayText = QString("用户名: %1\n名字: %2\n姓氏: %3").arg(username, firstName, lastName);
    m_usernameLabel->setText(displayText);
    QMessageBox::information(this, "用户信息", displayText);
}

void MainWindow::onProxySettingsAction()
{
    // 创建代理设置对话框（如果尚未创建）
    if (!m_proxyDialog) {
        createProxySettingsDialog();
    }
    
    // 显示对话框
    m_proxyDialog->exec();
}

void MainWindow::onProxyCheckBoxToggled(bool checked)
{
    // 启用/禁用代理设置输入框
    m_proxyHostEdit->setEnabled(checked);
    m_proxyPortSpinBox->setEnabled(checked);
    m_proxyUsernameEdit->setEnabled(checked);
    m_proxyPasswordEdit->setEnabled(checked);
}

void MainWindow::onApplyProxyButtonClicked()
{
    bool enabled = m_proxyEnabledCheckBox->isChecked();
    QString host = m_proxyHostEdit->text().trimmed();
    quint16 port = m_proxyPortSpinBox->value();
    QString username = m_proxyUsernameEdit->text().trimmed();
    QString password = m_proxyPasswordEdit->text().trimmed();
    
    // 验证输入
    if (enabled && (host.isEmpty() || port == 0)) {
        QMessageBox::warning(m_proxyDialog, "输入错误", "请填写代理服务器地址和端口");
        return;
    }
    
    // 应用设置
    m_client->setProxy(enabled, host, port, username, password);
    
    // 保存设置
    saveProxySettings();
    
    // 关闭对话框
    m_proxyDialog->accept();
    
    QMessageBox::information(this, "代理设置", "代理设置已更新");
} 