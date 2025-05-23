#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QMessageBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QStatusBar>

#include "core/telegram_client.h"
#include "core/config_manager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 登录相关槽函数
    void onLoginButtonClicked();
    void onVerificationCodeEntered();
    void onGetMeClicked();

    // 客户端信号响应槽
    void onLoginSuccess(const QString& username);
    void onLoginFailed(const QString& error);
    void onCodeRequested(const QString& phoneCodeHash);
    void onAuthorizationError(const QString& error);
    void onUserInfoReceived(const QString& username, const QString& firstName, const QString& lastName);
    
    // 代理设置相关槽
    void onProxySettingsAction();
    void onProxyCheckBoxToggled(bool checked);
    void onApplyProxyButtonClicked();
    
    // 配置管理器相关槽
    void onConfigLoaded();
    void onConfigSaved();
    
    // 显示配置文件路径
    void onShowConfigFilePathAction();

private:
    void setupUi();
    void createLoginPage();
    void createVerificationPage();
    void createMainPage();
    void createProxySettingsDialog();
    void createMenuBar();
    void createStatusBar();
    
    // 初始化界面值
    void initializeWithConfig();
    void updateUiFromConfig();

    // UI组件
    QStackedWidget* m_stackedWidget;
    
    // 登录页面
    QWidget* m_loginPage;
    QLineEdit* m_apiIdEdit;
    QLineEdit* m_apiHashEdit;
    QLineEdit* m_phoneNumberEdit;
    QPushButton* m_loginButton;
    
    // 验证码页面
    QWidget* m_verificationPage;
    QLineEdit* m_codeEdit;
    QPushButton* m_verifyButton;
    
    // 主页面
    QWidget* m_mainPage;
    QLabel* m_usernameLabel;
    QPushButton* m_getMeButton;
    
    // 代理设置对话框
    QDialog* m_proxyDialog;
    QCheckBox* m_proxyEnabledCheckBox;
    QLineEdit* m_proxyHostEdit;
    QSpinBox* m_proxyPortSpinBox;
    QLineEdit* m_proxyUsernameEdit;
    QLineEdit* m_proxyPasswordEdit;
    QPushButton* m_applyProxyButton;
    
    // 状态栏
    QLabel* m_statusLabel;
    
    // 客户端核心
    TelegramClient* m_client;
    QString m_phoneCodeHash;
    
    // 配置管理器
    ConfigManager* m_configManager;
}; 