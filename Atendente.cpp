#include <iostream>
#include "Atendente.h"

using namespace std;

string Atendente::getNome() 
{
    return nome;
}

void Atendente::setNome(string nome)
{
    this->nome = nome;
}

string Atendente::getSenha()
{
    return senha;
}

void Atendente::setSenha(string senha)
{
    this->senha = senha;
}

string Atendente::getLogin()
{
    return login;
}

void Atendente::setLogin(string login)
{
    this->login = login;
}

bool Atendente::getAutenticado()
{
    return autenticado;
}

void Atendente::setAutenticado(bool autenticado)
{
    this->autenticado = autenticado;
}