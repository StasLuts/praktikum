#pragma once

#include "common.h"
#include "formula.h"

#include <optional>
#include <functional>
#include <unordered_set>

class Sheet;

class Impl
{
public:

    virtual CellInterface::Value ImplGetValue() const = 0;
    virtual std::string ImplGetText() const = 0;
    virtual std::vector<Position> ImplGetReferencedCells() const = 0;
    virtual bool IsCacheValid() const;
    virtual void ResetCash();

    virtual ~Impl() = default;
};

// пустая ячейка
class EmptyImpl : public Impl
{
public:

    EmptyImpl() {}
    CellInterface::Value ImplGetValue() const override;
    std::string ImplGetText() const override;
    std::vector<Position> ImplGetReferencedCells() const override;

private:

    std::string empty_ = "";
};

class TextImpl : public Impl
{
public:

    TextImpl(const std::string& text);
    CellInterface::Value ImplGetValue() const override;
    std::string ImplGetText() const override;
    std::vector<Position> ImplGetReferencedCells() const override;

private:

    std::string text_;
};

class FormulaImpl : public Impl
{
public:

    FormulaImpl(SheetInterface& sheet, const std::string& text);
    CellInterface::Value ImplGetValue() const override;
    std::string ImplGetText() const override;
    std::vector<Position> ImplGetReferencedCells() const override;
    bool IsCacheValid() const override;
    void ResetCash() override;

private:

    mutable std::optional<CellInterface::Value> cache_value_;
    SheetInterface& sheet_;
    std::unique_ptr<FormulaInterface>formula_;
};

class Cell : public CellInterface
{
public:
    
    Cell(SheetInterface& sheet);
    ~Cell();
    void Set(std::string text);
    void Clear();
    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;
    bool CyclicalDependence(const Cell* main_cell, const Position& pos) const;
    void InvalidateCash();

private:

    std::unique_ptr<Impl> impl_;
    SheetInterface& sheet_;
};