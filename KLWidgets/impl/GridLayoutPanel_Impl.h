#pragma once
#include "UltraEngine.h"
#include "..\include\KLWidgets.h"
#include "KLWidgets_Impl.h"

using namespace UltraEngine;

namespace KLWidgets
{
	class GridChildInformation
	{
	public:
		GridChildInformation(shared_ptr<Widget> child, int columnspan, int rowspan, HorizontalAlignment horizontalalignment, VerticalAlignment verticalalignment);
		shared_ptr<Widget> Child;
		HorizontalAlignment HorizontalAlignment;
		VerticalAlignment VerticalAlignment;
		int RowSpan;
		int ColumnSpan;
		iVec4 OriginalShape;
	};



	class GridCellInformation
	{
	public:
		int row;
		int column;
		iVec2 position;
		iVec2 size;
		vector<GridChildInformation*> assignedWidgets;
	};

	class GridLayoutPanel_Impl : public GridLayoutPanel
	{
		friend class Widget;
		friend class GridCellInformation;
		friend class GridChildInformation;

		vector<GridCellInformation*> _cellInformations;
		vector<GridRowDefinition*> _rowDefinitions;
		vector<GridColumnDefinition*> _columnDefinitions;
		void CreateOrUpdateCells();
		void CalculateGridCells();
		void AddRowDefinition(GridLength length, int height = 1);
		void AddColumnDefinition(GridLength length, int width = 1);
		void AssignChild(shared_ptr<Widget> widget, int column, int row, int columnspan = 1, int rowspan = 1, HorizontalAlignment horizontalalignment = HorizontalAlignment::STRETCH, VerticalAlignment verticalalignment = VerticalAlignment::STRETCH);
		void CalculateLayout(vector<shared_ptr<Widget>>* visibleKids);
		void Draw(const int x, const int y, const int width, const int height);
		friend shared_ptr<GridLayoutPanel> CreateGridLayoutPanel(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent);
	};
}
