
#include "login_dialog.h"

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(" 系统登录 ");
    setFixedSize(300, 200);

    // 创建界面元素
    QLabel *usernameLabel = new QLabel(" 用户名: ", this);
    QLabel *passwordLabel = new QLabel(" 密码: ", this);

    m_usernameEdit = new QLineEdit(this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    m_loginButton = new QPushButton("  登录  ", this);
    m_cancelButton = new QPushButton("  取消  ", this);

    // 布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    mainLayout->addWidget(usernameLabel);
    mainLayout->addWidget(m_usernameEdit);
    mainLayout->addWidget(passwordLabel);
    mainLayout->addWidget(m_passwordEdit);

    buttonLayout->addWidget(m_loginButton);
    buttonLayout->addWidget(m_cancelButton);
    mainLayout->addLayout(buttonLayout);

    // 连接信号与槽
    connect(m_loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginButtonClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &LoginDialog::onCancelButtonClicked);
}

LoginDialog::~LoginDialog()
{
    // 自动释放子控件
}

QString LoginDialog::getUsername() const
{
    return m_usernameEdit->text();
}

QString LoginDialog::getPassword() const
{
    return m_passwordEdit->text();
}

void LoginDialog::onLoginButtonClicked()
{
    if (getUsername().isEmpty() || getPassword().isEmpty()) {
        QMessageBox::warning(this, "  登录失败  ", "  用户名或密码不能为空!  ");
        return;
    }

    // 验证逻辑将在Widget类中实现
    accept(); // 关闭对话框并返回Accepted状态
}

void LoginDialog::onCancelButtonClicked()
{
    reject(); // 关闭对话框并返回Rejected状态
}
