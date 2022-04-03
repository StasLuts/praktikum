#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;


void Sheet::SetCell(Position pos, std::string text)
{
    PositionCorrect(pos);
    //rjycnhernjhjr
}

const CellInterface* Sheet::GetCell(Position pos) const
{
    return const_cast<Sheet*>(this)->GetCell(pos);
}

CellInterface* Sheet::GetCell(Position pos)
{
    PositionCorrect(pos);
    if (sheet_.find(pos) != sheet_.end())
    {
        return sheet_.at(pos).get();
    }
    return nullptr;
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

void Sheet::PositionCorrect(Position pos) const
{
    if (!pos.IsValid())
    {
        throw InvalidPositionException("The position is incorrect");
    }
}

std::unique_ptr<SheetInterface> CreateSheet()
{
    return std::make_unique<Sheet>();
}