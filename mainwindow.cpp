#include "mainwindow.h"
#include <QTextEdit>
#include <QScrollArea>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Чат приложение");
    setMinimumSize(600, 400);
    
    // Создаем пути для файлов в директории приложения
    QString appDir = QCoreApplication::applicationDirPath();
    QDir().mkpath(appDir + "/data");
    QDir().mkpath(appDir + "/data/archive");
    
    chatFilePath = appDir + "/data/chat.json";
    archivePath = appDir + "/data/archive";
    
    // Выводим путь в консоль
    qDebug() << "Директория приложения:" << appDir;
    qDebug() << "Путь к файлу чата:" << chatFilePath;
    qDebug() << "Путь к архиву:" << archivePath;
    
    // Создаем стек виджетов
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);
    
    // Создаем виджеты
    createAuthWidget();
    createRegistrationWidget();
    createChatWidget();
    
    // Показываем начальный виджет
    stackedWidget->setCurrentWidget(authWidget);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createAuthWidget()
{
    authWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(authWidget);
    
    // Добавляем поля для ввода логина и пароля
    loginEdit = new QLineEdit(authWidget);
    passwordEdit = new QLineEdit(authWidget);
    passwordEdit->setEchoMode(QLineEdit::Password);
    
    QPushButton *loginBtn = new QPushButton("Войти", authWidget);
    QPushButton *registerBtn = new QPushButton("Регистрация", authWidget);
    
    // Добавляем метки и поля ввода
    layout->addWidget(new QLabel("Логин:", authWidget));
    layout->addWidget(loginEdit);
    layout->addWidget(new QLabel("Пароль:", authWidget));
    layout->addWidget(passwordEdit);
    layout->addWidget(loginBtn);
    layout->addWidget(registerBtn);
    
    connect(loginBtn, &QPushButton::clicked, this, &MainWindow::loginUser);
    connect(registerBtn, &QPushButton::clicked, this, &MainWindow::showRegistration);
    
    stackedWidget->addWidget(authWidget);
}

void MainWindow::createRegistrationWidget()
{
    registrationWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(registrationWidget);
    
    regLoginEdit = new QLineEdit(registrationWidget);
    regPasswordEdit = new QLineEdit(registrationWidget);
    regPasswordEdit->setEchoMode(QLineEdit::Password);
    
    QPushButton *registerBtn = new QPushButton("Зарегистрироваться", registrationWidget);
    QPushButton *backBtn = new QPushButton("Назад", registrationWidget);
    
    layout->addWidget(new QLabel("Логин:", registrationWidget));
    layout->addWidget(regLoginEdit);
    layout->addWidget(new QLabel("Пароль:", registrationWidget));
    layout->addWidget(regPasswordEdit);
    layout->addWidget(registerBtn);
    layout->addWidget(backBtn);
    
    connect(registerBtn, &QPushButton::clicked, this, &MainWindow::registerUser);
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::showRegistration);
    
    stackedWidget->addWidget(registrationWidget);
}

void MainWindow::createChatWidget()
{
    chatWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(chatWidget);
    
    QTextEdit *chatDisplay = new QTextEdit(chatWidget);
    chatDisplay->setReadOnly(true);
    
    messageEdit = new QLineEdit(chatWidget);
    QPushButton *sendBtn = new QPushButton("Отправить", chatWidget);
    QPushButton *logoutBtn = new QPushButton("Выйти", chatWidget);
    
    layout->addWidget(chatDisplay);
    layout->addWidget(messageEdit);
    layout->addWidget(sendBtn);
    layout->addWidget(logoutBtn);
    
    connect(sendBtn, &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::showLogin);
    
    stackedWidget->addWidget(chatWidget);
}

void MainWindow::showRegistration()
{
    stackedWidget->setCurrentWidget(registrationWidget);
}

void MainWindow::showLogin()
{
    stackedWidget->setCurrentWidget(authWidget);
}

void MainWindow::registerUser()
{
    QString username = regLoginEdit->text();
    QString password = regPasswordEdit->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, заполните все поля");
        return;
    }
    
    if (checkCredentials(username, password)) {
        QMessageBox::warning(this, "Ошибка", "Пользователь уже существует");
        return;
    }
    
    saveUser(username, password);
    QMessageBox::information(this, "Успех", "Регистрация успешна");
    showLogin();
}

void MainWindow::loginUser()
{
    QString username = loginEdit->text();
    QString password = passwordEdit->text();
    
    if (checkCredentials(username, password)) {
        currentUser = username;
        showChat();
        loadMessages();
    } else {
        QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль");
    }
}

void MainWindow::showChat()
{
    stackedWidget->setCurrentWidget(chatWidget);
}

void MainWindow::sendMessage()
{
    QString message = messageEdit->text();
    if (!message.isEmpty()) {
        saveMessage(message);
        messageEdit->clear();
        loadMessages();
    }
}

void MainWindow::saveMessage(const QString &message)
{
    QFile file(chatFilePath);
    QJsonArray messages;
    
    if (file.exists()) {
        file.open(QIODevice::ReadOnly);
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        messages = doc.array();
        file.close();
    }
    
    QJsonObject messageObj;
    messageObj["user"] = currentUser;
    messageObj["message"] = message;
    messageObj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    messages.append(messageObj);
    
    file.open(QIODevice::WriteOnly);
    file.write(QJsonDocument(messages).toJson());
    file.close();
}

void MainWindow::loadMessages()
{
    QFile file(chatFilePath);
    if (!file.exists()) return;
    
    file.open(QIODevice::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray messages = doc.array();
    file.close();
    
    QTextEdit *chatDisplay = chatWidget->findChild<QTextEdit*>();
    chatDisplay->clear();
    
    for (const QJsonValue &value : messages) {
        QJsonObject obj = value.toObject();
        QString user = obj["user"].toString();
        QString message = obj["message"].toString();
        QString timestamp = obj["timestamp"].toString();
        
        chatDisplay->append(QString("[%1] %2: %3")
                           .arg(timestamp)
                           .arg(user)
                           .arg(message));
    }
}

void MainWindow::archiveChat()
{
    if (!QFile::exists(chatFilePath)) return;
    
    QString archiveFileName = archivePath + "/chat_" + 
                            QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + 
                            ".json";
    
    QFile::copy(chatFilePath, archiveFileName);
    QFile::remove(chatFilePath);
}

bool MainWindow::checkCredentials(const QString &username, const QString &password)
{
    QString appDir = QCoreApplication::applicationDirPath();
    QFile usersFile(appDir + "/data/users.json");
    if (!usersFile.exists()) return false;
    
    usersFile.open(QIODevice::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(usersFile.readAll());
    QJsonArray users = doc.array();
    usersFile.close();
    
    for (const QJsonValue &value : users) {
        QJsonObject user = value.toObject();
        if (user["username"].toString() == username && 
            user["password"].toString() == password) {
            return true;
        }
    }
    
    return false;
}

void MainWindow::saveUser(const QString &username, const QString &password)
{
    QString appDir = QCoreApplication::applicationDirPath();
    QFile usersFile(appDir + "/data/users.json");
    QJsonArray users;
    
    if (usersFile.exists()) {
        usersFile.open(QIODevice::ReadOnly);
        QJsonDocument doc = QJsonDocument::fromJson(usersFile.readAll());
        users = doc.array();
        usersFile.close();
    }
    
    QJsonObject user;
    user["username"] = username;
    user["password"] = password;
    users.append(user);
    
    usersFile.open(QIODevice::WriteOnly);
    usersFile.write(QJsonDocument(users).toJson());
    usersFile.close();
} 