#include <vector>
#include <fstream>
#include <algorithm>
#include "GerenciadorDeReservas.h"
#include <QDate>
#include <QString>
#include <stdexcept>
#include <utility>
#include <numeric>
#include <string>
#include <iostream>
#include <memory>

using namespace std;

// Compara intervalos de hospedagem [check-in, check-out),
// considerando o mesmo hotel e tipo de quarto.
namespace {
bool periodosConflitam(const std::string& checkInA, int diariasA,
                       const std::string& checkInB, int diariasB)
{
    const QDate inicioA = QDate::fromString(QString::fromStdString(checkInA), "dd/MM/yyyy");
    const QDate inicioB = QDate::fromString(QString::fromStdString(checkInB), "dd/MM/yyyy");

    if (!inicioA.isValid() || !inicioB.isValid() || diariasA <= 0 || diariasB <= 0)
        return false;

    const QDate fimA = inicioA.addDays(diariasA);
    const QDate fimB = inicioB.addDays(diariasB);
    return inicioA < fimB && inicioB < fimA;
}

bool reservasConflitam(const Reserva& a, const Reserva& b)
{
    if (a.getLocalidade() != b.getLocalidade() ||
        a.getQuarto().getTipoQuarto() != b.getQuarto().getTipoQuarto()) {
        return false;
    }
    return periodosConflitam(a.getCheckIn(), a.getNumeroDiarias(),
                             b.getCheckIn(), b.getNumeroDiarias());
}
}

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

