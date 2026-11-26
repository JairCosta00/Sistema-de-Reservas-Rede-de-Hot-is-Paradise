 #ifndef ESTRATEGIADESCONTO_H
 #define ESTRATEGIADESCONTO_H

#include <string>

class EstrategiaDesconto 
{
    public:
        virtual float calcularValorFinal(float valorBruto) = 0;
        virtual std::string getDescricao() const = 0;
        virtual ~EstrategiaDesconto() = default;

};

class SemDesconto : public EstrategiaDesconto 
{
    public:
        float calcularValorFinal(float valorBruto) override { return valorBruto; }
        std::string getDescricao() const override { return "Sem desconto (0%)"; }
};

class DescontoVIP : public EstrategiaDesconto 
{
    public:
        float calcularValorFinal(float valorBruto) override { return valorBruto * 0.90f; } // 10% desconto
        std::string getDescricao() const override { return "Cliente VIP (10%)"; }
};

class DescontoBaixaTemporada : public EstrategiaDesconto 
{
    public:
        float calcularValorFinal(float valorBruto) override { return valorBruto * 0.80f; } // 20% desconto
        std::string getDescricao() const override { return "Baixa temporada (20%)"; }
};

class DescontoPromocional : public EstrategiaDesconto 
{
    public:
        float calcularValorFinal(float valorBruto) override { return valorBruto * 0.85f; } // 15% desconto
        std::string getDescricao() const override { return "Promocional (15%)"; }
};

#endif