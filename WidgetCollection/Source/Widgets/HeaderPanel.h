#pragma once
#include "UltraEngine.h"

using namespace UltraEngine;


enum class HeaderPlacement
{
	Left,
	Center,
	Right
};

class HeaderPanel : public Widget
{
private:
	static HCURSOR _cursorMove;
	static HCURSOR _cursorSizeNESW;
	static HCURSOR _cursorSizeNWSE;
	static HCURSOR _cursorSizeNS;
	static HCURSOR _cursorSizeWE;

	enum class State
	{
		None,
		OverHeader,
		MoveByHeader,
		ResizeTop,
		ResizeLeft,
		ResizeRight,
		ResizeBottom,
		ResizeTopLeft,
		ResizeTopRight,
		ResizeBottomLeft,
		ResizeBottomRight,
	};

	int _borderBlock;
	int _backgroundBlock;
	int _headerBackgroundBlock;
	int _headerTextBlock;
	int _collapseButtonBlock;
	int _collapseTextBlock;
	iVec3 _lastMousePosition;
	shared_ptr<Widget> _button;
	iVec2 _client;
	iVec4 _headerMargin;
	HeaderPlacement _headerPlacement;
	bool _mouseover;
	bool _resizemode;
	State _currentState;
	int _zIndex = -1;
	void SetTop();
	int GetZIndex();
	void SetZIndex(int zIndex);
protected:
	virtual void Draw_(const int x, const int y, const int width, const int height);
public:
	HeaderPanel();
	virtual void Draw(const int x, const int y, const int width, const int height);
	virtual bool Initialize(const WString& text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style);
	virtual void SetHeaderMargin(const int left, const int top, const int right, const int bottom);
	virtual void SetHeaderPlacement(HeaderPlacement placement);
	virtual void MouseMove(const int x, const int y);
	virtual void MouseEnter(const int x, const int y);
	virtual void MouseLeave(const int x, const int y);
	virtual void MouseDown(const MouseButton button, const int x, const int y);
	virtual void MouseUp(const MouseButton button, const int x, const int y);
	friend shared_ptr<HeaderPanel> CreateHeaderPanel(const WString& text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent);
};

shared_ptr<HeaderPanel> CreateHeaderPanel(const WString& text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent);

	
