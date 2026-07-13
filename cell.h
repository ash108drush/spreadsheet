#pragma once

#include "common.h"
#include "formula.h"
#include <functional>
#include <unordered_set>
#include <optional>
class Sheet;

class Cell : public CellInterface {
public:
    Cell(Sheet& sheet):sheet_(&sheet){};
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    void ResetCache();
    bool IsReferenced() const;
    void RemoveDependent(Cell& dependent);
    void SetReferencedCells(const std::vector<Position>& refs);
    void AddDependent(Cell& dependent);
    void InvalidateCache();
    const std::vector<Cell*>& GetDependents() const;
private:
    Sheet *sheet_ = nullptr;
    std::vector<Position> referenced_cells_;
    std::vector<Cell*> dependents_;
    mutable std::optional<CellInterface::Value> cache_;

    class Impl {
    public:
        virtual ~Impl() = default;
        virtual CellInterface::Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
        virtual std::vector<Position> GetReferencedCells() const  {
            return {}; }
    };
    class EmptyImpl : public Impl {
    public:
        EmptyImpl(){};
        std::string GetText() const override;
        CellInterface::Value GetValue() const override;
    };

    class TextImpl : public Impl {
    public:
        explicit TextImpl(std::string str):value_(std::move(str)){
        }

        std::string GetText() const override;
        CellInterface::Value GetValue() const override;
    private:
        std::string value_;
    };

    class FormulaImpl : public Impl {
    public:
        FormulaImpl(std::string expression, const SheetInterface& sheet):formula_(ParseFormula(expression)),sheet_(sheet){
        }
        CellInterface::Value GetValue() const override;
        std::string GetText() const override;
        std::vector<Position> GetReferencedCells() const override;


    private:
        std::vector<Position> referenced_cells_;
        std::vector<Cell*> dependents_;
        std::unique_ptr<FormulaInterface> formula_;
        const SheetInterface& sheet_;
    };

    std::unique_ptr<Impl> impl_;

};
