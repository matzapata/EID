#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QFile"
#include "styles.h"
#include <QTableView>
#include <QtDebug>
#include <QSerialPortInfo>
#include <QCryptographicHash>
#include "Config.h"
#include <cstring>
#include <QString>



// Main
MainWindow::MainWindow(QWidget * parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{

    State = S_IDLE;

    ui->setupUi(this);

    // Set up connection devices
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        ui->deviceSelect->addItem(info.portName() + " - " + info.description(), info.portName());
    }

    // Nos conectamos a la recepcion de serial
    connect(serial, SIGNAL(BufferReady(QByteArray)), this, SLOT(ConnectionProcessBuffer(QByteArray)));

    // Setup default pages
    ui->MainStackedWIdget->setCurrentWidget(ui->connection);
    ui->stackedWidget->setCurrentWidget(ui->LoginPage);
    ui->adminMainStackedWidget->setCurrentWidget(ui->adminDocsPage);
    ui->rightSideBar->setCurrentWidget(ui->helpPage);

    /* ==== Styles ==== */
    // Load styles
    QFile file(":/styles/main.css");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);

    // Connection page
    ui->ConnectionPage_Connect->setStyleSheet(primaryBtnStyle);
    ui->errorMessage->hide();
    ui->connectionStatus->hide();

    // Login page
    ui->LoginPage_AdminBtn->setStyleSheet(primaryBtnStyle);
    ui->LoginPage_UserNameError->hide();
    ui->LoginPage_PasswordError->hide();
    ui->LoginPage_PasswordInput->setEchoMode(QLineEdit::Password);
    ui->LoginPage_Authenticating->hide();
    ui->LoginPage_AuthErrorMsg->hide();

    // Sidebar
    ui->Sidebar_name->setStyleSheet(h1);
    ui->Sidebar_documentsBtn->setStyleSheet(primaryBtnStyle);
    ui->Sidebar_logoutBtn->setStyleSheet(secondaryBtnStyle);
    ui->Sidebar_settingsBtn->setStyleSheet(secondaryBtnStyle);

    // Main page
    ui->label_16->setStyleSheet(h2);
    ui->label_18->setStyleSheet(h2);
    ui->label_19->setStyleSheet(h2);
    ui->qaNewDoc->setStyleSheet(quickActionBtn);
    ui->qaNewContact->setStyleSheet(quickActionBtn);
    ui->qaNewPassword->setStyleSheet(quickActionBtn);
    ui->LoadContact->setStyleSheet(primaryBtnStyle);
    ui->LoadPassword->setStyleSheet(primaryBtnStyle);
    ui->LoadPasswordLabel->hide();
    ui->LoadContactLabel->hide();
    ui->LoadPswdError->hide();
    ui->LoadContactError->hide();

    // Help page
    ui->h1_2->setStyleSheet(h1);

    // New password page
    ui->label_38->setStyleSheet(h2);
    ui->saveNewPassword->setStyleSheet(primaryBtnLightStyle);
    ui->generateNewPassword->setStyleSheet(primaryBtnStyle);
    ui -> newPasswordErrorLabel -> hide();
    ui -> newPasswordStatusLabel -> hide();

    // View password page
    ui->passwordNameLabel->setStyleSheet(h2);
    ui->StopViewPasswordBtn->setStyleSheet(primaryBtnLightStyle);
    ui->PasswordDelete->setStyleSheet(alertBtnLightStyle);
    ui->PasswordDeleteLabel->hide();

    // New contact page
    ui->label_46->setStyleSheet(h1);
    ui->saveNewContactBtn->setStyleSheet(primaryBtnLightStyle);
    ui->discardNewContacBtn->setStyleSheet(alertBtnLightStyle);
    ui->NewContactErrorLabel->hide();
    ui->NewContactStatusLabel->hide();

    // View contact page
    ui->contactNameLabel->setStyleSheet(h2);
    ui->deleteContactBtn->setStyleSheet(alertBtnLightStyle);
    ui->contactDeleteLabel->hide();
    ui->contactStopView->setStyleSheet(primaryBtnLightStyle);

    // Sign file page
    ui->label_31->setStyleSheet(h2);
    ui->SigningLabel->hide();
    ui->SignFileBtn->setStyleSheet(secondaryBtnStyle);
    ui->FileHash->setWordWrap(true);

    // Settings page
    ui->label_15->setStyleSheet(h1);
    ui->label_27->setStyleSheet(h2);
    ui->label_20->setStyleSheet(h2);
    ui->SettingsNewPasswordBtn->setStyleSheet(primaryBtnStyle);
    ui->SettingsCurrentPassword->setEchoMode(QLineEdit::Password);
    ui->SettingsConfirmPassword->setEchoMode(QLineEdit::Password);
    ui->SettingsNewPassword->setEchoMode(QLineEdit::Password);
    ui->Settings_ErrorLabel->hide();
    ui->Settings_ProcessingLabel->hide();
    ui->Settings_SuccessLabel->hide();

    /* ==== End of styles ==== */

    /* Passwords table */
    ui->passwordsTable->setHorizontalHeaderLabels(QStringList() << "Nombre");
    ui->passwordsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    /* Contacts table */
    ui->contactsTable->setHorizontalHeaderLabels(QStringList() << "Nombre");
    ui->contactsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::ProcessBuffer(QByteArray buffer) {
    qDebug() << buffer;

    uint8_t * DecBuffer = NULL; // hash size
    char command = NO_COMMAND;
    if (buffer.at(0) == '#' && buffer.size() >= 2) {
        command = buffer.at(1);
    }

    // Si buffer no contiene # tira error de comunicacion
    switch (State) {
    case S_AWAIT_SIGNATURE:
        if (command == C_CREATE_SIGNATURE) {
            // recibimos firma
            crypto -> AesDecryptBuffer((uint8_t *)buffer.remove(0, 2).data(), &DecBuffer, 8*QCryptographicHash::hashLength(QCryptographicHash::Md5));

            QByteArray result((char *)DecBuffer, 8*QCryptographicHash::hashLength(QCryptographicHash::Md5)); // return the final hash value
            ui->SignResult->setPlainText(QString(result.toHex()));
            qDebug() << "SIGNATURE: " << QString(result.toHex());

            free(DecBuffer);
        }
        break;
    case S_AWAIT_AUTH_CONFIR:
        if (command == C_AUTH_ERROR) {
            // Fallo la autenticacion
            qDebug() << "Error de autenticacion";
            ui->LoginPage_AuthErrorMsg->show();
            ui->LoginPage_Authenticating->hide();
        }
        if (command == C_AUTH_SUCCESS) {
            // Autenticacion exitosa
            qDebug() << "Autenticacion correcta";
            ui->stackedWidget->setCurrentWidget(ui->AdminPage);
        }
        break;
    case S_AWAIT_AUTH_NEW_HASH_CONFIR:
        if (command == C_AUTH_SUCCESS) {
            ui->Settings_ErrorLabel->hide();
            ui->Settings_ProcessingLabel->hide();
            ui->Settings_SuccessLabel->show();

            AuthHash = NewAuthHash;
            Password = NewPassword;
        }
        else if (command == C_AUTH_ERROR) {
            ui->Settings_ErrorLabel->show();
            ui->Settings_ProcessingLabel->hide();
            ui->Settings_SuccessLabel->hide();
        }
        break;
    case S_AWAIT_PASSWORD:
        if (command == C_LOAD_KEY) {
            ui->LoadPasswordLabel->hide();

            uint8_t * password_p;
            QByteArray data = buffer.remove(0, 2);

            crypto -> AesDecryptBuffer((uint8_t *)data.data(), &password_p, 32);
            qDebug() << "Size: " << strlen(buffer.remove(0, 2).data());
            QString LoadedPassword = QString((char*)password_p);
            qDebug() << "Loaded key: " << LoadedPassword;

            QStringList pswd = LoadedPassword.split('#');
            if (pswd.size() >= 3) {
                PswdTableIndexes << pswd.at(0);
                PswdTableNames << pswd.at(1);
                PswdTableValues << pswd.at(2);
                qDebug() << "Index: " << PswdTableIndexes.at(PswdTableIndexes.size()-1);
                qDebug() << "Name: " << PswdTableNames.at(PswdTableNames.size()-1);
                qDebug() << "Value: " << PswdTableValues.at(PswdTableValues.size()-1);

                ui->passwordsTable->setItem(PswdTableInIndex, 0, new QTableWidgetItem(PswdTableNames.at(PswdTableNames.size()-1)));
                PswdTableInIndex++;
            }
            else {
                ui->LoadPswdError->show();
            }

            free(password_p);

        }
        else if (command == C_NO_KEY_AVAILABLE) {
            ui->LoadPasswordLabel->show();
            ui->LoadPasswordLabel->setText("No hay mas claves disponibles.");
            qDebug() << "No key available";
        }
        break;
    case S_AWAIT_CONTACT:
        if (command == C_LOAD_CONTACT) {

            uint8_t * contact_p = nullptr;
            QByteArray data = buffer.remove(0, 2);

            crypto -> AesDecryptBuffer((uint8_t *)data.data(), &contact_p, 32);
            qDebug() << "Size: " << strlen(buffer.remove(0, 2).data());
            QString LoadedContact = QString((char*)contact_p);
            qDebug() << "Loaded contact: " << LoadedContact;

            QStringList contact = LoadedContact.split('#');
            if (contact.size() >= 3) {
                ContTableIndexes << contact.at(0);
                ContTableNames << contact.at(1);
                ContTableValues << contact.at(2);
                ui->contactsTable->setItem(ContTableInIndex, 0, new QTableWidgetItem(ContTableNames.at(ContTableNames.size()-1)));
                ContTableInIndex++;

                ui->LoadContactLabel->hide();
            }
            else {
                ui->LoadContactError->show();
            }

            free(contact_p);
        }
        else if (command == C_NO_CONTACT_AVAILABLE) {
            ui->LoadContactLabel->show();
            ui->LoadContactLabel->setText("No hay mas contactos disponibles.");
            qDebug() << "No contact available";
        }
        break;
    case S_AWAIT_CONFIR_NEW_KEY:
        if (command == C_NEW_KEY) {
            ui->newPasswordErrorLabel->hide();
            ui->newPasswordStatusLabel->setText("Operacion exitosa");
            ui->newPasswordName->setText("");
            ui->newPasswordValue->setText("");

            //            PswdTableNames << NewPswdStr.split('#').first();
            //            PswdTableValues << NewPswdStr.split('#').last();
        }
        break;
    case S_AWAIT_CONFIR_NEW_CONT:
        if (command == C_NEW_CONTACT) {
            ui->NewContactErrorLabel->hide();
            ui->NewContactStatusLabel->setText("Operacion exitosa");
            ui->NewContactUsername->setText("");
            ui->NewContactPk->setText("");
        }
        break;
    case S_AWAIT_CONFIR_DELETE_KEY:
        if (command == C_DELETE_KEY) {
            ui->PasswordDeleteLabel->hide();
            ui->passwordsTable->hideRow( PswdCurrentIndex );
            ui -> rightSideBar -> setCurrentWidget(ui->helpPage);
        }
        else if (command == C_ERROR) ui->PasswordDeleteLabel->setText("Error");
        break;
    case S_AWAIT_CONFIR_DELETE_CONTACT:
        if (command == C_DELETE_CONTACT) {
            ui->contactDeleteLabel->hide();
            ui->contactsTable->hideRow( ContCurrentIndex );
            ui->rightSideBar->setCurrentWidget(ui->helpPage);
        }
        else if (command == C_ERROR) ui->contactDeleteLabel->setText("Error");
        break;
    case S_END_SESSION:
        serial -> Close();
        this -> close();
        break;
    }
}

