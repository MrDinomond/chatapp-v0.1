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
    setMinimumSize(800, 600);
    
    // Создаем пути для файлов в директории приложения
    QString appDir = QCoreApplication::applicationDirPath();
    QDir().mkpath(appDir + "/data");
    QDir().mkpath(appDir + "/data/archive");
    QDir().mkpath(appDir + "/data/private");
    
    chatFilePath = appDir + "/data/chat.json";
    privateChatPath = appDir + "/data/private";
    archivePath = appDir + "/data/archive";
    
    // Выводим путь в консоль
    qDebug() << "Директория приложения:" << appDir;
    qDebug() << "Путь к файлу чата:" << chatFilePath;
    qDebug() << "Путь к архиву:" << archivePath;
    
    // Создаем стек виджетов
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);
    
    // Создаем виджеты
    createMainMenuWidget();
    createLoginWidget();
    createRegistrationWidget();
    createChatWidget();
    
    // Показываем начальный виджет
    stackedWidget->setCurrentWidget(mainMenuWidget);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createMainMenuWidget()
{
    mainMenuWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(mainMenuWidget);
    
    QPushButton *loginBtn = new QPushButton("Войти", mainMenuWidget);
    QPushButton *registerBtn = new QPushButton("Зарегистрироваться", mainMenuWidget);
    
    layout->addWidget(loginBtn);
    layout->addWidget(registerBtn);
    
    connect(loginBtn, &QPushButton::clicked, this, &MainWindow::showLoginForm);
    connect(registerBtn, &QPushButton::clicked, this, &MainWindow::showRegistrationForm);
    
    stackedWidget->addWidget(mainMenuWidget);
}

void MainWindow::createLoginWidget()
{
    loginWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(loginWidget);
    
    loginEdit = new QLineEdit(loginWidget);
    passwordEdit = new QLineEdit(loginWidget);
    passwordEdit->setEchoMode(QLineEdit::Password);
    
    QPushButton *loginBtn = new QPushButton("Войти", loginWidget);
    QPushButton *backBtn = new QPushButton("Назад", loginWidget);
    
    layout->addWidget(new QLabel("Логин:", loginWidget));
    layout->addWidget(loginEdit);
    layout->addWidget(new QLabel("Пароль:", loginWidget));
    layout->addWidget(passwordEdit);
    layout->addWidget(loginBtn);
    layout->addWidget(backBtn);
    
    connect(loginBtn, &QPushButton::clicked, this, &MainWindow::loginUser);
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::showMainMenu);
    
    stackedWidget->addWidget(loginWidget);
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
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::showMainMenu);
    
    stackedWidget->addWidget(registrationWidget);
}

void MainWindow::showMainMenu()
{
    stackedWidget->setCurrentWidget(mainMenuWidget);
}

void MainWindow::showLoginForm()
{
    stackedWidget->setCurrentWidget(loginWidget);
}

void MainWindow::showRegistrationForm()
{
    stackedWidget->setCurrentWidget(registrationWidget);
}

