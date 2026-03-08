#ifndef UIL_LOGINWINDOW_H
#define UIL_LOGINWINDOW_H

#include "All.h"
#include "BLL_user.h"

extern HandleMessage *m_handleMessage; // 全局消息处理器

/**
 * @brief 登录窗口类，处理用户登录和注册界面
 */
class loginwindow : public QWidget
{
    Q_OBJECT

public:
    loginwindow(QWidget *parent = nullptr); // 构造函数
    ~loginwindow();                         // 析构函数
    void initloginUI();                     // 初始化登录界面UI

private slots:
    // 用户交互槽函数
    void onLoginButtonClicked();      // 登录按钮点击
    void onRegisterButtonClicked();   // 注册按钮点击
    void onToRegisterButtonClicked(); // 切换到注册界面按钮点击
    void onToLoginButtonClicked();    // 切换到登录界面按钮点击

    // 网络响应处理槽函数
    void onRegisterResponse(bool success, const QString &message);                                                                                                 // 注册响应处理
    void onLoginResponse(bool success, const QString &message, const QString &username, const QString &password, const QString &type, unsigned long long balance); // 登录响应处理

private:
    // 输入组件
    QLineEdit *m_usernameEdit; // 用户名输入框
    QLineEdit *m_passwordEdit; // 密码输入框

    // 按钮组件
    QPushButton *loginButton;      // 登录按钮
    QPushButton *registerButton;   // 注册按钮
    QPushButton *toLoginButton;    // 切换到登录按钮
    QPushButton *toRegisterButton; // 切换到注册按钮

    // 选择组件
    QCheckBox *iAmMerachant; // 商家身份选择框

signals:
    void loginwindowclosed(const QString &username, const QString &password, const QString &type, unsigned long long balance); // 登录窗口关闭信号
};

#endif // UIL_LOGINWINDOW_H