// Lê o relatório TXT existente e restaura os objetos de Reserva na memória.
// Mantém compatibilidade tanto com o formato novo (com Desconto) quanto com arquivos antigos.
void GerenciadorDeReservas::carregarDeArquivoTxt()
{
    // -------------------- ETAPA 1: ABERTURA DO ARQUIVO E FUNÇÕES AUXILIARES DE TEXTO --------------------

    std::ifstream arquivo("reservas_paradise.txt");
    if (!arquivo.is_open()) {
        return; // Primeira execução do sistema: ainda não existe histórico salvo.
    }

    // Remove o caractere '\r' caso o arquivo tenha sido salvo no Windows (CRLF)
    auto removerCR = [](std::string& linha) {
        if (!linha.empty() && linha.back() == '\r') linha.pop_back();
    };

    // Verifica se a linha começa exatamente com um prefixo esperado
    auto comecaCom = [](const std::string& texto, const std::string& prefixo) {
        return texto.rfind(prefixo, 0) == 0;
    };

    // Extrai o trecho de texto situado entre um prefixo inicial e um separador
    auto entre = [](const std::string& linha, const std::string& prefixo,
                    const std::string& separador) -> std::string {
        if (linha.rfind(prefixo, 0) != 0) return {};
        auto fim = linha.find(separador, prefixo.size());
        if (fim == std::string::npos) return {};
        return linha.substr(prefixo.size(), fim - prefixo.size());
    };

    // --------------------- RÓTULOS ESPERADAS EM CADA BLOCO DE RESERVA DO TXT -----------------------------

    const string pData      = "Data Check-in: ";
    const string pDiarias   = " | Diarias: ";
    const string pHotel     = "Localidade: ";
    const string pQuarto    = " | Quarto: ";
    const string pCliente   = "Cliente: ";
    const string pCpf       = " (CPF: ";
    const string pAtendente = "Atendente: ";
    const string pTotal     = "Total: R$ ";
    const string pEntrada   = " | Entrada (1/3): R$ ";
    const string pDesconto  = " | Desconto: "; // Novo campo adicionado
    const string pStatus    = "Status: ";

    vector<Reserva> carregadas;
    string linha;
    int ignoradas = 0;

    // ---------------------- LEITURA SEQUENCIAL DOS BLOCOS DE 6 LINHAS POR RESERVA ----------------------
    // =========================================================================
    while (getline(arquivo, linha)) {
        removerCR(linha);

        // Ignora cabeçalhos, linhas em branco e separadores "------" até achar uma reserva
        if (!comecaCom(linha, pData)) continue;

        // Lê as 5 linhas seguintes que compõem o registro completo daquela reserva
       string linhaHotel, linhaCliente, linhaAtendente, linhaValores, linhaStatus;
        if (!getline(arquivo, linhaHotel) ||
            !getline(arquivo, linhaCliente) ||
            !getline(arquivo, linhaAtendente) ||
            !getline(arquivo, linhaValores) ||
            !getline(arquivo, linhaStatus)) {
            ++ignoradas; // Arquivo terminou no meio de um bloco
            break;
        }
        removerCR(linhaHotel);
        removerCR(linhaCliente);
        removerCR(linhaAtendente);
        removerCR(linhaValores);
        removerCR(linhaStatus);

        try {
            // -------------------- EXTRAÇÃO E VALIDAÇÃO DOS CAMPOS BRUTOS --------------------
            string data   = entre(linha, pData, pDiarias);
            string numero = linha.substr(linha.find(pDiarias) + pDiarias.size());
            string hotel  = entre(linhaHotel, pHotel, pQuarto);
            string tipo   = linhaHotel.substr(linhaHotel.find(pQuarto) + pQuarto.size());

            const auto posCpf     = linhaCliente.rfind(pCpf);
            const auto posEntrada = linhaValores.find(pEntrada);

            // Garante que todos os prefixos obrigatórios e a data sejam válidos
            if (data.empty() || hotel.empty() || tipo.empty() ||
                !comecaCom(linhaCliente, pCliente) || posCpf == string::npos ||
                linhaCliente.back() != ')' ||
                !comecaCom(linhaAtendente, pAtendente) ||
                !comecaCom(linhaValores, pTotal) || posEntrada == string::npos ||
                !comecaCom(linhaStatus, pStatus) ||
                (linhaStatus != "Status: Pendente" &&
                 linhaStatus != "Status: Confirmada" &&
                 linhaStatus != "Status: Cancelada") ||
                !QDate::fromString(QString::fromStdString(data), "dd/MM/yyyy").isValid()) {
                ++ignoradas;
                continue;
            }

            string nomeCliente = linhaCliente.substr(
                pCliente.size(), posCpf - pCliente.size());
            string cpf = linhaCliente.substr(
                posCpf + pCpf.size(),
                linhaCliente.size() - (posCpf + pCpf.size()) - 1);
            string nomeAtendente = linhaAtendente.substr(pAtendente.size());

            // Converte números de diárias e valor total
            int diarias = stoi(numero);
            float total = stof(linhaValores.substr(
                pTotal.size(), posEntrada - pTotal.size()));

            // Extrai o valor da entrada e, se existir na linha, a descrição do desconto
            const auto posDesconto = linhaValores.find(pDesconto, posEntrada);
            float entrada = 0.0f;
            string descStr = "Sem desconto (0%)";

            if (posDesconto != std::string::npos) {
                // Formato novo: "Total: R$ X | Entrada (1/3): R$ Y | Desconto: Z"
                entrada = std::stof(linhaValores.substr(
                    posEntrada + pEntrada.size(),
                    posDesconto - (posEntrada + pEntrada.size())));
                descStr = linhaValores.substr(posDesconto + pDesconto.size());
            } else {
                // Retrocompatibilidade: arquivos antigos sem o campo "| Desconto: "
                entrada = std::stof(linhaValores.substr(
                    posEntrada + pEntrada.size()));
            }

            // Valida consistência numérica e textos obrigatórios
            if (diarias <= 0 || total < 0 || entrada < 0 ||
                nomeCliente.empty() || cpf.empty() || nomeAtendente.empty()) {
                ++ignoradas;
                continue;
            }

            // ------------------------- RECONSTRUÇÃO DOS OBJETOS DE DOMÍNIO (OO + STRATEGY) -------------------------
            // Busca o objeto Atendente correspondente na lista cadastrada
            Atendente responsavel(nomeAtendente, "", "");
            for (const auto& atendente : listaAtendentes) {
                if (atendente.getNome() == nomeAtendente) {
                    responsavel = atendente;
                    break;
                }
            }

            // Instancia a estratégia de desconto correspondente ao texto salvo
            shared_ptr<EstrategiaDesconto> estrategia = make_shared<SemDesconto>();
            if (descStr.find("VIP") != string::npos) {
                estrategia = make_shared<DescontoVIP>();
            } else if (descStr.find("Baixa temporada") != string::npos) {
                estrategia = make_shared<DescontoBaixaTemporada>();
            } else if (descStr.find("Promocional") != string::npos) {
                estrategia = make_shared<DescontoPromocional>();
            }

            // Monta a Reserva restaurando os valores exatos gravados (sem recalcular)
            Reserva reserva(Cliente(nomeCliente, cpf), responsavel,
                            Quarto(tipo, 0.0f), hotel, data, diarias);
            reserva.setEstrategiaDesconto(estrategia);
            reserva.setValorTotal(total);
            reserva.setValorEntrada(entrada);
            reserva.setStatusReserva(linhaStatus == "Status: Confirmada");
            if (linhaStatus == "Status: Cancelada") {
                reserva.cancelar();
            }

            carregadas.push_back(std::move(reserva));

        } catch (const exception&) {
            // Protege a inicialização contra linhas corrompidas ou malformadas
            ++ignoradas;
        }
    }

    // ------------------------------ ATUALIZAÇÃO DO VETOR PRINCIPAL EM MEMÓRIA ------------------------------
    listaReservas = std::move(carregadas);
}

