#include "login_page.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

LoginPage::LoginPage(UserAuth *auth, QWidget *parent)
    : QWidget(parent), m_auth(auth) {
    
    // Set fixed window size
    setFixedSize(400, 400);
    
    // Create background widget with Tic Tac Toe board
    m_backgroundWidget = new BackgroundWidget(BackgroundWidget::Mode::TicTacToeBoard, this);
    m_backgroundWidget->setGeometry(0, 0, width(), height());
    
    // Create logo with gradient text (split into three parts)
    QFont logoFont;
    logoFont.setFamilies(QStringList() << "Comic Sans MS" << "Arial");
    logoFont.setPointSize(32);
    logoFont.setBold(true);

    QLabel *ticLabel = new QLabel("TIC", this);
    ticLabel->setFont(logoFont);
    ticLabel->setStyleSheet("color: #FF69B4; text-shadow: 2px 2px 2px rgba(0, 0, 0, 0.5);");  // Pink

    QLabel *tacLabel = new QLabel("TAC", this);
    tacLabel->setFont(logoFont);
    tacLabel->setStyleSheet("color: #FFA500; text-shadow: 2px 2px 2px rgba(0, 0, 0, 0.5);");  // Orange

    QLabel *toeLabel = new QLabel("TOE", this);
    toeLabel->setFont(logoFont);
    toeLabel->setStyleSheet("color: #9370DB; text-shadow: 2px 2px 2px rgba(0, 0, 0, 0.5);");  // Purple

    QHBoxLayout *logoLayout = new QHBoxLayout();
    logoLayout->addWidget(ticLabel);
    logoLayout->addWidget(tacLabel);
    logoLayout->addWidget(toeLabel);
    logoLayout->setAlignment(Qt::AlignCenter);
    logoLayout->setSpacing(0);

    // Create widgets
    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setObjectName("m_usernameEdit");
    m_usernameEdit->setStyleSheet(
        "border: 2px solid #0000FF; "
        "background-color: white; "
        "padding: 5px; "
        "border-radius: 5px;"
    );
    m_usernameEdit->setFixedWidth(200);
    
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setObjectName("m_passwordEdit");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setStyleSheet(
        "border: 2px solid #0000FF; "
        "background-color: white; "
        "padding: 5px; "
        "border-radius: 5px;"
    );
    m_passwordEdit->setFixedWidth(200);
    
    m_loginButton = new QPushButton("Login", this);
    m_loginButton->setObjectName("m_loginButton");
    m_loginButton->setStyleSheet(
        "QPushButton {"
        "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #1E90FF, stop:1 #0000FF); "
        "color: white; "
        "padding: 8px; "
        "border-radius: 5px; "
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4682B4, stop:1 #1E90FF);"
        "}"
    );
    m_loginButton->setFixedWidth(100);
    
    m_registerButton = new QPushButton("Register", this);
    m_registerButton->setObjectName("m_registerButton");
    m_registerButton->setStyleSheet(
        "QPushButton {"
        "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FF4500, stop:1 #FF0000); "
        "color: white; "
        "padding: 8px; "
        "border-radius: 5px; "
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FF6347, stop:1 #FF4500);"
        "}"
    );
    m_registerButton->setFixedWidth(100);
    
    m_statusLabel = new QLabel(this);
    m_statusLabel->setObjectName("m_statusLabel");
    m_statusLabel->setStyleSheet("color: #FF0000; font-weight: bold;");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    
    // Create layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(logoLayout);
    mainLayout->addSpacing(20);
    
    QFormLayout *formLayout = new QFormLayout();
    QLabel *usernameLabel = new QLabel("Username:");
    usernameLabel->setStyleSheet("color: #0000FF; font-weight: bold;");
    formLayout->addRow(usernameLabel, m_usernameEdit);
    
    QLabel *passwordLabel = new QLabel("Password:");
    passwordLabel->setStyleSheet("color: #0000FF; font-weight: bold;");
    formLayout->addRow(passwordLabel, m_passwordEdit);
    formLayout->setHorizontalSpacing(10);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_loginButton);
    buttonLayout->addWidget(m_registerButton);
    buttonLayout->setAlignment(Qt::AlignCenter);
    
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(m_statusLabel);
    mainLayout->addStretch();
    
    // Ensure background widget is behind other widgets
    m_backgroundWidget->lower();
    
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
    std::string usernameStr = username.toStdString();
    std::string passwordStr = password.toStdString();

    if (usernameStr.empty()) {
        m_statusLabel->setText("Username cannot be empty");
        return;
    }
    if (passwordStr.empty()) {
        m_statusLabel->setText("Password cannot be empty");
        return;
    }
    if (passwordStr.length() < 5) {
        m_statusLabel->setText("Password must be at least 5 characters");
        return;
    }
    bool hasLetter = false, hasDigit = false;
    for (char c : passwordStr) {
        if (std::isalpha(static_cast<unsigned char>(c))) hasLetter = true;
        if (std::isdigit(static_cast<unsigned char>(c))) hasDigit = true;
    }
    if (!hasLetter || !hasDigit) {
        m_statusLabel->setText("Password must contain both letters and digits");
        return;
    }

    if (m_auth->registerUser(usernameStr, passwordStr)) {
        m_statusLabel->setText("User registered successfully");
        clearFields();
    } else {
        m_statusLabel->setText("Registration failed. Username may already exist.");
    }
}

void LoginPage::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    m_backgroundWidget->setGeometry(0, 0, width(), height());
}