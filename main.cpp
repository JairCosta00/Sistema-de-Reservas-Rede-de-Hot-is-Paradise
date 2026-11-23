#include "mainwindow.h"
#include "logindialog.h"
#include <QApplication>
#include <QIcon>
#include <QCoreApplication>
#include <QFont>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Carrega as fontes embutidas na pasta Resource Files
    const QString ResourceDir = QCoreApplication::applicationDirPath() + "/Resource Files/";
    const int idRegular = QFontDatabase::addApplicationFont(ResourceDir + "PlusJakartaSans-Regular.ttf");
    QFontDatabase::addApplicationFont(ResourceDir + "PlusJakartaSans-Bold.ttf");

    if (idRegular != -1) {
        const QString nomeFamilia = QFontDatabase::applicationFontFamilies(idRegular).at(0);
        QFont fonteSistema(nomeFamilia);
        fonteSistema.setStyleStrategy(QFont::PreferAntialias);
        a.setFont(fonteSistema);
    }

    // Ícone do Hotel Paradise
    a.setWindowIcon(QIcon(ResourceDir + "HP_Logo_GoldenNoBackground"));

    bool autenticado = false;   // variavel de controle.
    {   // garante que a tela de login é destuida após o login.
        LoginDialog login;
        autenticado = (login.exec() == QDialog::Accepted);
    }

    if (autenticado) {
        MainWindow w;
        w.show();
        return a.exec();
    }

    return 0;
}
