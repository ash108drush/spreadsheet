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
        if(cells_[pos.col][pos.row] == nullptr){
            cells_[pos.col][pos.row] = std::make_unique<Cell>();
        }
        cells_[pos.col][pos.row]->Set(text);
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
    for(int x = 0; x < table_size_.cols; ++x){
        for(int y = 0; y < table_size_.rows; ++y ){
            if (cells_[x][y]) {
                const auto& value = cells_[x][y]->GetValue();
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
    for(int x = 0; x < table_size_.cols; ++x){
        for(int y = 0; y < table_size_.rows; ++y ){
            if (cells_[x][y]) {
                const auto& value = cells_[x][y]->GetText();
                output << value;
            }else{
                output << "";
            }
            (y == table_size_.rows -1) ? output << '\n' : output << '\t';
        }

    }

}

void Sheet::ResizeTable(Size size){
    table_size_.rows = size.rows;
    table_size_.cols = size.cols;
    cells_.resize(size.cols);
    for(int y = 0; y < size.cols; ++y ){
        cells_[y].resize(size.rows);
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}
