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
            table_size_.rows = std::max(pos.row + 1, table_size_.rows);
            table_size_.cols = std::max(pos.col + 1, table_size_.cols);
        }
        if(cells_.find(pos) == cells_.end()){
            cells_[pos] = std::make_unique<Cell>();
        }
        cells_[pos]->Set(text);

    } else {
        throw InvalidPositionException("Invalid pos");
    }
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if(pos.IsValid()){
        if(pos.col < table_size_.cols && pos.row < table_size_.rows){
            if (cells_.find(pos) != cells_.end())
                return cells_.at(pos).get();
        }
    } else {
        throw InvalidPositionException("Invalid pos");
    }
    return nullptr;
}

CellInterface* Sheet::GetCell(Position pos) {
    if(pos.IsValid()){
        if(pos.col < table_size_.cols && pos.row < table_size_.rows){
            if (cells_.find(pos) != cells_.end())
                return cells_.at(pos).get();
        }
    } else {
        throw InvalidPositionException("Invalid pos");
    }
    return nullptr;
}

void Sheet::ClearCell(Position pos) {
    if(pos.IsValid()){
        if(pos.col < table_size_.cols && pos.row < table_size_.rows){
            auto it = cells_.find(pos);
            cells_.erase(it);
            RefreshTableSize();  // ВСЕГДА обновляем размер, не только для последней ячейки
        }
    } else {
        throw InvalidPositionException("Invalid pos");
    }
}

Size Sheet::GetPrintableSize() const {
    return table_size_;
}

void Sheet::PrintValues(std::ostream& output) const {
    for(int row = 0; row < table_size_.rows; ++row){
        for(int col = 0; col < table_size_.cols; ++col){
            Position pos = { row,col };
            if (cells_.find(pos) != cells_.end()) {
                const auto& value = cells_.at(pos)->GetValue();
                std::visit([&](const auto& v) {
                    output << v;
                }, value);
            }
            if (col == table_size_.cols - 1) {
                output << '\n';
            } else {
                output << '\t';
            }
        }
    }
}

void Sheet::PrintTexts(std::ostream& output) const {
    for(int row = 0; row < table_size_.rows; ++row){
        for(int col = 0; col < table_size_.cols; ++col){
            Position pos = { row,col };
            if (cells_.find(pos) != cells_.end()) {
                output << cells_.at(pos)->GetText();
            }
            if (col == table_size_.cols - 1) {
                output << '\n';
            } else {
                output << '\t';
            }
        }
    }
}



void Sheet::RefreshTableSize(){
    // ИСПРАВЛЕНО: начинаем с -1, чтобы отличать пустую таблицу
    int max_col = -1;
    int max_row = -1;
    for(const auto &[pos, cell] :cells_){
        if(pos.row > max_row){
            max_row = pos.row;
        }
        if(pos.col > max_col){
            max_col = pos.col;
        }
    }
    if (max_col >= 0 && max_row >= 0) {
        table_size_.cols = max_col + 1;
        table_size_.rows = max_row + 1;
    } else {
        table_size_.cols = 0;
        table_size_.rows = 0;
    }

}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}
