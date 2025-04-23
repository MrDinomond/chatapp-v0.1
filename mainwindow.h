#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QStandardPaths>
#include <QListWidget>
#include <QTextEdit>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showLoginForm();
    void showRegistrationForm();
    void showMainMenu();
    void registerUser();
    void loginUser();
    void showChat();
    void sendMessage();
    void searchUser();
    void handleChatSelection(QListWidgetItem *item);
    void openPrivateChat(const QString &username);

private:
    QStackedWidget *stackedWidget;
    QWidget *mainMenuWidget;
    QWidget *loginWidget;
    QWidget *registrationWidget;
    QWidget *chatWidget;
    
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QLineEdit *regLoginEdit;
    QLineEdit *regPasswordEdit;
    QLineEdit *messageEdit;
    QLineEdit *userSearchEdit;
    
    QTextEdit *chatDisplay;
    QListWidget *privateChatList;
    
    QString currentUser;
    QString currentPrivateChat;
    QString chatFilePath;
    QString privateChatPath;
    QString archivePath;
    
    void createMainMenuWidget();
    void createLoginWidget();
    void createRegistrationWidget();
    void createChatWidget();
    void saveMessage(const QString &message);
    void savePrivateMessage(const QString &toUser, const QString &message);
    void loadMessages();
    void loadPrivateMessages(const QString &otherUser);
    void archiveChat();
    bool checkCredentials(const QString &username, const QString &password);
    void saveUser(const QString &username, const QString &password);
    QString getPrivateChatFileName(const QString &user1, const QString &user2);
    void updatePrivateChatList();
};

#endif // MAINWINDOW_H 