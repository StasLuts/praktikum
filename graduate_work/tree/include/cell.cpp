#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

//----------------Cell-----------------

Cell::Cell(SheetInterface& sheet, std::string text)
	:sheet_(sheet)
{
	cell_value_ = std::make_unique<EmptyImpl>();
	Set(text);
};

Cell::~Cell()
{
	cell_value_.reset();
}

void Cell::Set(std::string text)
{
	if (text.empty())
	{
		cell_value_ = std::make_unique<EmptyImpl>();
		return;
	}
	else if (text[0] != FORMULA_SIGN || (text[0] == FORMULA_SIGN && text.size() == 1))
	{
		cell_value_ = std::make_unique<TextImpl>(text);
		return;
	}
	else
	{
		std::string sub_str = text.erase(0, 1);

		try
		{
			cell_value_ = std::make_unique<FormulaImpl>(sub_str, sheet_);
		}
		catch (...)
		{
			std::string ref = "#REF!";
			throw FormulaException(ref);
		}
		return;
	}
}

void Cell::Clear()
{
	cell_value_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const
{
	return cell_value_.get()->ImplGetValue();
}

std::string Cell::GetText() const
{
	return cell_value_.get()->ImplGetText();
}

std::vector<Position> Cell::GetReferencedCells() const
{
	return cell_value_.get()->GetReferencedCells();
}

bool Cell::hasCircularDependency(const Cell* main_cell, const Position& pos) const
{
	for (const auto& cell_pos : GetReferencedCells())
	{
		const Cell* ref_cell = dynamic_cast<const Cell*>(sheet_.GetCell(cell_pos));

		if (pos == cell_pos)
		{
			return true;
		}

		if (!ref_cell)
		{
			sheet_.SetCell(cell_pos, "");
			ref_cell = dynamic_cast<const Cell*>(sheet_.GetCell(cell_pos));
		}

		if (main_cell == ref_cell)
		{
			return true;
		}

		if (ref_cell->hasCircularDependency(main_cell, pos))
		{
			return true;
		}
	}
	return false;
}

void Cell::InvalidateCache()
{
	cell_value_->ResetCache();
}

ImplType Cell::GetType() const
{
	return cell_value_.get()->GetType();
}

//----------------private-----------------

//--------------Impl------------------

bool Cell::Impl::isCacheValid() const
{
	return true;
}

void Cell::Impl::ResetCache()
{
	return;
}

//------------------EmptyImpl---------------

Cell::EmptyImpl::EmptyImpl()
{
	empty_ = "";
}

CellInterface::Value Cell::EmptyImpl::ImplGetValue() const
{
	return 0.0;
}

std::string Cell::EmptyImpl::ImplGetText() const
{
	return empty_;
}

std::vector<Position> Cell::EmptyImpl::GetReferencedCells() const
{
	return {};
}

ImplType Cell::EmptyImpl::GetType() const
{
	return ImplType::EMPTY;
}

//---------------TextImpl-----------------------

Cell::TextImpl::TextImpl(std::string& text)
	:text_value_(std::move(text))
{
	if (text_value_[0] == ESCAPE_SIGN)
	{
		has_apostroph_ = true;
	}
}

CellInterface::Value Cell::TextImpl::ImplGetValue() const
{
	if (has_apostroph_)
	{
		std::string sub_str = text_value_;
		sub_str.erase(0, 1);
		return sub_str;
	}

	return text_value_;
}

std::string Cell::TextImpl::ImplGetText() const
{
	return text_value_;
}

std::vector<Position> Cell::TextImpl::GetReferencedCells() const
{
	return {};
}

ImplType Cell::TextImpl::GetType() const
{
	return ImplType::TEXT;
}

//-------------------FormulaImpl-----------------------------

Cell::FormulaImpl::FormulaImpl(std::string& expr, SheetInterface& sheet)
	:formula_(ParseFormula(expr)), sheet_(sheet) {}

CellInterface::Value Cell::FormulaImpl::ImplGetValue() const
{
	if (!cache_value_)
	{
		for (const auto& pos : formula_.get()->GetReferencedCells())
		{
			if (dynamic_cast<Cell*>(sheet_.GetCell(pos))->GetType() == ImplType::TEXT)
			{
				cache_value_ = FormulaError(FormulaError::Category::Value);
				return *cache_value_;
			}
		}

		const auto val = formula_->Evaluate(sheet_);

		if (std::holds_alternative<double>(val))
		{
			double res = std::get<double>(val);
			if (std::isinf(res))
			{
				cache_value_ = FormulaError(FormulaError::Category::Div0);
			}
			else
			{
				cache_value_ = res;
			}
		}
		else if (std::holds_alternative<FormulaError>(val))
		{
			cache_value_ = std::get<FormulaError>(val);
		}
	}

	return *cache_value_;
}

std::string Cell::FormulaImpl::ImplGetText() const
{
	return FORMULA_SIGN + formula_.get()->GetExpression();
}

std::vector<Position> Cell::FormulaImpl::GetReferencedCells() const
{
	return formula_.get()->GetReferencedCells();
}

bool Cell::FormulaImpl::isCacheValid() const
{
	return cache_value_.has_value();
}

void Cell::FormulaImpl::ResetCache()
{
	cache_value_.reset();
}

ImplType Cell::FormulaImpl::GetType() const
{
	return ImplType::FORMULA;
}