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
#include <numeric>

// Compara intervalos de hospedagem [check-in, check-out),
// considerando o mesmo hotel e tipo de quarto.
namespace {
bool reservasConflitam(const Reserva& a, const Reserva& b)
{
    if (a.getLocalidade() != b.getLocalidade() ||
        a.getQuarto().getTipoQuarto() != b.getQuarto().getTipoQuarto()) {
        return false;
    }

    const QDate inicioA = QDate::fromString(
        QString::fromStdString(a.getCheckIn()), "dd/MM/yyyy");
    const QDate inicioB = QDate::fromString(
        QString::fromStdString(b.getCheckIn()), "dd/MM/yyyy");

    if (!inicioA.isValid() || !inicioB.isValid() ||
        a.getNumeroDiarias() <= 0 || b.getNumeroDiarias() <= 0) {
        return false;
    }

    const QDate fimA = inicioA.addDays(a.getNumeroDiarias());
    const QDate fimB = inicioB.addDays(b.getNumeroDiarias());
    return inicioA < fimB && inicioB < fimA;
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
                 linhaStatus != "Status: Confirmada" &&
                 linhaStatus != "Status: Cancelada") ||
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
            if (linhaStatus == "Status: Cancelada") reserva.cancelar();
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


bool GerenciadorDeReservas::confirmarPagamentoReserva(
    std::size_t indice, std::vector<std::string>* canceladas,
    std::string* motivo)
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

    // Se outro pagamento já confirmou a ocupação, não podemos aceitar
    // a mesma hospedagem: cancela-se a solicitação ainda não paga.
    for (std::size_t i = 0; i < listaReservas.size(); ++i) {
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

    // O pagamento foi identificado: somente agora reserva efetivamente
    // o quarto para o período solicitado.
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
    QDate inicioNovo = QDate::fromString(
        QString::fromStdString(dataCheckin), "dd/MM/yyyy");
    if (!inicioNovo.isValid() || numeroDiarias <= 0) return false;
    QDate fimNovo = inicioNovo.addDays(numeroDiarias);

    for (const auto& r : listaReservas) {
        if (!r.getStatusReserva()) continue; // Pendente/cancelada não ocupa.
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

// Mantida para compatibilidade com o menu de console antigo.
void GerenciadorDeReservas::listarEConfirmarPendentes()
{
    cout << "\n--- SOLICITACOES PENDENTES ---" << endl;
    bool encontrou = false;
    for (std::size_t i = 0; i < listaReservas.size(); ++i) {
        if (listaReservas[i].estaPendente()) {
            cout << "[" << i << "] Cliente: "
                 << listaReservas[i].getCliente().getNome()
                 << " | Check-in: " << listaReservas[i].getCheckIn()
                 << " | Entrada: R$ " << listaReservas[i].getValorEntrada()
                 << endl;
            encontrou = true;
        }
    }
    if (!encontrou) {
        cout << "Nao ha solicitacoes pendentes." << endl;
        return;
    }

    int indice = -1;
    cout << "Digite o indice da solicitacao paga (-1 para sair): ";
    if (!(cin >> indice) || indice == -1) return;
    if (indice < 0) {
        cout << "Indice invalido." << endl;
        return;
    }

    std::vector<std::string> canceladas;
    std::string motivo;
    if (!confirmarPagamentoReserva(
            static_cast<std::size_t>(indice), &canceladas, &motivo)) {
        cout << "Nao foi possivel confirmar: " << motivo << endl;
        return;
    }
    cout << "Pagamento confirmado com sucesso." << endl;
    for (const auto& cliente : canceladas) {
        cout << "[Aviso] Solicitacao cancelada: " << cliente << endl;
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

// 3. Persistência em TXT; ordenação só na saída, sem modificar
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

    std::ofstream arquivo("reservas_paradise.txt", std::ios::trunc);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir reservas_paradise.txt para escrita." << endl;
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
                << " | Entrada (1/3): R$ " << r.getValorEntrada() << "\n";
        arquivo << "Status: "
                << (r.estaCancelada() ? "Cancelada" :
                    (r.getStatusReserva() ? "Confirmada" : "Pendente"))
                << "\n";
        arquivo << "--------------------------------------------------\n";
    }
    arquivo.close();
    if (!arquivo) {
        std::cerr << "Falha ao gravar todas as reservas no TXT." << endl;
    } else {
        cout << "Reservas salvas em reservas_paradise.txt" << endl;
    }
}
