#include <iostream>
#include <string>
#include <memory>   // biblioteca necessária para os padrôes de projeto, como Singleton e Regra de Negócio.
#include <vector>
#include <fstream>  // biblioteca necessária para usar a manipulação de ficheiros (ofstream)
#include <algorithm>    // biblioteca necessária para ordenar as reservas no arquivo (std::sort)

#include "Atendente.h"
#include "Cliente.h"
#include "EstrategiaDesconto.h"
#include "Quarto.h"
#include "Reserva.h"

// comando de compilação:
// ----> clang++ main.cpp Atendente.cpp Cliente.cpp EstrategiaDesconto.cpp Quarto.cpp Reserva.cpp -o HotelSystem
// ----> após, executar: ./HotelSystem

using namespace std;

class GerenciadorDeReservas 
{
    private:
        static GerenciadorDeReservas* instancia;
        std::vector<Reserva> listaReservas;
        
        // Construtor privado para impedir instâncias externas (singleton)
        GerenciadorDeReservas() {} 

    public:
        // Método estático para obter a instância única (singleton)
        static GerenciadorDeReservas* getInstancia() 
        {
            if (instancia == nullptr) {
                instancia = new GerenciadorDeReservas();
            }
            return instancia;
        }

        // Verificação de Disponibilidade (1 quarto de cada tipo por hotel)
        bool verificarDisponibilidade(string local, string dataCheckin, string tipoQuarto)
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
        void adicionarReserva(Reserva r) 
        {
            listaReservas.push_back(r);
        }
        
        // opção para confirmar alguma reserva pendente.
        void listarEConfirmarPendentes()
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
        void salvarEmArquivoTxt()
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
};

// Inicialização da variável estática para o singleton
GerenciadorDeReservas* GerenciadorDeReservas::instancia = nullptr;

//  ----------------------- CONSTRUTORES E DESTRUTORES -----------------------
    // OBS: destrutores estão comentados pois estavam dando conflito na hora de usar a função std::sort, por conta de ela usar 'swap' e precisar mover na memoria os itens para ordenação.


//  ----------------------- MENU / AUXILIARES -----------------------

