#pragma once
#include "UltraEngine.h"
#include "LayoutInterface.h"
#include "LayoutPanel.h"

using namespace UltraEngine;
using namespace UltraEngine::LayoutSystem;

enum class FlowPanelBehaviour
{
	DEFAULT,
	UNIFORM
};


class FlowPanel : public LayoutPanel
{
private:
	struct ChildData
	{
		iVec2 ContainerSize;
		iVec4 ChildArea;
	};

	ChildData CalculateChildArea(shared_ptr<Widget> widget);
protected:
	virtual void Arrange(iVec4 clientarea);
public:
	void SetItemSize(int widht, int height);
	iVec2 GetItemSize();
	friend shared_ptr<FlowPanel> CreateFlowPanel(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const FlowPanelBehaviour orientation);
};

extern shared_ptr<FlowPanel> CreateFlowPanel(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const FlowPanelBehaviour orientation);



/*int FlowPanel::ReorderChildren()
{
	int tempx = itemspacing;
	int tempy = -offset + itemspacing;
	int nexty = -1;
	int maxy = -1;

	for each (auto child in this->m_kids)
	{
		if (child == slider)
		{
			continue;
		}

		int sliderpadding = 0;
		if (!slider->Hidden())
			sliderpadding = slider->size.x + this->padding.y;

		auto clientsize = this->ClientSize();

		if (behaviour == FlowPanelBehaviour::FLOWPANEL_UNIFORM)
		{
			child->size.x = itemsize.x;
			child->size.y = itemsize.y;
		}

		if (tempx + child->size.x > clientsize.x - sliderpadding)
		{
			tempx = itemspacing;
			tempy += nexty;
			nexty = -1;
		}

		child->position = iVec2(tempx, tempy);
		tempx += child->size.x + itemspacing;

		if (nexty < 0 || nexty < child->size.y + itemspacing)
		{
			nexty = child->size.y + itemspacing;
		}

		auto tempmaxy = tempy + child->size.y;
		if (maxy < tempmaxy)
			maxy = tempmaxy;
	}

	return maxy + offset;
}*/





