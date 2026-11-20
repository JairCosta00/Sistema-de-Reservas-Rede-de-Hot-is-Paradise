#ifndef TELALISTAGEM_H
#define TELALISTAGEM_H

#include <QWidget>

namespace Ui {
class TelaListagem;
}

class TelaListagem : public QWidget
{
    Q_OBJECT

public:
    explicit TelaListagem(QWidget *parent = nullptr);
    ~TelaListagem() override;

    // Atualize sempre que abrir a página para mostrar novas reservas.
    void atualizarLista();

signals:
    void voltarSolicitado();

private:
    Ui::TelaListagem *ui;
};

#endif // TELALISTAGEM_H
