#include "UltraEngine.h"
#include "..\include\GridLayoutPanel.h"
#include "GridLayoutPanel_Impl.h"

namespace KLWidgets
{
	void GridLayoutPanel_Impl::CreateOrUpdateCells()
	{
		int row = 0;
		do
		{
			int column = 0;
			do
			{
				bool exists = false;
				for each (auto cell in _cellInformations)
				{
					if (cell->column == column && cell->row == row)
					{
						exists = true;
						break;
					}
				}
				if (!exists)
				{
					auto cinfo = new GridCellInformation();
					cinfo->column = column;
					cinfo->row = row;
					_cellInformations.push_back(cinfo);
				}
				column++;
			} while (column < _columnDefinitions.size());
			row++;
		} while (row < _rowDefinitions.size());
	}

	void GridLayoutPanel_Impl::CalculateGridCells()
	{
		iVec2 clientSize = ClientSize();
		int cstars = 0;
		int rstars = 0;

		for each (auto  column in _columnDefinitions)
		{
			if (column->length == GridLength::PIXEL)
			{
				clientSize.x -= column->width;
			}
			else
			{
				cstars += column->width;
			}
		}

		for each (auto  row in _rowDefinitions)
		{
			if (row->length == GridLength::PIXEL)
			{
				clientSize.y -= row->height;
			}
			else
			{
				rstars += row->height;
			}
		}

		int csize = clientSize.x / Max(1, cstars);
		int rsize = clientSize.y / Max(1, rstars);

		int columnCount = _columnDefinitions.size();
		int rowCount = _rowDefinitions.size();

		auto dimensions = new iVec2[columnCount * rowCount];


		for each (auto cell in _cellInformations)
		{
			if (cell->column < columnCount && cell->row < rowCount)
			{
				if (_columnDefinitions[cell->column]->length == GridLength::PIXEL)
				{
					cell->size.x = _columnDefinitions[cell->column]->width;
				}
				else
				{
					cell->size.x = _columnDefinitions[cell->column]->width * csize;
				}

				if (_rowDefinitions[cell->row]->length == GridLength::PIXEL)
				{
					cell->size.y = _rowDefinitions[cell->row]->height;
				}
				else
				{
					cell->size.y = _rowDefinitions[cell->row]->height * rsize;
				}

				dimensions[cell->column + (cell->row * columnCount)] = cell->size;
			}
		}

		for each (auto cell in _cellInformations)
		{

			int x = 0;
			int y = 0;
			for (int c = 0; c < cell->column; c++)
			{
				x += dimensions[c + (cell->row * columnCount)].x;
			}

			for (int r = 0; r < cell->row; r++)
			{
				y += dimensions[cell->column + (r * columnCount)].y;
			}

			cell->position.x = x;
			cell->position.y = y;

			for each (auto ci in cell->assignedWidgets)
			{
				iVec2 position = iVec2(ci->OriginalShape.x, ci->OriginalShape.y);
				iVec2 size = iVec2(ci->OriginalShape.width, ci->OriginalShape.height);
				iVec2 cellSize = iVec2(0, 0);

				for (int c = cell->column; c < cell->column + ci->ColumnSpan; c++)
				{
					cellSize.x += dimensions[c + (cell->row * columnCount)].x;
				}

				for (int r = cell->row; r < cell->row + ci->RowSpan; r++)
				{
					cellSize.y += dimensions[cell->column + (r * columnCount)].y;
				}

				if (ci->HorizontalAlignment == HorizontalAlignment::STRETCH)
				{
					position.x = cell->position.x;
					size.x = cellSize.x;
				}

				if (ci->VerticalAlignment == VerticalAlignment::STRETCH)
				{
					position.y = cell->position.y;
					size.y = cellSize.y;
				}

				ci->Child->SetShape(position, size);
				/*	ci->Child->position = position;
					ci->Child->size = size;*/
			}
		}
	}

	void GridLayoutPanel_Impl::AddRowDefinition(GridLength length, int height)
	{
		auto def = new GridRowDefinition();
		def->length = length;
		def->height = height;
		_rowDefinitions.push_back(def);
		CreateOrUpdateCells();
	}

	void GridLayoutPanel_Impl::AddColumnDefinition(GridLength length, int width)
	{
		auto def = new GridColumnDefinition();
		def->length = length;
		def->width = width;
		_columnDefinitions.push_back(def);
		CreateOrUpdateCells();
	}

	void GridLayoutPanel_Impl::AssignChild(shared_ptr<Widget> widget, int column, int row, int columnspan, int rowspan, HorizontalAlignment horizontalalignment, VerticalAlignment verticalalignment)
	{
		for each (auto cell in _cellInformations)
		{
			if (cell->column == column && cell->row == row)
			{
				cell->assignedWidgets.push_back(new GridChildInformation(widget, columnspan, rowspan, horizontalalignment, verticalalignment));
				break;
			}
		}
	}

	void GridLayoutPanel_Impl::CalculateLayout(vector<shared_ptr<Widget>>* visibleKids)
	{
		CalculateGridCells();
		visibleKids->insert(visibleKids->begin(), m_kids.begin(), m_kids.end());
	}

	void GridLayoutPanel_Impl::Draw(const int x, const int y, const int width, const int height)
	{
		blocks.clear();
		Vec4 gridLineColor = Vec4(1, 1, 0, 1);

		for each (auto cell in _cellInformations)
		{
			AddBlock(cell->position, cell->size, gridLineColor);
			AddBlock(cell->position + iVec2(1, 1), cell->size - iVec2(2, 2), color[WIDGETCOLOR_BACKGROUND]);
		}
	}


	shared_ptr<GridLayoutPanel> CreateGridLayoutPanel(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent)
	{
		auto widget = make_shared<GridLayoutPanel_Impl>();
		widget->Initialize("", x, y, width, height, parent, 0);
		widget->CreateOrUpdateCells();
		widget->Invalidate();
		widget->Redraw();

		auto pos = widget->GetPosition(true);
		widget->gui->Redraw(pos.x, pos.y, widget->size.x, widget->size.y);
		return widget;
	}

	GridChildInformation::GridChildInformation(shared_ptr<Widget> child, int columnspan, int rowspan, ::HorizontalAlignment horizontalalignment, ::VerticalAlignment verticalalignment)
	{
		Child = child;
		HorizontalAlignment = horizontalalignment;
		VerticalAlignment = verticalalignment;
		ColumnSpan = columnspan;
		RowSpan = rowspan;
		OriginalShape = iVec4(Child->position, Child->size);
	}

}