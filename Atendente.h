#ifndef ATENDENTE_H
#define ATENDENTE_H

#include <iostream>

using namespace std;

class Atendente
{
    protected:
        string nome;
        string login;
        string senha;
        bool autenticado;
        
    public:
        Atendente();
        Atendente(string nome, string login, string senha);
        // ~Atendente();
        void autenticar(string senhaDigitada);

        string getNome();

        void setNome(string nome);

        string getSenha();

        void setSenha(string senha);

        string getLogin();

        void setLogin(string login);

        bool getAutenticado();

        void setAutenticado(bool autenticado);
};

#endif