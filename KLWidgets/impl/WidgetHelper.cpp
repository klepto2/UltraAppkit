#include "UltraEngine.h"
#include "WidgetHelper.h"

namespace KLWidgets
{
    bool ChildVisibleInContainer(iVec2 childposition, iVec2 childsize, iVec4 containerShape)
    {
        return !(childposition.x < 0
            || childposition.x + childsize.x > containerShape.width
            || childposition.y < 0
            || childposition.y + childsize.y > containerShape.height);

    }
}
