#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet()
{}

void Sheet::SetCell(Position pos, std::string text)
{
    if (pos.IsValid())
    {
        sheet_[pos.row][pos.col].Set(text);
    }
}

const CellInterface* Sheet::GetCell(Position pos) const
{
    return const_cast<Sheet*>(this)->GetCell(pos);
}

CellInterface* Sheet::GetCell(Position pos)
{
    if (pos.IsValid() && sheet_.count(pos.row))
    {
        if (sheet_.at(pos.row).count(pos.col))
        {
            return &sheet_.at(pos.row).at(pos.col);
        }
    }
    return nullptr;
}

void Sheet::ClearCell(Position pos)
{
    if (pos.IsValid() && sheet_.count(pos.row))
    {
        if (sheet_.at(pos.row).count(pos.col))
        {
            sheet_.at(pos.row).erase(pos.col);
            if (sheet_.at(pos.row).empty())
            {
                sheet_.erase(pos.row);
            }
        }
    }
}

Size Sheet::GetPrintableSize() const
{
    Size s;
    if (!sheet_.empty())
    {
        s.rows = sheet_.rbegin()->first + 1;
        for (const auto& [row, col] : sheet_)
        {
            int len = col.rbegin()->first + 1;
            if (len > s.cols)
            {
                s.cols = len;
            }
        }
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

std::unique_ptr<SheetInterface> CreateSheet()
{
    return std::make_unique<Sheet>();
}