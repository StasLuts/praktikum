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
    return output << "#DIV/0!";
}

namespace
{
    class Formula : public FormulaInterface
    {
    public:
        // Реализуйте следующие методы:
        explicit Formula(std::string expression)
            : ast_(ParseFormulaAST(std::move(expression))) {}

        Value Evaluate(const SheetInterface& sheet) const override
        {
            try
            {
                return ast_.Execute(0);//////
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
            return { {} };
        }

    private:

        FormulaAST ast_;
    };
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression)
{
    return std::make_unique<Formula>(std::move(expression));
}