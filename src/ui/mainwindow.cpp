#include "mainwindow.h"
#include <QDialog>
#include <QFormLayout>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_client(new TelegramClient(this))
    , m_proxyDialog(nullptr)
    , m_configManager(ConfigManager::instance())
{
    // 设置窗口标题和大小
    setWindowTitle("Telegram 客户端");
    resize(450, 350);
    
    // 设置UI
    setupUi();
    
    // 创建菜单栏
    createMenuBar();
    
    // 创建状态栏
    createStatusBar();
    
    // 连接配置管理器信号
    connect(m_configManager, &ConfigManager::configLoaded, this, &MainWindow::onConfigLoaded);
    connect(m_configManager, &ConfigManager::configSaved, this, &MainWindow::onConfigSaved);
    
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
    
    // 初始化界面值
    initializeWithConfig();
}

MainWindow::~MainWindow()
{
}

void MainWindow::onConfigLoaded()
{
    // 当配置加载完成后，更新UI
    updateUiFromConfig();
    
    // 更新状态栏信息
    QString path = QDir::toNativeSeparators(m_configManager->configFilePath());
    m_statusLabel->setText(tr("配置已从 %1 加载").arg(path));
}

void MainWindow::onConfigSaved()
{
    // 当配置保存后，更新状态栏
    QString path = QDir::toNativeSeparators(m_configManager->configFilePath());
    m_statusLabel->setText(tr("配置已保存至 %1").arg(path));
}

void MainWindow::onShowConfigFilePathAction()
{
    // 获取配置文件路径
    QString configPath = QDir::toNativeSeparators(m_configManager->configFilePath());
    
    // 显示配置文件路径
    QMessageBox::information(this, tr("配置文件位置"), 
                           tr("配置文件保存在:\n%1\n\n点击确定将打开文件所在目录。").arg(configPath));
    
    // 打开配置文件所在目录
    QFileInfo fileInfo(m_configManager->configFilePath());
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
}

void MainWindow::initializeWithConfig()
{
    // 从配置中初始化界面值
    updateUiFromConfig();
}

void MainWindow::updateUiFromConfig()
{
    // 更新API凭据输入框
    m_apiIdEdit->setText(QString::number(m_configManager->apiId()));
    m_apiHashEdit->setText(m_configManager->apiHash());
    m_phoneNumberEdit->setText(m_configManager->phoneNumber());
    
    // 如果代理对话框存在，更新代理设置
    if (m_proxyDialog) {
        m_proxyEnabledCheckBox->setChecked(m_configManager->proxyEnabled());
        m_proxyHostEdit->setText(m_configManager->proxyHost());
        m_proxyPortSpinBox->setValue(m_configManager->proxyPort());
        m_proxyUsernameEdit->setText(m_configManager->proxyUsername());
        m_proxyPasswordEdit->setText(m_configManager->proxyPassword());
        
        // 更新代理设置输入框的启用状态
        onProxyCheckBoxToggled(m_configManager->proxyEnabled());
    }
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
    
    // 添加配置文件位置菜单项
    settingsMenu->addSeparator();
    QAction* configPathAction = settingsMenu->addAction("配置文件位置...");
    
    connect(proxyAction, &QAction::triggered, this, &MainWindow::onProxySettingsAction);
    connect(configPathAction, &QAction::triggered, this, &MainWindow::onShowConfigFilePathAction);
}

void MainWindow::createStatusBar()
{
    // 创建状态栏
    QStatusBar* statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    
    // 添加状态标签
    m_statusLabel = new QLabel(this);
    statusBar->addWidget(m_statusLabel, 1);
    
    // 显示初始配置文件路径
    QString path = QDir::toNativeSeparators(m_configManager->configFilePath());
    m_statusLabel->setText(tr("配置文件: %1").arg(path));
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
    
    // 欢迎标签
    QLabel* welcomeLabel = new QLabel("欢迎使用Telegram客户端!", m_mainPage);
    welcomeLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    layout->addWidget(welcomeLabel);
    
    // 用户信息组
    QGroupBox* userInfoGroup = new QGroupBox("账户信息", m_mainPage);
    QVBoxLayout* userInfoLayout = new QVBoxLayout(userInfoGroup);
    
    m_usernameLabel = new QLabel("", m_mainPage);
    m_usernameLabel->setTextFormat(Qt::RichText);
    m_usernameLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    userInfoLayout->addWidget(m_usernameLabel);
    
    layout->addWidget(userInfoGroup);
    
    // 获取账户信息按钮
    m_getMeButton = new QPushButton("刷新账户信息", m_mainPage);
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
    
    // 连接信号与槽
    connect(m_proxyEnabledCheckBox, &QCheckBox::toggled, this, &MainWindow::onProxyCheckBoxToggled);
    connect(m_applyProxyButton, &QPushButton::clicked, this, &MainWindow::onApplyProxyButtonClicked);
    connect(cancelButton, &QPushButton::clicked, m_proxyDialog, &QDialog::reject);
    
    // 从配置管理器初始化值
    m_proxyEnabledCheckBox->setChecked(m_configManager->proxyEnabled());
    m_proxyHostEdit->setText(m_configManager->proxyHost());
    m_proxyPortSpinBox->setValue(m_configManager->proxyPort());
    m_proxyUsernameEdit->setText(m_configManager->proxyUsername());
    m_proxyPasswordEdit->setText(m_configManager->proxyPassword());
    
    // 初始化设置输入框状态
    onProxyCheckBoxToggled(m_proxyEnabledCheckBox->isChecked());
}

