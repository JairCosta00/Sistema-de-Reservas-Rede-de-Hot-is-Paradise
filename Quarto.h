#ifndef QUARTO_H
#define QUARTO_H

#include <string>
using namespace std;

class Quarto
{
    private:
        string tipoQuarto;
        float diaria;
        
        public:
        Quarto();
        Quarto(string tipo, float diaria);
        // ~Quarto();

        string getTipoQuarto();
        
        void setTipoQuarto(string tipoQuarto);
};

#endif