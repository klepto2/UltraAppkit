#include "UltraEngine.h"
#include "HeaderPanel.h"

HCURSOR HeaderPanel::_cursorMove = LoadCursor(NULL, IDC_SIZEALL);
HCURSOR HeaderPanel::_cursorSizeNESW = LoadCursor(NULL, IDC_SIZENESW);
HCURSOR HeaderPanel::_cursorSizeNWSE= LoadCursor(NULL, IDC_SIZENWSE);
HCURSOR HeaderPanel::_cursorSizeNS=LoadCursor(NULL, IDC_SIZENS);
HCURSOR HeaderPanel::_cursorSizeWE= LoadCursor(NULL, IDC_SIZEWE);

void HeaderPanel::SetTop()
{
    auto parent = GetParent();
    if (parent != nullptr)
    {
        parent->m_kids.erase(parent->m_kids.begin() + GetZIndex());
        parent->m_kids.push_back(Self()->As<HeaderPanel>());
        parent->Redraw();
    }
}

int HeaderPanel::GetZIndex()
{
    auto parent = GetParent();
    if (parent != nullptr)
    {
        for (int index = 0; index < parent->m_kids.size();index++)
        {
            if (parent->m_kids[index] == Self())
            {
                return index;
            }
        }
    }

    return -1;
}

void HeaderPanel::SetZIndex(int zIndex)
{
    auto parent = GetParent();
    if (parent != nullptr)
    {
        parent->m_kids.erase(parent->m_kids.begin() + GetZIndex());
        parent->m_kids.insert(parent->m_kids.begin() + zIndex,Self()->As<HeaderPanel>());
        parent->Redraw();
    }
}

void HeaderPanel::Draw_(const int x, const int y, const int width, const int height)
{
    Widget::Draw__(x, y, width, height);
    //GetParent()->Draw__(x, y, width, height);
}

HeaderPanel::HeaderPanel() :  _headerPlacement(HeaderPlacement::Center)
                            , _headerMargin(iVec4())
                            , _borderBlock(0)
                            , _backgroundBlock(0)
                            , _headerBackgroundBlock(0)
                            , _headerTextBlock(0)
{
    _borderBlock = AddBlock(iVec2(0), iVec2(0));
    _backgroundBlock = AddBlock(iVec2(0), iVec2(0));
    _headerBackgroundBlock = AddBlock(iVec2(0), iVec2(0));
    _headerTextBlock = AddBlock(iVec2(0), iVec2(0));
    _collapseButtonBlock = AddBlock(iVec2(0), iVec2(0));
    _collapseTextBlock = AddBlock(iVec2(0), iVec2(0));
}

