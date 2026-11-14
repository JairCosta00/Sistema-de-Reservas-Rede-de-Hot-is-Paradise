#include <iostream>
#include "Cliente.h"

Cliente::Cliente()
{
    cout << "Objeto Cliente criado sem parametros." << endl;
}

Cliente::Cliente(string nome, string cpf)
{
    this->nome = nome;
    this->cpf = cpf;
}

string Cliente::getNome()
{
    return nome;
}

void Cliente::setNome(string nome) 
{
    this->nome = nome;
}

string Cliente::getCpf() 
{
    return cpf;
}

void Cliente::setCpf(string cpf)
{
    this->cpf = cpf;
}