// Login
void MainWindow::on_LoginPage_AdminBtn_clicked()
{
    Username = ui->LoginPage_UsernameInput->text();
    Password = ui->LoginPage_PasswordInput->text();
    if (Username.isEmpty()) {
        ui->LoginPage_UserNameError->show();
    }
    else if (Password.isEmpty()) {
        ui->LoginPage_UserNameError->hide();
        ui->LoginPage_PasswordError->show();
    }
    else {
        ui->LoginPage_UserNameError->hide();
        ui->LoginPage_PasswordError->hide();
        ui->LoginPage_Authenticating->show();

        QByteArray HashHex = crypto->HashPswd(Username, Password).toHex();
        uint32_t HashHexSize = 2*QCryptographicHash::hashLength(QCryptographicHash::Md5);
        uint8_t * HashHexEnc;

        crypto->AesEncryptBuffer((uint8_t *)HashHex.data(), &HashHexEnc, HashHexSize);

        qDebug() << "Enviamos hash y esperamos autenticacion antes de pasar a Admin page";
        qDebug() << "Auth hash: " << HashHex;
        qDebug() << "Auth hash enc: " << HashHexEnc;

        serial->SendCommand(C_AUTH);
        serial->SendBuffer((char *)HashHexEnc, HashHexSize);

        AuthHash = HashHex;

        State = S_AWAIT_AUTH_CONFIR;
    }
}

