#pragma once
#include "UltraEngine.h"
#include "KLWidgets.h"
using namespace UltraEngine;

namespace KLWidgets
{

	class BaseWidget : public Widget
	{
	protected:
		iVec4 _itemmargin = iVec4(0, 0, 0, 0);
		vector<shared_ptr<Widget>> _kidsToDraw;
		int _childcount = -1;
	public:
		iVec4 GetClientArea();
		bool IsElementVisible(shared_ptr<Widget> element);
		void SetItemMargin(iVec4 margin) { _itemmargin = margin; }
		iVec4 GetItemMargin() { return _itemmargin; }
		virtual void UpdateLayout();
		virtual void CalculateLayout(vector<shared_ptr<Widget>>* visibleKids);
		virtual void Draw__(const int x, const int y, const int width, const int height);
	};
}


