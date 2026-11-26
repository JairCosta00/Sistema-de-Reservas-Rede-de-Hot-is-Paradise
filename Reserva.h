#ifndef RESERVA_H
#define RESERVA_H

#include <memory>
#include "Quarto.h"
#include "Cliente.h"
#include "Atendente.h"
#include "EstrategiaDesconto.h"

using namespace std;

class Reserva
{
    private:
        Cliente cliente;
        Atendente atendente;
        Quarto quarto;
        string localidade;
        string checkIn;
        int numeroDiarias;
        std::shared_ptr<EstrategiaDesconto> estrategiaDesconto; // ponteiro para o padrão de projeto dos descontos
        float valorTotal;
        float valorEntrada;
        bool statusReserva;
        bool cancelada = false; // Solicitação cancelada por conflito

    public:
        Reserva();
        // ~Reserva();
        Reserva(Cliente cliente, Atendente atendente, Quarto quarto, string localidade, string checkIn, int numeroDiarias);

        float calcularValores();    // calcular o valor total com base na localidade, diarias e tipo do quarto, alem dos descontos sazonais. 
        void confirmarPagamento();
        void cancelar();
        bool estaCancelada() const { return cancelada; }
        bool estaPendente() const { return !statusReserva && !cancelada; }
        string getDescricaoDesconto() const;

    // ------------------------ GETTERS AND SETTERS ------------------------
        Cliente getCliente() const;

        void setCliente(const Cliente& cliente);

        Atendente getAtendente() const;

        void setAtendente(Atendente atendente);

        Quarto getQuarto() const;

        void setQuarto(Quarto quarto);

        string getLocalidade() const;

        void setLocalidade(string localidade);

        string getCheckIn() const;

        void setcheckIn(string checkIn);

        int getNumeroDiarias() const;

        void setNumeroDiarias(int numeroDiarias);

        std::shared_ptr<EstrategiaDesconto> getEstrategiaDesconto() const;

        void setEstrategiaDesconto(std::shared_ptr<EstrategiaDesconto> estrategiaDesconto);

        float getValorTotal() const;

        void setValorTotal(float valorTotal);

        float getValorEntrada() const;

        void setValorEntrada(float valorEntrada);

        bool getStatusReserva() const;

        void setStatusReserva(bool statusReserva);
};

#endif