#pragma once
#include "UltraEngine.h"
#include "LayoutInterface.h"

using namespace UltraEngine;
using namespace UltraEngine::LayoutSystem;

class LayoutPanel : public Widget
{
	protected:
		vector<shared_ptr<Widget>> _kidsToDraw;
		int _childcount = -1;
		virtual void Paint(const int x, const int y, const int width, const int height);
		virtual void Draw__(const int x, const int y, const int width, const int height);
		virtual void Paint();
		virtual void Arrange(iVec4 clientarea) = 0;
		virtual void StartLayout();
		bool IsElementVisible(shared_ptr<Widget> element);
		virtual void UpdateLayout();
		iVec4 GetClientArea();
};