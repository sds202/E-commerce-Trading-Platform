#ifndef UIL_CHANGEPASSWORD_H
#define UIL_CHANGEPASSWORD_H

#include "All.h"

/**
 * @brief 密码修改对话框类，处理用户密码修改界面
 */
class PasswordDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PasswordDialog(QWidget *parent = nullptr); // 构造函数

    // 获取方法
    QString getOldPassword() const; // 获取旧密码
    QString getNewPassword() const; // 获取新密码

private slots:
    void onConfirmClicked(); // 确认按钮点击槽函数

private:
    // 输入组件
    QLineEdit *oldPasswordEdit;     // 旧密码输入框
    QLineEdit *newPasswordEdit;     // 新密码输入框
    QLineEdit *confirmPasswordEdit; // 确认密码输入框

    // 数据存储
    QString newPassword; // 验证通过的新密码
};

#endif // UIL_CHANGEPASSWORD_H
