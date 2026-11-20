
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "GerenciadorDeReservas.h"

#include <QApplication>
#include <QMessageBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    Atendente* atendente =
        GerenciadorDeReservas::getInstancia()
            ->getAtendenteLogado();

    if (atendente != nullptr) {
        ui->labelAtendente->setText(
            "Atendente: " +
            QString::fromStdString(atendente->getNome())
            );
    }


    // Botão Sair do Sistema
    connect(ui->btnSair, &QPushButton::clicked,
            this, [this]() {

                QMessageBox::StandardButton resposta =
                    QMessageBox::question(
                        this,
                        "Sair do Sistema",
                        "Deseja realmente sair do sistema?",
                        QMessageBox::Yes | QMessageBox::No,
                        QMessageBox::No
                        );

                if (resposta == QMessageBox::Yes) {

                    // Salva as reservas existentes
                    GerenciadorDeReservas::getInstancia()
                        ->salvarEmArquivoTxt();

                    // Encerra o sistema
                    QApplication::quit();
                }
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}
