#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <iostream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe)
{
    FormulaError::Category cat = fe.GetCategory();
    switch (cat)
    {
    case FormulaError::Category::Ref:
        return output << "#REF!";
    case FormulaError::Category::Value:
        return output << "#VALUE!";
    case FormulaError::Category::Div0:
        return output << "#DIV/0!";
    }
    return output << "";
}

namespace
{
    class Formula : public FormulaInterface
    {
    public:
        
        explicit Formula(std::string expression)
            : ast_(ParseFormulaAST(std::move(expression))),
            referenced_cells_(ast_.GetCells().begin(), ast_.GetCells().end()){}

        Value Evaluate(const SheetInterface& sheet) const override
        {
            try
            {
                return ast_.Execute([&sheet](const Position& pos)
                    {
                        if (sheet.GetCell(pos) == nullptr)
                        {
                            return 0.0;
                        }
                        auto val = sheet.GetCell(pos)->GetValue();

                        if (std::holds_alternative<double>(val))
                        {
                            return std::get<double>(val);
                        }
                        else if (std::holds_alternative<std::string>(val))
                        {
                            try
                            {
                                return std::stod(std::move(std::get<std::string>(val)));
                            }
                            catch (...)
                            {
                                throw FormulaError(FormulaError::Category::Value);
                            }
                        }
                        else
                        {
                            throw std::get<FormulaError>(val);
                        }
                        return 0.0;
                    });
            }
            catch (FormulaError& fe)
            {
                return fe;
            }
        }

        std::string GetExpression() const override
        {
            std::stringstream ss;
            ast_.PrintFormula(ss);
            return ss.str();
        }

        std::vector<Position> GetReferencedCells() const override
        {
            std::vector<Position> res;
            std::set<Position> poses(referenced_cells_.begin(), referenced_cells_.end());
            for (const auto& pos : poses)
            {
                res.push_back(pos);
            }
            return res;
        }

    private:

        FormulaAST ast_;
        std::vector<Position> referenced_cells_;
    };

}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression)
{
    try
    {
        return std::make_unique<Formula>(std::move(expression));
    }
    catch (const std::exception&)
    {
        throw FormulaException("ParseError");
    }
}