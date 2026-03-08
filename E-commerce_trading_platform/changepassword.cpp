
#include "changepassword.h"

PasswordDialog::PasswordDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("修改密码");

    // 创建输入框和标签
    QLabel *oldPasswordLabel = new QLabel("原密码:", this);
    oldPasswordEdit = new QLineEdit(this);
    oldPasswordEdit->setEchoMode(QLineEdit::Password);

    QLabel *newPasswordLabel = new QLabel("新密码:", this);
    newPasswordEdit = new QLineEdit(this);
    newPasswordEdit->setEchoMode(QLineEdit::Password);

    QLabel *confirmPasswordLabel = new QLabel("确认新密码:", this);
    confirmPasswordEdit = new QLineEdit(this);
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);

    // 创建按钮
    QPushButton *confirmButton = new QPushButton("确认", this);
    QPushButton *cancelButton = new QPushButton("取消", this);

    // 布局
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *oldPasswordLayout = new QHBoxLayout;
    QHBoxLayout *newPasswordLayout = new QHBoxLayout;
    QHBoxLayout *confirmPasswordLayout = new QHBoxLayout;
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    oldPasswordLayout->addWidget(oldPasswordLabel);
    oldPasswordLayout->addWidget(oldPasswordEdit);
    newPasswordLayout->addWidget(newPasswordLabel);
    newPasswordLayout->addWidget(newPasswordEdit);
    confirmPasswordLayout->addWidget(confirmPasswordLabel);
    confirmPasswordLayout->addWidget(confirmPasswordEdit);
    buttonLayout->addWidget(confirmButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(oldPasswordLayout);
    mainLayout->addLayout(newPasswordLayout);
    mainLayout->addLayout(confirmPasswordLayout);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    // 连接信号和槽
    connect(confirmButton, &QPushButton::clicked, this, &PasswordDialog::onConfirmClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QString PasswordDialog::getNewPassword() const
{
    return newPassword;
}
QString PasswordDialog::getOldPassword() const
{
    return oldPasswordEdit->text();
}

void PasswordDialog::onConfirmClicked()
{
    QString oldPassword = oldPasswordEdit->text();
    QString newPassword1 = newPasswordEdit->text();
    QString newPassword2 = confirmPasswordEdit->text();

    if (oldPassword.isEmpty() || newPassword1.isEmpty() || newPassword2.isEmpty())
    {
        QMessageBox::warning(this, "错误", "所有字段都必须填写！");
        return;
    }

    if (newPassword1 != newPassword2)
    {
        QMessageBox::warning(this, "错误", "两次输入的新密码不一致！");
        return;
    }

    newPassword = newPassword1;
    accept();
}