// Settings
void MainWindow::on_SettingsNewPasswordBtn_clicked()
{
    QString FormCurrentPassword = ui->SettingsCurrentPassword->text();
    QString FormNewPassword = ui->SettingsNewPassword->text();
    QString FormNewPasswordConfir = ui->SettingsConfirmPassword->text();

    ui->Settings_ErrorLabel->hide();
    ui->Settings_ProcessingLabel->show();

    if (FormCurrentPassword == Password && FormNewPassword.length() > 5 && FormNewPassword == FormNewPasswordConfir && FormNewPassword != Password) {

        QByteArray HashHex = crypto->HashPswd(Username, FormNewPassword).toHex();
        uint32_t HashHexSize = 2*QCryptographicHash::hashLength(QCryptographicHash::Md5);
        uint8_t * HashHexEnc;

        crypto->AesEncryptBuffer((uint8_t *)HashHex.data(), &HashHexEnc, HashHexSize);

        qDebug() << "Enviamos nuevo hash:";
        qDebug() << "Auth hash: " << HashHex;
        qDebug() << "Auth hash enc: " << HashHexEnc;

        serial->SendCommand(C_AUTH_POST_NEW_HASH);
        serial->SendBuffer((char *)HashHexEnc, HashHexSize);

        NewAuthHash = HashHex;
        NewPassword = FormNewPassword;

        State = S_AWAIT_AUTH_NEW_HASH_CONFIR;

    } else {
        ui->Settings_ErrorLabel->show();
        ui->Settings_ProcessingLabel->hide();
    }
}

