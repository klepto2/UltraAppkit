#include "UltraEngine.h"
#include "LayoutPanel.h"

void LayoutPanel::Paint(const int x, const int y, const int width, const int height)
{
   
}

void LayoutPanel::Draw__(const int x, const int y, const int width, const int height)
{
    if (collapsed || hidestate)
    {
        return;
    }

    if (_childcount != m_kids.size())
    {
        UpdateLayout();
    }

    Widget::Draw__(x, y, width, height);
}

void LayoutPanel::UpdateLayout()
{
    Widget::UpdateLayout();
    iVec4 clientArea = GetClientArea();
    StartLayout();
    Arrange(clientArea);
    m_kids.clear();
    for (auto child : _kidsToDraw)
    {
        m_kids.push_back(child);
    }
}

void LayoutPanel::StartLayout()
{
    _kidsToDraw.clear();
    _childcount = m_kids.size();
}

void LayoutPanel::Paint()
{
    Widget::Paint();
}



bool LayoutPanel::IsElementVisible(shared_ptr<Widget> element)
{
    iVec2 clientRect = ClientSize();
    return !(element->position.x < 0
        || element->position.x + element->size.x > clientRect.x
        || element->position.y < 0
        || element->position.y + element->size.y > clientRect.y);
}

//void LayoutPanel::UpdateLayout()
//{
//    Widget::UpdateLayout();
//    iVec4 clientArea = GetClientArea();
//    StartLayout();
//    Arrange(clientArea);
//}

iVec4 LayoutPanel::GetClientArea()
{
    return iVec4(GetPosition(false), ClientSize());
}
