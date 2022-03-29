#pragma once

#include "common.h"
#include "formula.h"

class Cell : public CellInterface
{
public:

    Cell();
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;

private:
    

    // базовый класс ждя ячеек разных типов
    class Impl
    {
    public:

        // виртуальный конструктор

        //метод возвращающий значение
    };

    class EmptyImpl : Impl
    {
    public:

        // констрпуктор без параметорв

    private:

        //некая пустота
    };

    class TextImpl : Impl
    {
    public:

        // конструкторп 

    private:

        std::string data;
    };

    class FormulaImpl
    {
    public:

    private:

        // некаф формула
    };

    std::unique_ptr<Impl> impl_;
};