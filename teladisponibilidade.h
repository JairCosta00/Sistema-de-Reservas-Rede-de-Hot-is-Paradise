#ifndef TELADISPONIBILIDADE_H
#define TELADISPONIBILIDADE_H

#include <QDialog>

namespace Ui { class TelaDisponibilidade; }

class TelaDisponibilidade : public QDialog
{
    Q_OBJECT

public:
    explicit TelaDisponibilidade(QWidget *parent = nullptr);
    ~TelaDisponibilidade() override;

signals:
    void voltarSolicitado();

private:
    Ui::TelaDisponibilidade *ui;
};

#endif // TELADISPONIBILIDADE_H