// Sidebar
void MainWindow::on_Sidebar_documentsBtn_clicked()
{
    ui->adminMainStackedWidget->setCurrentWidget(ui->adminDocsPage);

    // Styles change
    ui->Sidebar_documentsBtn->setStyleSheet(primaryBtnStyle);
    ui->Sidebar_settingsBtn->setStyleSheet(secondaryBtnStyle);
}
void MainWindow::on_Sidebar_settingsBtn_clicked()
{
    ui->adminMainStackedWidget->setCurrentWidget(ui->adminSettingsPage);

    // Styles change
    ui->Sidebar_documentsBtn->setStyleSheet(secondaryBtnStyle);
    ui->Sidebar_settingsBtn->setStyleSheet(primaryBtnStyle);
}
void MainWindow::on_Sidebar_logoutBtn_clicked()
{
    serial -> SendCommand(C_END_SESSION);
    State = S_END_SESSION;
}

// View contact
void MainWindow::on_contactStopView_clicked()
{
    ui -> rightSideBar -> setCurrentWidget(ui->helpPage);
}
void MainWindow::on_deleteContactBtn_clicked()
{
    qDebug() << "Eliminamos contacto: " << ContCurrentIndex;
    ui->contactDeleteLabel->show();

    serial->SendCommand(C_DELETE_CONTACT);
    serial->SendByte(ContCurrentIndex);

    State = S_AWAIT_CONFIR_DELETE_CONTACT;
}

