#include "login_page.h"
#include "ui_constants.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>

LoginPage::LoginPage(UserAuth *auth, QWidget *parent)
    : QMainWindow(parent), m_auth(auth), m_mode(LoginPageMode::InitialLogin) {
    setupUI();
}

void LoginPage::setupUI() {
    // Set window properties
    setWindowTitle("Tic-Tac-Toe Login");
    setFixedSize(UIConstants::WindowSize::LOGIN_WIDTH, UIConstants::WindowSize::LOGIN_HEIGHT);
    
    // Create and set central widget with gradient background
    m_centralWidget = new QWidget(this);
    m_centralWidget->setStyleSheet("QWidget { background-color: #e8eff1; }");
    setCentralWidget(m_centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(m_centralWidget);
    mainLayout->setSpacing(UIConstants::Spacing::GAME_ELEMENT_SPACING);
    mainLayout->setContentsMargins(UIConstants::Spacing::LOGIN_VERTICAL_MARGIN, 
                                  UIConstants::Spacing::LOGIN_VERTICAL_MARGIN, 
                                  UIConstants::Spacing::LOGIN_VERTICAL_MARGIN, 
                                  UIConstants::Spacing::LOGIN_VERTICAL_MARGIN);

    // Create title label for different modes
    m_titleLabel = new QLabel("Welcome to Tic-Tac-Toe!");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont;
    titleFont.setFamilies(QStringList() << "Segoe UI" << "Arial");
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setStyleSheet("color: #34495e; margin-bottom: 10px;");

    // Create logo with gradient text
    QFont logoFont;
    logoFont.setFamilies(QStringList() << "Segoe UI" << "Arial");
    logoFont.setPointSize(UIConstants::Font::LOGO_SIZE);
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
    m_usernameEdit->setPlaceholderText("Enter your username");
    m_usernameEdit->setFixedSize(UIConstants::InputField::WIDTH, UIConstants::InputField::HEIGHT);
    m_usernameEdit->setStyleSheet(
        "QLineEdit {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    font-size: " + QString::number(UIConstants::Font::INPUT_TEXT_SIZE) + "px;"
        "    color: #2c3e50;"
        "    background-color: white;"
        "    border: " + QString::number(UIConstants::Style::BORDER_WIDTH) + "px solid #bdc3c7;"
        "    border-radius: " + QString::number(UIConstants::Style::LARGE_BORDER_RADIUS) + "px;"
        "    padding: " + QString::number(UIConstants::Padding::INPUT_VERTICAL) + "px " + QString::number(UIConstants::Padding::INPUT_HORIZONTAL) + "px;"
        "    selection-background-color: #3498db;"
        "    selection-color: white;"
        "}"
        "QLineEdit:focus {"
        "    border-color: #3498db;"
        "    outline: none;"
        "    background-color: #f8f9fa;"
        "}"
        "QLineEdit:hover {"
        "    border-color: #85929e;"
        "    background-color: #f8f9fa;"
        "}"
        "QLineEdit::placeholder {"
        "    color: #7f8c8d;"
        "    font-style: normal;"
        "    font-size: " + QString::number(UIConstants::Font::PLACEHOLDER_SIZE) + "px;"
        "}"
    );

    m_passwordEdit = new QLineEdit();
    m_passwordEdit->setObjectName("m_passwordEdit");
    m_passwordEdit->setPlaceholderText("Enter your password");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setFixedSize(UIConstants::InputField::WIDTH, UIConstants::InputField::HEIGHT);
    m_passwordEdit->setStyleSheet(m_usernameEdit->styleSheet());

    // Create status label
    m_statusLabel = new QLabel();
    m_statusLabel->setObjectName("m_statusLabel");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet(
        "QLabel {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    font-size: " + QString::number(UIConstants::Font::BUTTON_SIZE) + "px;"
        "    color: #e74c3c;"
        "    font-weight: bold;"
        "    background: transparent;"
        "    border: none;"
        "    padding: 0px;"
        "}"
    );

    // Create buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(UIConstants::Spacing::LOGIN_BUTTON_SPACING);
    buttonLayout->setAlignment(Qt::AlignCenter);

    m_loginButton = new QPushButton("Login");
    m_loginButton->setObjectName("m_loginButton");
    m_loginButton->setStyleSheet(getBaseButtonStyle("#5dade2", "#4a9fcc"));
    m_loginButton->setCursor(Qt::PointingHandCursor);
    m_loginButton->setMinimumWidth(UIConstants::Button::MIN_WIDTH);

    m_registerButton = new QPushButton("Register");
    m_registerButton->setObjectName("m_registerButton");
    m_registerButton->setStyleSheet(getBaseButtonStyle("#e67e22", "#d35400"));
    m_registerButton->setCursor(Qt::PointingHandCursor);
    m_registerButton->setMinimumWidth(UIConstants::Button::MIN_WIDTH);

    m_backButton = new QPushButton("Back");
    m_backButton->setObjectName("m_backButton");
    m_backButton->setStyleSheet(getBaseButtonStyle("#95a5a6", "#7f8c8d"));
    m_backButton->setCursor(Qt::PointingHandCursor);
    m_backButton->setMinimumWidth(UIConstants::Button::MIN_WIDTH);
    m_backButton->setVisible(false); // Hidden by default

    buttonLayout->addWidget(m_backButton);
    buttonLayout->addWidget(m_loginButton);
    buttonLayout->addWidget(m_registerButton);

    // Add input fields with generous spacing
    QVBoxLayout *inputLayout = new QVBoxLayout();
    inputLayout->setSpacing(UIConstants::Spacing::LOGIN_FIELD_SPACING);
    inputLayout->setContentsMargins(0, UIConstants::Spacing::LOGIN_VERTICAL_MARGIN, 0, UIConstants::Spacing::LOGIN_VERTICAL_MARGIN);
    
    inputLayout->addWidget(m_usernameEdit, 0, Qt::AlignCenter);
    inputLayout->addWidget(m_passwordEdit, 0, Qt::AlignCenter);

    formLayout->addLayout(inputLayout);
    formLayout->addSpacing(UIConstants::Spacing::LOGIN_FORM_SPACING);
    formLayout->addLayout(buttonLayout);
    formLayout->addSpacing(UIConstants::Spacing::GAME_SETUP_SPACING);
    formLayout->addWidget(m_statusLabel);

    // Add everything to main layout
    mainLayout->addWidget(m_titleLabel);
    mainLayout->addLayout(logoLayout);
    mainLayout->addWidget(formContainer, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    // Connect signals
    connect(m_loginButton, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    connect(m_registerButton, &QPushButton::clicked, this, &LoginPage::onRegisterClicked);
    connect(m_backButton, &QPushButton::clicked, this, &LoginPage::onBackClicked);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginPage::onLoginClicked);
    
    // Apply initial mode state
    updateUIForMode();
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
        "    font-size: %4px;"
        "    font-weight: bold;"
        "    color: white;"
        "    background-color: %1;"
        "    border: none;"
        "    border-radius: %5px;"
        "    padding: %6px %7px;"
        "    min-width: %8px;"
        "}"
        "QPushButton:hover {"
        "    background-color: %2;"
        "}"
        "QPushButton:pressed {"
        "    background-color: %3;"
        "}"
    ).arg(bgColor, borderColor, borderColor)
     .arg(UIConstants::Font::BUTTON_SIZE)
     .arg(UIConstants::Style::BORDER_RADIUS)
     .arg(UIConstants::Padding::BUTTON_VERTICAL)
     .arg(UIConstants::Padding::BUTTON_HORIZONTAL)
     .arg(UIConstants::Button::STANDARD_MIN_WIDTH);
}

void LoginPage::clearFields() {
    m_usernameEdit->clear();
    m_passwordEdit->clear();
}

void LoginPage::setMode(LoginPageMode mode, const QString &firstPlayerName) {
    m_mode = mode;
    m_firstPlayerName = firstPlayerName;
    updateUIForMode();
}

void LoginPage::updateUIForMode() {
    switch (m_mode) {
        case LoginPageMode::InitialLogin:
            m_titleLabel->setText("Welcome to Tic-Tac-Toe!");
            m_backButton->setVisible(false);
            m_registerButton->setVisible(true);
            break;
        case LoginPageMode::SecondPlayerLogin:
            m_titleLabel->setText("Player 2 Authentication");
            m_backButton->setVisible(true);
            m_registerButton->setVisible(true); // Keep register button for second player
            break;
    }
    // Clear fields and status when mode changes
    m_usernameEdit->clear();
    m_passwordEdit->clear();
    m_statusLabel->clear();
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
        if (m_mode == LoginPageMode::InitialLogin) {
            emit loginSuccessful(username);
        } else {
            // Check if second player is trying to use same username as first player
            if (username == m_firstPlayerName) {
                m_statusLabel->setText("Player 2 cannot use the same username as Player 1");
                return;
            }
            emit secondPlayerLoginSuccessful(username);
        }
    } else {
        m_statusLabel->setText("Login failed. Invalid username or password.");
    }
}

void LoginPage::onBackClicked() {
    emit backRequested();
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

    // Check if second player is trying to register with same username as first player
    if (m_mode == LoginPageMode::SecondPlayerLogin && username == m_firstPlayerName) {
        m_statusLabel->setText("Player 2 cannot register with the same username as Player 1");
        return;
    }

    if (m_auth->registerUser(usernameStr, passwordStr)) {
        m_statusLabel->setText("User registered successfully");
        m_usernameEdit->clear();
        m_passwordEdit->clear();
    } else {
        m_statusLabel->setText("Registration failed. Username may already exist.");
    }
}

void LoginPage::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}
