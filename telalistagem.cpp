
#include "telalistagem.h"
#include "ui_telalistagem.h"
#include "GerenciadorDeReservas.h"

#include <QDate>
#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QHeaderView>
#include <QLocale>
#include <QPushButton>
#include <QTableWidgetItem>

// ==========================================
// ORDENAÇÃO CORRETA DAS DATAS DE CHECK-IN
// ==========================================

class ItemDataCheckin : public QTableWidgetItem
{
public:
    explicit ItemDataCheckin(const QString& data)
        : QTableWidgetItem(data) {}

    bool operator<(const QTableWidgetItem& outro) const override
    {
        QDate dataA = QDate::fromString(
            text(), "dd/MM/yyyy"
            );

        QDate dataB = QDate::fromString(
            outro.text(), "dd/MM/yyyy"
            );

        if (dataA.isValid() && dataB.isValid()) {
            return dataA < dataB;
        }

        return QTableWidgetItem::operator<(outro);
    }
};

// ==========================================
// CONSTRUTOR DA TELA
// ==========================================

TelaListagem::TelaListagem(QWidget *parent)
    : QWidget(parent), ui(new Ui::TelaListagem)
{
    ui->setupUi(this);

    // Fundo branco da tela
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

    // ==========================================
    // ESTILO DO BOTÃO VOLTAR
    // ==========================================

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

    // ==========================================
    // ESTILO DO BOTÃO ATUALIZAR
    // ==========================================

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

    // Garante a visibilidade dos botões
    ui->btnVoltar->show();
    ui->btnAtualizar->show();

    // ==========================================
    // CONFIGURAÇÃO DA TABELA
    // ==========================================

    auto *tabela = ui->tabelaReservas;

    tabela->setColumnCount(9);

    tabela->setHorizontalHeaderLabels({
        "Cliente",
        "CPF",
        "Hotel",
        "Check-in",
        "Diárias",
        "Quarto",
        "Atendente",
        "Valor total",
        "Status"
    });

    // Configurações de visualização
    tabela->setEditTriggers(
        QAbstractItemView::NoEditTriggers
        );

    tabela->setSelectionBehavior(
        QAbstractItemView::SelectRows
        );

    tabela->setSelectionMode(
        QAbstractItemView::SingleSelection
        );

    tabela->setAlternatingRowColors(true);
    tabela->setShowGrid(false);

    // Configuração dos cabeçalhos
    tabela->verticalHeader()->setVisible(false);
    tabela->verticalHeader()->setDefaultSectionSize(42);

    tabela->horizontalHeader()->setStretchLastSection(true);

    tabela->horizontalHeader()->setDefaultAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
        );

    // Largura das colunas
    tabela->setColumnWidth(0, 170);
    tabela->setColumnWidth(1, 135);
    tabela->setColumnWidth(2, 155);
    tabela->setColumnWidth(3, 105);
    tabela->setColumnWidth(4, 80);
    tabela->setColumnWidth(5, 105);
    tabela->setColumnWidth(6, 120);
    tabela->setColumnWidth(7, 130);
    tabela->setColumnWidth(8, 115);

    // ==========================================
    // CONEXÕES DOS BOTÕES
    // ==========================================

    // Voltar ao menu principal
    connect(
        ui->btnVoltar,
        &QPushButton::clicked,
        this,
        [this]() {
            emit voltarSolicitado();
        }
        );

    // Atualizar a lista de reservas
    connect(
        ui->btnAtualizar,
        &QPushButton::clicked,
        this,
        &TelaListagem::atualizarLista
        );

    // Carrega as reservas ao abrir a tela
    atualizarLista();
}

// ==========================================
// DESTRUTOR
// ==========================================

TelaListagem::~TelaListagem()
{
    delete ui;
}

// ==========================================
// ATUALIZAR LISTA DE RESERVAS
// ==========================================

void TelaListagem::atualizarLista()
{
    // Obtém as reservas do Singleton
    const auto& reservas =
        GerenciadorDeReservas::getInstancia()
            ->getReservas();

    auto *tabela = ui->tabelaReservas;

    // Desativa a ordenação durante o preenchimento
    tabela->setSortingEnabled(false);

    // Limpa os dados anteriores
    tabela->clearContents();

    tabela->setRowCount(
        static_cast<int>(reservas.size())
        );

    // Formatação monetária brasileira
    const QLocale moeda(
        QLocale::Portuguese,
        QLocale::Brazil
        );

    // ==========================================
    // PREENCHIMENTO DA TABELA
    // ==========================================

    for (int linha = 0;
         linha < static_cast<int>(reservas.size());
         ++linha)
    {
        const Reserva& reserva =
            reservas[static_cast<std::size_t>(linha)];

        // Função auxiliar para inserir células
        auto adicionarCelula =
            [tabela, linha](int coluna, const QString& valor)
        {
            auto *item = new QTableWidgetItem(valor);

            tabela->setItem(linha, coluna, item);

            return item;
        };

        // Cliente
        adicionarCelula(
            0,
            QString::fromStdString(
                reserva.getCliente().getNome()
                )
            );

        // CPF
        adicionarCelula(
            1,
            QString::fromStdString(
                reserva.getCliente().getCpf()
                )
            );

        // Hotel
        adicionarCelula(
            2,
            QString::fromStdString(
                reserva.getLocalidade()
                )
            );

        // ==========================================
        // CHECK-IN COM ORDENAÇÃO CRONOLÓGICA
        // ==========================================

        tabela->setItem(
            linha,
            3,
            new ItemDataCheckin(
                QString::fromStdString(
                    reserva.getCheckIn()
                    )
                )
            );

        // Número de diárias
        adicionarCelula(
            4,
            QString::number(
                reserva.getNumeroDiarias()
                )
            );

        // Tipo de quarto
        adicionarCelula(
            5,
            QString::fromStdString(
                reserva.getQuarto().getTipoQuarto()
                )
            );

        // Atendente responsável
        adicionarCelula(
            6,
            QString::fromStdString(
                reserva.getAtendente().getNome()
                )
            );

        // Valor total
        adicionarCelula(
            7,
            moeda.toCurrencyString(
                static_cast<double>(
                    reserva.getValorTotal()
                    )
                )
            );

        // ==========================================
        // STATUS DA RESERVA
        // ==========================================

        const bool confirmada =
            reserva.getStatusReserva();

        auto *status = adicionarCelula(
            8,
            confirmada ? "Confirmada" : "Pendente"
            );

        // Verde para confirmada e laranja para pendente
        status->setForeground(
            QBrush(
                QColor(
                    confirmada ? "#15803D" : "#B45309"
                    )
                )
            );

        QFont fonte = status->font();
        fonte.setBold(true);
        status->setFont(fonte);
    }

    // ==========================================
    // CONTADOR DE RESERVAS
    // ==========================================

    const int quantidade =
        static_cast<int>(reservas.size());

    ui->labelTotal->setText(
        QString::number(quantidade) +
        (quantidade == 1
             ? " reserva cadastrada"
             : " reservas cadastradas")
        );

    // Exibe a mensagem se não houver reservas
    ui->labelVazio->setVisible(reservas.empty());

    // ==========================================
    // ORDENAÇÃO AUTOMÁTICA POR CHECK-IN
    // ==========================================

    // Reativa a ordenação da tabela
    tabela->setSortingEnabled(true);

    // Coluna 3 = Check-in
    // AscendingOrder = data mais antiga primeiro
    tabela->sortItems(3, Qt::AscendingOrder);
}
