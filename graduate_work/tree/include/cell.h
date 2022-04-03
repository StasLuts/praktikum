#pragma once

#include "common.h"
#include "formula.h"

#include <functional>
#include <unordered_set>

class Sheet;

class Cell : public CellInterface
{
public:

    Cell(Sheet& sheet);
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    bool IsReferenced() const;

private:

    class Impl
    {
    public:

        virtual CellInterface::Value ImplGetValue() const = 0;

        virtual std::string ImplGetText() const = 0;

        virtual ~Impl() = default;
    };


    class EmptyImpl : public Impl
    {
    public:

        EmptyImpl() {}

        CellInterface::Value ImplGetValue() const override;

        std::string ImplGetText() const;

    private:

        std::string empty_ = "";
    };

    class TextImpl : public Impl
    {
    public:

        TextImpl(const std::string& text);

        CellInterface::Value ImplGetValue() const override;

        std::string ImplGetText() const;

    private:

        std::string text_;
    };

    class FormulaImpl : public Impl
    {
    public:

        FormulaImpl(const std::string& text);

        CellInterface::Value ImplGetValue() const override;

        std::string ImplGetText() const;

    private:

        std::unique_ptr<FormulaInterface>formula_;
    };

    std::unique_ptr<Impl> impl_;
    Sheet& sheet_;
};