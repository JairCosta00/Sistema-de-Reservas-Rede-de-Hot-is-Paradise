#ifndef GERENCIADORDERESERVAS_H
#define GERENCIADORDERESERVAS_H

#include <vector>
#include <string.h>
#include <fstream>
#include <algorithm>
#include <limits>
#include "Reserva.h"

class GerenciadorDeReservas
{
    private:
        static GerenciadorDeReservas* instancia;    // instancia (Singleton)
        std::vector<Reserva> listaReservas;
        std::vector<Atendente> listaAtendentes; // Movido do main
        Atendente* atendenteLogado = nullptr;   // Movido do main
        
        // Construtor privado para impedir instâncias externas (singleton)
        GerenciadorDeReservas();

    public:
        // Método estático para obter a instância única (singleton)
        static GerenciadorDeReservas* getInstancia();

        // Verificação de Disponibilidade (1 quarto de cada tipo por hotel)
        bool verificarDisponibilidade(string local, string dataCheckin, string tipoQuarto);

        // Adiciona nova reserva à memória
        void adicionarReserva(Reserva r);
        
        // opção para confirmar alguma reserva pendente.
        void listarEConfirmarPendentes();

        // Persistência de dados (arquivo .txt), e ordenação.
        void salvarEmArquivoTxt();

        // Autenticação de atendentes
        bool realizarLogin(string login, string senha);

        // Getter do Atendente logado atualmente
        Atendente* getAtendenteLogado();

        const std::vector<Reserva>& getReservas() const {
            return listaReservas;
        }

};

#endif