#include <iostream>
#include "Quarto.h"

/*

Cliente::~Cliente()
{
    cout << "Um objeto do tipo Cliente foi destruido." << endl;
}
*/

Quarto::Quarto()
{
    cout << "Objeto Quarto criado sem parametros." << endl;
}

Quarto::Quarto(string tipoQuarto, float diaria)
{
    this->tipoQuarto = tipoQuarto;
    this->diaria = diaria;
}

string Quarto::getTipoQuarto() const
{
    return tipoQuarto;
}

void Quarto::setTipoQuarto(string tipoQuarto)
{
    this->tipoQuarto = tipoQuarto;
}