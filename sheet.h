#pragma once

#include "cell.h"
#include "common.h"
#include <unordered_map>
#include <functional>

class Sheet : public SheetInterface {
public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;
    Cell& SetCell(Position pos);

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;


    // Можете дополнить ваш класс нужными полями и методами

private:
    void PrintCells(std::ostream& output,
        const std::function<std::string(const CellInterface*)>& getter) const;
    bool PositionValid(Position pos);
    bool CheckCycle(Position pos, const std::vector<Position>& new_refs) const;
    bool HasPathToTarget(Position start, Position target) const;
    void RefreshTableSize();
    void RebuildDependencies(Position pos, const std::vector<Position>& new_refs);
    void InvalidateCache(Cell& changed);
    //std::vector<std::vector<std::unique_ptr<Cell>>> cells_;
    struct PositionHasher {
        size_t operator()(const Position& p) const {
            // Простой пример: комбинируем хэши полей x и y
            return std::hash<int>()(p.row) ^ (std::hash<int>()(p.col) << 1);
        }
    };
    std::unordered_map<Position,std::unique_ptr<Cell>,PositionHasher> cells_;
    Size table_size_ = {0, 0};
};
