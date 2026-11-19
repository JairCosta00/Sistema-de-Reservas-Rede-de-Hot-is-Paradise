#include "Reserva.h"

Cliente Reserva::getCliente() const
{
    return cliente; 
}

void Reserva::setCliente(const Cliente& cliente) 
{ 
    this->cliente = cliente; 
}

Atendente Reserva::getAtendente() const
{ 
    return atendente; 
}

void Reserva::setAtendente(Atendente atendente) 
{ 
    this->atendente = atendente; 
}

Quarto Reserva::getQuarto() const
{
    return quarto;
}

void Reserva::setQuarto(Quarto quarto) 
{
    this->quarto = quarto;
}

string Reserva::getLocalidade() const
{ 
    return localidade; 
}

void Reserva::setLocalidade(string localidade) 
{ 
    this->localidade = localidade; 
}

string Reserva::getCheckIn() const
{ 
    return checkIn; 
}

void Reserva::setcheckIn(string checkIn)
{ 
    this->checkIn = checkIn; 
}

int Reserva::getNumeroDiarias() const
{ 
    return numeroDiarias; 
}

void Reserva::setNumeroDiarias(int numeroDiarias) 
{ 
    this->numeroDiarias = numeroDiarias; 
}

std::shared_ptr<EstrategiaDesconto> Reserva::getEstrategiaDesconto() const
{
    return estrategiaDesconto;
}

void Reserva::setEstrategiaDesconto(std::shared_ptr<EstrategiaDesconto> estrategiaDesconto) 
{
    this->estrategiaDesconto = estrategiaDesconto;
}

float Reserva::getValorTotal() const
{ 
    return valorTotal; 
}
void Reserva::setValorTotal(float valorTotal) 
{ 
    this->valorTotal = valorTotal; 
}

float Reserva::getValorEntrada() const
{ 
    return valorEntrada; 
}

void Reserva::setValorEntrada(float valorEntrada) 
{ 
    this->valorEntrada = valorEntrada; 
}

bool Reserva::getStatusReserva() const
{ 
    return statusReserva; 
}

void Reserva::setStatusReserva(bool statusReserva) 
{ 
    this->statusReserva = statusReserva; 
}

Reserva::Reserva() : numeroDiarias(0), valorTotal(0.0f), valorEntrada(0.0f), statusReserva(false) {}

Reserva::Reserva(Cliente cliente, Atendente atendente, Quarto quarto, string localidade, string checkIn, int numeroDiarias)
    : cliente(cliente), atendente(atendente), quarto(quarto),
      localidade(localidade), checkIn(checkIn), numeroDiarias(numeroDiarias),
      valorTotal(0.0f), valorEntrada(0.0f), statusReserva(false) {}

float Reserva::calcularValores()
{
    float valorDiaria = 0.0f;
    if (quarto.getTipoQuarto() == "Solteiro") valorDiaria = 200.0f;
    else if (quarto.getTipoQuarto() == "Duplo") valorDiaria = 300.0f;
    else if (quarto.getTipoQuarto() == "Casal") valorDiaria = 350.0f;
    else if (quarto.getTipoQuarto() == "Triplo") valorDiaria = 450.0f;
    else if (quarto.getTipoQuarto() == "Quadruplo") valorDiaria = 550.0f;

    float valorBruto = valorDiaria * numeroDiarias;
    valorTotal = (estrategiaDesconto != nullptr)
                 ? estrategiaDesconto->calcularValorFinal(valorBruto)
                 : valorBruto;

    valorEntrada = valorTotal / 3.0f;
    return valorTotal;
}

void Reserva::confirmarPagamento()
{
    this->statusReserva = true;
}