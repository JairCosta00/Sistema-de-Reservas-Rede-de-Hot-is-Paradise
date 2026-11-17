#include <iostream>
#include <string>
#include "Atendente.h"
#include "Cliente.h"
#include "EstrategiaDesconto.h"
#include "Quarto.h"
#include "Reserva.h"
#include "GerenciadorDeReservas.h"

// comando de compilação (descomente a seção do código comentado após o namespace):
// ----> clang++ main.cpp Atendente.cpp Cliente.cpp Quarto.cpp Reserva.cpp GerenciadorDeReservas.cpp -o HotelSystem
// ----> após, executar: ./HotelSystem

using namespace std;

/*
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
*/

int main()
{
    // inicia o sistema 
    //iniciarSistema();

    return EXIT_SUCCESS;
}