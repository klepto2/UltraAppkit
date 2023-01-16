#include "UltraEngine.h"
#include "LayoutInterface.h"

using namespace UltraEngine::LayoutSystem;

map<shared_ptr<Widget>, shared_ptr<LayoutInformation>> LayoutInformation::_layoutinfos = map<shared_ptr<Widget>, shared_ptr<LayoutInformation>>();

shared_ptr<LayoutInformation> LayoutInformation::CreateDefault(shared_ptr<Widget> widget)
{
	auto info = make_shared<LayoutInformation>();
	info->_Column = 0;
	info->_ColumnSpan = 1;
	info->_Row = 0;
	info->_RowSpan = 1;
	info->_Margin = iVec4(0,0,0,0);
	info->_Padding = iVec4(0,0,0,0);
	info->_Size = widget->GetSize();
	info->_Position = widget->GetPosition();
	info->_VerticalAlignement = VerticalAlignment::Stretch;
	info->_HorizontalAlignment = HorizontalAlignment::Stretch;
	return info;
}

shared_ptr<LayoutInformation> UltraEngine::LayoutSystem::LayoutInformation::GetLayoutInformationOrDefault(shared_ptr<Widget> widget)
{
	if (LayoutInformation::_layoutinfos.find(widget) == LayoutInformation::_layoutinfos.end()) {
		_layoutinfos[widget] = CreateDefault(widget);
	}
	return _layoutinfos[widget];
}

void UltraEngine::LayoutSystem::SetVerticalAlignment(shared_ptr<Widget> widget, VerticalAlignment alignment)
{
	auto info = LayoutInformation::GetLayoutInformationOrDefault(widget);
	info->_VerticalAlignement = alignment;
}

VerticalAlignment UltraEngine::LayoutSystem::GetVerticalAlignment(shared_ptr<Widget> widget)
{
	auto info = LayoutInformation::GetLayoutInformationOrDefault(widget);
	return info->_VerticalAlignement;
}

void UltraEngine::LayoutSystem::SetHorizontalAlignment(shared_ptr<Widget> widget, HorizontalAlignment alignment)
{
	auto info = LayoutInformation::GetLayoutInformationOrDefault(widget);
	info->_HorizontalAlignment = alignment;
}

HorizontalAlignment UltraEngine::LayoutSystem::GetHorizontalAlignment(shared_ptr<Widget> widget)
{
	auto info = LayoutInformation::GetLayoutInformationOrDefault(widget);
	return info->_HorizontalAlignment;
}

void UltraEngine::LayoutSystem::SetMargin(shared_ptr<Widget> widget, iVec4 margin)
{
	auto info = LayoutInformation::GetLayoutInformationOrDefault(widget);
	info->_Margin = margin;
}

iVec4 UltraEngine::LayoutSystem::GetMargin(shared_ptr<Widget> widget)
{
	auto info = LayoutInformation::GetLayoutInformationOrDefault(widget);
	return info->_Margin;
}

void UltraEngine::LayoutSystem::SetPadding(shared_ptr<Widget> widget, iVec4 padding)
{
	auto info = LayoutInformation::GetLayoutInformationOrDefault(widget);
	info->_Margin = padding;
}

iVec4 UltraEngine::LayoutSystem::GetPadding(shared_ptr<Widget> widget)
{
	auto info = LayoutInformation::GetLayoutInformationOrDefault(widget);
	return info->_Padding;
}

void UltraEngine::LayoutSystem::SetRow(shared_ptr<Widget> widget, int row, int rowspan)
{
	auto info = LayoutInformation::GetLayoutInformationOrDefault(widget);
	info->_Row = row;
	info->_RowSpan = rowspan;
}

iVec2 UltraEngine::LayoutSystem::GetRow(shared_ptr<Widget> widget)
{
	auto info = LayoutInformation::GetLayoutInformationOrDefault(widget);
	return iVec2(info->_Row, info->_RowSpan);
}

void UltraEngine::LayoutSystem::SetColumn(shared_ptr<Widget> widget, int column, int columnspan)
{
	auto info = LayoutInformation::GetLayoutInformationOrDefault(widget);
	info->_Column = column;
	info->_ColumnSpan = columnspan;
}

iVec2 UltraEngine::LayoutSystem::GetColumn(shared_ptr<Widget> widget)
{
	auto info = LayoutInformation::GetLayoutInformationOrDefault(widget);
	return iVec2(info->_Column, info->_ColumnSpan);
}

iVec2 UltraEngine::LayoutSystem::GetSize(shared_ptr<Widget> widget)
{
	auto info = LayoutInformation::GetLayoutInformationOrDefault(widget);
	return info->_Size;
}

iVec2 UltraEngine::LayoutSystem::GetPosition(shared_ptr<Widget> widget)
{
	auto info = LayoutInformation::GetLayoutInformationOrDefault(widget);
	return info->_Position;
}

