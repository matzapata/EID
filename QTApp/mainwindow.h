#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "serial.h"
#include "crypto.h"

#define S_IDLE 0
#define S_AWAIT_SIGNATURE 1
#define S_AWAIT_AUTH_CONFIR 2
#define S_AWAIT_AUTH_NEW_HASH_CONFIR 3
#define S_END_SESSION 4
#define S_AWAIT_PASSWORD 5
#define S_AWAIT_CONTACT 6
#define S_AWAIT_CONFIR_NEW_KEY 7
#define S_AWAIT_CONFIR_NEW_CONT 8
#define S_AWAIT_CONFIR_DELETE_KEY 9
#define S_AWAIT_CONFIR_DELETE_CONTACT 10

#define S_AWAIT_PK 11
#define S_PING 12
#define S_NO_CONNECTION 13
#define S_AWAIT_PING_CONFIR 14
#define CONNECTION_FINISHED 24

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget * parent = nullptr);
    ~MainWindow();

public slots:
    void ProcessBuffer(QByteArray buffer);
    void ConnectionProcessBuffer(QByteArray buffer);

private slots:

    void on_ConnectionPage_Connect_clicked();
    void on_LoginPage_AdminBtn_clicked();
    void on_Sidebar_logoutBtn_clicked();
    void on_Sidebar_documentsBtn_clicked();
    void on_Sidebar_settingsBtn_clicked();
    void on_qaNewDoc_clicked();
    void on_qaNewContact_clicked();
    void on_discardNewContacBtn_clicked();
    void on_SignFileBtn_clicked();
    void on_SettingsNewPasswordBtn_clicked();
    void on_qaNewPassword_clicked();
    void on_generateNewPassword_clicked();
    void on_saveNewPassword_clicked();
    void on_LoadPassword_clicked();
    void on_passwordsTable_cellClicked(int row, int column);
    void on_StopViewPasswordBtn_clicked();
    void on_PasswordDelete_clicked();
    void on_LoadContact_clicked();
    void on_saveNewContactBtn_clicked();
    void on_contactsTable_cellClicked(int row, int column);
    void on_contactStopView_clicked();
    void on_deleteContactBtn_clicked();

    void ResetConnection();

private:
    QString Conbuffer;
    unsigned int ConState = S_AWAIT_PK;
    Serial * serial = new Serial();
    Crypto * crypto = new Crypto();
    Ui::MainWindow *ui;
    int State;
    QString Username = "";
    QString Password = "";
    QString AuthHash = "";
    QString NewAuthHash = "";
    QString NewPassword = "";
    uint8_t GeneratedKey[AES128_KEY_SIZE];

    QStringList PswdTableNames;
    QStringList PswdTableValues;
    QStringList PswdTableIndexes; // Los indices que represenyan los items en memoria del micro. Los uso para eliminar despues
    int PswdTableInIndex = 0;
    char PswdCurrentIndex = 0;

    QStringList ContTableNames;
    QStringList ContTableValues;
    QStringList ContTableIndexes; // Los indices que represenyan los items en memoria del micro. Los uso para eliminar despues
    int ContTableInIndex = 0;
    char ContCurrentIndex = 0;

    QString NewPswdStr;
    QString NewContactStr;

};
#endif // MAINWINDOW_H
