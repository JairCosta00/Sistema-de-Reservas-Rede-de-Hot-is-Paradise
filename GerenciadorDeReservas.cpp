#include <vector>
#include <string.h>
#include <fstream>
#include <algorithm>
#include <limits>
#include "GerenciadorDeReservas.h"

// Inicialização da variável estática para o singleton
GerenciadorDeReservas* GerenciadorDeReservas::instancia = nullptr;

GerenciadorDeReservas::GerenciadorDeReservas()
{
    listaAtendentes = {
        Atendente("Ana", "atendente1", "senha1"),
        Atendente("Bruno", "atendente2", "senha2"),
        Atendente("Carlos", "atendente3", "senha3"),
        Atendente("Diana", "a", "a")
    };
}

bool GerenciadorDeReservas::realizarLogin(string login, string senha)
{
    for (auto& atendente : listaAtendentes) {
        if (atendente.getLogin() == login) {
            try {
                atendente.autenticar(senha);
                if (atendente.getAutenticado()) {
                    atendenteLogado = &atendente;
                    return true;
                }
            } catch (const std::exception&) {
                return false;
            }
        }
    }
    return false;
}

Atendente* GerenciadorDeReservas::getAtendenteLogado() 
{
    return atendenteLogado;
}

GerenciadorDeReservas* GerenciadorDeReservas::getInstancia() 
{
    if (instancia == nullptr) 
    {
        instancia = new GerenciadorDeReservas();
    }
    return instancia;
}

// Verificação de Disponibilidade (1 quarto de cada tipo por hotel)
bool GerenciadorDeReservas::verificarDisponibilidade(string local, string dataCheckin, string tipoQuarto)
{
    for (auto& r : listaReservas)
    {
        // Se encontrar uma reserva no mesmo hotel, mesma data e mesmo tipo de quarto, significa que o quarto está ocupado.
        if (r.getLocalidade() == local &&
            r.getCheckIn() == dataCheckin &&
            r.getQuarto().getTipoQuarto() == tipoQuarto) 
        {
            return false; // Indisponível
        }
    }
    return true; // Livre para reservar
}

// 2. Adicionar nova reserva à memória
void GerenciadorDeReservas::adicionarReserva(Reserva r) 
{
    listaReservas.push_back(r);
}

// opção para confirmar alguma reserva pendente.
void GerenciadorDeReservas::listarEConfirmarPendentes()
{
    cout << "\n--- RESERVAS PENDENTES ---" << endl;
    bool encontrou = false;
    
    for (size_t i = 0; i < listaReservas.size(); ++i) 
    {
        if (listaReservas[i].getStatusReserva() == false) 
        {
            cout << "[" << i << "] Cliente: " << listaReservas[i].getCliente().getNome()
                 << " | CPF: " << listaReservas[i].getCliente().getCpf()
                 << " | Data: " << listaReservas[i].getCheckIn()
                 << " | Valor Entrada: R$ " << listaReservas[i].getValorEntrada() << endl;
            encontrou = true;
        }
    }
    if (!encontrou) 
    {
        cout << "Nao ha reservas pendentes aguardando pagamento." << endl;
        return;
    }
    int indice;
    cout << "\nDigite o numero [ID] da reserva para confirmar o pagamento (ou -1 para cancelar): ";
    cin >> indice;
    // Verifica se o índice digitado é válido e se essa reserva realmente está pendente
    if (indice >= 0 && indice < listaReservas.size() && !listaReservas[indice].getStatusReserva()) 
    {
        listaReservas[indice].confirmarPagamento();
        cout << "\n[Sucesso] Pagamento confirmado! A reserva [" << indice << "] agora esta Confirmada." << endl;
    } 
    else if (indice != -1) 
    {
        cout << "\n[Erro] ID invalido." << endl;
    }
}

// 3. Persistência de dados (arquivo .txt), e ordenação.
void GerenciadorDeReservas::salvarEmArquivoTxt()
{
    if (listaReservas.empty()) {
        cout << "\nNenhuma reserva no sistema para salvar." << endl;
        return;
    }
    // Ordenar as reservas por data de check-in (da mais próxima para a mais distante)
    // Como a data é DD/MM/AAAA, antes ocorree uma conversão para AAAAMMDD para comparar corretamente (sugestão do Gemini)
    std::sort(listaReservas.begin(), listaReservas.end(), [](const Reserva& a, const Reserva& b) {
        string dataA = a.getCheckIn();
        string dataB = b.getCheckIn();
        
        if(dataA.length() == 10 && dataB.length() == 10) {  // se a data estiver no tamanho correto, 
            string formatadaA = dataA.substr(6,4) + dataA.substr(3,2) + dataA.substr(0,2);  // quebra as strings em substrings para 
            string formatadaB = dataB.substr(6,4) + dataB.substr(3,2) + dataB.substr(0,2);
            return formatadaA < formatadaB;
        }
        return dataA < dataB; // Fallback caso digitem a data num formato estranho
    });
    // Cria e escreve no .txt
    ofstream arquivo("reservas_paradise.txt");
    
    if (arquivo.is_open())
    {
        arquivo << "--- RELATORIO DE RESERVAS - REDE PARADISE ---\n\n";
        
        for (auto& r : listaReservas) 
        {
            arquivo << "Data Check-in: " << r.getCheckIn() << " | Diarias: " << r.getNumeroDiarias() << "\n";
            arquivo << "Localidade: " << r.getLocalidade() << " | Quarto: " << r.getQuarto().getTipoQuarto() << "\n";
            arquivo << "Cliente: " << r.getCliente().getNome() << " (CPF: " << r.getCliente().getCpf() << ")\n";
            arquivo << "Atendente: " << r.getAtendente().getNome() << "\n";
            arquivo << "Total: R$ " << r.getValorTotal() << " | Entrada (1/3): R$ " << r.getValorEntrada() << "\n";
            arquivo << "Status: " << (r.getStatusReserva() ? "Confirmada" : "Pendente") << "\n";
            arquivo << "--------------------------------------------------\n";
        }
        
        arquivo.close();
        cout << "\n++++++++++ Reservas ordenadas e salvas no arquivo 'reservas_paradise.txt' ++++++++++" << endl;
    }
    else
    {
        cout << "\nOcorreu um erro ao criar o arquivo de texto." << endl;
    }
}