void HeaderPanel::Draw(const int x, const int y, const int width, const int height)
{
    float scale = gui->GetScale();
   
    blocks[_backgroundBlock].color = this->color[WIDGETCOLOR_BACKGROUND];
    blocks[_backgroundBlock].size = this->size-2;
    blocks[_backgroundBlock].position = iVec2(1, 1);


    blocks[_borderBlock].color = this->color[WIDGETCOLOR_BORDER];
    //blocks[_borderBlock].wireframe = true;
    blocks[_borderBlock].size = this->size;

    int fontHeight = gui->GetFontHeight(font, fontscale, fontweight);

    
    blocks[_headerBackgroundBlock].color = this->color[WIDGETCOLOR_SUNKEN];

    if ((_currentState == State::OverHeader || _currentState == State::MoveByHeader) && _mouseover)
    {
        blocks[_headerBackgroundBlock].color = this->color[WIDGETCOLOR_HIGHLIGHT];
    }

    blocks[_headerBackgroundBlock].position = iVec2(1, 1);
    blocks[_headerBackgroundBlock].size = iVec2(size.x-2, fontHeight + _headerMargin.y + _headerMargin.w);

    switch (_headerPlacement)
    {
    case HeaderPlacement::Center:
        blocks[_headerTextBlock].position = iVec2(1, _headerMargin.y);
        blocks[_headerTextBlock].size = iVec2(size.x - 2, fontHeight);
        blocks[_headerTextBlock].textalignment = TextAlignment::TEXT_CENTER;
        break;
    case HeaderPlacement::Left:
        blocks[_headerTextBlock].position = iVec2(_headerMargin.x, _headerMargin.y);
        blocks[_headerTextBlock].size = iVec2(size.x - 2 - _headerMargin.x - _headerMargin.z, fontHeight);
        blocks[_headerTextBlock].textalignment = TextAlignment::TEXT_MIDDLE;
        break;
    case HeaderPlacement::Right:
        auto textwidth = gui->GetTextWidth(font, fontscale, text, fontweight);
        blocks[_headerTextBlock].position = iVec2(size.x - textwidth - _headerMargin.z, _headerMargin.y);
        blocks[_headerTextBlock].size = iVec2(textwidth, fontHeight);
        blocks[_headerTextBlock].textalignment = TextAlignment::TEXT_MIDDLE;
        break;
    }
   
    blocks[_headerTextBlock].text = text;
    blocks[_headerTextBlock].textdefined = true;
    blocks[_headerTextBlock].color = this->color[WIDGETCOLOR_FOREGROUND];

    switch (_currentState)
    {
    case State::MoveByHeader:
    {
        SetCursor(HeaderPanel::_cursorMove);
        break;
    }
    case State::ResizeBottom:
    case State::ResizeTop:
    {
        //gui->GetWindow()->SetPointer(MousePointer::POINTER_SIZENS);
        SetCursor(HeaderPanel::_cursorSizeNS);
        break;
    }
    case State::ResizeLeft:
    case State::ResizeRight:
    {
        //gui->GetWindow()->SetPointer(MousePointer::POINTER_SIZEWE);
        SetCursor(HeaderPanel::_cursorSizeWE);
        break;
    }   
    case State::ResizeTopLeft:
    {
        //gui->GetWindow()->SetPointer(MousePointer::POINTER_SIZENWSE);
        SetCursor(HeaderPanel::_cursorSizeNWSE);
        break;
    }
    case State::ResizeTopRight:
    {
        //gui->GetWindow()->SetPointer(MousePointer::POINTER_SIZENESW);
        SetCursor(HeaderPanel::_cursorSizeNESW);
        break;
    }
    case State::ResizeBottomLeft:
    {
        //gui->GetWindow()->SetPointer(MousePointer::POINTER_SIZENESW);
        SetCursor(HeaderPanel::_cursorSizeNESW);
        break;
    }
    case State::ResizeBottomRight:
    {
        //gui->GetWindow()->SetPointer(MousePointer::POINTER_SIZENWSE);
        SetCursor(HeaderPanel::_cursorSizeNWSE);
        break;
    }
    default:
    {
        gui->GetWindow()->SetPointer(MousePointer::POINTER_DEFAULT);
    }
    }

    /*AddBlock(iVec2(0), this->size, this->color[WIDGETCOLOR_BORDER]);
    AddBlock(iVec2(1), this->size-2, this->color[WIDGETCOLOR_BACKGROUND]);
    int textblock = AddBlock(iVec2(6,1), iVec2(this->size.x-2, 30));
    blocks[textblock].text = text;
    blocks[textblock].textalignment = TEXT_CENTER | TEXT_MIDDLE;
    blocks[textblock].textdefined = true;
    blocks[textblock].color = this->color[WIDGETCOLOR_FOREGROUND];*/
}

bool HeaderPanel::Initialize(const WString& text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style)
{
    if (Widget::Initialize(text, x, y, width, height, parent, style))
    {
        SetPadding(1, 1, 31, 1);
        _button = CreateButton(L"\x2022", -20, 2, 20, 20, Self()->As<HeaderPanel>());
        m_kids.clear();
        return true;
    }

    return false;
}

void HeaderPanel::SetHeaderMargin(const int left, const int top, const int right, const int bottom)
{
    _headerMargin.x = left;
    _headerMargin.y = top;
    _headerMargin.z = right;
    _headerMargin.w = bottom;
}

