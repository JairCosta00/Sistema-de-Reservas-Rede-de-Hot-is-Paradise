#include "Pessoa.h"

Pessoa::Pessoa() : nome("") {}

Pessoa::Pessoa(std::string nome) : nome(nome) {}

std::string Pessoa::getNome() const
{
    return nome;
}

void Pessoa::setNome(std::string nome)
{
    this->nome = nome;
}