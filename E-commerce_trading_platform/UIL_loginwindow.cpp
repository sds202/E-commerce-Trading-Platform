#include "UIL_loginwindow.h"

loginwindow::loginwindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("登录注册");
    initloginUI();

    connect(loginButton, &QPushButton::clicked, this, &loginwindow::onLoginButtonClicked);
    connect(registerButton, &QPushButton::clicked, this, &loginwindow::onRegisterButtonClicked);
    connect(toRegisterButton, &QPushButton::clicked, this, &loginwindow::onToRegisterButtonClicked);
    connect(toLoginButton, &QPushButton::clicked, this, &loginwindow::onToLoginButtonClicked);

    connect(m_handleMessage, &HandleMessage::registerResponse,
            this, &loginwindow::onRegisterResponse);
    connect(m_handleMessage, &HandleMessage::loginResponse,
            this, &loginwindow::onLoginResponse);
}
loginwindow::~loginwindow()
{
}

void loginwindow::initloginUI()
{
    iAmMerachant = new QCheckBox("我是商家");
    iAmMerachant->hide();
    // 创建标签
    QLabel *usernameLabel = new QLabel("用户名:");
    QLabel *passwordLabel = new QLabel("密码:");

    // 创建输入框
    m_usernameEdit = new QLineEdit();
    m_passwordEdit = new QLineEdit();
    m_passwordEdit->setEchoMode(QLineEdit::Password); // 密码输入框

    // 创建按钮
    loginButton = new QPushButton("登录");
    registerButton = new QPushButton("注册");
    toLoginButton = new QPushButton("已有账号，我要登录");
    toLoginButton->setStyleSheet(
        "QPushButton {"
        "    border: none;"               // 移除边框
        "    background: transparent;"    // 透明背景
        "    color: blue;"                // 设置文本颜色（类似超链接）
        "    text-decoration: underline;" // 添加下划线，模拟超链接
        "}"
        "QPushButton:hover {"
        "    color: darkblue;" // 鼠标悬停时改变颜色
        "    text-decoration: underline;"
        "}");
    toRegisterButton = new QPushButton("没有账号，我要注册");
    toRegisterButton->setStyleSheet(
        "QPushButton {"
        "    border: none;"               // 移除边框
        "    background: transparent;"    // 透明背景
        "    color: blue;"                // 设置文本颜色（类似超链接）
        "    text-decoration: underline;" // 添加下划线，模拟超链接
        "}"
        "QPushButton:hover {"
        "    color: darkblue;" // 鼠标悬停时改变颜色
        "    text-decoration: underline;"
        "}");

    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QHBoxLayout *usernameLayout = new QHBoxLayout();
    QHBoxLayout *passwordLayout = new QHBoxLayout();
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QVBoxLayout *l_buttonLayout = new QVBoxLayout();
    QVBoxLayout *r_buttonLayout = new QVBoxLayout();

    // 布局设置
    usernameLayout->addWidget(usernameLabel);
    usernameLayout->addWidget(m_usernameEdit);

    passwordLayout->addWidget(passwordLabel);
    passwordLayout->addWidget(m_passwordEdit);

    buttonLayout->addLayout(l_buttonLayout);
    buttonLayout->addLayout(r_buttonLayout);

    l_buttonLayout->addWidget(loginButton);
    l_buttonLayout->addWidget(registerButton);
    r_buttonLayout->addWidget(toLoginButton);
    r_buttonLayout->addWidget(toRegisterButton);

    mainLayout->addLayout(usernameLayout);
    mainLayout->addLayout(passwordLayout);
    mainLayout->addWidget(iAmMerachant);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    toLoginButton->hide();
    registerButton->hide();
}

// 登录按钮和收到登录报文
void loginwindow::onLoginButtonClicked()
{
    QString username = m_usernameEdit->text();
    QString password = m_passwordEdit->text();

    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "输入错误", "用户名和密码不能为空!");
        return;
    }

    QRegularExpression regex("^[a-zA-Z0-9]+$");
    QRegularExpressionMatch usernameMatch = regex.match(username);
    QRegularExpressionMatch passwordMatch = regex.match(password);

    if (!usernameMatch.hasMatch())
    {
        QMessageBox::warning(this, "输入错误", "用户名只能包含字母或数字!");
        return;
    }
    if (username.length() > 20)
    {
        QMessageBox::warning(this, "输入错误", "用户名长度不能超过20个字符!");
        return;
    }

    if (!passwordMatch.hasMatch())
    {
        QMessageBox::warning(this, "输入错误", "密码只能包含字母或数字!");
        return;
    }
    if (password.length() > 20)
    {
        QMessageBox::warning(this, "输入错误", "密码长度不能超过20个字符!");
        return;
    }

    User user;
    user.userLogin(username, password);
    loginButton->setEnabled(false);
}
void loginwindow::onLoginResponse(bool success, const QString &message, const QString &username, const QString &password, const QString &type, unsigned long long balance)
{
    loginButton->setEnabled(true);
    if (success)
    {
        QMessageBox::information(this, "登录成功", "欢迎 " + username);
        emit loginwindowclosed(username, password, type, balance);
        this->close();
    }
    else
    {
        QMessageBox::warning(this, "登录失败", message);
    }
}

// 注册按钮和收到注册报文
void loginwindow::onRegisterButtonClicked()
{
    QString username = m_usernameEdit->text();
    QString password = m_passwordEdit->text();

    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "输入错误", "用户名和密码不能为空!");
        return;
    }

    QRegularExpression regex("^[a-zA-Z0-9]+$");
    QRegularExpressionMatch usernameMatch = regex.match(username);
    QRegularExpressionMatch passwordMatch = regex.match(password);

    if (!usernameMatch.hasMatch())
    {
        QMessageBox::warning(this, "输入错误", "用户名只能包含字母或数字!");
        return;
    }
    if (username.length() > 20)
    {
        QMessageBox::warning(this, "输入错误", "用户名长度不能超过20个字符!");
        return;
    }

    if (!passwordMatch.hasMatch())
    {
        QMessageBox::warning(this, "输入错误", "密码只能包含字母或数字!");
        return;
    }
    if (password.length() > 20)
    {
        QMessageBox::warning(this, "输入错误", "密码长度不能超过20个字符!");
        return;
    }

    User user;
    QString kind = iAmMerachant->isChecked() ? "Merchant" : "Customer";
    user.userRegister(username, password, kind);
    registerButton->setEnabled(false);
}
void loginwindow::onRegisterResponse(bool success, const QString &message)
{
    if (success)
    {
        QMessageBox::information(this, "注册成功", "用户注册成功！");
        onToLoginButtonClicked();
    }
    else
    {
        QMessageBox::warning(this, "注册失败", message);
    }
    registerButton->setEnabled(true);
}

// 两个转换按钮
void loginwindow::onToRegisterButtonClicked()
{
    // 隐藏登录按钮和“没有账号，我要注册”按钮
    loginButton->hide();
    toRegisterButton->hide();
    // 显示注册按钮和“已有账号，我要登录”按钮
    registerButton->show();
    toLoginButton->show();
    iAmMerachant->show();
}
void loginwindow::onToLoginButtonClicked()
{
    // 隐藏注册按钮和“已有账号，我要登录”按钮
    registerButton->hide();
    toLoginButton->hide();
    iAmMerachant->hide();
    // 显示登录按钮和“没有账号，我要注册”按钮
    loginButton->show();
    toRegisterButton->show();
}