void realizarNovaReserva(Atendente* atendenteLogado)    // submenu para realização de reservas
{
    cout << "\n------------------------------------------" << endl;
    cout << "          INICIANDO NOVA RESERVA          " << endl;
    cout << "------------------------------------------" << endl;

    // 1. Dados do Cliente
    string nomeCliente, cpfCliente;
    cout << "\n[1] DADOS DO CLIENTE" << endl;
    cout << "Nome completo: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpa o buffer do cin
    getline(cin, nomeCliente);
    cout << "CPF: ";
    getline(cin, cpfCliente);
    
    Cliente clienteRealizandoReserva(nomeCliente, cpfCliente);

    // 2. Dados da Hospedagem
    int opcaoLocal, numeroDiarias, opcaoQuarto;
    string localidade, checkIn, tipoQuarto;
    float valorDiaria = 0.0f;

    cout << "\n[2] DADOS DA HOSPEDAGEM" << endl;
    cout << "Localidade (1- Jericoacoara, 2- Canoa Quebrada, 3- Cumbuco): ";
    cin >> opcaoLocal;
    if (opcaoLocal == 1) localidade = "Jericoacoara";
    else if (opcaoLocal == 2) localidade = "Canoa Quebrada";
    else localidade = "Cumbuco";

    cout << "Data de check-in (DD/MM/AAAA): ";
    cin >> checkIn;
    
    cout << "Numero de diarias: ";
    cin >> numeroDiarias;

    cout << "Tipo de quarto (1- Solteiro, 2- Duplo, 3- Casal, 4- Triplo, 5- Quadruplo): ";
    cin >> opcaoQuarto;
    // Define o tipo de quarto e a respetiva diaria
    if (opcaoQuarto == 1) { tipoQuarto = "Solteiro"; valorDiaria = 200.0f; }
    else if (opcaoQuarto == 2) { tipoQuarto = "Duplo"; valorDiaria = 300.0f; }
    else if (opcaoQuarto == 3) { tipoQuarto = "Casal"; valorDiaria = 350.0f; }
    else if (opcaoQuarto == 4) { tipoQuarto = "Triplo"; valorDiaria = 450.0f; }
    else { tipoQuarto = "Quadruplo"; valorDiaria = 550.0f; }

    // 3. Aplicação de Desconto (Padrão Strategy)
    int opcaoDesconto;
    shared_ptr<EstrategiaDesconto> estrategiaAplicada = nullptr;
    string nomeDesconto = "";

    cout << "\n[3] POLITICA DE DESCONTO" << endl;
    cout << "1- Sem desconto (Tarifa padrao)\n2- Cliente VIP (10%)\n3- Baixa temporada (20%)\n4- Promocional (15%)" << endl;
    cout << "Escolha: ";
    cin >> opcaoDesconto;

    if (opcaoDesconto == 2) {
        estrategiaAplicada = make_shared<DescontoVIP>();
        nomeDesconto = "Cliente VIP (10%)";
    } else if (opcaoDesconto == 3) {
        estrategiaAplicada = make_shared<DescontoBaixaTemporada>();
        nomeDesconto = "Baixa Temporada (20%)";
    } else if (opcaoDesconto == 4) {
        estrategiaAplicada = make_shared<DescontoPromocional>(); 
        nomeDesconto = "Promocao Feriado (15%)";
    } else {
        estrategiaAplicada = make_shared<SemDesconto>();
        nomeDesconto = "Sem desconto";
    }

    // 4. Instanciar o objeto Quarto
    Quarto quartoEscolhido(tipoQuarto, valorDiaria);

    // 5. Instanciar a Reserva
    Reserva novaReserva(clienteRealizandoReserva, *atendenteLogado, quartoEscolhido, localidade, checkIn, numeroDiarias);
    
    // 6. Configurar as regras de negocio e os valores
    novaReserva.setEstrategiaDesconto(estrategiaAplicada); 
    novaReserva.calcularValores();
    novaReserva.setStatusReserva(false); // Pendente de pagamento

    // 7. Exibição do Resumo
    cout << "+------------- RESUMO DA RESERVA -------------+" << endl;
    cout << "Atendente: " << novaReserva.getAtendente().getNome() << endl;
    cout << "Cliente: " << novaReserva.getCliente().getNome() << " (CPF: " << novaReserva.getCliente().getCpf() << ")" << endl;
    cout << "Localidade: " << novaReserva.getLocalidade() << endl;
    cout << "Check-in: " << novaReserva.getCheckIn() << " | Diarias: " << novaReserva.getNumeroDiarias() << endl;
    cout << "Quarto: " << tipoQuarto << endl; 
    cout << "Desconto aplicado: " << nomeDesconto << endl;
    cout << "+---------------------------------------------+" << endl;
    cout << "Valor Total: R$ " << novaReserva.getValorTotal() << endl;
    cout << "Valor da Entrada (1/3): R$ " << novaReserva.getValorEntrada() << endl;
    cout << "Status: " << (novaReserva.getStatusReserva() ? "Confirmada" : "Pendente de confirmacao") << endl;
    cout << "+---------------------------------------------+\n" << endl;

    // adiciona a reserva validada no Singleton
    GerenciadorDeReservas::getInstancia()->adicionarReserva(novaReserva);

    // Pausa para o usuário ler antes de voltar ao menu
    cout << "Pressione ENTER para voltar ao menu principal...";
    cin.ignore(); // Limpa o buffer deixado pelo ultimo 'cin >>'
    cin.get();    // Espera que o utilizador pressione enter
}

