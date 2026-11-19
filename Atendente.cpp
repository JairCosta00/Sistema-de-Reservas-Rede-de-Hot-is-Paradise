#include <iostream>
#include <stdexcept>
#include "Atendente.h"

using namespace std;

string Atendente::getNome() const
{
    return nome;
}

void Atendente::setNome(string nome)
{
    this->nome = nome;
}

string Atendente::getSenha() const
{
    return senha;
}

void Atendente::setSenha(string senha)
{
    this->senha = senha;
}

string Atendente::getLogin() const
{
    return login;
}

void Atendente::setLogin(string login)
{
    this->login = login;
}

bool Atendente::getAutenticado() const
{
    return autenticado;
}

void Atendente::setAutenticado(bool autenticado)
{
    this->autenticado = autenticado;
}

Atendente::Atendente() : autenticado(false) {}

Atendente::Atendente(string nome, string login, string senha)
    : nome(nome), login(login), senha(senha), autenticado(false) {}

void Atendente::autenticar(string senhaDigitada)
{
    if (this->senha != senhaDigitada) {
        throw std::runtime_error("Login / Senha invalido.");
    }
    this->autenticado = true;
}