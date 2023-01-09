#include "UltraEngine.h"
#include "KLWidgets_Impl.h"

using namespace UltraEngine;

namespace KLWidgets
{

	void StackPanel_Impl::CalculateLayout(vector<shared_ptr<Widget>>* visibleKids)
	{
		BaseWidget::CalculateLayout(visibleKids);
		iVec4 margin = GetItemMargin();
		if (style == STACKPANEL_HORIZONTAL)
		{
			int nextx = margin.x;
			int maxy = -1;
			for each (auto child in m_kids)
			{
				child->SetShape(iVec2(nextx, 0), child->GetSize());
				nextx += child->GetSize().x + margin.x;
				if (child->size.y > maxy)
				{
					maxy = child->size.y;
				}

				if (IsElementVisible(child))
				{
					visibleKids->push_back(child);
				}
			}

		}
		else
		{
			int nexty = margin.y;
			int maxx = -1;
			for each (auto child in m_kids)
			{
				child->SetShape(iVec2(0, nexty), child->GetSize());
				nexty += child->GetSize().y + margin.y;
				if (child->size.x > maxx)
				{
					maxx = child->size.x;
				}

				if (IsElementVisible(child))
				{
					visibleKids->push_back(child);
				}
			}

		}
	}

	shared_ptr<StackPanel> CreateStackPanel(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const StackPanelStyle style)
	{
		auto widget = make_shared<StackPanel_Impl>();
		parent->kids.push_back(widget);
		widget->parent = parent;
		widget->position.x = x; widget->position.y = y; widget->size.x = width; widget->size.y = height;
		widget->gui = parent->gui;
		widget->edgedistance[0] = x;
		widget->edgedistance[2] = y;
		widget->style = style;
		widget->Initialize("", x, y, width, height, parent, style);
		widget->Invalidate();
		widget->Redraw();
		auto pos = widget->GetPosition(true);
		widget->gui->Redraw(pos.x, pos.y, widget->size.x, widget->size.y);
		return widget;
	}

	//shared_ptr<OverlayWindow> CreateOverlayWindow(const WString& title, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent)
	//{
	//	auto widget = make_shared<OverlayWindow_Impl>();
	//	widget->Initialize(title, x, y, width, height, parent, 0);
	//	widget->Invalidate();
	//	widget->Redraw();
	//	auto pos = widget->GetPosition(true);
	//	widget->gui->Redraw(pos.x, pos.y, widget->size.x, widget->size.y);
	//	return widget;
	//}
}