void HeaderPanel::SetHeaderPlacement(HeaderPlacement placement)
{
    _headerPlacement = placement;
}

void HeaderPanel::MouseMove(const int x, const int y)
{
    int resizegrip = 6;
    if (_currentState != State::MoveByHeader && !_resizemode)
    {
        //if (_mouseover)
        {
            if (x < resizegrip && y < resizegrip)
            {
                Print("TL");
                _currentState = State::ResizeTopLeft;
            }
            else if (x < resizegrip && y > resizegrip && y < size.y - resizegrip)
            {
                Print("L");
                _currentState = State::ResizeLeft;
            }
            else if (x < resizegrip && y > size.y - resizegrip)
            {
                Print("BL");
                _currentState = State::ResizeBottomLeft;
            }
            else if (x > size.x - resizegrip && y < 4)
            {
                _currentState = State::ResizeTopRight;
            }
            else if (x < 4 && y < 4)
            {

            }
            else
            {
                if (x < blocks[_headerBackgroundBlock].size.x + 1 &&
                    y < blocks[_headerBackgroundBlock].size.y + 1)
                {
                    _currentState = State::OverHeader;
                }
                else
                {
                    _currentState = State::None;
                }
            }
        }
    }
    else
    {
        iVec3 mousediff = gui->GetWindow()->GetMousePosition() - _lastMousePosition;
        iVec2 direction = iVec2(mousediff.x, mousediff.y);
        iVec2 size = GetSize();
        iVec2 position = GetPosition();

        switch (_currentState)
        {
        case State::MoveByHeader:
            position += direction;
            break;
        case State::ResizeTop:
            break;
        case State::ResizeLeft:
            break;
        case State::ResizeRight:
            break;
        case State::ResizeBottom:
            break;
        case State::ResizeTopLeft:
        {
            position += direction;
            size -= direction;
            break;
        }
        case State::ResizeTopRight:
            break;
        case State::ResizeBottomLeft:
            break;
        case State::ResizeBottomRight: 
            break;
        }

        SetShape(position, size);
        _lastMousePosition = gui->GetWindow()->GetMousePosition();
       
    }

    GetParent()->Redraw();
  
}

void HeaderPanel::MouseEnter(const int x, const int y)
{
    if (_currentState != State::MoveByHeader && !_resizemode)
    {
        _mouseover = true;
        Redraw();
    }
}

void HeaderPanel::MouseLeave(const int x, const int y)
{
    if (_currentState != State::MoveByHeader && !_resizemode)
    {
        _mouseover = false;
        Redraw();
    }
}

void HeaderPanel::MouseDown(const MouseButton button, const int x, const int y)
{
    if (button == MouseButton::MOUSE_LEFT)
    {
        _resizemode = false;
        switch (_currentState)
        {
        case State::OverHeader:
            _currentState = State::MoveByHeader;
            _zIndex = GetZIndex();
            SetTop();
            _lastMousePosition = gui->GetWindow()->GetMousePosition();
            break;
        case State::ResizeTop:
        case State::ResizeLeft:
        case State::ResizeRight:
        case State::ResizeBottom:
        case State::ResizeTopLeft:
        case State::ResizeTopRight:
        case State::ResizeBottomLeft:
        case State::ResizeBottomRight:
            _resizemode = true;
            _zIndex = GetZIndex();
            SetTop();
            _lastMousePosition = gui->GetWindow()->GetMousePosition();
            break;
        }

        if (_currentState == State::OverHeader)
        {
            
        }
        
    }
}

void HeaderPanel::MouseUp(const MouseButton button, const int x, const int y)
{
    if (button == MouseButton::MOUSE_LEFT)
    {
        //SetZIndex(_zIndex);
        _resizemode = false;
        _currentState = State::None;
        Redraw();
    }
}

shared_ptr<HeaderPanel> CreateHeaderPanel(const WString& text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent)
{
    auto widget = make_shared<HeaderPanel>();
    widget->Initialize(text, x, y, width, height, parent, 0);
    return widget;
}
