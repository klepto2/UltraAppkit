#pragma once
#include "UltraEngine.h"
#include "..\include\KLWidgets.h"

using namespace UltraEngine;


namespace KLWidgets
{

	class StackPanel_Impl : public StackPanel
	{
		void CalculateLayout(vector<shared_ptr<Widget>>* visibleKids);
		friend shared_ptr<StackPanel> CreateStackPanel(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const StackPanelStyle style);
	};

	/*class OverlayWindow_Impl : public OverlayWindow
	{
		bool Initialize(const WString& text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style);
		friend shared_ptr<StackPanel> CreateOverlayWindow(const WString& title, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent);
	};*/
}
