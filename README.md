# Sistema de Reservas — Rede de Hotéis Paradise

Aplicação desktop desenvolvida em **C++** e **Qt 6 (Qt Widgets)** para gerenciamento centralizado de reservas da **Rede de Hotéis Paradise** (unidades de *Jericoacoara*, *Canoa Quebrada* e *Cumbuco*). O projeto aplica conceitos de **Programação Orientada a Objetos (POO)**, padrões de projeto (**Singleton** e **Strategy**), interface gráfica reativa orientada a eventos (*Signals & Slots*) e persistência local em arquivo texto.

---

## Funcionalidades

- **Autenticação e Controle de Sessão:** Login de atendentes com validação de credenciais, tratamento de exceções (`std::runtime_error`) e exibição do operador logado na tela principal.
- **Consulta de Disponibilidade:** Verificação em tempo real por unidade, categoria de quarto (*Solteiro*, *Duplo*, *Casal*, *Triplo* e *Quádruplo*), data de check-in (janela de até 12 meses a partir da data atual) e quantidade de diárias, validando intervalos semiabertos `[check-in, check-out)`.
- **Cadastro de Reservas e Descontos (Padrão Strategy):** Cálculo automático do valor total, entrada obrigatória de 1/3 e aplicação de políticas de desconto:
  - **Sem desconto (0%)**
  - **Cliente VIP (10%)**
  - **Promocional (15%)**
  - **Baixa temporada (20%)**
- **Gestão de Pagamentos e Prevenção de Conflitos:** Listagem ordenada cronologicamente com indicadores visuais de status (*Pendente*, *Confirmada* e *Cancelada*). Ao confirmar o pagamento de uma reserva, solicitações pendentes sobrepostas para o mesmo hotel e categoria de quarto são canceladas automaticamente.
- **Persistência Ordenada:** Gravação e carregamento automático do histórico no arquivo `reservas_paradise.txt`, ordenado pela data de check-in mais próxima.

---

## Arquitetura e Padrões de Projeto

- **Herança:** A classe base `Pessoa` concentra os atributos e métodos comuns de identificação, sendo estendida por `Cliente` (CPF) e `Atendente` (login, senha e autenticação).
- **Composição e Encapsulamento:** A classe `Reserva` agrega instâncias de `Cliente`, `Atendente` e `Quarto`, encapsulando regras financeiras e transições de status.
- **Singleton (`GerenciadorDeReservas`):** Garante uma única instância compartilhada entre todas as telas da interface para controle de disponibilidade, sessão e persistência.
- **Strategy (`EstrategiaDesconto`):** Desacopla as regras de cálculo de desconto por meio de polimorfismo, permitindo adicionar novas campanhas sem alterar a classe `Reserva`.

---

## Credenciais de Acesso (Ambiente de Teste)

| Atendente | Login | Senha |
| :--- | :--- | :--- |
| Ana | `atendente1` | `senha1` |
| Bruno | `atendente2` | `senha2` |
| Carlos | `atendente3` | `senha3` |
| Diana | `atendente4` | `senha4` |
| Administrador | `admin` | `admin` |

---

## Requisitos do Sistema

- **Compilador C++17 ou superior** (Clang, GCC ou MSVC)
- **Qt 6.0 ou superior** (módulos `Core`, `Gui` e `Widgets`)
- **CMake 3.16 ou superior**
- **Sistemas Operacionais suportados:** macOS, Linux e Windows

---

## Como Compilar e Executar

### Pelo Qt Creator (Recomendado)
1. Clone este repositório:
   ```bash
   git clone [https://github.com/seu-usuario/Sistema-de-Reservas---Rede-de-Hot-is-Paradise.git](https://github.com/seu-usuario/Sistema-de-Reservas---Rede-de-Hot-is-Paradise.git)

2. Abra o Qt Creator e selecione File → Open File or Project...

3. Escolha o arquivo CMakeLists.txt na raiz do projeto e configure o kit Qt 6.

4. Clique em Build → Run CMake e execute com Run (Ctrl + R / Cmd + R).

### Pelo Terminal (CMake CLI)

mkdir build && cd build
cmake ..
cmake --build .

### Estrutura do Projeto

├── CMakeLists.txt               # Configuração de build e cópia de recursos
├── main.cpp                     # Ponto de entrada, fontes e gerenciamento de memória
├── Pessoa.h / Pessoa.cpp        # Classe base da hierarquia de usuários
├── Cliente.h / Cliente.cpp      # Especialização de Pessoa para hóspedes
├── Atendente.h / Atendente.cpp  # Especialização de Pessoa com autenticação
├── Quarto.h / Quarto.cpp        # Categorias e diárias base dos quartos
├── EstrategiaDesconto.h         # Interface e implementações do padrão Strategy
├── Reserva.h / Reserva.cpp      # Entidade de reserva, cálculos e estados
├── GerenciadorDeReservas.h/.cpp # Singleton de regras de negócio e persistência TXT
├── logindialog.h/.cpp/.ui       # Tela de autenticação
├── mainwindow.h/.cpp/.ui        # Janela principal com QStackedWidget
├── teladisponibilidade.h/.cpp/.ui # Consulta de quartos e cadastro de reservas
├── telalistagem.h/.cpp/.ui      # Tabela de reservas e confirmação de pagamento
└── Resource Files/              # Imagens de interface e fontes Plus Jakarta Sans

### Autores

- Uriel Vitor Barbosa de Sousa — Engenharia da Computação (UTFPR)
- Jair Costa da Silva Martins - Engenharia da Computação (UTFPR)