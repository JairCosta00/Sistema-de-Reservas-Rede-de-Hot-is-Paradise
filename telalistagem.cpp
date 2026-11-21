#include "telalistagem.h"
#include "ui_telalistagem.h"
#include "GerenciadorDeReservas.h"

#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QHeaderView>
#include <QLocale>
#include <QPushButton>
#include <QTableWidgetItem>

TelaListagem::TelaListagem(QWidget *parent)
    : QWidget(parent), ui(new Ui::TelaListagem)
{
    ui->setupUi(this);


    // Garante o fundo branco na tela de listagem
    setObjectName("TelaListagem");
    setAttribute(Qt::WA_StyledBackground, true);

    setStyleSheet(styleSheet() + R"(
        QWidget#TelaListagem {
            background-color: #F8FAFC;
        }

        QLabel#labelTitulo {
            color: #1F2937;
        }

        QLabel#labelDescricao {
            color: #64748B;
        }

        QLabel#labelMarca {
            color: #B8863B;
        }

        QLabel#labelRodape {
            color: #64748B;
        }
    )");



    // Restaurar o botão Voltar
    ui->btnVoltar->setStyleSheet(R"(
    QPushButton {
        background-color: #FFFFFF;
        color: #334155;
        border: 1px solid #CBD5E1;
        border-radius: 9px;
        padding: 10px 18px;
        font-size: 13px;
        font-weight: bold;
    }

    QPushButton:hover {
        background-color: #F1F5F9;
    }
)");

    ui->btnAtualizar->setStyleSheet(R"(
    QPushButton {
        background-color: #2563EB;
        color: #FFFFFF;
        border: 1px solid #2563EB;
        border-radius: 9px;
        padding: 10px 18px;
        font-size: 13px;
        font-weight: bold;
    }

    QPushButton:hover {
        background-color: #1D4ED8;
    }
)");

    // Garantir que os botões estejam visíveis
    ui->btnVoltar->show();
    ui->btnAtualizar->show();


    auto *tabela = ui->tabelaReservas;
    tabela->setColumnCount(9);
    tabela->setHorizontalHeaderLabels({
        "Cliente", "CPF", "Hotel", "Check-in", "Diárias",
        "Quarto", "Atendente", "Valor total", "Status"
    });

    tabela->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabela->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabela->setSelectionMode(QAbstractItemView::SingleSelection);
    tabela->setAlternatingRowColors(true);
    tabela->setShowGrid(false);
    tabela->verticalHeader()->setVisible(false);
    tabela->verticalHeader()->setDefaultSectionSize(42);
    tabela->horizontalHeader()->setStretchLastSection(true);
    tabela->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    tabela->setColumnWidth(0, 170);
    tabela->setColumnWidth(1, 135);
    tabela->setColumnWidth(2, 155);
    tabela->setColumnWidth(3, 105);
    tabela->setColumnWidth(4, 80);
    tabela->setColumnWidth(5, 105);
    tabela->setColumnWidth(6, 120);
    tabela->setColumnWidth(7, 130);
    tabela->setColumnWidth(8, 115);

    connect(ui->btnVoltar, &QPushButton::clicked,
            this, [this]() { emit voltarSolicitado(); });

    connect(ui->btnAtualizar, &QPushButton::clicked,
            this, &TelaListagem::atualizarLista);

    atualizarLista();
}

TelaListagem::~TelaListagem()
{
    delete ui;
}

void TelaListagem::atualizarLista()
{
    const auto& reservas =
        GerenciadorDeReservas::getInstancia()->getReservas();

    auto *tabela = ui->tabelaReservas;
    tabela->setSortingEnabled(false);
    tabela->clearContents();
    tabela->setRowCount(static_cast<int>(reservas.size()));

    const QLocale moeda(QLocale::Portuguese, QLocale::Brazil);

    for (int linha = 0; linha < static_cast<int>(reservas.size()); ++linha) {
        const Reserva& reserva = reservas[static_cast<std::size_t>(linha)];

        auto adicionarCelula = [tabela, linha](int coluna, const QString& valor) {
            auto *item = new QTableWidgetItem(valor);
            tabela->setItem(linha, coluna, item);
            return item;
        };

        adicionarCelula(0, QString::fromStdString(reserva.getCliente().getNome()));
        adicionarCelula(1, QString::fromStdString(reserva.getCliente().getCpf()));
        adicionarCelula(2, QString::fromStdString(reserva.getLocalidade()));
        adicionarCelula(3, QString::fromStdString(reserva.getCheckIn()));
        adicionarCelula(4, QString::number(reserva.getNumeroDiarias()));
        adicionarCelula(5, QString::fromStdString(reserva.getQuarto().getTipoQuarto()));
        adicionarCelula(6, QString::fromStdString(reserva.getAtendente().getNome()));
        adicionarCelula(7, moeda.toCurrencyString(static_cast<double>(reserva.getValorTotal())));

        const bool confirmada = reserva.getStatusReserva();
        auto *status = adicionarCelula(8, confirmada ? "Confirmada" : "Pendente");
        status->setForeground(QBrush(QColor(confirmada ? "#15803D" : "#B45309")));
        QFont fonte = status->font();
        fonte.setBold(true);
        status->setFont(fonte);
    }

    const int quantidade = static_cast<int>(reservas.size());
    ui->labelTotal->setText(
        QString::number(quantidade) +
        (quantidade == 1 ? " reserva cadastrada" : " reservas cadastradas")
    );
    ui->labelVazio->setVisible(reservas.empty());
    tabela->setSortingEnabled(true);
}
