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
