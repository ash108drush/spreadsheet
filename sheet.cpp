#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
    if(pos.IsValid()){
        if(pos.col >= table_size_.cols || pos.row >= table_size_.rows){
            ResizeTable({pos.row +1 , pos.col +1 });
        }
        int x = pos.col;
        int y = pos.row;
        if(cells_[x][y] == nullptr){
            cells_[x][y] = std::make_unique<Cell>();
        }
        cells_[x][y]->Set(text);
    }
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if(pos.IsValid()){
        if(pos.col < table_size_.cols && pos.row < table_size_.rows){
            return cells_[pos.col][pos.row].get();
        }
    }
    return nullptr;
}
CellInterface* Sheet::GetCell(Position pos) {
    if(pos.IsValid()){
        if(pos.col < table_size_.cols && pos.row < table_size_.rows){
            return cells_[pos.col][pos.row].get();
        }
    }
    return nullptr;
}

void Sheet::ClearCell(Position pos) {
    if(pos.IsValid()){
        if(pos.col < table_size_.cols && pos.row < table_size_.rows){
            cells_[pos.col][pos.row]->Clear();
        }
    }
}

Size Sheet::GetPrintableSize() const {
    return table_size_;
}

void Sheet::PrintValues(std::ostream& output) const {
    for(int row = 0; row < table_size_.rows; ++row){
        for(int col = 0; col < table_size_.cols; ++col ){
            if (cells_[col][row]) {
                const auto& value = cells_[col][row]->GetValue();
                std::visit([&](const auto& v) {
                    output << v << '\t';
                    }, value);
            }else{
                output << "" << '\t';
            }

        }
        output << '\n';
    }
}
void Sheet::PrintTexts(std::ostream& output) const {
    for(int row = 0; row < table_size_.rows; ++row){
        for(int col = 0; col < table_size_.cols; ++col ){
            if (cells_[col][row]) {
                const auto& value = cells_[col][row]->GetText();
                output << value;
            }else{
                output << "";
            }
            (col == table_size_.cols -1) ? output << '\n' : output << '\t';
        }

    }

}

void Sheet::ResizeTable(Size size){
    table_size_.rows = size.rows;
    table_size_.cols = size.cols;
    cells_.resize(size.cols);
    for(int x = 0; x < size.cols; ++x ){
        cells_[x].resize(size.rows);
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}
