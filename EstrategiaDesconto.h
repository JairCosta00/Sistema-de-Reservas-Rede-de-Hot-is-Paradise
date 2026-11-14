 #ifndef ESTRATEGIADESCONTO_H
 #define ESTRATEGIADESCONTO_H          

class EstrategiaDesconto 
{
    public:
        virtual float calcularValorFinal(float valorBruto) = 0;
        virtual ~EstrategiaDesconto() = default;
};

class SemDesconto : public EstrategiaDesconto 
{
    public:
        float calcularValorFinal(float valorBruto) override { return valorBruto; }
};

class DescontoVIP : public EstrategiaDesconto 
{
    public:
        float calcularValorFinal(float valorBruto) override { return valorBruto * 0.90f; } // 10% desconto
};

class DescontoBaixaTemporada : public EstrategiaDesconto 
{
    public:
        float calcularValorFinal(float valorBruto) override { return valorBruto * 0.80f; } // 20% desconto
};

class DescontoPromocional : public EstrategiaDesconto 
{
    public:
        float calcularValorFinal(float valorBruto) override { return valorBruto * 0.85f; } // 15% desconto
};

#endif