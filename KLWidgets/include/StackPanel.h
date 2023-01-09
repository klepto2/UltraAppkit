#pragma once
#include "UltraEngine.h"

using namespace UltraEngine;

namespace KLWidgets
{

	enum StackPanelStyle
	{
		STACKPANEL_HORIZONTAL,
		STACKPANEL_VERTICAL
	};

	class BaseWidget;

	class StackPanel : public BaseWidget
	{
	public:

		friend BaseWidget;
		friend shared_ptr<StackPanel> CreateStackPanel(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const StackPanelStyle style);
	};

	extern shared_ptr<StackPanel> CreateStackPanel(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const StackPanelStyle style);

	/*class OverlayWindow : public Widget
	{
	public:
		friend shared_ptr<OverlayWindow> CreateOverlayWindow(const WString& title, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent);
	};

	extern shared_ptr<OverlayWindow> CreateOverlayWindow(const WString& title, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent);*/
}