#pragma once

#include "common.h"
#include "formula.h"

class Cell : public CellInterface {
public:
    Cell();
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;

private:

    class Impl {
    public:
        virtual ~Impl() = default;
        virtual CellInterface::Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
    };
    class EmptyImpl : public Impl {
    public:
        EmptyImpl(){};
        std::string GetText() const override{
            return "";
        }

        CellInterface::Value GetValue() const override {
            return "";
        };

    };
    class TextImpl : public Impl {
    public:
        explicit TextImpl(std::string str):value_(std::move(str)){

        }
        std::string GetText() const override{
            return value_;
        }
        CellInterface::Value GetValue() const override {
            if(value_[0] == ESCAPE_SIGN){
                return value_.substr(1);
            }
            return value_;
        };
    private:
        std::string value_;

    };
    class FormulaImpl : public Impl {
    public:
        FormulaImpl(std::string expression):formula_(ParseFormula(expression)){
        }

        CellInterface::Value GetValue() const override{
            FormulaInterface::Value f_value = formula_->Evaluate();
            if (std::holds_alternative<double>(f_value)) {
                return std::get<double>(f_value);
            }

            if (std::holds_alternative<FormulaError>(f_value)) {
                return std::get<FormulaError>(f_value);
            }
            return "";

        };
        std::string GetText() const override{
            return "=" + formula_->GetExpression();
        }

    private:
        std::unique_ptr<FormulaInterface> formula_;
    };
    std::unique_ptr<Impl> impl_;

};
