#include "UltraEngine.h"
#include "StackPanel.h"

StackPanel::ChildData StackPanel::CalculateChildArea(shared_ptr<Widget> widget)
{
	auto clientarea = GetClientArea();
	auto margin = GetMargin(widget);
	StackPanel::ChildData result;
	iVec4 childArea = iVec4(iVec2(margin.x, margin.y), LayoutSystem::GetSize(widget));
	
	if ((Orientation)style == Orientation::VERTICAL)
	{
		//Left,Top,Right,Bottom
		switch (GetHorizontalAlignment(widget))
		{
		case HorizontalAlignment::Stretch:
			childArea.width = clientarea.width - margin.x - margin.z;
			break;
		case HorizontalAlignment::Right:
			childArea.x = clientarea.width - childArea.width - margin.z;
			break;
		case HorizontalAlignment::Center:
			childArea.x = clientarea.width / 2.0 - childArea.width / 2.0;
			break;
		}
	}
	else
	{
		//Left,Top,Right,Bottom
		switch (GetVerticalAlignment(widget))
		{
		case VerticalAlignment::Stretch:
			childArea.height = clientarea.height - margin.y - margin.w;
			break;
		case VerticalAlignment::Bottom:
			childArea.y = clientarea.height - childArea.height - margin.w;
			break;
		case VerticalAlignment::Center:
			childArea.y = clientarea.height / 2.0 - childArea.height / 2.0;
			break;
		}
	}

	iVec2 containerSize = iVec2(childArea.width + margin.x + margin.z, childArea.height + margin.y + margin.w);

	result.ChildArea = childArea;
	result.ContainerSize = containerSize;

	return result;
}

void StackPanel::Arrange(iVec4 clientarea)
{
	int nextx = 0;
	int nexty = 0;

	switch ((Orientation)style)
	{
	case Orientation::HORIZONTAL:
	
		for each (auto child in m_kids)
		{
			auto childData = CalculateChildArea(child);
			child->SetShape(iVec2(nextx + childData.ChildArea.x, childData.ChildArea.y), iVec2(childData.ChildArea.width, childData.ChildArea.height));
			nextx += childData.ContainerSize.x;

			if (IsElementVisible(child))
			{
				_kidsToDraw.push_back(child);
			}
		}
		break;
	case Orientation::VERTICAL:
		
		for each (auto child in m_kids)
		{
			auto childData = CalculateChildArea(child);
			child->SetShape(iVec2(childData.ChildArea.x,nexty + childData.ChildArea.y), iVec2(childData.ChildArea.width, childData.ChildArea.height));
			nexty += childData.ContainerSize.y;

			if (IsElementVisible(child))
			{
				_kidsToDraw.push_back(child);
			}
		}
		break;
	}
}

shared_ptr<StackPanel> CreateStackPanel(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const Orientation orientation)
{
	auto widget = make_shared<StackPanel>();
	parent->kids.push_back(widget);
	widget->parent = parent;
	widget->position.x = x; widget->position.y = y; widget->size.x = width; widget->size.y = height;
	widget->gui = parent->gui;
	widget->edgedistance[0] = x;
	widget->edgedistance[2] = y;
	widget->style = (int)orientation;
	widget->Initialize("", x, y, width, height, parent, (int)orientation);
	widget->Invalidate();
	widget->Redraw();
	auto pos = widget->GetPosition(true);
	widget->gui->Redraw(pos.x, pos.y, widget->size.x, widget->size.y);
	return widget;
}
