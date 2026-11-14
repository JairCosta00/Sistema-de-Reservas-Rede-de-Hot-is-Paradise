#include "Reserva.h"

Cliente Reserva::getCliente() 
{
    return cliente; 
}

void Reserva::setCliente(const Cliente& cliente) 
{ 
    this->cliente = cliente; 
}

Atendente Reserva::getAtendente() 
{ 
    return atendente; 
}

void Reserva::setAtendente(Atendente atendente) 
{ 
    this->atendente = atendente; 
}

Quarto Reserva::getQuarto() 
{
    return quarto;
}

void Reserva::setQuarto(Quarto quarto) 
{
    this->quarto = quarto;
}

string Reserva::getLocalidade() 
{ 
    return localidade; 
}

void Reserva::setLocalidade(string localidade) 
{ 
    this->localidade = localidade; 
}

string Reserva::getCheckIn()
{ 
    return checkIn; 
}

void Reserva::setcheckIn(string checkIn)
{ 
    this->checkIn = checkIn; 
}

int Reserva::getNumeroDiarias()
{ 
    return numeroDiarias; 
}

void Reserva::setNumeroDiarias(int numeroDiarias) 
{ 
    this->numeroDiarias = numeroDiarias; 
}

std::shared_ptr<EstrategiaDesconto> Reserva::getEstrategiaDesconto() 
{
    return estrategiaDesconto;
}

void Reserva::setEstrategiaDesconto(std::shared_ptr<EstrategiaDesconto> estrategiaDesconto) 
{
    this->estrategiaDesconto = estrategiaDesconto;
}

float Reserva::getValorTotal() 
{ 
    return valorTotal; 
}
void Reserva::setValorTotal(float valorTotal) 
{ 
    this->valorTotal = valorTotal; 
}

float Reserva::getValorEntrada() 
{ 
    return valorEntrada; 
}

void Reserva::setValorEntrada(float valorEntrada) 
{ 
    this->valorEntrada = valorEntrada; 
}

bool Reserva::getStatusReserva() 
{ 
    return statusReserva; 
}

void Reserva::setStatusReserva(bool statusReserva) 
{ 
    this->statusReserva = statusReserva; 
}