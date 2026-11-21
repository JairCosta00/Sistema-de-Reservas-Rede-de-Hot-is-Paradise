#include <vector>
#include <string.h>
#include <fstream>
#include <algorithm>
#include <limits>
#include "GerenciadorDeReservas.h"
#include <QDate>
#include <QString>
#include <iostream>
#include <stdexcept>
#include <utility>

// Inicialização da variável estática para o singleton
GerenciadorDeReservas* GerenciadorDeReservas::instancia = nullptr;

GerenciadorDeReservas::GerenciadorDeReservas()
{
    listaAtendentes = {
        Atendente("Ana", "atendente1", "senha1"),
        Atendente("Bruno", "atendente2", "senha2"),
        Atendente("Carlos", "atendente3", "senha3"),
        Atendente("Diana", "atendente4", "senha4"),
        Atendente("Administrador", "admin", "admin")

    };

    // Ao iniciar o sistema, recupera o historico salvo.
    carregarDeArquivoTxt();
}

// Le o relatorio TXT existente e restaura os objetos de Reserva.
// Mantem compatibilidade com o formato gerado por salvarEmArquivoTxt().
void GerenciadorDeReservas::carregarDeArquivoTxt()
{
    std::ifstream arquivo("reservas_paradise.txt");
    if (!arquivo.is_open()) {
        return; // Primeira execucao: ainda nao existe historico.
    }

    auto removerCR = [](std::string& linha) {
        if (!linha.empty() && linha.back() == '\r') linha.pop_back();
    };

    auto comecaCom = [](const std::string& texto, const std::string& prefixo) {
        return texto.rfind(prefixo, 0) == 0;
    };

    auto entre = [](const std::string& linha, const std::string& prefixo,
                    const std::string& separador) -> std::string {
        if (linha.rfind(prefixo, 0) != 0) return {};
        auto fim = linha.find(separador, prefixo.size());
        if (fim == std::string::npos) return {};
        return linha.substr(prefixo.size(), fim - prefixo.size());
    };

    const std::string pData = "Data Check-in: ";
    const std::string pDiarias = " | Diarias: ";
    const std::string pHotel = "Localidade: ";
    const std::string pQuarto = " | Quarto: ";
    const std::string pCliente = "Cliente: ";
    const std::string pCpf = " (CPF: ";
    const std::string pAtendente = "Atendente: ";
    const std::string pTotal = "Total: R$ ";
    const std::string pEntrada = " | Entrada (1/3): R$ ";
    const std::string pStatus = "Status: ";

    std::vector<Reserva> carregadas;
    std::string linha;
    int ignoradas = 0;

    while (std::getline(arquivo, linha)) {
        removerCR(linha);
        if (!comecaCom(linha, pData)) continue;

        std::string linhaHotel, linhaCliente, linhaAtendente, linhaValores, linhaStatus;
        if (!std::getline(arquivo, linhaHotel) ||
            !std::getline(arquivo, linhaCliente) ||
            !std::getline(arquivo, linhaAtendente) ||
            !std::getline(arquivo, linhaValores) ||
            !std::getline(arquivo, linhaStatus)) {
            ++ignoradas;
            break;
        }
        removerCR(linhaHotel);
        removerCR(linhaCliente);
        removerCR(linhaAtendente);
        removerCR(linhaValores);
        removerCR(linhaStatus);

        try {
            std::string data = entre(linha, pData, pDiarias);
            std::string numero = linha.substr(linha.find(pDiarias) + pDiarias.size());
            std::string hotel = entre(linhaHotel, pHotel, pQuarto);
            std::string tipo = linhaHotel.substr(linhaHotel.find(pQuarto) + pQuarto.size());

            const auto posCpf = linhaCliente.rfind(pCpf);
            const auto posEntrada = linhaValores.find(pEntrada);
            if (data.empty() || hotel.empty() || tipo.empty() ||
                !comecaCom(linhaCliente, pCliente) || posCpf == std::string::npos ||
                linhaCliente.back() != ')' ||
                !comecaCom(linhaAtendente, pAtendente) ||
                !comecaCom(linhaValores, pTotal) || posEntrada == std::string::npos ||
                !comecaCom(linhaStatus, pStatus) ||
                (linhaStatus != "Status: Pendente" &&
                 linhaStatus != "Status: Confirmada") ||
                !QDate::fromString(QString::fromStdString(data), "dd/MM/yyyy").isValid()) {
                ++ignoradas;
                continue;
            }

            std::string nomeCliente = linhaCliente.substr(
                pCliente.size(), posCpf - pCliente.size());
            std::string cpf = linhaCliente.substr(
                posCpf + pCpf.size(),
                linhaCliente.size() - (posCpf + pCpf.size()) - 1);
            std::string nomeAtendente = linhaAtendente.substr(pAtendente.size());

            int diarias = std::stoi(numero);
            float total = std::stof(linhaValores.substr(
                pTotal.size(), posEntrada - pTotal.size()));
            float entrada = std::stof(linhaValores.substr(
                posEntrada + pEntrada.size()));
            if (diarias <= 0 || total < 0 || entrada < 0 ||
                nomeCliente.empty() || cpf.empty() || nomeAtendente.empty()) {
                ++ignoradas;
                continue;
            }

            // Conserva o nome de quem criou a reserva, inclusive no historico.
            Atendente responsavel(nomeAtendente, "", "");
            for (const auto& atendente : listaAtendentes) {
                if (atendente.getNome() == nomeAtendente) {
                    responsavel = atendente;
                    break;
                }
            }

            // O valor original e recuperado, sem reaplicar o desconto.
            Reserva reserva(Cliente(nomeCliente, cpf), responsavel,
                           Quarto(tipo, 0.0f), hotel, data, diarias);
            reserva.setValorTotal(total);
            reserva.setValorEntrada(entrada);
            reserva.setStatusReserva(linhaStatus == "Status: Confirmada");
            carregadas.push_back(reserva);
        } catch (const std::exception&) {
            ++ignoradas; // Registro antigo malformado: nao interrompe a abertura.
        }
    }

    listaReservas = std::move(carregadas);
    std::cout << "Reservas carregadas: " << listaReservas.size() << '\n';
    if (ignoradas > 0) {
        std::cerr << "Atencao: " << ignoradas
                  << " registro(s) incompleto(s) ignorado(s) no TXT.\n";
    }
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

// Verificação de disponibilidade ao longo de toda a hospedagem.
// O checkout não ocupa o quarto: [check-in, checkout).
bool GerenciadorDeReservas::verificarDisponibilidade(
    string local, string dataCheckin, string tipoQuarto, int numeroDiarias)
{
    QDate inicioNovo = QDate::fromString(
        QString::fromStdString(dataCheckin), "dd/MM/yyyy");
    if (!inicioNovo.isValid() || numeroDiarias <= 0) return false;
    QDate fimNovo = inicioNovo.addDays(numeroDiarias);

    for (const auto& r : listaReservas) {
        if (r.getLocalidade() != local ||
            r.getQuarto().getTipoQuarto() != tipoQuarto) continue;

        QDate inicioExistente = QDate::fromString(
            QString::fromStdString(r.getCheckIn()), "dd/MM/yyyy");
        if (!inicioExistente.isValid()) continue;
        QDate fimExistente = inicioExistente.addDays(
            std::max(1, r.getNumeroDiarias()));

        if (inicioNovo < fimExistente && fimNovo > inicioExistente)
            return false;  // As hospedagens se sobrepõem.
    }
    return true;
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
//Encerrar Sessão
void GerenciadorDeReservas::encerrarSessao()
{
    if (atendenteLogado != nullptr) {
        atendenteLogado->setAutenticado(false);
        atendenteLogado = nullptr;
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