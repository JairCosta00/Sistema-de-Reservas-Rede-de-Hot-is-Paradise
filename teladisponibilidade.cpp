#include "teladisponibilidade.h"
#include "ui_teladisponibilidade.h"
#include "GerenciadorDeReservas.h"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QLocale>
#include <QPushButton>
#include <QRegularExpression>
#include <QSpinBox>

#include <memory>

TelaDisponibilidade::TelaDisponibilidade(QWidget *parent)
    : QDialog(parent), ui(new Ui::TelaDisponibilidade)
{
    ui->setupUi(this);
    ui->dataCheckin->setMinimumDate(QDate::currentDate());
    ui->dataCheckin->setDate(QDate::currentDate());
    ui->btnRealizarReserva->setEnabled(false);

    // Qualquer alteração na pesquisa exige verificar novamente.
    auto limparConsulta = [this]() {
        ui->btnRealizarReserva->setEnabled(false);
        ui->labelResultado->setText("O resultado da consulta aparecerá aqui.");
        ui->labelResultado->setStyleSheet(
            "color:#64748B;background:#F1F5F9;border-radius:8px;padding:12px;");
    };
    connect(ui->comboHotel, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, limparConsulta);
    connect(ui->comboQuarto, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, limparConsulta);
    connect(ui->dataCheckin, &QDateEdit::dateChanged, this, limparConsulta);
    connect(ui->spinDiarias, QOverload<int>::of(&QSpinBox::valueChanged),
            this, limparConsulta);

    connect(ui->btnConsultar, &QPushButton::clicked, this, [this]() {
        auto *gerenciador = GerenciadorDeReservas::getInstancia();
        bool disponivel = gerenciador->verificarDisponibilidade(
            ui->comboHotel->currentText().toStdString(),
            ui->dataCheckin->date().toString("dd/MM/yyyy").toStdString(),
            ui->comboQuarto->currentText().toStdString(),
            ui->spinDiarias->value());

        ui->btnRealizarReserva->setEnabled(disponivel);
        if (disponivel) {
            ui->labelResultado->setText("Quarto disponível para todas as diárias! Preencha os dados do cliente.");
            ui->labelResultado->setStyleSheet(
                "color:#15803D;background:#DCFCE7;border-radius:8px;padding:12px;font-weight:bold;");
        } else {
            ui->labelResultado->setText("Quarto indisponível neste período. Escolha outra data ou quarto.");
            ui->labelResultado->setStyleSheet(
                "color:#B91C1C;background:#FEE2E2;border-radius:8px;padding:12px;font-weight:bold;");
        }
    });

    connect(ui->btnRealizarReserva, &QPushButton::clicked, this, [this]() {
        auto *gerenciador = GerenciadorDeReservas::getInstancia();
        Atendente *atendente = gerenciador->getAtendenteLogado();
        QString nome = ui->editNomeCliente->text().trimmed();
        QString cpf = ui->editCpfCliente->text().trimmed();
        cpf.remove(QRegularExpression("[^0-9]"));

        if (!atendente) {
            ui->labelResultado->setText("É necessário fazer login antes de reservar.");
            return;
        }
        if (nome.isEmpty() || cpf.size() != 11) {
            ui->labelResultado->setText("Informe o nome completo e um CPF com 11 dígitos.");
            ui->labelResultado->setStyleSheet(
                "color:#B91C1C;background:#FEE2E2;border-radius:8px;padding:12px;");
            return;
        }

        std::string hotel = ui->comboHotel->currentText().toStdString();
        std::string checkin = ui->dataCheckin->date().toString("dd/MM/yyyy").toStdString();
        std::string tipo = ui->comboQuarto->currentText().toStdString();
        int diarias = ui->spinDiarias->value();
        if (!gerenciador->verificarDisponibilidade(hotel, checkin, tipo, diarias)) {
            ui->btnRealizarReserva->setEnabled(false);
            ui->labelResultado->setText("O quarto ficou indisponível. Faça uma nova consulta.");
            ui->labelResultado->setStyleSheet(
                "color:#B91C1C;background:#FEE2E2;border-radius:8px;padding:12px;");
            return;
        }

        float diaria = 200.0f;
        if (tipo == "Duplo") diaria = 300.0f;
        else if (tipo == "Casal") diaria = 350.0f;
        else if (tipo == "Triplo") diaria = 450.0f;
        else if (tipo == "Quadruplo") diaria = 550.0f;

        Cliente cliente(nome.toStdString(), cpf.toStdString());
        Quarto quarto(tipo, diaria);
        Reserva reserva(cliente, *atendente, quarto, hotel, checkin, diarias);
        std::shared_ptr<EstrategiaDesconto> estrategia;
        switch (ui->comboDesconto->currentIndex()) {
            case 1: estrategia = std::make_shared<DescontoVIP>(); break;
            case 2: estrategia = std::make_shared<DescontoBaixaTemporada>(); break;
            case 3: estrategia = std::make_shared<DescontoPromocional>(); break;
            default: estrategia = std::make_shared<SemDesconto>(); break;
        }
        reserva.setEstrategiaDesconto(estrategia);
        reserva.calcularValores();
        reserva.setStatusReserva(false); // Reserva pendente de pagamento.
        const float total = reserva.getValorTotal();
        const float entrada = reserva.getValorEntrada();
        gerenciador->adicionarReserva(reserva);
        gerenciador->salvarEmArquivoTxt();

        const QLocale br(QLocale::Portuguese, QLocale::Brazil);
        ui->labelResultado->setText(
            "Reserva cadastrada! Total: " + br.toCurrencyString(total) +
            " | Entrada: " + br.toCurrencyString(entrada) +
            " | Status: pendente. Consulte a tela Listar reservas.");
        ui->labelResultado->setStyleSheet(
            "color:#15803D;background:#DCFCE7;border-radius:8px;padding:12px;font-weight:bold;");
        ui->btnRealizarReserva->setEnabled(false);
        ui->editNomeCliente->clear();
        ui->editCpfCliente->clear();
    });

    connect(ui->btnVoltar, &QPushButton::clicked, this,
            [this]() { emit voltarSolicitado(); });
}

TelaDisponibilidade::~TelaDisponibilidade()
{
    delete ui;
}
