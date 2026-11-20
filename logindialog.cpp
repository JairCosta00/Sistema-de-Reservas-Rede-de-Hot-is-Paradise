#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>
#include <QString>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    // Garante que a senha comece oculta
    ui->lineEditSenha->setEchoMode(QLineEdit::Password);

    // acabamento visual e de usabilidade:
    ui->lineEditLogin->setClearButtonEnabled(true);
    ui->pushButtonEntrar->setCursor(Qt::PointingHandCursor);
    ui->checkBoxMostrarSenha->setCursor(Qt::PointingHandCursor);
    ui->labelTelaNomeHotel->setAlignment(Qt::AlignCenter);
    ui->labelTituloEVersao->setAlignment(Qt::AlignCenter);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_pushButtonEntrar_clicked()
{
    std::string login = ui->lineEditLogin->text().toStdString();
    std::string senha = ui->lineEditSenha->text().toStdString();

    bool autenticado = GerenciadorDeReservas::getInstancia()->realizarLogin(login, senha);

    if (autenticado) // se foi autenticado com sucesso
    {
        // pega o nome do atendente logado
        Atendente* atendente = GerenciadorDeReservas::getInstancia()->getAtendenteLogado();
        QString nome = QString::fromStdString(atendente->getNome());

        // exibe uma pequena saudação em uma MessageBox
        QMessageBox::information(this, "Acesso Permitido", "Bem-Vindo(a), " + nome + "!");

        // limpa o login e a senha da tela
        ui->lineEditLogin->clear();
        ui->lineEditSenha->clear();

        accept(); // Fecha o LoginDialog e libera a abertura da MainWindow
    }
    else
    {
        QMessageBox::warning(this, "Acesso Negado", "Login ou senha Inválidos. Tente novamente.");
        ui->lineEditSenha->clear(); // limpa o texto da senha
        ui->lineEditSenha->setFocus();  // efeito de "foco" / "highlight" no lineEdit da senha
    }
}

void LoginDialog::on_checkBoxMostrarSenha_toggled(bool checked)
{
    if (checked) {
        ui->lineEditSenha->setEchoMode(QLineEdit::Normal);
    } else {
        ui->lineEditSenha->setEchoMode(QLineEdit::Password);
    }
}