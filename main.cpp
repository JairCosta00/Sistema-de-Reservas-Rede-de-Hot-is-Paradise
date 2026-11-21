#include "mainwindow.h"
#include "logindialog.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Ícone do Hotel Paradise
    a.setWindowIcon(QIcon(":/imagens/icone_hotel.png"));

    LoginDialog login;
    if (login.exec() == QDialog::Accepted) {
        MainWindow w;
        w.show();
        return a.exec();
    }

    return 0;
}
