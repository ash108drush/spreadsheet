#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>
#include <algorithm>
#include "sheet.h"
// Реализуйте следующие методы


Cell::~Cell() {}

void Cell::Set(std::string text) {
    if( text.size() > 1 && text[0] == FORMULA_SIGN ){
        impl_ = std::make_unique<FormulaImpl>(text.substr(1), static_cast<const SheetInterface&>(*sheet_));
    } else if(text.size() > 0){
        impl_ = std::make_unique<TextImpl>(std::move(text));
    } else {
        impl_ = std::make_unique<EmptyImpl>();
    }
    cache_.reset();
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
    referenced_cells_.clear();
    cache_.reset();
}

Cell::Value Cell::GetValue() const {
    if (!cache_) {
            cache_ = impl_->GetValue();
    }
    return *cache_;
}
std::string Cell::GetText() const {
    return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const{
    return impl_->GetReferencedCells();

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
    return std::get<FormulaError>(f_value);
}

std::string Cell::FormulaImpl::GetText() const{
    return "=" + formula_->GetExpression();
}

std::vector<Position> Cell::FormulaImpl::GetReferencedCells() const{
        return formula_->GetReferencedCells();
}

std::string Cell::EmptyImpl::GetText() const{
    return "";
}

CellInterface::Value Cell::EmptyImpl::GetValue() const {
    return "";
}

void Cell::RemoveDependent(Cell& dependent) {
    auto new_end = std::remove_if(
        dependents_.begin(),
        dependents_.end(),
        [&dependent](Cell* cell) { return cell == &dependent; }
        );
    dependents_.erase(new_end, dependents_.end());
}
