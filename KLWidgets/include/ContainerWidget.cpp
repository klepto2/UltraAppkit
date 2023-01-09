#include "UltraEngine.h"
#include "KLWidgets.h"

namespace KLWidgets
{

    iVec4 BaseWidget::GetClientArea()
    {
        return iVec4();
    }

    bool BaseWidget::IsElementVisible(shared_ptr<Widget> element)
    {
        iVec2 clientRect = ClientSize();
        return !(element->position.x < 0
            || element->position.x + element->size.x > clientRect.x
            || element->position.y < 0
            || element->position.y + element->size.y > clientRect.y);
    }

    void BaseWidget::UpdateLayout()
    {
        Widget::UpdateLayout();
        _kidsToDraw.clear();
        CalculateLayout(&_kidsToDraw);
    }

    void BaseWidget::CalculateLayout(vector<shared_ptr<Widget>>* visibleKids)
    {

    }

    void BaseWidget::Draw__(const int x, const int y, const int width, const int height)
    {
        if (_childcount != m_kids.size())
        {
            _childcount = m_kids.size();
            UpdateLayout();
        }

        Draw(x, y, width, height);

        for each (auto child in _kidsToDraw)
        {
            child->Draw__(x, y, width, height);
        }
    }
}

