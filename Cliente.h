#ifndef CLIENTE_H
#define CLIENTE_H

#include "Pessoa.h"
#include <string>

using namespace std;

class Cliente : public Pessoa
{
    protected:
        string cpf;
        
    public:
        Cliente();
        Cliente(string nome, string cpf);
        // ~Cliente();

        string getCpf() const;
        void setCpf(string cpf);
};

#endif