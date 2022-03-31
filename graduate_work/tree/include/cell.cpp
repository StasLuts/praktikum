#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>
#include <memory>


//--------------------CellInterface------------------

CellInterface::Value EmptyImpl::ImplGetValue() const
{
	return empty_;
}

std::string EmptyImpl::ImplGetText() const
{
	return empty_;
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

//-----------------FormulaImpl------------------

FormulaImpl::FormulaImpl(const std::string& text)
	: formula_(ParseFormula(text)) {}

CellInterface::Value FormulaImpl::ImplGetValue() const
{
	const auto val = formula_->Evaluate();
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

//---------------Cell-----------------------

Cell::Cell()
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