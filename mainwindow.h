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

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showRegistration();
    void showLogin();
    void registerUser();
    void loginUser();
    void showChat();
    void sendMessage();

private:
    QStackedWidget *stackedWidget;
    QWidget *authWidget;
    QWidget *registrationWidget;
    QWidget *chatWidget;
    
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QLineEdit *regLoginEdit;
    QLineEdit *regPasswordEdit;
    QLineEdit *messageEdit;
    
    QString currentUser;
    QString chatFilePath;
    QString archivePath;
    
    void createAuthWidget();
    void createRegistrationWidget();
    void createChatWidget();
    void saveMessage(const QString &message);
    void loadMessages();
    void archiveChat();
    bool checkCredentials(const QString &username, const QString &password);
    void saveUser(const QString &username, const QString &password);
};

#endif // MAINWINDOW_H 