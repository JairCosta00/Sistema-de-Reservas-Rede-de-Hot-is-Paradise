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
        bool verificarDisponibilidade(string local, string dataCheckin,
                                     string tipoQuarto, int numeroDiarias = 1);

        // Adiciona nova reserva à memória
        void adicionarReserva(Reserva r);
        
        // opção para confirmar alguma reserva pendente.
        void listarEConfirmarPendentes();

        // Persistência de dados (arquivo .txt), e ordenação.
        void salvarEmArquivoTxt();

        // Recupera as reservas salvas no relatorio TXT.
        void carregarDeArquivoTxt();

        // Autenticação de atendentes
        bool realizarLogin(string login, string senha);

        // Getter do Atendente logado atualmente
        Atendente* getAtendenteLogado();

        // Retorna todas as reservas para a tela de listagem.
        const std::vector<Reserva>& getReservas() const { return listaReservas; }

        // Retorna a tela de login
        void encerrarSessao();
};

#endif