#ifndef RESERVA_H
#define RESERVA_H

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

    public:
        Reserva();
        // ~Reserva();
        Reserva(Cliente cliente, Atendente atendente, Quarto quarto, string localidade, string checkIn, int numeroDiarias);

        float calcularValores();    // calcular o valor total com base na localidade, diarias e tipo do quarto, alem dos descontos sazonais. 
        void confirmarPagamento();

    // ------------------------ GETTERS AND SETTERS ------------------------
        Cliente getCliente();

        void setCliente(const Cliente& cliente);

        Atendente getAtendente();

        void setAtendente(Atendente atendente);

        Quarto getQuarto();

        void setQuarto(Quarto quarto);

        string getLocalidade();

        void setLocalidade(string localidade);

        string getCheckIn();

        void setcheckIn(string checkIn);

        int getNumeroDiarias();

        void setNumeroDiarias(int numeroDiarias);

        std::shared_ptr<EstrategiaDesconto> getEstrategiaDesconto();

        void setEstrategiaDesconto(std::shared_ptr<EstrategiaDesconto> estrategiaDesconto);

        float getValorTotal();

        void setValorTotal(float valorTotal);

        float getValorEntrada();

        void setValorEntrada(float valorEntrada);

        bool getStatusReserva();

        void setStatusReserva(bool statusReserva);
};

#endif