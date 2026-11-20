
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "GerenciadorDeReservas.h"
#include "teladisponibilidade.h"
#include "telalistagem.h"

#include <QStackedWidget>
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Configuração da janela
    setWindowFlags(Qt::Window |
                   Qt::WindowCloseButtonHint |
                   Qt::WindowTitleHint);

    // Exibe o nome do atendente autenticado
    Atendente* atendente =
        GerenciadorDeReservas::getInstancia()
            ->getAtendenteLogado();

    if (atendente != nullptr) {
        ui->labelAtendente->setText(
            "Atendente: " +
            QString::fromStdString(atendente->getNome())
            );
    }

    // ========================================
    // GERENCIAMENTO DAS TELAS
    // ========================================

    // Guarda a tela principal existente
    QWidget* menuPrincipal = takeCentralWidget();

    // Cria o gerenciador de páginas
    QStackedWidget* paginas = new QStackedWidget(this);

    // Adiciona o menu principal
    paginas->addWidget(menuPrincipal);

    // Cria a tela de disponibilidade
    TelaDisponibilidade* telaDisponibilidade =
        new TelaDisponibilidade(paginas);

    // Impede que abra uma janela separada
    telaDisponibilidade->setWindowFlags(Qt::Widget);

    // Adiciona a tela de disponibilidade
    paginas->addWidget(telaDisponibilidade);

    // Define o conteúdo da janela
    setCentralWidget(paginas);

    // Inicia no menu principal
    paginas->setCurrentWidget(menuPrincipal);

    // ========================================
    // BOTÃO CONSULTAR DISPONIBILIDADE
    // ========================================

    connect(ui->btnConsultarReserva,
            &QPushButton::clicked,
            this, [=]() {

                paginas->setCurrentWidget(telaDisponibilidade);

            });

    // ========================================
    // BOTÃO VOLTAR AO MENU PRINCIPAL
    // ========================================

    connect(telaDisponibilidade,
            &TelaDisponibilidade::voltarSolicitado,
            this, [=]() {

                paginas->setCurrentWidget(menuPrincipal);

            });

    // Cria a tela de listagem de reservas
    TelaListagem* telaListagem =
        new TelaListagem(paginas);

    // Adiciona ao gerenciador de páginas
    paginas->addWidget(telaListagem);


    // ========================================
    // BOTÃO SAIR DO SISTEMA
    // ========================================

    connect(ui->btnSair,
            &QPushButton::clicked,
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

    // Botão Listar Reservas
    connect(ui->btnListarReservas,
            &QPushButton::clicked,
            this, [=]() {

                telaListagem->atualizarLista();
                paginas->setCurrentWidget(telaListagem);

            });

    // Botão Voltar ao Menu Principal
    connect(telaListagem,
            &TelaListagem::voltarSolicitado,
            this, [=]() {

                paginas->setCurrentWidget(menuPrincipal);

            });

}


MainWindow::~MainWindow()
{
    delete ui;
}
