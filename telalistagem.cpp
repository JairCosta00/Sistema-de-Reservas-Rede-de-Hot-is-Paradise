#include "telalistagem.h"
#include "ui_telalistagem.h"
#include "GerenciadorDeReservas.h"

#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QDate>
#include <QFont>
#include <QHeaderView>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QCoreApplication>

// Mantém a ordenação correta das datas no formato dd/MM/yyyy.
class ItemDataCheckin : public QTableWidgetItem
{
public:
    explicit ItemDataCheckin(const QString& texto)
        : QTableWidgetItem(texto) {}

    bool operator<(const QTableWidgetItem& outro) const override
    {
        const QDate a = QDate::fromString(text(), "dd/MM/yyyy");
        const QDate b = QDate::fromString(outro.text(), "dd/MM/yyyy");
        if (a.isValid() && b.isValid())
            return a < b;
        return QTableWidgetItem::operator<(outro);
    }
};

TelaListagem::TelaListagem(QWidget *parent)
    : QWidget(parent), ui(new Ui::TelaListagem)
{
    ui->setupUi(this);

    setObjectName("TelaListagem");

    setAttribute(Qt::WA_StyledBackground, true);
    QString ResourceDir =
        QCoreApplication::applicationDirPath() + "/Resource Files/";

    setStyleSheet(
        styleSheet() +
        QString("QWidget#TelaListagem { border-image: url(\"%1\") 0 0 0 0 stretch stretch; }")
            .arg(ResourceDir + "Background_TelaListagem.jpg")  // OLD: GreenBackground5
        );

    ui->btnConfirmarPagamento->setEnabled(false);

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

    // Usa o índice original da reserva (Qt::UserRole) para evitar
    // confirmar a reserva errada quando a tabela for ordenada.
    auto indiceSelecionado = [this]() -> int {
        auto *tabela = ui->tabelaReservas;
        if (tabela->selectedItems().isEmpty())
            return -1;

        const int linha = tabela->currentRow();
        QTableWidgetItem *cliente = tabela->item(linha, 0);
        if (!cliente)
            return -1;

        bool ok = false;
        const int indice = cliente->data(Qt::UserRole).toInt(&ok);
        return ok ? indice : -1;
    };

    connect(tabela, &QTableWidget::itemSelectionChanged,
            this, [this, indiceSelecionado]() {
        const auto& reservas = GerenciadorDeReservas::getInstancia()->getReservas();
        const int indice = indiceSelecionado();
        const bool pendente = indice >= 0 &&
            static_cast<std::size_t>(indice) < reservas.size() &&
            reservas[static_cast<std::size_t>(indice)].estaPendente();
        ui->btnConfirmarPagamento->setEnabled(pendente);
    });

    connect(ui->btnConfirmarPagamento, &QPushButton::clicked,
            this, [this, indiceSelecionado]() {
        auto *gerenciador = GerenciadorDeReservas::getInstancia();
        const int indice = indiceSelecionado();
        const auto& reservas = gerenciador->getReservas();

        if (indice < 0 || static_cast<std::size_t>(indice) >= reservas.size()) {
            return;
        }

        const Reserva& reserva = reservas[static_cast<std::size_t>(indice)];
        if (!reserva.estaPendente()) {
            atualizarLista();
            return;
        }

        const QLocale br(QLocale::Portuguese, QLocale::Brazil);
        QMessageBox pergunta(this);
        pergunta.setWindowTitle("Hotel Paradise - Confirmar pagamento");
        pergunta.setIcon(QMessageBox::NoIcon);
        pergunta.setText("Confirmar recebimento da entrada?");
        pergunta.setInformativeText(
            "Cliente: " + QString::fromStdString(reserva.getCliente().getNome()) +
            "\nHotel: " + QString::fromStdString(reserva.getLocalidade()) +
            "\nCheck-in: " + QString::fromStdString(reserva.getCheckIn()) +
            "\nEntrada: " + br.toCurrencyString(static_cast<double>(reserva.getValorEntrada()))
        );

        QPushButton *btnConfirmar = pergunta.addButton("Confirmar pagamento", QMessageBox::AcceptRole);
        QPushButton *btnCancelar = pergunta.addButton("Cancelar", QMessageBox::RejectRole);
        btnConfirmar->setObjectName("btnConfirmar");
        btnCancelar->setObjectName("btnCancelar");
        pergunta.setDefaultButton(btnCancelar);
        pergunta.setEscapeButton(btnCancelar);
        pergunta.setStyleSheet(R"(
            QMessageBox { background-color: #FFFFFF; }
            QMessageBox QLabel { color: #1F2937; background: transparent; font-size: 13px; }
            QPushButton { border-radius: 8px; padding: 9px 14px; font-weight: bold; min-width: 110px; }
            QPushButton#btnConfirmar { color: white; background-color: #15803D; border: none; }
            QPushButton#btnConfirmar:hover { background-color: #166534; }
            QPushButton#btnCancelar { color: #334155; background-color: white; border: 1px solid #CBD5E1; }
        )");
        pergunta.exec();
        if (pergunta.clickedButton() != btnConfirmar)
            return;

        std::vector<std::string> canceladas;
        std::string motivo;
        const bool confirmado = gerenciador->confirmarPagamentoReserva(
            static_cast<std::size_t>(indice), &canceladas, &motivo);

        atualizarLista();

        if (!confirmado) {
            QMessageBox aviso(this);
            aviso.setWindowTitle("Hotel Paradise - Solicitação não confirmada");
            aviso.setIcon(QMessageBox::NoIcon);
            aviso.setText("Não foi possível confirmar o pagamento.");
            aviso.setInformativeText(QString::fromStdString(motivo));
            QPushButton* fechar = aviso.addButton("Entendi", QMessageBox::AcceptRole);
            fechar->setObjectName("btnFecharAviso");
            aviso.setStyleSheet(R"(
                QMessageBox { background-color: #FFFFFF; }
                QMessageBox QLabel { color: #1F2937; background: transparent; font-size: 13px; }
                QPushButton#btnFecharAviso { background-color: #2563EB; color: white;
                     border: none; border-radius: 8px; padding: 9px 18px; font-weight: bold; }
            )");
            aviso.exec();
            return;
        }

        QMessageBox sucesso(this);
        sucesso.setWindowTitle("Hotel Paradise - Pagamento confirmado");
        sucesso.setIcon(QMessageBox::NoIcon);
        sucesso.setText("Pagamento confirmado! A reserva está Confirmada.");

        QString detalhes =
            "Cliente: " + QString::fromStdString(reserva.getCliente().getNome()) +
            "\nHotel: " + QString::fromStdString(reserva.getLocalidade()) +
            "\nQuarto: " + QString::fromStdString(reserva.getQuarto().getTipoQuarto()) +
            "\nCheck-in: " + QString::fromStdString(reserva.getCheckIn());

        if (!canceladas.empty()) {
            detalhes += "\n\nSolicitações pendentes canceladas por conflito:";
            for (const auto& nome : canceladas) {
                detalhes += "\n• " + QString::fromStdString(nome);
            }
        }
        sucesso.setInformativeText(detalhes);
        QPushButton* fechar = sucesso.addButton("Fechar", QMessageBox::AcceptRole);
        fechar->setObjectName("btnFecharSucesso");
        sucesso.setStyleSheet(R"(
            QMessageBox { background-color: #FFFFFF; }
            QMessageBox QLabel { color: #1F2937; background: transparent; font-size: 13px; }
            QPushButton#btnFecharSucesso { background-color: #15803D; color: white;
                 border: none; border-radius: 8px; padding: 9px 18px; font-weight: bold; }
        )");
        sucesso.exec();
    });

    atualizarLista();
}

TelaListagem::~TelaListagem()
{
    delete ui;
}

void TelaListagem::atualizarLista()
{
    const auto& reservas = GerenciadorDeReservas::getInstancia()->getReservas();
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

        // Guarda o índice original da reserva nesta célula para uso após a ordenação.
        QTableWidgetItem* itemCliente = adicionarCelula(
            0, QString::fromStdString(reserva.getCliente().getNome())
        );
        itemCliente->setData(Qt::UserRole, linha);

        adicionarCelula(1, QString::fromStdString(reserva.getCliente().getCpf()));
        adicionarCelula(2, QString::fromStdString(reserva.getLocalidade()));
        tabela->setItem(linha, 3,
            new ItemDataCheckin(QString::fromStdString(reserva.getCheckIn())));
        adicionarCelula(4, QString::number(reserva.getNumeroDiarias()));
        adicionarCelula(5, QString::fromStdString(reserva.getQuarto().getTipoQuarto()));
        adicionarCelula(6, QString::fromStdString(reserva.getAtendente().getNome()));
        adicionarCelula(7, moeda.toCurrencyString(static_cast<double>(reserva.getValorTotal())));

        const bool confirmada = reserva.getStatusReserva();
        const bool cancelada = reserva.estaCancelada();
        auto *status = adicionarCelula(8,
            cancelada ? "Cancelada" : (confirmada ? "Confirmada" : "Pendente"));
        status->setForeground(QBrush(QColor(cancelada ? "#B91C1C" :
                                              (confirmada ? "#15803D" : "#B45309"))));
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
    tabela->sortItems(3, Qt::AscendingOrder);
    tabela->clearSelection();
    ui->btnConfirmarPagamento->setEnabled(false);
}
