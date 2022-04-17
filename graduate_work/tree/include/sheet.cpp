#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

//------------------Sheet--------------------------

void Sheet::SetCell(Position pos, std::string text)
{
    PositionCorrect(pos);
    const auto existing_cell = GetCell(pos);
    if (existing_cell && existing_cell->GetText() == text)
    {
        return;
    }

    if (existing_cell)
    {
        std::string old_text = existing_cell->GetText();
        InvalidateCellsByPos(pos);
        DeleteDependencedCell(pos);
        dynamic_cast<Cell*>(existing_cell)->Set(std::move(text));
        if (dynamic_cast<Cell*>(existing_cell)->
            hasCircularDependency(dynamic_cast<Cell*>(existing_cell), pos)) 
        {
            dynamic_cast<Cell*>(existing_cell)->Set(std::move(old_text));
            throw CircularDependencyException("Circular Exception!");
        }

        for (const auto& ref_pos : dynamic_cast<Cell*>(existing_cell)->GetReferencedCells())
        {
            AddDependencedCell(ref_pos, pos);
        }
    }
    else 
    {
        auto tmp_cell = std::make_unique<Cell>(*this, text); 
        if (tmp_cell.get()->hasCircularDependency(tmp_cell.get(), pos))
        {
            throw CircularDependencyException("Circular Exception!");
        }

        for (const auto ref_pos : tmp_cell.get()->GetReferencedCells())
        {
            AddDependencedCell(ref_pos, pos);
        }

        sheet_[pos] = std::move(tmp_cell);
    }
}

const CellInterface* Sheet::GetCell(Position pos) const
{
    return const_cast<Sheet*>(this)->GetCell(pos);
}

CellInterface* Sheet::GetCell(Position pos)
{
    PositionCorrect(pos);
    try
    {
        return sheet_.at(pos).get();
    }
    catch (...)
    {
        return nullptr;
    }
}

void Sheet::ClearCell(Position pos)
{
    PositionCorrect(pos);
    if (pos.IsValid() && sheet_.find(pos) != sheet_.end())
    {
        sheet_.erase(pos);
    }
}

Size Sheet::GetPrintableSize() const
{
    Size s;
    for (const auto& [pos, cell] : sheet_)
    {
        s.cols = std::max(s.cols, pos.col + 1);
        s.rows = std::max(s.rows, pos.row + 1);
    }
    return s;
}

void Sheet::PrintValues(std::ostream& output) const
{
    auto size = GetPrintableSize();
    for (int i = 0; i < size.rows; ++i)
    {
        bool flag = false;
        for (int j = 0; j < size.cols; ++j)
        {
            if (flag)
            {
                output << '\t';
            }
            flag = true;
            if (auto cell = GetCell({ i, j }); cell)
            {
                auto val = cell->GetValue();
                if (std::holds_alternative<std::string>(val))
                {
                    output << std::get<std::string>(val);
                }
                if (std::holds_alternative<double>(val))
                {
                    output << std::get<double>(val);
                }
                if (std::holds_alternative<FormulaError>(val))
                {
                    output << std::get<FormulaError>(val);
                }
            }
        }
        output << '\n';
    }
}

void Sheet::PrintTexts(std::ostream& output) const
{
    auto size = GetPrintableSize();
    for (int i = 0; i < size.rows; ++i)
    {
        bool flag = false;
        for (int j = 0; j < size.cols; ++j)
        {
            if (flag)
            {
                output << '\t';
            }
            flag = true;
            if (auto cell = GetCell({ i, j }); cell)
            {
                output << cell->GetText();
            }
        }
        output << '\n';
    }
}

void Sheet::InvalidateCellsByPos(const Position& pos)
{
    for (const auto cell_pos : GetDepCellByPos(pos))
    {
        auto cell = GetCell(cell_pos);
        dynamic_cast<Cell*>(cell)->InvalidateCache();
        InvalidateCellsByPos(cell_pos);
    }
}

const std::set<Position> Sheet::GetDepCellByPos(const Position& pos)
{
    try
    {
        return sheet_dependenced_cells_.at(pos);
    }
    catch (...)
    {
        return {};
    }
}

const std::map<Position, std::set<Position>> Sheet::GetSheetDepCells() const
{
    return sheet_dependenced_cells_;
}

void Sheet::AddDependencedCell(const Position& pos_key, const Position& pos_value)
{
    sheet_dependenced_cells_[pos_key].insert(pos_value);
}

void Sheet::DeleteDependencedCell(const Position& pos)
{
    sheet_dependenced_cells_.erase(pos);
}

//--------------------private-------------------------

void Sheet::PositionCorrect(Position pos) const
{
    if (!pos.IsValid())
    {
        throw InvalidPositionException("The position is incorrect");
    }
}

//--------------------functions---------------------------

std::unique_ptr<SheetInterface> CreateSheet()
{
    return std::make_unique<Sheet>();
}