// New contact
void MainWindow::on_saveNewContactBtn_clicked()
{
    QString Username = ui->NewContactUsername->text();
    QString PubKey = ui->NewContactPk->text();
    if (Username.length() == 0 || PubKey.length()==0) {
        ui->NewContactErrorLabel->show();
        ui->NewContactStatusLabel->hide();
    }
    else {
        ui->NewContactErrorLabel->hide();
        ui->NewContactStatusLabel->show();

        NewContactStr = "";
        NewContactStr.append(Username);
        NewContactStr.append('#');
        NewContactStr.append(PubKey);
        NewContactStr.append('#');
        NewContactStr.resize(32, ' ');

        qDebug() << "Encriptamos y enviamos: " << NewContactStr;

        uint8_t * NewContactEnc = nullptr;
        crypto -> AesEncryptBuffer((uint8_t *)NewContactStr.toLocal8Bit().data(), &NewContactEnc, 32);

        serial->SendCommand(C_NEW_CONTACT);
        serial->SendBuffer((char *)NewContactEnc, 32);

        State = S_AWAIT_CONFIR_NEW_CONT;
    }
}
void MainWindow::on_discardNewContacBtn_clicked()
{
    ui->rightSideBar->setCurrentWidget(ui->helpPage);
}

// View password
void MainWindow::on_StopViewPasswordBtn_clicked()
{
    ui -> rightSideBar -> setCurrentWidget(ui->helpPage);
}
void MainWindow::on_PasswordDelete_clicked()
{
    qDebug() << "Eliminamos pswd: " << PswdCurrentIndex;
    ui->PasswordDeleteLabel->show();

    serial->SendCommand(C_DELETE_KEY);
    serial->SendByte(PswdCurrentIndex);

    State = S_AWAIT_CONFIR_DELETE_KEY;
}

// New password
void MainWindow::on_generateNewPassword_clicked()
{
    uint8_t * genKey = crypto -> KeyGen(AES128_KEY_SIZE);
    memcpy(GeneratedKey, genKey, AES128_KEY_SIZE);

    ui->newPasswordValue->setText(QString((char*)GeneratedKey));
}
void MainWindow::on_saveNewPassword_clicked()
{
    QString newPswdValue = ui -> newPasswordValue -> text();
    QString newPswdName = ui -> newPasswordName -> text();

    ui -> newPasswordErrorLabel -> hide();
    ui -> newPasswordStatusLabel -> hide();

    if(newPswdValue.length() < 5 || newPswdValue.length() > AES128_KEY_SIZE) {
        ui -> newPasswordErrorLabel -> setText("Valor debe tener entre 5 y 16 caracteres.");
        ui -> newPasswordErrorLabel -> show();
    }
    else if(newPswdName.length() < 5 || newPswdName.length() > AES128_KEY_SIZE) {
        ui -> newPasswordErrorLabel -> setText("Nombre debe tener entre 5 y 16 caracteres.");
        ui -> newPasswordErrorLabel -> show();
    }
    else {
        ui -> newPasswordStatusLabel -> setText("Procesando...");
        ui -> newPasswordStatusLabel -> show();

        NewPswdStr = "";
        NewPswdStr.append(newPswdName);
        NewPswdStr.append('#');
        NewPswdStr.append(newPswdValue);
        NewPswdStr.append('#');
        NewPswdStr.resize(32, ' ');

        qDebug() << "Encriptamos y enviamos: " << NewPswdStr;

        uint8_t * NewPswdStrEnc = nullptr;
        crypto -> AesEncryptBuffer((uint8_t *)NewPswdStr.toLocal8Bit().data(), &NewPswdStrEnc, 32);

        serial->SendCommand(C_NEW_KEY);
        serial->SendBuffer((char *)NewPswdStrEnc, 32);

        State = S_AWAIT_CONFIR_NEW_KEY;
    }
}

