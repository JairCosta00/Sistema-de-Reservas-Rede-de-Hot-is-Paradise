
#include "teladisponibilidade.h"
#include "ui_teladisponibilidade.h"
#include "GerenciadorDeReservas.h"

#include <QDate>
#include <QPushButton>

TelaDisponibilidade::TelaDisponibilidade(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TelaDisponibilidade)
{
    ui->setupUi(this);

    setWindowTitle("Hotel Paradise - Disponibilidade");

    ui->dataCheckin->setDate(QDate::currentDate());
    ui->dataCheckin->setMinimumDate(QDate::currentDate());
    ui->dataCheckin->setCalendarPopup(true);

    // Consultar disponibilidade
    connect(ui->btnConsultar, &QPushButton::clicked,
            this, [this]() {

                QString hotel = ui->comboHotel->currentText();
                QString quarto = ui->comboQuarto->currentText();

                QString data = ui->dataCheckin->date()
                                   .toString("dd/MM/yyyy");

                bool disponivel =
                    GerenciadorDeReservas::getInstancia()
                        ->verificarDisponibilidade(
                            hotel.toStdString(),
                            data.toStdString(),
                            quarto.toStdString()
                            );

                if (disponivel) {
                    ui->labelResultado->setText(
                        "Quarto disponível para reserva!"
                        );

                    ui->labelResultado->setStyleSheet(
                        "color: #15803D;"
                        "background-color: #DCFCE7;"
                        "border-radius: 8px;"
                        "padding: 12px;"
                        "font-weight: bold;"
                        );
                } else {
                    ui->labelResultado->setText(
                        "Quarto indisponível para esta data."
                        );

                    ui->labelResultado->setStyleSheet(
                        "color: #B91C1C;"
                        "background-color: #FEE2E2;"
                        "border-radius: 8px;"
                        "padding: 12px;"
                        "font-weight: bold;"
                        );
                }
            });

    // Voltar para o menu principal
    connect(ui->btnVoltar, &QPushButton::clicked,
            this, [this]() {
                emit voltarSolicitado();
            });
}

TelaDisponibilidade::~TelaDisponibilidade()
{
    delete ui;
}