void MainWindow::createChatWidget()
{
    chatWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(chatWidget);
    
    // Левая панель с чатами
    QWidget *leftPanel = new QWidget(chatWidget);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    
    // Поиск пользователя для нового чата
    QLabel *searchLabel = new QLabel("Новый чат:", leftPanel);
    userSearchEdit = new QLineEdit(leftPanel);
    QPushButton *searchBtn = new QPushButton("Найти", leftPanel);
    
    // Список существующих чатов
    QLabel *chatsLabel = new QLabel("Ваши чаты:", leftPanel);
    privateChatList = new QListWidget(leftPanel);
    
    // Добавляем элемент "Общий чат" в начало списка
    QListWidgetItem *generalChatItem = new QListWidgetItem("Общий чат");
    generalChatItem->setData(Qt::UserRole, "general");
    privateChatList->addItem(generalChatItem);
    
    leftLayout->addWidget(searchLabel);
    leftLayout->addWidget(userSearchEdit);
    leftLayout->addWidget(searchBtn);
    leftLayout->addSpacing(20);
    leftLayout->addWidget(chatsLabel);
    leftLayout->addWidget(privateChatList);
    
    // Правая панель с чатом
    QWidget *rightPanel = new QWidget(chatWidget);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    
    // Виджет чата
    chatDisplay = new QTextEdit(rightPanel);
    chatDisplay->setReadOnly(true);
    messageEdit = new QLineEdit(rightPanel);
    QPushButton *sendBtn = new QPushButton("Отправить", rightPanel);
    
    rightLayout->addWidget(chatDisplay);
    rightLayout->addWidget(messageEdit);
    rightLayout->addWidget(sendBtn);
    
    // Кнопка выхода
    QPushButton *logoutBtn = new QPushButton("Выйти", rightPanel);
    rightLayout->addWidget(logoutBtn);
    
    // Добавляем панели в главный layout
    mainLayout->addWidget(leftPanel, 1);
    mainLayout->addWidget(rightPanel, 3);
    
    // Подключаем сигналы
    connect(sendBtn, &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::showLoginForm);
    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::searchUser);
    connect(userSearchEdit, &QLineEdit::returnPressed, this, &MainWindow::searchUser);
    connect(privateChatList, &QListWidget::itemClicked, this, &MainWindow::handleChatSelection);
    
    stackedWidget->addWidget(chatWidget);
}

void MainWindow::handleChatSelection(QListWidgetItem *item)
{
    QString chatId = item->data(Qt::UserRole).toString();
    if (chatId == "general") {
        currentPrivateChat.clear();
        loadMessages();
    } else {
        currentPrivateChat = item->text();
        loadPrivateMessages(currentPrivateChat);
    }
    messageEdit->clear();
    messageEdit->setFocus();
}

void MainWindow::sendMessage()
{
    QString message = messageEdit->text();
    if (!message.isEmpty()) {
        if (currentPrivateChat.isEmpty()) {
            // Отправляем в общий чат
            saveMessage(message);
            loadMessages();
        } else {
            // Отправляем в личный чат
            savePrivateMessage(currentPrivateChat, message);
            loadPrivateMessages(currentPrivateChat);
        }
        messageEdit->clear();
    }
}

