#include <iostream>
#include "Cliente.h"

Cliente::Cliente() : Pessoa()
{
    cout << "Objeto Cliente criado sem parametros." << endl;
}

Cliente::Cliente(string nome, string cpf)
    : Pessoa(nome)
{
    this->cpf = cpf;
}

string Cliente::getCpf() const
{
    return cpf;
}

void Cliente::setCpf(string cpf)
{
    this->cpf = cpf;
}