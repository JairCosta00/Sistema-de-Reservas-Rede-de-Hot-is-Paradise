#ifndef PESSOA_H
#define PESSOA_H

#include <string>

class Pessoa
{
protected:
    std::string nome;

public:
    Pessoa();
    Pessoa(std::string nome);
    virtual ~Pessoa() = default;

    std::string getNome() const;
    void setNome(std::string nome);
};

#endif // PESSOA_H