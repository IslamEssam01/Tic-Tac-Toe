#include "login_page.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>

LoginPage::LoginPage(UserAuth *auth, QWidget *parent)
    : QMainWindow(parent), m_auth(auth) {
    setupUI();
}

void LoginPage::setupUI() {
    // Set window properties
    setWindowTitle("Tic-Tac-Toe Login");
    setFixedSize(500, 400);
    
    // Create and set central widget with gradient background
    m_centralWidget = new QWidget(this);
    m_centralWidget->setStyleSheet("QWidget { background-color: #e8eff1; }");
    setCentralWidget(m_centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(m_centralWidget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Create logo with gradient text
    QFont logoFont;
    logoFont.setFamilies(QStringList() << "Segoe UI" << "Arial");
    logoFont.setPointSize(32);
    logoFont.setBold(true);

    QHBoxLayout *logoLayout = new QHBoxLayout();
    
    QLabel *ticLabel = new QLabel("TIC");
    ticLabel->setFont(logoFont);
    ticLabel->setStyleSheet("color: #5dade2;");

    QLabel *tacLabel = new QLabel("TAC");
    tacLabel->setFont(logoFont);
    tacLabel->setStyleSheet("color: #e67e22;");

    QLabel *toeLabel = new QLabel("TOE");
    toeLabel->setFont(logoFont);
    toeLabel->setStyleSheet("color: #58d68d;");

    logoLayout->addWidget(ticLabel, 0, Qt::AlignCenter);
    logoLayout->addWidget(tacLabel, 0, Qt::AlignCenter);
    logoLayout->addWidget(toeLabel, 0, Qt::AlignCenter);
    logoLayout->setSpacing(0);

    // Create form container
    QWidget *formContainer = createFormContainer();
    QVBoxLayout *formLayout = new QVBoxLayout(formContainer);
    formLayout->setSpacing(15);
    formLayout->setContentsMargins(20, 20, 20, 20);

    // Create input fields
    m_usernameEdit = new QLineEdit();
    m_usernameEdit->setObjectName("m_usernameEdit");
    m_usernameEdit->setPlaceholderText("Username");
    m_usernameEdit->setFixedWidth(300);
    m_usernameEdit->setStyleSheet(
        "QLineEdit {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    font-size: 16px;"
        "    color: #34495e;"
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #fdfefe, stop:1 #e8eff1);"
        "    border: 1px solid #dce4e8;"
        "    border-radius: 8px;"
        "    padding: 12px;"
        "}"
        "QLineEdit:focus {"
        "    border-color: #5dade2;"
        "    outline: none;"
        "}"
        "QLineEdit:hover {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ffffff, stop:1 #e0e9ec);"
        "    border-color: #c8d0d4;"
        "}"
    );

    m_passwordEdit = new QLineEdit();
    m_passwordEdit->setObjectName("m_passwordEdit");
    m_passwordEdit->setPlaceholderText("Password");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setFixedWidth(300);
    m_passwordEdit->setStyleSheet(m_usernameEdit->styleSheet());

    // Create status label
    m_statusLabel = new QLabel();
    m_statusLabel->setObjectName("m_statusLabel");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet(
        "QLabel {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    font-size: 16px;"
        "    color: #e74c3c;"
        "    font-weight: bold;"
        "    background: transparent;"
        "    border: none;"
        "    padding: 0px;"
        "}"
    );

    // Create buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    m_loginButton = new QPushButton("Login");
    m_loginButton->setObjectName("m_loginButton");
    m_loginButton->setStyleSheet(getBaseButtonStyle("#5dade2", "#4a9fcc"));
    m_loginButton->setCursor(Qt::PointingHandCursor);

    m_registerButton = new QPushButton("Register");
    m_registerButton->setObjectName("m_registerButton");
    m_registerButton->setStyleSheet(getBaseButtonStyle("#e67e22", "#d35400"));
    m_registerButton->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(m_loginButton);
    buttonLayout->addWidget(m_registerButton);

    // Add widgets to form layout
    QFormLayout *inputLayout = new QFormLayout();
    inputLayout->setSpacing(10);
    
    QLabel *usernameLabel = new QLabel("Username:");
    usernameLabel->setStyleSheet("color: #34495e; font-weight: bold; font-family: 'Segoe UI', sans-serif;");
    inputLayout->addRow(usernameLabel, m_usernameEdit);
    
    QLabel *passwordLabel = new QLabel("Password:");
    passwordLabel->setStyleSheet("color: #34495e; font-weight: bold; font-family: 'Segoe UI', sans-serif;");
    inputLayout->addRow(passwordLabel, m_passwordEdit);

    formLayout->addLayout(inputLayout);
    formLayout->addLayout(buttonLayout);
    formLayout->addWidget(m_statusLabel);

    // Add everything to main layout
    mainLayout->addLayout(logoLayout);
    mainLayout->addWidget(formContainer, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    // Connect signals
    connect(m_loginButton, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    connect(m_registerButton, &QPushButton::clicked, this, &LoginPage::onRegisterClicked);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginPage::onLoginClicked);
}

QWidget* LoginPage::createFormContainer() {
    QWidget *container = new QWidget();
    container->setStyleSheet(
        "QWidget {"
        "    background-color: white;"
        "    border-radius: 8px;"
        "    border: 1px solid #dce4e8;"
        "}"
    );
    
    // Add drop shadow effect
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 50));
    shadow->setOffset(0, 2);
    container->setGraphicsEffect(shadow);
    
    return container;
}

QString LoginPage::getBaseButtonStyle(const QString &bgColor, const QString &borderColor) const {
    return QString(
        "QPushButton {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    color: white;"
        "    background-color: %1;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 12px 25px;"
        "    min-width: 120px;"
        "}"
        "QPushButton:hover {"
        "    background-color: %2;"
        "}"
        "QPushButton:pressed {"
        "    background-color: %3;"
        "}"
    ).arg(bgColor, borderColor, borderColor);
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
}
