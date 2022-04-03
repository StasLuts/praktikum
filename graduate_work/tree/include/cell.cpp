#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>
#include <memory>


//--------------------EmptyImpl------------------

CellInterface::Value Cell::EmptyImpl::ImplGetValue() const
{
	return empty_;
}

std::string Cell::EmptyImpl::ImplGetText() const
{
	return empty_;
}

//-----------------TextImpl--------------------

Cell::TextImpl::TextImpl(const std::string& text)
	: text_(text) {}

CellInterface::Value Cell::TextImpl::ImplGetValue() const
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

std::string Cell::TextImpl::ImplGetText() const
{
	return text_;
}

//-----------------FormulaImpl------------------

Cell::FormulaImpl::FormulaImpl(const std::string& text)
	: formula_(ParseFormula(text)) {}

CellInterface::Value Cell::FormulaImpl::ImplGetValue() const
{
	const auto val = formula_->Evaluate();
	if (std::holds_alternative<double>(val))
	{
		return std::get<double>(val);
	}
	return std::get<FormulaError>(val);
}

std::string Cell::FormulaImpl::ImplGetText() const
{
	return '=' + formula_->GetExpression();
}

//---------------Cell-----------------------

Cell::Cell(Sheet& sheet)
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
		impl_ = std::make_unique<FormulaImpl>(text.substr(1));
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

std::vector<Position> Cell::GetReferencedCells() const
{
	return std::vector<Position>();
}

bool Cell::IsReferenced() const
{
	return false;
}
