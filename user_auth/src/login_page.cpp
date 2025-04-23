#include "login_page.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

LoginPage::LoginPage(UserAuth *auth, QWidget *parent)
    : QWidget(parent), m_auth(auth) {
    
    // Create widgets
    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setObjectName("m_usernameEdit");
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setObjectName("m_passwordEdit");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    
    m_loginButton = new QPushButton("Login", this);
    m_loginButton->setObjectName("m_loginButton");
    m_registerButton = new QPushButton("Register", this);
    m_registerButton->setObjectName("m_registerButton");
    
    m_statusLabel = new QLabel(this);
    m_statusLabel->setObjectName("m_statusLabel");
    m_statusLabel->setStyleSheet("color: red;");
    
    // Create layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("Username:", m_usernameEdit);
    formLayout->addRow("Password:", m_passwordEdit);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_loginButton);
    buttonLayout->addWidget(m_registerButton);
    
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(m_statusLabel);
    mainLayout->addStretch();
    
    // Connect signals to slots
    connect(m_loginButton, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    connect(m_registerButton, &QPushButton::clicked, this, &LoginPage::onRegisterClicked);
    
    // Allow pressing Enter to login
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginPage::onLoginClicked);
}

void LoginPage::clearFields() {
    m_usernameEdit->clear();
    m_passwordEdit->clear();
}

void LoginPage::onLoginClicked() {
    QString username = m_usernameEdit->text();
    QString password = m_passwordEdit->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        m_statusLabel->setText("Username and password cannot be empty");
        return;
    }
    
    if (m_auth->login(username.toStdString(), password.toStdString())) {
        m_statusLabel->setText("");
        emit loginSuccessful(username);
    } else {
        m_statusLabel->setText("Login failed. Invalid username or password.");
    }
}

void LoginPage::onRegisterClicked() {
    QString username = m_usernameEdit->text();
    QString password = m_passwordEdit->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        m_statusLabel->setText("Username and password cannot be empty");
        return;
    }
    
    if (m_auth->registerUser(username.toStdString(), password.toStdString())) {
        m_statusLabel->setText("User registered successfully");
        clearFields();
    } else {
        m_statusLabel->setText("Registration failed. Username may already exist or password is invalid.");
    }
}