bool GerenciadorDeReservas::confirmarPagamentoReserva(
    size_t indice, vector<string>* canceladas,
    string* motivo)
{
    if (canceladas) canceladas->clear();
    if (motivo) motivo->clear();

    if (indice >= listaReservas.size()) {
        if (motivo) *motivo = "Solicitação não encontrada.";
        return false;
    }

    Reserva& escolhida = listaReservas[indice];
    if (!escolhida.estaPendente()) {
        if (motivo) *motivo = escolhida.estaCancelada()
            ? "Esta solicitação já foi cancelada."
            : "Esta reserva já está confirmada.";
        return false;
    }

    // Se outro pagamento já confirmou a ocupação, não pode aceitar
    // a mesma hospedagem: cancela-se a solicitação ainda não paga.
    for (size_t i = 0; i < listaReservas.size(); ++i) {
        if (i != indice && listaReservas[i].getStatusReserva() &&
            reservasConflitam(escolhida, listaReservas[i])) {
            escolhida.cancelar();
            salvarEmArquivoTxt();
            if (motivo) *motivo =
                "Solicitação cancelada: o quarto já possui uma reserva paga "
                "para esse período.";
            return false;
        }
    }

    // O pagamento identificado: reserva efetivamente o quarto para o período solicitado.
    escolhida.confirmarPagamento();

    // Outras solicitações não pagas que se sobrepõem perdem a vaga.
    for (std::size_t i = 0; i < listaReservas.size(); ++i) {
        if (i == indice || !listaReservas[i].estaPendente()) continue;
        if (reservasConflitam(escolhida, listaReservas[i])) {
            listaReservas[i].cancelar();
            if (canceladas) {
                canceladas->push_back(
                    listaReservas[i].getCliente().getNome() +
                    " (check-in: " + listaReservas[i].getCheckIn() + ")");
            }
        }
    }

    salvarEmArquivoTxt();
    return true;
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
    if (!QDate::fromString(QString::fromStdString(dataCheckin), "dd/MM/yyyy").isValid() ||
        numeroDiarias <= 0) {
        return false;
    }

    for (const auto& r : listaReservas) {
        if (r.getStatusReserva() &&
            r.getLocalidade() == local &&
            r.getQuarto().getTipoQuarto() == tipoQuarto &&
            periodosConflitam(dataCheckin, numeroDiarias, r.getCheckIn(), r.getNumeroDiarias())) {
            return false;
        }
    }
    return true;
}

// Adicionar nova reserva à memória
void GerenciadorDeReservas::adicionarReserva(Reserva r) 
{
    listaReservas.push_back(r);
}

//Encerrar Sessão
void GerenciadorDeReservas::encerrarSessao()
{
    if (atendenteLogado != nullptr) {
        atendenteLogado->setAutenticado(false);
        atendenteLogado = nullptr;
    }
}

// Persistência em TXT; ordenação só na saída, sem modificar
// o vetor de memória (os índices são usados pela tabela de pagamento).
void GerenciadorDeReservas::salvarEmArquivoTxt()
{
    if (listaReservas.empty()) {
        cout << "Nenhuma reserva no sistema para salvar." << endl;
        return;
    }

    // Ordena uma lista de índices, sem alterar a listaReservas.
    std::vector<std::size_t> indices(listaReservas.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::stable_sort(indices.begin(), indices.end(), [this](std::size_t a, std::size_t b) {
        const QDate dataA = QDate::fromString(
            QString::fromStdString(listaReservas[a].getCheckIn()), "dd/MM/yyyy");
        const QDate dataB = QDate::fromString(
            QString::fromStdString(listaReservas[b].getCheckIn()), "dd/MM/yyyy");
        return dataA < dataB;
    });

    ofstream arquivo("reservas_paradise.txt", ios::trunc);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir reservas_paradise.txt para escrita." << endl;
        return;
    }

    arquivo << "--- RELATORIO DE RESERVAS - REDE PARADISE ---\n\n";
    for (std::size_t indice : indices) {
        const Reserva& r = listaReservas[indice];
        arquivo << "Data Check-in: " << r.getCheckIn()
                << " | Diarias: " << r.getNumeroDiarias() << "\n";
        arquivo << "Localidade: " << r.getLocalidade()
                << " | Quarto: " << r.getQuarto().getTipoQuarto() << "\n";
        arquivo << "Cliente: " << r.getCliente().getNome()
                << " (CPF: " << r.getCliente().getCpf() << ")\n";
        arquivo << "Atendente: " << r.getAtendente().getNome() << "\n";
        arquivo << "Total: R$ " << r.getValorTotal()
                << " | Entrada (1/3): R$ " << r.getValorEntrada()
                << " | Desconto: " << r.getDescricaoDesconto() << "\n";
        arquivo << "Status: "
                << (r.estaCancelada() ? "Cancelada" :
                    (r.getStatusReserva() ? "Confirmada" : "Pendente"))
                << "\n";
        arquivo << "--------------------------------------------------\n";
    }
    arquivo.close();
    if (!arquivo) {
        cerr << "Falha ao gravar todas as reservas no TXT." << endl;
    } else {
        cout << "Reservas salvas em reservas_paradise.txt" << endl;
    }
}
