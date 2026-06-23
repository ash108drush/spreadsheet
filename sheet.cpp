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
        int col = pos.col;
        int row = pos.row;
        if(!cells_[row][col]){
            cells_[row][col] = std::make_unique<Cell>();
        }
        cells_[row][col]->Set(text);
    }else{
        throw InvalidPositionException("Invalid pos");
    }
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if(pos.IsValid()){
        if(pos.col < table_size_.cols && pos.row < table_size_.rows){
            return cells_[pos.col][pos.row].get();
        }
    } else{
        throw InvalidPositionException("Invalid pos");
    }
    return nullptr;
}
CellInterface* Sheet::GetCell(Position pos) {
    if(pos.IsValid()){
        if(pos.col < table_size_.cols && pos.row < table_size_.rows){
            return cells_[pos.row][pos.col].get();
        }
    }else{
        throw InvalidPositionException("Invalid pos");
    }
    return nullptr;
}

void Sheet::ClearCell(Position pos) {
    if(pos.IsValid()){
        if(pos.col < table_size_.cols && pos.row < table_size_.rows){
            cells_[pos.row][pos.col] = nullptr;
        }
        if(pos.col == table_size_.cols -1 && pos.row == table_size_.rows -1){
            RefreshTableSize();
        }

    }else{
        throw InvalidPositionException("Invalid pos");
    }
}

Size Sheet::GetPrintableSize() const {
    return table_size_;
}

void Sheet::PrintValues(std::ostream& output) const {
    for(int row = 0; row < table_size_.rows; ++row){
        for(int col = 0; col < table_size_.cols; ++col ){
            if (cells_[row][col]) {
                const auto& value = cells_[row][col]->GetValue();
                std::visit([&](const auto& v) {
                    output << v ;
                    }, value);
            }else{
                output << "";
            }
            (col == table_size_.cols -1) ? output << '\n' : output << '\t';

        }

    }
}
void Sheet::PrintTexts(std::ostream& output) const {
    for(int row = 0; row < table_size_.rows; ++row){
        for(int col = 0; col < table_size_.cols; ++col ){
            if (cells_[row][col]) {
                const auto& value = cells_[row][col]->GetText();
                output << value;
            }else{
                output << "";
            }
            (col == table_size_.cols -1) ? output << '\n' : output << '\t';
        }

    }

}

void Sheet::ResizeTable(Size size){
    table_size_.rows = std::max(size.rows, table_size_.rows );
    table_size_.cols = std::max(size.cols, table_size_.cols );
   // std::cout << "col: " << table_size_.cols  << " row: " << table_size_.rows << std::endl;
    cells_.resize(table_size_.rows);
    for (auto& row : cells_) {
        row.resize(table_size_.cols);
    }

}

void Sheet::RefreshTableSize(){
    int max_col = 0;
    int max_row = 0;
    for(int row = 0; row < table_size_.rows; ++row){
        for(int col = 0; col < table_size_.cols; ++col ){
            if (cells_[row][col]) {
                if(col > max_col)
                    max_col = col;
                if(row > max_row)
                    max_row = row;
            }

        }
    }
    table_size_.cols = max_col + 1;
    table_size_.rows = max_row + 1;
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}