// Main scroll area
void MainWindow::on_qaNewPassword_clicked()
{
    ui -> rightSideBar -> setCurrentWidget(ui->newPasswordPage);
}
void MainWindow::on_qaNewContact_clicked()
{
    ui->rightSideBar->setCurrentWidget(ui->newContactPage);
}
void MainWindow::on_qaNewDoc_clicked()
{
    ui->rightSideBar->setCurrentWidget(ui->addFilePage);
}
void MainWindow::on_LoadPassword_clicked()
{
    serial->SendCommand(C_LOAD_KEY);
    ui->LoadPasswordLabel->show();
    ui->LoadPswdError->hide();
    State = S_AWAIT_PASSWORD;
}
void MainWindow::on_LoadContact_clicked()
{
    serial->SendCommand(C_LOAD_CONTACT);
    ui->LoadContactLabel->show();
    ui->LoadContactError->hide();
    State = S_AWAIT_CONTACT;
}
void MainWindow::on_SignFileBtn_clicked()
{
    QString filepath = ui->SignFilePath->text();

    if (!filepath.isEmpty()) {
        ui->SigningLabel->show();

        QByteArray fileHash = crypto->HashFile(filepath);
        ui->FileHash->setText(QString(fileHash.toHex()));
        ui->SigningLabel->hide();

        qDebug() << "FileHash: " << fileHash.toHex();

        QByteArray HashHex = fileHash.toHex(); // return the final hash value
        uint8_t * HashHexEnc;

        crypto->AesEncryptBuffer((uint8_t *)HashHex.data(), &HashHexEnc, 32);

        serial->SendCommand(C_CREATE_SIGNATURE);
        serial->SendBuffer((char *)HashHexEnc, 32);

        State = S_AWAIT_SIGNATURE;
    }

}
void MainWindow::on_contactsTable_cellClicked(int row, int column)
{
    qDebug() << "Cell clicked: " << row << column;

    if (row+1 <= ContTableIndexes.size()) {
        ui->contactNameLabel->setText(ContTableNames.at(row));
        ui->contactValueLabel->setText(ContTableValues.at(row));
        ui->rightSideBar->setCurrentWidget(ui->viewContactPage);
        ContCurrentIndex = row;
    }
}
void MainWindow::on_passwordsTable_cellClicked(int row, int column)
{
    qDebug() << "Cell clicked: " << row << column;

    if (row+1 <= PswdTableIndexes.size()) {
        ui->passwordNameLabel->setText(PswdTableNames.at(row));
        ui->PasswordViewValue->setText(PswdTableValues.at(row));
        ui->rightSideBar->setCurrentWidget(ui->viewPasswordPage);
        PswdCurrentIndex = row;
    }
}

