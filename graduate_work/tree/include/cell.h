#pragma once

#include "common.h"
#include "formula.h"

#include <optional>
#include <functional>
#include <unordered_set>

enum class ImplType
{
	EMPTY,
	TEXT,
	FORMULA
};

class Cell : public CellInterface
{
public:

	using CellPtr = std::unique_ptr<Cell>;

	Cell(SheetInterface& sheet, const std::string& text);

	~Cell();

	void Set(std::string text);

	void Clear();

	Value GetValue() const override;
	
	std::string GetText() const override;
	
	std::vector<Position> GetReferencedCells() const override;
	
	bool hasCircularDependency(const Cell* main_cell, const Position& pos) const;
	
	void InvalidateCache();
	
	ImplType GetType() const;

private:

	class Impl
	{
	public:

		virtual CellInterface::Value ImplGetValue() const = 0;
		
		virtual std::string ImplGetText() const = 0;
		
		virtual std::vector<Position> GetReferencedCells() const = 0;
		
		virtual ImplType GetType() const = 0;
		
		virtual bool isCacheValid() const;
		
		virtual void ResetCache();

		virtual ~Impl() = default;

	};

	class EmptyImpl : public Impl
	{
	public:

		EmptyImpl();
		
		CellInterface::Value ImplGetValue() const override;
		
		std::string ImplGetText() const override;

		std::vector<Position> GetReferencedCells() const override;

		ImplType GetType() const override;

	private:

		std::string empty_;
	};

	class TextImpl : public Impl
	{
	public:

		TextImpl(std::string& text);

		CellInterface::Value ImplGetValue() const override;

		std::string ImplGetText() const override;

		std::vector<Position> GetReferencedCells() const override;

		ImplType GetType() const override;

	private:

		std::string text_value_;
		bool has_apostroph_ = false;
	};


	class FormulaImpl : public Impl
	{
	public:

		FormulaImpl(std::string& expr, SheetInterface& sheet);

		CellInterface::Value ImplGetValue() const override;

		std::string ImplGetText() const override;

		std::vector<Position> GetReferencedCells() const override;

		bool isCacheValid() const override;

		void ResetCache() override;

		ImplType GetType() const override;

	private:

		std::unique_ptr<FormulaInterface> formula_;
		SheetInterface& sheet_;
		mutable std::optional<CellInterface::Value> cache_value_;
	};

	SheetInterface& sheet_;
	std::unique_ptr<Impl> cell_value_;
};