void MainWindow::onProxySettingsAction()
{
    // 创建代理设置对话框（如果尚未创建）
    if (!m_proxyDialog) {
        createProxySettingsDialog();
    } else {
        // 更新代理设置对话框的值
        updateUiFromConfig();
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
    
    // 应用设置到客户端，客户端会自动保存配置
    m_client->setProxy(enabled, host, port, username, password);
    
    // 关闭对话框
    m_proxyDialog->accept();
    
    QMessageBox::information(this, "代理设置", "代理设置已更新并保存到配置文件");
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
    
    // 禁用登录按钮并修改文本
    m_loginButton->setEnabled(false);
    m_loginButton->setText("发送验证码中...");
    
    // 更新状态栏
    m_statusLabel->setText("正在发送验证码...");
    
    // 请求验证码
    m_client->setApiCredentials(apiId.toInt(), apiHash);
    m_client->sendAuthenticationCode(phoneNumber);
    
    // 在2秒后恢复按钮状态（无论操作成功与否，回调函数都会处理UI更新）
    QTimer::singleShot(2000, this, [this]() {
        m_loginButton->setEnabled(true);
        m_loginButton->setText("登录");
    });
}

void MainWindow::onVerificationCodeEntered()
{
    QString code = m_codeEdit->text().trimmed();
    
    if (code.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入验证码");
        return;
    }
    
    // 禁用验证按钮并修改文本
    m_verifyButton->setEnabled(false);
    m_verifyButton->setText("验证中...");
    
    // 更新状态栏
    m_statusLabel->setText("正在验证登录...");
    
    // 验证码登录
    m_client->signIn(m_client->phoneNumber(), m_phoneCodeHash, code);
    
    // 在1秒后恢复按钮状态（无论登录成功与否，回调函数都会处理UI更新）
    QTimer::singleShot(1000, this, [this]() {
        m_verifyButton->setEnabled(true);
        m_verifyButton->setText("验证");
    });
}

void MainWindow::onGetMeClicked()
{
    m_client->getMe();
}

void MainWindow::onLoginSuccess(const QString& username)
{
    // 更新状态栏
    m_statusLabel->setText("登录成功，正在获取账户信息...");
    
    // 显示登录成功消息
    QMessageBox::information(this, "登录成功", "成功登录到Telegram！\n正在获取您的账户详细信息...");
    
    // 设置初始用户名显示
    m_usernameLabel->setText("<p style='text-align:center;'>正在加载账户信息...</p>");
    
    // 切换到主页面
    m_stackedWidget->setCurrentWidget(m_mainPage);
    
    // 自动获取账户详细信息
    QTimer::singleShot(500, this, [this]() {
        m_client->getMe();
    });
}

void MainWindow::onLoginFailed(const QString& error)
{
    // 更新状态栏
    m_statusLabel->setText("登录失败");
    
    // 显示错误消息
    QMessageBox::critical(this, "登录失败", "错误: " + error);
}

void MainWindow::onCodeRequested(const QString& phoneCodeHash)
{
    // 保存验证码哈希
    m_phoneCodeHash = phoneCodeHash;
    
    // 更新状态栏
    m_statusLabel->setText("验证码已发送，请输入");
    
    // 显示提示消息
    QMessageBox::information(this, "验证码已发送", "验证码已发送到您的手机，请查看短信或Telegram应用获取验证码。");
    
    // 清空验证码输入框并设置焦点
    m_codeEdit->clear();
    
    // 切换到验证页面
    m_stackedWidget->setCurrentWidget(m_verificationPage);
    
    // 设置焦点到验证码输入框
    QTimer::singleShot(100, this, [this]() {
        m_codeEdit->setFocus();
    });
}

void MainWindow::onAuthorizationError(const QString& error)
{
    // 更新状态栏
    m_statusLabel->setText("授权错误，请重新登录");
    
    // 显示错误消息
    QMessageBox::critical(this, "授权错误", "错误: " + error);
    
    // 切换回登录页面
    m_stackedWidget->setCurrentWidget(m_loginPage);
}

void MainWindow::onUserInfoReceived(const QString& username, const QString& firstName, const QString& lastName)
{
    // 使用HTML格式化显示用户信息
    QString htmlText = QString(
        "<table style='margin: 5px;'>"
        "<tr><td style='font-weight:bold;'>用户名:</td><td>%1</td></tr>"
        "<tr><td style='font-weight:bold;'>名字:</td><td>%2</td></tr>"
        "<tr><td style='font-weight:bold;'>姓氏:</td><td>%3</td></tr>"
        "<tr><td style='font-weight:bold;'>登录状态:</td><td>已登录</td></tr>"
        "</table>"
    ).arg(username, firstName, lastName);
    
    // 更新用户信息标签
    m_usernameLabel->setText(htmlText);
    
    // 显示成功获取信息的提示
    m_statusLabel->setText("已成功获取账户信息");
} 