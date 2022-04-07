#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>
#include <memory>

//---------------------Impl--------------------

bool Impl::IsCacheValid() const
{
	return true;
}

void Impl::ResetCash()
{
	return;
}

//--------------------EmptyImpl------------------

CellInterface::Value EmptyImpl::ImplGetValue() const
{
	return empty_;
}

std::string EmptyImpl::ImplGetText() const
{
	return empty_;
}

std::vector<Position> EmptyImpl::ImplGetReferencedCells() const
{
	return {};
}

//-----------------TextImpl--------------------

TextImpl::TextImpl(const std::string& text)
	: text_(text) {}

CellInterface::Value TextImpl::ImplGetValue() const
{
	if (text_.front() == ESCAPE_SIGN)
	{
		return text_.substr(1);
	}
	else
	{
		return text_;
	}
}

std::string TextImpl::ImplGetText() const
{
	return text_;
}

std::vector<Position> TextImpl::ImplGetReferencedCells() const
{
	return {};
}

//-----------------FormulaImpl------------------

FormulaImpl::FormulaImpl(SheetInterface& sheet, const std::string& text)
	: formula_(ParseFormula(text)), sheet_(sheet) {}

CellInterface::Value FormulaImpl::ImplGetValue() const
{
	const auto val = formula_->Evaluate(sheet_);
	if (std::holds_alternative<double>(val))
	{
		return std::get<double>(val);
	}
	return std::get<FormulaError>(val);
}

std::string FormulaImpl::ImplGetText() const
{
	return '=' + formula_->GetExpression();
}

std::vector<Position> FormulaImpl::ImplGetReferencedCells() const
{
	return formula_->GetReferencedCells();
}

bool FormulaImpl::IsCacheValid() const
{
	return cache_value_.has_value();
}

void FormulaImpl::ResetCash()
{
	cache_value_.reset();
}

//---------------Cell-----------------------

Cell::Cell(SheetInterface& sheet)
	: sheet_(sheet)
{
	impl_ = std::make_unique<EmptyImpl>();
}

Cell::~Cell()
{
	impl_.reset();
}

void Cell::Set(std::string text)
{
	if (text.empty())
	{
		impl_ = std::make_unique<EmptyImpl>();
	}
	else if (text.front() != FORMULA_SIGN || text == "=")
	{
		impl_ = std::make_unique<TextImpl>(text);
	}
	else
	{
		impl_ = std::make_unique<FormulaImpl>(sheet_, text.substr(1));
	}
}

void Cell::Clear()
{
	impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const
{
	return impl_->ImplGetValue();
}

std::string Cell::GetText() const
{
	return impl_->ImplGetText();
}

std::vector<Position> Cell::GetReferencedCells() const //
{
	return impl_->ImplGetReferencedCells();
}

// принимает создаваемую // измен€емую €чейку и ее позицию
bool Cell::CyclicalDependence(const Cell* main_cell, const Position& pos) const
{
	for (const auto& cell_pos : GetReferencedCells()) // проходимс€ по спискк позиций включенных €чеек
	{
		const Cell* ref_cell = dynamic_cast<const Cell*>(sheet_.GetCell(cell_pos)); // берем €чейку из списка

		if (pos == cell_pos) // €чейка ссылаетс€ сама на себ€ 
		{
			return true; // цикличиска€ зависимоть
		}

		if (!ref_cell) // если включена нулл €чейка
		{
			sheet_.SetCell(cell_pos, ""); // зоздаем на ее позиции просто пустую €чейку
			ref_cell = dynamic_cast<const Cell*>(sheet_.GetCell(cell_pos)); // вызывем созданную нами €чейку?
		}

		if (main_cell == ref_cell) // если указатели ссылаюс€ на одно и тоже 
		{
			return true; // цикличска€ зависимоть
		}

		if (ref_cell->CyclicalDependence(main_cell, pos)) // рекурси€  
		{
			return true;
		}
	}
	return false;
}

void Cell::InvalidateCash()
{
	impl_->ResetCash();
}