void exibirMenuPrincipal(Atendente* atendenteLogado)    // menu principal do programa
{
    int opcao = 0;

    while (opcao != 4)
    {
        cout << "\n++++++++++ MENU PRINCIPAL ++++++++++" << endl;
        cout << "Atendente: " << atendenteLogado->getNome() << endl;
        cout << "1. Consultar disponibilidade de quartos" << endl;
        cout << "2. Realizar uma nova reserva" << endl;
        cout << "3. Confirmar pagamento de reserva pendente" << endl;
        cout << "4. Sair do sistema" << endl;
        cout << "Escolha uma opcao: ";
        cin >> opcao;

        switch (opcao)
        {
            case 1:
            {
                cout << "\n[Verificando Disponibilidade...]" << endl;
                int opcaoLocal, opcaoQuarto;
                string localidade, dataCheckin, tipoQuarto;
                
                cout << "Localidade (1- Jericoacoara, 2- Canoa Quebrada, 3- Cumbuco): ";
                cin >> opcaoLocal;
                if (opcaoLocal == 1) localidade = "Jericoacoara";
                else if (opcaoLocal == 2) localidade = "Canoa Quebrada";
                else localidade = "Cumbuco";

                cout << "Data pretendida (DD/MM/AAAA): ";
                cin >> dataCheckin;

                cout << "Tipo de quarto (1- Solteiro, 2- Duplo, 3- Casal, 4- Triplo, 5- Quadruplo): ";
                cin >> opcaoQuarto;
                if (opcaoQuarto == 1) tipoQuarto = "Solteiro";
                else if (opcaoQuarto == 2) tipoQuarto = "Duplo";
                else if (opcaoQuarto == 3) tipoQuarto = "Casal";
                else if (opcaoQuarto == 4) tipoQuarto = "Triplo";
                else tipoQuarto = "Quadruplo";

                // Chama o Singleton para fazer a verificação
                bool estaLivre = GerenciadorDeReservas::getInstancia()->verificarDisponibilidade(localidade, dataCheckin, tipoQuarto);
                
                cout << "\n----------------------------------------" << endl;
                if (estaLivre) {
                    cout << "STATUS: LIVRE. O quarto esta disponivel para esta data." << endl;
                } else {
                    cout << "STATUS: INDISPONIVEL. Ja existe uma reserva para este quarto nesta data." << endl;
                }
                cout << "----------------------------------------" << endl;
                break;
            }

            case 2:
                cout << "\n[Iniciando Nova Reserva...]" << endl;
                    realizarNovaReserva(atendenteLogado);   // vai para o menu de reserva.
                break;

            case 3:
                cout << "\n[Confirmacao de Pagamento]" << endl;
                // Chama a nova função do Singleton
                GerenciadorDeReservas::getInstancia()->listarEConfirmarPendentes();
                break;

            case 4:
                cout << "\nEncerrando sistema e salvando arquivo..." << endl;
                GerenciadorDeReservas::getInstancia()->salvarEmArquivoTxt();    // chama a persistencia de dados no Singleton
                break;
            default:
                cout << "\nOpcao invalida. Tente novamente." << endl;
                break;
        }
    }
}

void iniciarSistema()   // carrega os dados de teste no sistema e inicia o programa.
{
    // "Banco de dados" em memória com os 4 atendentes
    vector<Atendente> listaAtendentes = 
    {
        Atendente("Ana", "atendente1", "senha1"),
        Atendente("Bruno", "atendente2", "senha2"),
        Atendente("Carlos", "atendente3", "senha3"),
        Atendente("Diana", "atendente4", "senha4")
    };

    Atendente* atendenteLogado = nullptr;
    bool sair = false;

    // loop de login, se repete até obter sucesso com o login, ou o usuário decidir sair
    while (atendenteLogado == nullptr)
    {
        string loginDigitado, senhaDigitada;
        cout << "\n--- HOTEL PARADISE: TELA DE LOGIN ---" << endl;
        cout << "Login (ou digite 'sair' para encerrar): ";
        cin >> loginDigitado;

        if (loginDigitado == "sair") 
        {
        break;
        }

        cout << "Senha: ";
        cin >> senhaDigitada;

        for (auto& atendente : listaAtendentes)
        {
            if (atendente.getLogin() == loginDigitado) 
            {
                try
                {
                    atendente.autenticar(senhaDigitada); 
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                    break; 
                }
                
                if (atendente.getAutenticado()) 
                {
                    atendenteLogado = &atendente; 
                    break;
                }
            }
        }

        if (atendenteLogado == nullptr) 
        {
            cout << "Atendente nao encontrado ou credenciais incorretas. Tente novamente." << endl;
        }
    }

    // Encaminhamento para o menu principal, caso o login seja validado.
    if (atendenteLogado != nullptr) 
    {
        cout << "\nLogin efetuado com sucesso! Bem-vindo(a), " << atendenteLogado->getNome() << "." << endl;
        exibirMenuPrincipal(atendenteLogado);
    }
    else
    {
        cout << "\nSistema encerrado." << endl;
    }
}

int main()
{
    // inicia o sistema 
    iniciarSistema();

    return EXIT_SUCCESS;
}