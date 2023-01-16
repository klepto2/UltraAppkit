#pragma once
#include "UltraEngine.h"
#include "LayoutInterface.h"
#include "LayoutPanel.h"

using namespace UltraEngine;
using namespace UltraEngine::LayoutSystem;

enum class Orientation
{
	HORIZONTAL,
	VERTICAL
};


class StackPanel : public LayoutPanel
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
	friend shared_ptr<StackPanel> CreateStackPanel(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const Orientation orientation);
};

extern shared_ptr<StackPanel> CreateStackPanel(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const Orientation orientation);