// Connection page
void MainWindow::on_ConnectionPage_Connect_clicked()
{
    ui->errorMessage->hide();
    QTimer::singleShot(10000, this, &MainWindow::ResetConnection);

    if (serial->Connect((ui->deviceSelect->currentData()).toString())) {
        qDebug() << "CONECTADOS";
        connect(serial, SIGNAL(BufferReady(QByteArray)), this, SLOT(ProcessBuffer(QByteArray)));

        ui->connectionStatus->setText("Estableciendo comunicacion segura...");
        ui->connectionStatus->show();

        // Requerimos la llave publica
        serial -> SendCommand(C_GET_PK);

    } else {
        qDebug() << "Error de conexion";
        ui->errorMessage->setText("Error en la conexion. Revise el dispositivo y reintente.");
        ui->errorMessage->show();
    }
}
void MainWindow::ConnectionProcessBuffer(QByteArray buffer)
{
    char command = NO_COMMAND;
    if (buffer.at(0) == '#' && buffer.size() >= 2) {
        command = buffer.at(1);
    }

    switch (ConState) {
    case S_AWAIT_PK:
        if (command == C_PUT_PK) {
            qDebug() << "Recibimos la clave publica";
            int64_t modulus, exponent;
            sscanf((char *)buffer.data(), "#3%lld#%lld", &modulus, &exponent);
            qDebug() << "Modulus: " << modulus << " Exponent: " << exponent;

            uint8_t * key = crypto -> KeyGen(AES128_KEY_SIZE);
            crypto -> SetAesKey(key);
            crypto -> SetRsaPubKey(modulus, exponent);

            int64_t * RsaEnc; // 8 veces mas grande que original, misma cantidad de bloques pero en vez de ser de 8 bits son de 64
            RsaEnc = crypto -> RsaEncryptBuffer((int8_t *)key, (uint32_t)AES128_KEY_SIZE);

            // Enviamos clave sesion encriptaded
            serial -> SendCommand(C_POST_SESSION_KEY);
            serial -> SendBuffer((char *)RsaEnc, 8 * AES128_KEY_SIZE);

            delete [] key;

            ConState = S_PING;
        }
        else if (command == C_ERROR) {
            ui->connectionStatus->hide();
            ui->errorMessage->setText("Error en la conexion.");
            ui->errorMessage->show();

            serial->Close();
        }
        break;
    case S_PING:
        if (command == C_POST_PING) {
            qDebug() << "PING value: " << buffer.mid(2, buffer.length());

            uint8_t * out;
            crypto -> AesDecryptBuffer((uint8_t *)buffer.mid(2, buffer.length()).data(), &out, 16);
            qDebug() << QString((char *)out);

            uint8_t * PingEnc = nullptr;
            uint8_t * PingDec = nullptr;
            QString Ping = "PING";
            Ping.resize(16, ' ');
            crypto -> AesEncryptBuffer((uint8_t *)Ping.toLocal8Bit().data(), &PingEnc, 16);
            crypto -> AesDecryptBuffer(PingEnc, &PingDec, 16);
            qDebug() << "Decripted ping: " << QString((char *)PingDec);

            serial->SendCommand(C_POST_PING);
            serial->SendBuffer((char *)PingEnc, 16);

            free(out);
            ConState = S_AWAIT_PING_CONFIR;
            qDebug() << "State = AWAIT PING CONFIR";
        }
        break;
    case S_AWAIT_PING_CONFIR:
        if (command == C_OK_PING) {
            qDebug() << "COMUNICACION EXITOSA";

            uint8_t * user = nullptr;
            crypto -> AesDecryptBuffer((uint8_t *)buffer.remove(0, 2).data(), &user, 16);
            ui->Sidebar_name->setText(QString((char *)user));

            QString PublicKeyStr = QString("%1:%2").arg(crypto->GetRsaPubKey()->modulus).arg(crypto->GetRsaPubKey()->exponent);
            ui->PublicKey->setText(PublicKeyStr);
            ui->SettingsPublicKey_Label->setText(PublicKeyStr);

            // Connexion exitosa, pasamos a la pagina principal
            free(user);
            ui->MainStackedWIdget->setCurrentWidget(ui->mainPage);
            ConState = CONNECTION_FINISHED;
        }
        else if (command == C_FAILED_PING) {
            qDebug() << "COMUNICACION FALLIDA";
            ui->errorMessage->setText("Error en la conexion");
            ui->errorMessage->show();

            ConState = S_AWAIT_PK;
        }
        break;
    }
}
void MainWindow::ResetConnection() {
    if (ConState != CONNECTION_FINISHED) {
        ui->connectionStatus->hide();
        ui->errorMessage->setText("Error en la conexion. Reinicie la app y reintente.");
        ui->errorMessage->show();
    }
}

