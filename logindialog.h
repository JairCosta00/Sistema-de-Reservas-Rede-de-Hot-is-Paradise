#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "GerenciadorDeReservas.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog() override;

private slots:
    void on_pushButtonEntrar_clicked();
    void on_checkBoxMostrarSenha_toggled(bool checked);

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H