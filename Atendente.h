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

        string getNome() const;

        void setNome(string nome);

        string getSenha() const;

        void setSenha(string senha);

        string getLogin() const;

        void setLogin(string login);

        bool getAutenticado() const;

        void setAutenticado(bool autenticado);
};

#endif