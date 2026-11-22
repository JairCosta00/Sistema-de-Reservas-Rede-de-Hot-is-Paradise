#include "logindialog.h"
#include "ui_logindialog.h"
#include "GerenciadorDeReservas.h"

#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QTimer>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    // Configurações dos campos e botões
    ui->lineEditSenha->setEchoMode(QLineEdit::Password);
    ui->lineEditLogin->setClearButtonEnabled(true);
    ui->pushButtonEntrar->setCursor(Qt::PointingHandCursor);
    ui->checkBoxMostrarSenha->setCursor(Qt::PointingHandCursor);
    ui->labelTelaNomeHotel->setAlignment(Qt::AlignCenter);
    ui->labelTituloEVersao->setAlignment(Qt::AlignCenter);

    // A mensagem aparece abaixo do botão Entrar
    ui->labelMensagem->clear();
    ui->labelMensagem->hide();
    ui->labelMensagem->setAlignment(Qt::AlignCenter);
    ui->labelMensagem->setWordWrap(true);

    // Ao corrigir login ou senha, o aviso desaparece
    connect(ui->lineEditLogin, &QLineEdit::textChanged,
            this, [this]() {
                ui->labelMensagem->clear();
                ui->labelMensagem->hide();
            });

    connect(ui->lineEditSenha, &QLineEdit::textChanged,
            this, [this]() {
                ui->labelMensagem->clear();
                ui->labelMensagem->hide();
            });
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_pushButtonEntrar_clicked()
{
    const QString login = ui->lineEditLogin->text().trimmed();
    const QString senha = ui->lineEditSenha->text();

    // Mostra um erro discreto abaixo do botão, sem janela modal
    auto mostrarErro = [this](const QString& mensagem) {
        ui->labelMensagem->setStyleSheet(
            "QLabel#labelMensagem { color: #B42318; background: transparent; "
            "border: none; font-size: 12px; font-weight: 600; "
            "padding-top: 5px; }");
        ui->labelMensagem->setText(mensagem);
        ui->labelMensagem->show();
    };

    if (login.isEmpty() || senha.isEmpty()) {
        mostrarErro("Preencha o usuário e a senha para continuar.");
        if (login.isEmpty()) {
            ui->lineEditLogin->setFocus();
        } else {
            ui->lineEditSenha->setFocus();
        }
        return;
    }

    // Usa a autenticação do Singleton já existente no projeto
    const bool autenticado =
        GerenciadorDeReservas::getInstancia()->realizarLogin(
            login.toStdString(), senha.toStdString());

    if (autenticado) {
        // Mostra uma confirmação discreta antes de abrir o menu principal
        Atendente* atendente =
            GerenciadorDeReservas::getInstancia()->getAtendenteLogado();

        QString mensagem = "Acesso permitido!";
        if (atendente != nullptr) {
            mensagem += " Bem-vindo(a), " +
                QString::fromStdString(atendente->getNome()) + "!";
        }

        // Limpa os campos antes de exibir a mensagem:
        // textChanged oculta mensagens anteriores.
        ui->lineEditLogin->clear();
        ui->lineEditSenha->clear();
        ui->pushButtonEntrar->setEnabled(false);

        ui->labelMensagem->setStyleSheet(
            "QLabel#labelMensagem { color: #15803D; background: transparent; "
            "border: none; font-size: 12px; font-weight: 600; "
            "padding-top: 5px; }");
        ui->labelMensagem->setText(mensagem);
        ui->labelMensagem->show();

        // Dá tempo de ler a mensagem, depois abre a MainWindow
        QTimer::singleShot(1100, this, [this]() {
            accept();
        });
        return;
    }

    // Limpa somente a senha, mantém o usuário para nova tentativa
    ui->lineEditSenha->clear();
    mostrarErro("Acesso negado. Verifique seu usuário e senha.");
    ui->lineEditSenha->setFocus();
}

void LoginDialog::on_checkBoxMostrarSenha_toggled(bool checked)
{
    ui->lineEditSenha->setEchoMode(
        checked ? QLineEdit::Normal : QLineEdit::Password);
}
