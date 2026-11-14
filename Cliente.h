#ifndef CLIENTE_H
#define CLIENTE_H

#include <string>

using namespace std;

class Cliente
{
    protected:
        string nome;
        string cpf;
        
    public:
        Cliente();
        Cliente(string nome, string cpf);
        // ~Cliente();

        string getNome();
        
        void setNome(string nome);

        string getCpf();
        
        void setCpf(string cpf);
};

#endif