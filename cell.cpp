#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>
#include "sheet.h"
// Реализуйте следующие методы


Cell::~Cell() {}

void Cell::Set(std::string text) {
    if( text.size() > 1 && text[0] == FORMULA_SIGN ){
        impl_ = std::make_unique<FormulaImpl>(text.substr(1), static_cast<const SheetInterface&>(sheet_));
    } else if(text.size() > 0){
        impl_ = std::make_unique<TextImpl>(std::move(text));
    } else {
        impl_ = std::make_unique<EmptyImpl>();
    }
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const {
    return impl_->GetValue();
}
std::string Cell::GetText() const {
    return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const{
    return {};

}

bool Cell::IsReferenced() const
{
    return false;
}

std::string Cell::TextImpl::GetText() const{
    return value_;
}

CellInterface::Value Cell::TextImpl::GetValue() const {
    if(value_[0] == ESCAPE_SIGN){
        return value_.substr(1);
    }
    return value_;
}

CellInterface::Value Cell::FormulaImpl::GetValue() const{
    FormulaInterface::Value f_value = formula_->Evaluate(sheet_);
    if (std::holds_alternative<double>(f_value)) {
        return std::get<double>(f_value);
    }

    if (std::holds_alternative<FormulaError>(f_value)) {
        return std::get<FormulaError>(f_value);
    }
    return "";

}

std::string Cell::FormulaImpl::GetText() const{
    return "=" + formula_->GetExpression();
}

std::string Cell::EmptyImpl::GetText() const{
    return "";
}

CellInterface::Value Cell::EmptyImpl::GetValue() const {
    return "";
}
