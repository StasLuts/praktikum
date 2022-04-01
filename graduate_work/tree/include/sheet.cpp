#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text)
{
    if (pos.IsValid())
    {
        sheet_[pos.row][pos.col].Set(text);
    }
}

const CellInterface* Sheet::GetCell(Position pos) const
{
    if (pos.IsValid() && sheet_.find(pos.row) != sheet_.end())
    {
        if (sheet_.at(pos.row).find(pos.col) != sheet_.at(pos.row).end())
        {
            return &sheet_.at(pos.row).at(pos.col);
        }
    }
    return nullptr;
}

CellInterface* Sheet::GetCell(Position pos)
{
    if (pos.IsValid() && sheet_.find(pos.row) != sheet_.end())
    {
        if (sheet_.at(pos.row).find(pos.col) != sheet_.at(pos.row).end())
        {
            return &sheet_.at(pos.row).at(pos.col);
        }
    }
    return nullptr;
}

void Sheet::ClearCell(Position pos)
{

}

Size Sheet::GetPrintableSize() const
{
    return {};
}

void Sheet::PrintValues(std::ostream& output) const
{

}

void Sheet::PrintTexts(std::ostream& output) const
{

}

std::unique_ptr<SheetInterface> CreateSheet()
{
    return std::make_unique<Sheet>();
}