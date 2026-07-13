#include "sheet.h"
#include "cell.h"
#include "common.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <utility>

using namespace std::literals;

Sheet::~Sheet() {}

Cell& Sheet::SetCell(Position pos) {
    ValidatePosition(pos);
    auto it = cells_.find(pos);

    if (it == cells_.end()) {
        auto cell = std::make_unique<Cell>(*this);
        Cell* ptr = cell.get();
        cells_.emplace(pos, std::move(cell));
        return *ptr;
    }

    return *it->second;
}

void Sheet::SetCell(Position pos, std::string text) {
    ValidatePosition(pos);

    if (const Cell* existing = GetCellImpl(pos)) {
        if (existing->GetText() == text) {
            return;
        }
    }

    if (text.empty()) {
        ClearCell(pos);
        return;
    }

    Cell tmp_cell(*this);
    tmp_cell.Set(text);

    std::vector<Position> new_refs;
    if (text.size() > 1 && text[0] == FORMULA_SIGN) {
        new_refs = tmp_cell.GetReferencedCells();
    }

    if (CheckCycle(pos, new_refs)) {
        throw CircularDependencyException("Circular dependency detected");
    }

    Cell* cell = GetCellImpl(pos);

    // Удаляем старые зависимости

    for (const Position& old_ref : cell->GetReferencedCellsList()) {
        if (old_ref.IsValid()) {
            if (Cell* ref_cell = GetCellImpl(old_ref)) {
                ref_cell->RemoveDependent(*cell);
            }
        }
    }


    // Устанавливаем новый текст
    cell->Set(text);

    // Устанавливаем новые ссылки
    cell->SetReferencedCells(new_refs);

    // Добавляем новые зависимости (только для существующих ячеек)
    for (const Position& new_ref : new_refs) {
        if (new_ref.IsValid()) {
            if (Cell* ref_cell = GetCellImpl(new_ref)) {
                ref_cell->AddDependent(*cell);
            }
        }
    }

    InvalidateCache(*cell);
}

void Sheet::InvalidateCache(Cell& changed) {
    std::queue<Cell*> q;
    std::set<Cell*> visited;

    q.push(&changed);

    while (!q.empty()) {
        Cell* cur = q.front();
        q.pop();

        if (!visited.insert(cur).second) {
            continue;
        }

        cur->InvalidateCache();

        for (Cell* dependent : cur->GetDependents()) {
            q.push(dependent);
        }
    }
}


bool Sheet::CheckCycle(Position pos, const std::vector<Position>& new_refs) const {
    // Проверка на прямую ссылку на себя
    for (const Position& ref : new_refs) {
        if (ref == pos) {
            return true;
        }
    }

    // Проверка на цикл через другие ячейки
    for (const Position& ref : new_refs) {
        if (HasPathToTarget(ref, pos)) {
            return true;
        }
    }

    return false;
}

bool Sheet::HasPathToTarget(Position start, Position target) const {
    if (!start.IsValid()) {
        return false;
    }

    std::queue<Position> q;
    std::set<Position> visited;
    q.push(start);

    while (!q.empty()) {
        const Position cur = q.front();
        q.pop();

        if (cur == target) {
            return true;
        }

        if (!visited.insert(cur).second) {
            continue;
        }

        const Cell* cell = GetCellImpl(cur);
        if (!cell) {
            continue;
        }

        for (const Position& next : cell->GetReferencedCellsList()) {
            if (!next.IsValid()) {
                continue;
            }

            q.push(next);
        }
    }

    return false;
}

const CellInterface* Sheet::GetCell(Position pos) const {
    return GetCellImpl(pos);
}

CellInterface* Sheet::GetCell(Position pos) {
    return GetCellImpl(pos);
}

const Cell* Sheet::GetCellImpl(Position pos) const {
    ValidatePosition(pos);
    auto it = cells_.find(pos);
    if (it == cells_.end()) {
        return nullptr;
    }
    return it->second.get();
}

Cell* Sheet::GetCellImpl(Position pos) {
    ValidatePosition(pos);
    auto it = cells_.find(pos);
    if (it == cells_.end()) {
        return nullptr;
    }
    return it->second.get();
}


void Sheet::ClearCell(Position pos) {
    ValidatePosition(pos);
    auto it = cells_.find(pos);
    if (it == cells_.end()) {
        return;
    }

    Cell* cell = it->second.get();

    for (const Position& old_ref : cell->GetReferencedCellsList()) {
        if (old_ref.IsValid()) {
            if (Cell* ref_cell = GetCellImpl(old_ref)) {
                ref_cell->RemoveDependent(*cell);
            }
        }
    }

    cell->SetReferencedCells({});
    InvalidateCache(*cell);
    cells_.erase(it);
}

Size Sheet::GetPrintableSize() const {
    return table_size_;
}

void Sheet::PrintValues(std::ostream& output) const {
    PrintCells(output, [](const CellInterface* cell) -> std::string {
        if (!cell) {
            return {};
        }

        const auto value = cell->GetValue();

        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        }

        if (std::holds_alternative<double>(value)) {
            std::ostringstream out;
            out << std::get<double>(value);
            return out.str();
        }

        std::ostringstream out;
        out << std::get<FormulaError>(value);
        return out.str();
    });
}

void Sheet::PrintCells(
    std::ostream& output,
    const std::function<std::string(const CellInterface*)>& getter
    ) const {
    Size size = GetPrintableSize();

    for (int r = 0; r < size.rows; ++r) {
        bool first = true;
        for (int c = 0; c < size.cols; ++c) {
            if (!first) {
                output << '\t';
            }
            first = false;

            const Position pos{r, c};
            const CellInterface* cell = GetCell(pos);

            output << getter(cell);
        }
        output << '\n';
    }
}


void Sheet::PrintTexts(std::ostream& output) const {
    PrintCells(output, [](const CellInterface* cell) -> std::string {
        return cell ? cell->GetText() : std::string{};
    });
}

void Sheet::ValidatePosition(Position pos)  const {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position: " + pos.ToString());
    }
}

void Sheet::RebuildDependencies(Position pos, const std::vector<Position>& new_refs) {
    Cell& cell = SetCell(pos);

    for (const Position& old_ref : cell.GetReferencedCellsList()) {
        if (!old_ref.IsValid()) continue;
        if (Cell* ref_cell = GetCellImpl(old_ref)) {
            ref_cell->RemoveDependent(cell);
        }
    }

    cell.SetReferencedCells(new_refs);

    for (const Position& new_ref : new_refs) {
        if (!new_ref.IsValid()) continue;
        if (Cell* ref_cell = GetCellImpl(new_ref)) {
            ref_cell->AddDependent(cell);
        }
    }
}


std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}
