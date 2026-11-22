#include "teladisponibilidade.h"
#include "ui_teladisponibilidade.h"
#include "GerenciadorDeReservas.h"

#include <QMessageBox>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
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

        const QLocale br(QLocale::Portuguese, QLocale::Brazil);

        // Monta o resumo da reserva
        QString resumo = "Cliente: " + nome + "\n";
        resumo += "CPF: " + cpf + "\n";
        resumo += "Hotel: " + QString::fromStdString(hotel) + "\n";
        resumo += "Quarto: " + QString::fromStdString(tipo) + "\n";
        resumo += "Check-in: " + QString::fromStdString(checkin) + "\n";
        resumo += "Check-out: " +
                  ui->dataCheckin->date()
                      .addDays(diarias).toString("dd/MM/yyyy") + "\n";
        resumo += "Diárias: " + QString::number(diarias) + "\n";
        resumo += "Desconto: " + ui->comboDesconto->currentText() + "\n";
        resumo += "Valor total: " + br.toCurrencyString(double(total)) + "\n";
        resumo += "Entrada (1/3): " + br.toCurrencyString(double(entrada)) + "\n";
        resumo += "Status: Pendente de confirmação";

        // Solicita confirmação

        // ==========================================
        // JANELA DE CONFIRMAÇÃO DA RESERVA
        // ==========================================

        QMessageBox confirmacao(this);

        confirmacao.setWindowTitle("Hotel Paradise - Confirmar reserva");
        confirmacao.setIcon(QMessageBox::NoIcon);
        confirmacao.setTextFormat(Qt::RichText);

        // Organiza os dados em uma tabela
        QString html = R"(
    <h2 style="color:#0F172A;">
        Confirmar reserva
    </h2>
    <p style="color:#64748B;">
        Confira os dados antes de continuar.
    </p>
    <table width="100%" cellpadding="7" cellspacing="0">
)";

        // Aproveita o resumo que já foi calculado
        for (const QString& linha : resumo.split('\n', Qt::SkipEmptyParts)) {

            int posicao = linha.indexOf(": ");

            if (posicao >= 0) {
                QString campo = linha.left(posicao).toHtmlEscaped();
                QString valor = linha.mid(posicao + 2).toHtmlEscaped();

                html += "<tr>";
                html += "<td><b>" + campo + "</b></td>";
                html += "<td>" + valor + "</td>";
                html += "</tr>";
            }
        }

        html += R"(
    </table>
    <p style="color:#334155;">
        <b>Deseja confirmar esta reserva?</b>
    </p>
)";

        confirmacao.setText(html);

        // Botões personalizados
        QPushButton* btnConfirmar =
            confirmacao.addButton(
                "Confirmar reserva",
                QMessageBox::AcceptRole
                );

        QPushButton* btnCancelar =
            confirmacao.addButton(
                "Cancelar",
                QMessageBox::RejectRole
                );

        btnConfirmar->setObjectName("btnConfirmar");
        btnCancelar->setObjectName("btnCancelar");

        confirmacao.setDefaultButton(btnCancelar);
        confirmacao.setEscapeButton(btnCancelar);

        // Estilo da janela
        confirmacao.setStyleSheet(R"(
    QMessageBox {
        background-color: #F8FAFC;
    }

    QMessageBox QLabel {
        color: #0F172A;
        background: transparent;
        font-size: 13px;
        font-family: "Segoe UI";
    }

    QPushButton {
        min-width: 110px;
        min-height: 38px;
        padding: 4px 16px;
        border-radius: 8px;
        font-size: 13px;
        font-weight: bold;
    }

    QPushButton#btnConfirmar {
        background-color: #2563EB;
        color: white;
        border: none;
    }

    QPushButton#btnConfirmar:hover {
        background-color: #1D4ED8;
    }

    QPushButton#btnCancelar {
        background-color: white;
        color: #334155;
        border: 1px solid #CBD5E1;
    }

    QPushButton#btnCancelar:hover {
        background-color: #F1F5F9;
    }
)");

        // Evita que a janela fique muito estreita
        if (QLabel* label =
            confirmacao.findChild<QLabel*>("qt_msgbox_label")) {
            label->setMinimumWidth(450);
            label->setWordWrap(true);
        }

        // Exibe a confirmação
        confirmacao.exec();

        // Cancela caso o usuário não confirme
        if (confirmacao.clickedButton() != btnConfirmar) {
            return;
        }


        // Registra e salva a reserva
        gerenciador->adicionarReserva(reserva);
        gerenciador->salvarEmArquivoTxt();

        // Exibe o resumo da reserva cadastrada

        // ==========================================
        // MENSAGEM DE RESERVA CADASTRADA COM SUCESSO
        // ==========================================

        QMessageBox sucesso(this);

        sucesso.setWindowTitle("Hotel Paradise - Reserva cadastrada");
        sucesso.setIcon(QMessageBox::NoIcon);
        sucesso.setTextFormat(Qt::RichText);

        // Cabeçalho e resumo da reserva
        QString htmlSucesso = R"(
    <h2 style="color:#15803D;">
        Reserva cadastrada com sucesso!
    </h2>
    <p style="color:#64748B;">
        A reserva foi registrada no sistema.
    </p>
    <table width="100%" cellpadding="7" cellspacing="0">
)";

        // Preenche os dados da reserva
        for (const QString& linha : resumo.split('\n', Qt::SkipEmptyParts)) {

            int posicao = linha.indexOf(": ");

            if (posicao >= 0) {
                QString campo = linha.left(posicao).toHtmlEscaped();
                QString valor = linha.mid(posicao + 2).toHtmlEscaped();

                htmlSucesso += "<tr>";
                htmlSucesso += "<td><b>" + campo + "</b></td>";
                htmlSucesso += "<td>" + valor + "</td>";
                htmlSucesso += "</tr>";
            }
        }

        htmlSucesso += "</table>";

        sucesso.setText(htmlSucesso);

        // Botão Fechar
        QPushButton* btnFechar =
            sucesso.addButton("Fechar", QMessageBox::AcceptRole);

        btnFechar->setObjectName("btnFechar");
        sucesso.setDefaultButton(btnFechar);
        sucesso.setEscapeButton(btnFechar);

        // Estilo visual
        sucesso.setStyleSheet(R"(
    QMessageBox {
        background-color: #F8FAFC;
    }

    QMessageBox QLabel {
        color: #0F172A;
        background: transparent;
        font-size: 13px;
        font-family: "Segoe UI";
    }

    QPushButton#btnFechar {
        background-color: #16A34A;
        color: white;
        border: none;
        border-radius: 8px;
        min-width: 120px;
        min-height: 38px;
        padding: 4px 16px;
        font-size: 13px;
        font-weight: bold;
    }

    QPushButton#btnFechar:hover {
        background-color: #15803D;
    }

    QPushButton#btnFechar:pressed {
        background-color: #166534;
    }
)");

        // Ajusta a largura da mensagem
        if (QLabel* label =
            sucesso.findChild<QLabel*>("qt_msgbox_label")) {
            label->setMinimumWidth(450);
            label->setWordWrap(true);
        }

        // Exibe a mensagem
        sucesso.exec();


        ui->labelResultado->setText(
            "Reserva cadastrada com sucesso!"
            );

        ui->labelResultado->setStyleSheet(
            "color: #15803D;"
            "background-color: #DCFCE7;"
            "border-radius: 8px;"
            "padding: 12px;"
            "font-weight: bold;"
            );

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
