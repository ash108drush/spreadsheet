#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <variant>
#include <iostream>

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
        std::vector<Position> result = {};
        for (const auto& cell : ast_.GetCells()) {
            result.push_back(cell);
        }
      //  std::cout << result.size();
        result.erase(std::unique(result.begin(), result.end()), result.end());
       // std::cout << result.size() << std::endl;
        return result;
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try {
        return std::make_unique<Formula>(std::move(expression));
    }
    catch (const ParsingError& e) {
        throw FormulaException(e.what());
    }
    catch (const std::exception& e) {
        throw FormulaException(e.what());
    }
}
