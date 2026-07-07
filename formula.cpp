#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <variant>

using namespace std::literals;

//std::ostream& operator<<(std::ostream& output, const FormulaError& fe) {
//    return output << "#ARITHM!";
//}

namespace {
class Formula : public FormulaInterface {
public:
// Реализуйте следующие методы:
    explicit Formula(std::string expression):ast_(ParseFormulaAST(expression)){

    }
    Value Evaluate(const SheetInterface& sheet) const override{
        try {
            return ast_.Execute(sheet);
        } catch (const FormulaError& fe) {
            return fe;  // Если выбрасывается FormulaError, помещаем её в variant
        }

    }
    std::string GetExpression() const override{
        std::string out_str;
        std::ostringstream out(out_str);
        ast_.PrintFormula(out);
        return out.str();
    }
    std::vector<Position> GetReferencedCells() const override{
        return {};
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}
