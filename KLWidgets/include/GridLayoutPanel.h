#pragma once
#include "UltraEngine.h"
#include "KLWidgets.h"

using namespace UltraEngine;

namespace KLWidgets
{

	enum class GridLength
	{
		STAR,
		PIXEL
	};

	enum class HorizontalAlignment
	{
		LEFT,
		CENTER,
		RIGHT,
		STRETCH
	};

	enum class VerticalAlignment
	{
		TOP,
		CENTER,
		BOTTOM,
		STRETCH
	};


	struct GridRowDefinition
	{
		GridLength length;
		int height;
	};

	struct GridColumnDefinition
	{
		GridLength length;
		int width;
	};


	class GridLayoutPanel : public BaseWidget
	{
	public:
		virtual void AddRowDefinition(GridLength length, int height = 1) = 0;
		virtual void AddColumnDefinition(GridLength length, int width = 1) = 0;
		virtual void AssignChild(shared_ptr<Widget> widget, int column, int row, int columnspan = 1, int rowspan = 1, HorizontalAlignment horizontalalignment = HorizontalAlignment::STRETCH, VerticalAlignment verticalalignment = VerticalAlignment::STRETCH) = 0;
		friend BaseWidget;
		friend shared_ptr<GridLayoutPanel> CreateGridLayoutPanel(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent);
	};

	extern shared_ptr<GridLayoutPanel> CreateGridLayoutPanel(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent);
}