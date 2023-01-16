#pragma once
#include "UltraEngine.h"
using namespace UltraEngine;

namespace UltraEngine
{
	namespace LayoutSystem
	{
		enum class VerticalAlignment
		{
			Stretch,
			Top,
			Center,
			Bottom
		};

		enum class HorizontalAlignment
		{
			Stretch,
			Left,
			Center,
			Right
		};

		class LayoutInformation : public Object
		{
		private:
			static map<shared_ptr<Widget>, shared_ptr<LayoutInformation>> _layoutinfos;
			static shared_ptr<LayoutInformation> CreateDefault(shared_ptr<Widget> widget);
			
			VerticalAlignment _VerticalAlignement;
			HorizontalAlignment _HorizontalAlignment;
			iVec4 _Margin;
			iVec4 _Padding;
			iVec2 _Size;
			iVec2 _Position;
			int _Row;
			int _Column;
			int _RowSpan;
			int _ColumnSpan;
		public:
			static shared_ptr<LayoutInformation> GetLayoutInformationOrDefault(shared_ptr<Widget> widget);

			friend void SetVerticalAlignment(shared_ptr<Widget> widget, VerticalAlignment alignment);
			friend VerticalAlignment GetVerticalAlignment(shared_ptr<Widget> widget);
			friend void SetHorizontalAlignment(shared_ptr<Widget> widget, HorizontalAlignment alignment);
			friend HorizontalAlignment GetHorizontalAlignment(shared_ptr<Widget> widget);
			friend void SetMargin(shared_ptr<Widget> widget, iVec4 margin);
			friend iVec4 GetMargin(shared_ptr<Widget> widget);
			friend void SetPadding(shared_ptr<Widget> widget, iVec4 padding);
			friend iVec4 GetPadding(shared_ptr<Widget> widget);
			friend void SetRow(shared_ptr<Widget> widget, int row, int rowspan);
			friend iVec2 GetRow(shared_ptr<Widget> widget);
			friend void SetColumn(shared_ptr<Widget> widget, int column, int columnspan);
			friend iVec2 GetColumn(shared_ptr<Widget> widget);

			friend iVec2 GetSize(shared_ptr<Widget> widget);
			friend iVec2 GetPosition(shared_ptr<Widget> widget);
		};

		void SetVerticalAlignment(shared_ptr<Widget> widget, VerticalAlignment alignment);
		VerticalAlignment GetVerticalAlignment(shared_ptr<Widget> widget);

		void SetHorizontalAlignment(shared_ptr<Widget> widget, HorizontalAlignment alignment);
		HorizontalAlignment GetHorizontalAlignment(shared_ptr<Widget> widget);

		void SetMargin(shared_ptr<Widget> widget, iVec4 margin);
		iVec4 GetMargin(shared_ptr<Widget> widget);

		void SetPadding(shared_ptr<Widget> widget, iVec4 padding);
		iVec4 GetPadding(shared_ptr<Widget> widget);

		void SetRow(shared_ptr<Widget> widget, int row, int rowspan = 1);
		iVec2 GetRow(shared_ptr<Widget> widget);

		void SetColumn(shared_ptr<Widget> widget, int column, int columnspan = 1);
		iVec2 GetColumn(shared_ptr<Widget> widget);

		iVec2 GetSize(shared_ptr<Widget> widget);
		iVec2 GetPosition(shared_ptr<Widget> widget);
	}
}