void MainWindow::loadMessages()
{
    QFile file(chatFilePath);
    if (!file.exists()) return;
    
    file.open(QIODevice::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray messages = doc.array();
    file.close();
    
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

void MainWindow::loadPrivateMessages(const QString &otherUser)
{
    QString fileName = getPrivateChatFileName(currentUser, otherUser);
    QFile file(fileName);
    if (!file.exists()) return;
    
    file.open(QIODevice::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray messages = doc.array();
    file.close();
    
    chatDisplay->clear();
    
    for (const QJsonValue &value : messages) {
        QJsonObject obj = value.toObject();
        QString from = obj["from"].toString();
        QString message = obj["message"].toString();
        QString timestamp = obj["timestamp"].toString();
        
        chatDisplay->append(QString("[%1] %2: %3")
                           .arg(timestamp)
                           .arg(from)
                           .arg(message));
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

void MainWindow::searchUser()
{
    QString searchText = userSearchEdit->text().trimmed();
    if (searchText.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите имя пользователя для поиска");
        return;
    }
    
    if (searchText == currentUser) {
        QMessageBox::warning(this, "Ошибка", "Нельзя начать чат с самим собой");
        return;
    }
    
    // Проверяем существование пользователя
    QString appDir = QCoreApplication::applicationDirPath();
    QFile usersFile(appDir + "/data/users.json");
    if (!usersFile.exists()) {
        QMessageBox::warning(this, "Ошибка", "Пользователь не найден");
        return;
    }
    
    usersFile.open(QIODevice::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(usersFile.readAll());
    QJsonArray users = doc.array();
    usersFile.close();
    
    bool userFound = false;
    for (const QJsonValue &value : users) {
        if (value.toObject()["username"].toString() == searchText) {
            userFound = true;
            break;
        }
    }
    
    if (userFound) {
        // Создаем файл чата, если его еще нет
        QString chatFileName = getPrivateChatFileName(currentUser, searchText);
        if (!QFile::exists(chatFileName)) {
            QFile file(chatFileName);
            file.open(QIODevice::WriteOnly);
            file.write("[]");
            file.close();
        }
        
        // Добавляем чат в список и делаем его активным
        QListWidgetItem *newChatItem = new QListWidgetItem(searchText);
        newChatItem->setData(Qt::UserRole, searchText);
        privateChatList->addItem(newChatItem);
        privateChatList->setCurrentItem(newChatItem);
        
        // Открываем чат
        currentPrivateChat = searchText;
        loadPrivateMessages(searchText);
        
        userSearchEdit->clear();
    } else {
        QMessageBox::warning(this, "Ошибка", "Пользователь не найден");
    }
}

void MainWindow::openPrivateChat(const QString &username)
{
    currentPrivateChat = username;
    
    // Проверяем, есть ли уже такой чат в списке
    bool chatExists = false;
    for (int i = 0; i < privateChatList->count(); ++i) {
        if (privateChatList->item(i)->text() == username) {
            privateChatList->setCurrentRow(i);
            chatExists = true;
            break;
        }
    }
    
    // Если чата нет в списке, добавляем его
    if (!chatExists) {
        QListWidgetItem *newChatItem = new QListWidgetItem(username);
        newChatItem->setData(Qt::UserRole, username);
        privateChatList->addItem(newChatItem);
        privateChatList->setCurrentItem(newChatItem);
    }
    
    // Загружаем сообщения
    loadPrivateMessages(username);
    
    // Очищаем поле ввода сообщения
    messageEdit->clear();
    messageEdit->setFocus();
}

QString MainWindow::getPrivateChatFileName(const QString &user1, const QString &user2)
{
    QString sortedUsers = user1 < user2 ? user1 + "_" + user2 : user2 + "_" + user1;
    return privateChatPath + "/" + sortedUsers + ".json";
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
    showLoginForm();
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
    updatePrivateChatList();
    loadMessages();
}

void MainWindow::savePrivateMessage(const QString &toUser, const QString &message)
{
    QString fileName = getPrivateChatFileName(currentUser, toUser);
    QFile file(fileName);
    QJsonArray messages;
    
    if (file.exists()) {
        file.open(QIODevice::ReadOnly);
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        messages = doc.array();
        file.close();
    }
    
    QJsonObject messageObj;
    messageObj["from"] = currentUser;
    messageObj["to"] = toUser;
    messageObj["message"] = message;
    messageObj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    messages.append(messageObj);
    
    file.open(QIODevice::WriteOnly);
    file.write(QJsonDocument(messages).toJson());
    file.close();
}

void MainWindow::updatePrivateChatList()
{
    // Сохраняем текущий выбранный элемент
    QString currentChat;
    if (privateChatList->currentItem()) {
        currentChat = privateChatList->currentItem()->text();
    }
    
    // Очищаем список, кроме первого элемента (Общий чат)
    while (privateChatList->count() > 1) {
        delete privateChatList->item(1);
    }
    
    // Добавляем личные чаты
    QDir privateDir(privateChatPath);
    QStringList files = privateDir.entryList(QStringList() << "*.json", QDir::Files);
    
    for (const QString &file : files) {
        QString chatName = file.mid(0, file.lastIndexOf('.'));
        if (chatName.contains(currentUser)) {
            QString otherUser = chatName.split("_").first() == currentUser ? 
                              chatName.split("_").last() : chatName.split("_").first();
            QListWidgetItem *item = new QListWidgetItem(otherUser);
            item->setData(Qt::UserRole, otherUser);
            privateChatList->addItem(item);
            
            // Восстанавливаем выбор
            if (otherUser == currentChat) {
                privateChatList->setCurrentItem(item);
            }
        }
    }
} 