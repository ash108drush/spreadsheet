#pragma once

#include "common.h"
#include "formula.h"
#include <functional>
#include <unordered_set>

class Sheet;

class Cell : public CellInterface {
public:
    Cell(Sheet& sheet):sheet_(sheet){};
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;
    void SetReferencedCells(std::vector<Position> referenced_cells){
        referenced_cells_ = std::move(referenced_cells);
    }

    bool IsReferenced() const;

private:
    Sheet &sheet_;
    std::vector<Position> referenced_cells_;
    class Impl {
    public:
        virtual ~Impl() = default;
        virtual CellInterface::Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
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
        std::vector<Position> GetReferencedCells() const;

    private:
        std::unique_ptr<FormulaInterface> formula_;
        const SheetInterface& sheet_;
    };

    std::unique_ptr<Impl> impl_;

};
