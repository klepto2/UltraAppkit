#include "UltraEngine.h"
#include "TextEditor_Impl.h"

namespace KLWidgets
{
    bool TextEditor_Impl::SCI_REGISTERED = false;
    vector<shared_ptr<Widget>> TextEditor_Impl::TextEditorMap;

	shared_ptr<TextEditor> CreateTextEditor(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent)
	{
		auto widget = make_shared<TextEditor_Impl>();

		widget->Initialize("", x, y, width, height, parent, 0);

		float scale = widget->gui->scale;
		widget->_verticalSlider = CreateSlider(width - widget->slidersize * scale, 0, widget->slidersize * scale, height, widget, SLIDER_VERTICAL | SLIDER_SCROLLBAR);
		widget->_horizontalSlider = CreateSlider(0, width - widget->slidersize * scale, width, widget->slidersize * scale, widget, SLIDER_HORIZONTAL | SLIDER_SCROLLBAR);

		TextEditor_Impl::TextEditorMap.push_back(widget);
		return widget;
	}

	void TextEditor_Impl::Draw(const int x, const int y, const int width, const int height)
	{
		blocks.clear();
		if (initialized)
		{
			if (!_horizontalSlider->Hidden() && !_verticalSlider->Hidden())
			{
				AddBlock(iVec2(sciShape.width, sciShape.height), iVec2(slidersize * gui->scale, slidersize * gui->scale), this->color[COLOR_BACKGROUND]);
			}
		}
	}

	void TextEditor_Impl::Draw__(const int x, const int y, const int width, const int height)
	{
		auto pos = GetPosition(true);
		auto size = this->GetParent()->ClientSize();
		int vScroll = 0;
		int hScroll = 0;


		if (initialized)
		{
			int hScrollValue = _horizontalSlider->GetValue();
			int vScrollValue = _verticalSlider->GetValue();

			UpdateSlider(x, y, width, height);

			int nhScrollValue = _horizontalSlider->GetValue();
			int nvScrollValue = _verticalSlider->GetValue();

			if (nhScrollValue != hScrollValue || nvScrollValue != vScrollValue)
			{
				SetXOffset(hScrollValue * 10.0);
				_horizontalSlider->SetValue(hScrollValue);
				SetFirstVisibleLine(vScrollValue);
				_verticalSlider->SetValue(vScrollValue);
			}


			if (!_horizontalSlider->Hidden())
			{
				hScroll = _horizontalSlider->size.y;
			}

			if (!_verticalSlider->Hidden())
			{
				vScroll = _verticalSlider->size.x;
			}
		}

		sciShape = iVec4(pos.x, pos.y, size.x - vScroll, size.y - hScroll);
		SetWindowPos(hWND, HWND_TOP, pos.x, pos.y, size.x - vScroll, size.y - hScroll, 0);
		Widget::Draw__(x, y, width, height);
	}

	void TextEditor_Impl::UpdateLayout()
	{
		Widget::UpdateLayout();
	}

	bool TextEditor_Impl::ProcessEvent(const Event& e)
	{
		return true;
	}

	void TextEditor_Impl::UpdateSlider(const int x, const int y, const int width, const int height)
	{
		int hscrollwidthlimiter = 0;
		int vscrollheightlimiter = 0;
		if (ScrollWidth() > GetSize().y)
		{
			_horizontalSlider->Show();
			int margin_count = Margins();
			int msize = MarginLeft() + MarginRight();
			for (int m = 0; m < margin_count; m++)
			{
				msize += MarginWidthN(m);
			}

			_horizontalSlider->SetRange((sciShape.width - msize) / 10.0, ScrollWidth() / 10.0);
			_horizontalSlider->SetValue(XOffset() / 10.0);
			vscrollheightlimiter = slidersize * gui->scale;
		}
		else
		{
			_horizontalSlider->Hide();
			//offset = 0;
		}

		if (LineCount() > LinesOnScreen())
		{
			_verticalSlider->Show();
			int lc = LineCount();
			int l = LinesOnScreen();
			_verticalSlider->SetRange(LinesOnScreen(), LineCount());
			_verticalSlider->SetValue(FirstVisibleLine());
			hscrollwidthlimiter = slidersize * gui->scale;
		}
		else
		{
			_verticalSlider->Hide();
		}

		_horizontalSlider->SetShape(0, this->size.y - slidersize * gui->scale, this->size.x - hscrollwidthlimiter, slidersize * gui->scale);
		_verticalSlider->SetShape(this->size.x - slidersize * gui->scale - 0, 0, slidersize * gui->scale, this->size.y - vscrollheightlimiter);
		_horizontalSlider->Show();
	}

	void TextEditor_Impl::ProcessNotification(SCNotification* n)
	{
		if (n->nmhdr.code == SCN_PAINTED)
		{
			UpdateSlider(0, 0, 0, 0);
		}
		auto notification = make_shared<TextEditorNotification>();
		notification->position = n->position;
		notification->ch = n->ch;
		notification->modifiers = n->modifiers;
		notification->modificationType = n->modificationType;
		notification->text = "";
		if (n->text != NULL)
		{
			notification->text = String(n->text);
		}
		notification->length = n->length;
		notification->linesAdded = n->linesAdded;
		notification->message = n->message;
		notification->line = n->line;
		notification->foldLevelNow = n->foldLevelNow;
		notification->foldLevelPrev = n->foldLevelPrev;
		notification->margin = n->margin;
		notification->listType = n->listType;
		notification->x = n->x;
		notification->y = n->y;
		notification->token = n->token;
		notification->annotationLinesAdded = n->annotationLinesAdded;
		notification->updated = n->updated;
		notification->listCompletionMethod = n->listCompletionMethod;
		notification->characterSource = n->characterSource;

		int id = -1;
		switch (n->nmhdr.code)
		{
		case SCN_STYLENEEDED:
			id = EVENT_STYLENEEDED;
			break;
		case SCN_CHARADDED:
			id = EVENT_CHARADDED;
			break;
		case SCN_SAVEPOINTREACHED:
			id = EVENT_SAVEPOINTREACHED;
			break;
		case SCN_SAVEPOINTLEFT:
			id = EVENT_SAVEPOINTLEFT;
			break;
		case SCN_MODIFYATTEMPTRO:
			id = EVENT_MODIFYATTEMPTRO;
			break;
		case SCN_KEY:
			id = EVENT_KEY;
			break;
		case SCN_DOUBLECLICK:
			id = EVENT_SCI_DOUBLECLICK;
			break;
		case SCN_UPDATEUI:
			id = EVENT_UPDATEUI;
			break;
		case SCN_MODIFIED:
			id = EVENT_MODIFIED;
			break;
		case SCN_MACRORECORD:
			id = EVENT_MACRORECORD;
			break;
		case SCN_MARGINCLICK:
			id = EVENT_MARGINCLICK;
			break;
		case SCN_NEEDSHOWN:
			id = EVENT_NEEDSHOWN;
			break;
		case SCN_PAINTED:
			id = EVENT_PAINTED;
			break;
		case SCN_USERLISTSELECTION:
			id = EVENT_USERLISTSELECTION;
			break;
		case SCN_URIDROPPED:
			id = EVENT_URIDROPPED;
			break;
		case SCN_DWELLSTART:
			id = EVENT_DWELLSTART;
			break;
		case SCN_DWELLEND:
			id = EVENT_DWELLEND;
			break;
		case SCN_ZOOM:
			id = EVENT_ZOOM;
			break;
		case SCN_HOTSPOTCLICK:
			id = EVENT_HOTSPOTCLICK;
			break;
		case SCN_HOTSPOTDOUBLECLICK:
			id = EVENT_HOTSPOTDOUBLECLICK;
			break;
		case SCN_HOTSPOTRELEASECLICK:
			id = EVENT_HOTSPOTRELEASECLICK;
			break;
		case SCN_INDICATORCLICK:
			id = EVENT_INDICATORCLICK;
			break;
		case SCN_INDICATORRELEASE:
			id = EVENT_INDICATORRELEASE;
			break;
		case SCN_CALLTIPCLICK:
			id = EVENT_CALLTIPCLICK;
			break;
		case SCN_AUTOCSELECTION:
			id = EVENT_AUTOCSELECTION;
			break;
		case SCN_AUTOCCANCELLED:
			id = EVENT_AUTOCCANCELLED;
			break;
		case SCN_AUTOCCHARDELETED:
			id = EVENT_AUTOCCHARDELETED;
			break;
		case SCN_FOCUSIN:
			id = EVENT_FOCUSIN;
			break;
		case SCN_FOCUSOUT:
			id = EVENT_FOCUSOUT;
			break;
		case SCN_AUTOCCOMPLETED:
			id = EVENT_AUTOCCOMPLETED;
			break;
		case SCN_MARGINRIGHTCLICK:
			id = EVENT_MARGINRIGHTCLICK;
			break;
		case SCN_AUTOCSELECTIONCHANGE:
			id = EVENT_AUTOCSELECTIONCHANGE;
			break;
		}

		if (id != -1)
		{
			EmitEvent(id, Self(), notification->ch, notification->x, notification->y, 0, 0, notification);
		}
	}

	LRESULT CALLBACK HandleScintillaNotifications(int nCode, WPARAM wParam, LPARAM lParam)
	{
		NMHDR* lpnmhdr;
		const CWPSTRUCT& cwps = *(CWPSTRUCT*)lParam;

		//Print();
		switch (cwps.message)
		{
		case WM_NOTIFY:
			lpnmhdr = (LPNMHDR)cwps.lParam;

			for (auto w : TextEditor_Impl::TextEditorMap)
			{
				auto editor = static_cast<TextEditor_Impl*>(w.get());
				if (editor->hWND == lpnmhdr->hwndFrom)
				{
					editor->ProcessNotification(reinterpret_cast<SCNotification*>(cwps.lParam));
					break;
				}
			}
			break;
		}

		return NULL;
	}

	bool TextEditor_Impl::Initialize(const WString& text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style)
	{
		if (Widget::Initialize(text, x, y, width, height, parent, style))
		{
			auto parentWindow = gui->window.lock();
			hInstance = (HINSTANCE)GetWindowLong(parentWindow->GetHandle(), GWLP_HINSTANCE);
			if (!TextEditor_Impl::SCI_REGISTERED)
			{


				if (!Scintilla_RegisterClasses(hInstance))
				{
					return false;
				}
			}

			hWND = CreateWindowA(
				"Scintilla",
				"Source",
				WS_CHILD | WS_CLIPCHILDREN,
				x, y,
				width, height,
				parentWindow->GetHandle(),
				0,
				hInstance,
				0);


			::ShowWindow(hWND, SW_SHOW);
			::SetFocus(hWND);

			SciFnDirect fn_ = reinterpret_cast<SciFnDirect>(
				SendMessage(hWND, SCI_GETDIRECTFUNCTION, 0, 0));
			const sptr_t ptr_ = SendMessage(hWND, SCI_GETDIRECTPOINTER, 0, 0);
			SetFnPtr(fn_, ptr_);

			::SetWindowsHookEx(WH_CALLWNDPROC, HandleScintillaNotifications, hInstance, GetCurrentThreadId());
			SetVScrollBar(false);
			SetHScrollBar(false);

			//_horizontalSlider->Hide();
			//UpdateLayout();
			Redraw();
			initialized = true;

			return true;
		}
		return false;
	}



	//sptr_t TextEditor_Impl::Call(unsigned int iMessage, sptr_t wParam, sptr_t lParam)
	//{
	//	return SendMessage(hWND, iMessage, wParam, lParam);
	//}

	void TextEditor_Impl::SetFnPtr(FunctionDirect fn_, intptr_t ptr_) noexcept {
		fn = fn_;
		ptr = ptr_;
	}

	bool TextEditor_Impl::IsValid() const noexcept {
		return fn && ptr;
	}


	intptr_t TextEditor_Impl::Call(Message msg, uintptr_t wParam, intptr_t lParam) {
		if (!fn)
			throw Failure(Status::Failure);
		const intptr_t retVal = fn(ptr, static_cast<unsigned int>(msg), wParam, lParam);
		return retVal;
	}

	intptr_t TextEditor_Impl::CallPointer(Message msg, uintptr_t wParam, void* s) {
		return Call(msg, wParam, reinterpret_cast<intptr_t>(s));
	}

	intptr_t TextEditor_Impl::CallString(Message msg, uintptr_t wParam, const char* s) {
		return Call(msg, wParam, reinterpret_cast<intptr_t>(s));
	}

	std::string TextEditor_Impl::CallReturnString(Message msg, uintptr_t wParam) {
		size_t len = CallPointer(msg, wParam, nullptr);
		if (len) {
			std::string value(len, '\0');
			CallPointer(msg, wParam, value.data());
			return value;
		}
		else {
			return std::string();
		}
	}

	// Common APIs made more structured and type-safe

	Position TextEditor_Impl::LineStart(Line line) {
		return Call(Message::PositionFromLine, line);
	}

	Position TextEditor_Impl::LineEnd(Line line) {
		return Call(Message::GetLineEndPosition, line);
	}

	Range TextEditor_Impl::SelectionRange() {
		return Range(
			Call(Message::GetSelectionStart),
			Call(Message::GetSelectionEnd));
	}

	Range TextEditor_Impl::TargetRange() {
		return Range(
			Call(Message::GetTargetStart),
			Call(Message::GetTargetEnd));
	}

	void TextEditor_Impl::SetTarget(Range range) {
		Call(Message::SetTargetRange, range.start, range.end);
	}

	void TextEditor_Impl::ColouriseAll() {
		Colourise(0, -1);
	}

	char TextEditor_Impl::CharacterAt(Position position) {
		return static_cast<char>(Call(Message::GetCharAt, position));
	}

	int TextEditor_Impl::UnsignedStyleAt(Position position) {
		// Returns signed value but easier to use as unsigned
		return static_cast<unsigned char>(Call(Message::GetStyleAt, position));
	}

	std::string TextEditor_Impl::StringOfRange(Range range) {
		if (range.Length() == 0) {
			return std::string();
		}
		else {
			std::string text(range.Length(), '\0');
			SetTarget(range);
			TargetText(text.data());
			return text;
		}
	}

	Position TextEditor_Impl::ReplaceTarget(std::string_view text) {
		return TextEditor_Impl::CallString(Message::ReplaceTarget, text.length(), text.data());
	}

	Position TextEditor_Impl::ReplaceTargetRE(std::string_view text) {
		return CallString(Message::ReplaceTargetRE, text.length(), text.data());
	}

	Position TextEditor_Impl::SearchInTarget(std::string_view text) {
		return CallString(Message::SearchInTarget, text.length(), text.data());
	}

	Range TextEditor_Impl::RangeSearchInTarget(std::string_view text) {
		const Position posFound = SearchInTarget(text);
		if (posFound >= 0)
			return Range(posFound, TargetEnd());
		else
			return Range(posFound, 0);
	}

	// Generated methods

	// ScintillaCall requires automatically generated casts as it is converting
	// specific types to/from generic arguments and return values of 'Call'.
	// Suppress Visual C++ Code Analysis warnings for these casts and pointer returns.
	// 26472 = Don't use a static_cast for arithmetic conversions.
	// 26487 = Don't return a pointer '*' that may be invalid (lifetime.4).
	// 26490 = Don't use reinterpret_cast.
#if defined(_MSC_VER)
#pragma warning(disable: 26472 26487 26490)
#endif

//++Autogenerated -- start of section automatically generated from Scintilla.iface
	void TextEditor_Impl::AddText(Position length, const char* text) {
		CallString(Message::AddText, length, text);
	}

	void TextEditor_Impl::AddStyledText(Position length, const char* c) {
		CallString(Message::AddStyledText, length, c);
	}

	void TextEditor_Impl::InsertText(Position pos, const char* text) {
		CallString(Message::InsertText, pos, text);
	}

	void TextEditor_Impl::ChangeInsertion(Position length, const char* text) {
		CallString(Message::ChangeInsertion, length, text);
	}

	void TextEditor_Impl::ClearAll() {
		Call(Message::ClearAll);
	}

	void TextEditor_Impl::DeleteRange(Position start, Position lengthDelete) {
		Call(Message::DeleteRange, start, lengthDelete);
	}

	void TextEditor_Impl::ClearDocumentStyle() {
		Call(Message::ClearDocumentStyle);
	}

	Position TextEditor_Impl::Length() {
		return Call(Message::GetLength);
	}

	int TextEditor_Impl::CharAt(Position pos) {
		return static_cast<int>(Call(Message::GetCharAt, pos));
	}

	Position TextEditor_Impl::CurrentPos() {
		return Call(Message::GetCurrentPos);
	}

	Position TextEditor_Impl::Anchor() {
		return Call(Message::GetAnchor);
	}

	int TextEditor_Impl::StyleAt(Position pos) {
		return static_cast<int>(Call(Message::GetStyleAt, pos));
	}

	void TextEditor_Impl::Redo() {
		Call(Message::Redo);
	}

	void TextEditor_Impl::SetUndoCollection(bool collectUndo) {
		Call(Message::SetUndoCollection, collectUndo);
	}

	void TextEditor_Impl::SelectAll() {
		Call(Message::SelectAll);
	}

	void TextEditor_Impl::SetSavePoint() {
		Call(Message::SetSavePoint);
	}

	Position TextEditor_Impl::GetStyledText(void* tr) {
		return CallPointer(Message::GetStyledText, 0, tr);
	}

	bool TextEditor_Impl::CanRedo() {
		return Call(Message::CanRedo);
	}

	Line TextEditor_Impl::MarkerLineFromHandle(int markerHandle) {
		return Call(Message::MarkerLineFromHandle, markerHandle);
	}

	void TextEditor_Impl::MarkerDeleteHandle(int markerHandle) {
		Call(Message::MarkerDeleteHandle, markerHandle);
	}

	int TextEditor_Impl::MarkerHandleFromLine(Line line, int which) {
		return static_cast<int>(Call(Message::MarkerHandleFromLine, line, which));
	}

	int TextEditor_Impl::MarkerNumberFromLine(Line line, int which) {
		return static_cast<int>(Call(Message::MarkerNumberFromLine, line, which));
	}

	bool TextEditor_Impl::UndoCollection() {
		return Call(Message::GetUndoCollection);
	}

	WhiteSpace TextEditor_Impl::ViewWS() {
		return static_cast<KLWidgets::WhiteSpace>(Call(Message::GetViewWS));
	}

	void TextEditor_Impl::SetViewWS(KLWidgets::WhiteSpace viewWS) {
		Call(Message::SetViewWS, static_cast<uintptr_t>(viewWS));
	}

	TabDrawMode TextEditor_Impl::TabDrawMode() {
		return static_cast<KLWidgets::TabDrawMode>(Call(Message::GetTabDrawMode));
	}

	void TextEditor_Impl::SetTabDrawMode(KLWidgets::TabDrawMode tabDrawMode) {
		Call(Message::SetTabDrawMode, static_cast<uintptr_t>(tabDrawMode));
	}

	Position TextEditor_Impl::PositionFromPoint(int x, int y) {
		return Call(Message::PositionFromPoint, x, y);
	}

	Position TextEditor_Impl::PositionFromPointClose(int x, int y) {
		return Call(Message::PositionFromPointClose, x, y);
	}

	void TextEditor_Impl::GotoLine(Line line) {
		Call(Message::GotoLine, line);
	}

	void TextEditor_Impl::GotoPos(Position caret) {
		Call(Message::GotoPos, caret);
	}

	void TextEditor_Impl::SetAnchor(Position anchor) {
		Call(Message::SetAnchor, anchor);
	}

	Position TextEditor_Impl::GetCurLine(Position length, char* text) {
		return CallPointer(Message::GetCurLine, length, text);
	}

	std::string TextEditor_Impl::GetCurLine(Position length) {
		return CallReturnString(Message::GetCurLine, length);
	}

	Position TextEditor_Impl::EndStyled() {
		return Call(Message::GetEndStyled);
	}

	void TextEditor_Impl::ConvertEOLs(KLWidgets::EndOfLine eolMode) {
		Call(Message::ConvertEOLs, static_cast<uintptr_t>(eolMode));
	}

	EndOfLine TextEditor_Impl::EOLMode() {
		return static_cast<KLWidgets::EndOfLine>(Call(Message::GetEOLMode));
	}

	void TextEditor_Impl::SetEOLMode(KLWidgets::EndOfLine eolMode) {
		Call(Message::SetEOLMode, static_cast<uintptr_t>(eolMode));
	}

	void TextEditor_Impl::StartStyling(Position start, int unused) {
		Call(Message::StartStyling, start, unused);
	}

	void TextEditor_Impl::SetStyling(Position length, int style) {
		Call(Message::SetStyling, length, style);
	}

	bool TextEditor_Impl::BufferedDraw() {
		return Call(Message::GetBufferedDraw);
	}

	void TextEditor_Impl::SetBufferedDraw(bool buffered) {
		Call(Message::SetBufferedDraw, buffered);
	}

	void TextEditor_Impl::SetTabWidth(int tabWidth) {
		Call(Message::SetTabWidth, tabWidth);
	}

	int TextEditor_Impl::TabWidth() {
		return static_cast<int>(Call(Message::GetTabWidth));
	}

	void TextEditor_Impl::SetTabMinimumWidth(int pixels) {
		Call(Message::SetTabMinimumWidth, pixels);
	}

	int TextEditor_Impl::TabMinimumWidth() {
		return static_cast<int>(Call(Message::GetTabMinimumWidth));
	}

	void TextEditor_Impl::ClearTabStops(Line line) {
		Call(Message::ClearTabStops, line);
	}

	void TextEditor_Impl::AddTabStop(Line line, int x) {
		Call(Message::AddTabStop, line, x);
	}

	int TextEditor_Impl::GetNextTabStop(Line line, int x) {
		return static_cast<int>(Call(Message::GetNextTabStop, line, x));
	}

	void TextEditor_Impl::SetCodePage(int codePage) {
		Call(Message::SetCodePage, codePage);
	}

	void TextEditor_Impl::SetFontLocale(const char* localeName) {
		CallString(Message::SetFontLocale, 0, localeName);
	}

	int TextEditor_Impl::FontLocale(char* localeName) {
		return static_cast<int>(CallPointer(Message::GetFontLocale, 0, localeName));
	}

	std::string TextEditor_Impl::FontLocale() {
		return CallReturnString(Message::GetFontLocale, 0);
	}

	IMEInteraction TextEditor_Impl::IMEInteraction() {
		return static_cast<KLWidgets::IMEInteraction>(Call(Message::GetIMEInteraction));
	}

	void TextEditor_Impl::SetIMEInteraction(KLWidgets::IMEInteraction imeInteraction) {
		Call(Message::SetIMEInteraction, static_cast<uintptr_t>(imeInteraction));
	}

	void TextEditor_Impl::MarkerDefine(int markerNumber, KLWidgets::MarkerSymbol markerSymbol) {
		Call(Message::MarkerDefine, markerNumber, static_cast<intptr_t>(markerSymbol));
	}

	void TextEditor_Impl::MarkerSetFore(int markerNumber, Colour fore) {
		Call(Message::MarkerSetFore, markerNumber, fore);
	}

	void TextEditor_Impl::MarkerSetBack(int markerNumber, Colour back) {
		Call(Message::MarkerSetBack, markerNumber, back);
	}

	void TextEditor_Impl::MarkerSetBackSelected(int markerNumber, Colour back) {
		Call(Message::MarkerSetBackSelected, markerNumber, back);
	}

	void TextEditor_Impl::MarkerSetForeTranslucent(int markerNumber, ColourAlpha fore) {
		Call(Message::MarkerSetForeTranslucent, markerNumber, fore);
	}

	void TextEditor_Impl::MarkerSetBackTranslucent(int markerNumber, ColourAlpha back) {
		Call(Message::MarkerSetBackTranslucent, markerNumber, back);
	}

	void TextEditor_Impl::MarkerSetBackSelectedTranslucent(int markerNumber, ColourAlpha back) {
		Call(Message::MarkerSetBackSelectedTranslucent, markerNumber, back);
	}

	void TextEditor_Impl::MarkerSetStrokeWidth(int markerNumber, int hundredths) {
		Call(Message::MarkerSetStrokeWidth, markerNumber, hundredths);
	}

	void TextEditor_Impl::MarkerEnableHighlight(bool enabled) {
		Call(Message::MarkerEnableHighlight, enabled);
	}

	int TextEditor_Impl::MarkerAdd(Line line, int markerNumber) {
		return static_cast<int>(Call(Message::MarkerAdd, line, markerNumber));
	}

	void TextEditor_Impl::MarkerDelete(Line line, int markerNumber) {
		Call(Message::MarkerDelete, line, markerNumber);
	}

	void TextEditor_Impl::MarkerDeleteAll(int markerNumber) {
		Call(Message::MarkerDeleteAll, markerNumber);
	}

	int TextEditor_Impl::MarkerGet(Line line) {
		return static_cast<int>(Call(Message::MarkerGet, line));
	}

	Line TextEditor_Impl::MarkerNext(Line lineStart, int markerMask) {
		return Call(Message::MarkerNext, lineStart, markerMask);
	}

	Line TextEditor_Impl::MarkerPrevious(Line lineStart, int markerMask) {
		return Call(Message::MarkerPrevious, lineStart, markerMask);
	}

	void TextEditor_Impl::MarkerDefinePixmap(int markerNumber, const char* pixmap) {
		CallString(Message::MarkerDefinePixmap, markerNumber, pixmap);
	}

	void TextEditor_Impl::MarkerAddSet(Line line, int markerSet) {
		Call(Message::MarkerAddSet, line, markerSet);
	}

	void TextEditor_Impl::MarkerSetAlpha(int markerNumber, KLWidgets::Alpha alpha) {
		Call(Message::MarkerSetAlpha, markerNumber, static_cast<intptr_t>(alpha));
	}

	void TextEditor_Impl::SetMarginTypeN(int margin, KLWidgets::MarginType marginType) {
		Call(Message::SetMarginTypeN, margin, static_cast<intptr_t>(marginType));
	}

	MarginType TextEditor_Impl::MarginTypeN(int margin) {
		return static_cast<KLWidgets::MarginType>(Call(Message::GetMarginTypeN, margin));
	}

	void TextEditor_Impl::SetMarginWidthN(int margin, int pixelWidth) {
		Call(Message::SetMarginWidthN, margin, pixelWidth);
	}

	int TextEditor_Impl::MarginWidthN(int margin) {
		return static_cast<int>(Call(Message::GetMarginWidthN, margin));
	}

	void TextEditor_Impl::SetMarginMaskN(int margin, int mask) {
		Call(Message::SetMarginMaskN, margin, mask);
	}

	int TextEditor_Impl::MarginMaskN(int margin) {
		return static_cast<int>(Call(Message::GetMarginMaskN, margin));
	}

	void TextEditor_Impl::SetMarginSensitiveN(int margin, bool sensitive) {
		Call(Message::SetMarginSensitiveN, margin, sensitive);
	}

	bool TextEditor_Impl::MarginSensitiveN(int margin) {
		return Call(Message::GetMarginSensitiveN, margin);
	}

	void TextEditor_Impl::SetMarginCursorN(int margin, KLWidgets::CursorShape cursor) {
		Call(Message::SetMarginCursorN, margin, static_cast<intptr_t>(cursor));
	}

	CursorShape TextEditor_Impl::MarginCursorN(int margin) {
		return static_cast<KLWidgets::CursorShape>(Call(Message::GetMarginCursorN, margin));
	}

	void TextEditor_Impl::SetMarginBackN(int margin, Colour back) {
		Call(Message::SetMarginBackN, margin, back);
	}

	Colour TextEditor_Impl::MarginBackN(int margin) {
		return static_cast<Colour>(Call(Message::GetMarginBackN, margin));
	}

	void TextEditor_Impl::SetMargins(int margins) {
		Call(Message::SetMargins, margins);
	}

	int TextEditor_Impl::Margins() {
		return static_cast<int>(Call(Message::GetMargins));
	}

	void TextEditor_Impl::StyleClearAll() {
		Call(Message::StyleClearAll);
	}

	void TextEditor_Impl::StyleSetFore(int style, Colour fore) {
		Call(Message::StyleSetFore, style, fore);
	}

	void TextEditor_Impl::StyleSetBack(int style, Colour back) {
		Call(Message::StyleSetBack, style, back);
	}

	void TextEditor_Impl::StyleSetBold(int style, bool bold) {
		Call(Message::StyleSetBold, style, bold);
	}

	void TextEditor_Impl::StyleSetItalic(int style, bool italic) {
		Call(Message::StyleSetItalic, style, italic);
	}

	void TextEditor_Impl::StyleSetSize(int style, int sizePoints) {
		Call(Message::StyleSetSize, style, sizePoints);
	}

	void TextEditor_Impl::StyleSetFont(int style, const char* fontName) {
		CallString(Message::StyleSetFont, style, fontName);
	}

	void TextEditor_Impl::StyleSetEOLFilled(int style, bool eolFilled) {
		Call(Message::StyleSetEOLFilled, style, eolFilled);
	}

	void TextEditor_Impl::StyleResetDefault() {
		Call(Message::StyleResetDefault);
	}

	void TextEditor_Impl::StyleSetUnderline(int style, bool underline) {
		Call(Message::StyleSetUnderline, style, underline);
	}

	Colour TextEditor_Impl::StyleGetFore(int style) {
		return static_cast<Colour>(Call(Message::StyleGetFore, style));
	}

	Colour TextEditor_Impl::StyleGetBack(int style) {
		return static_cast<Colour>(Call(Message::StyleGetBack, style));
	}

	bool TextEditor_Impl::StyleGetBold(int style) {
		return Call(Message::StyleGetBold, style);
	}

	bool TextEditor_Impl::StyleGetItalic(int style) {
		return Call(Message::StyleGetItalic, style);
	}

	int TextEditor_Impl::StyleGetSize(int style) {
		return static_cast<int>(Call(Message::StyleGetSize, style));
	}

	int TextEditor_Impl::StyleGetFont(int style, char* fontName) {
		return static_cast<int>(CallPointer(Message::StyleGetFont, style, fontName));
	}

	std::string TextEditor_Impl::StyleGetFont(int style) {
		return CallReturnString(Message::StyleGetFont, style);
	}

	bool TextEditor_Impl::StyleGetEOLFilled(int style) {
		return Call(Message::StyleGetEOLFilled, style);
	}

	bool TextEditor_Impl::StyleGetUnderline(int style) {
		return Call(Message::StyleGetUnderline, style);
	}

	CaseVisible TextEditor_Impl::StyleGetCase(int style) {
		return static_cast<KLWidgets::CaseVisible>(Call(Message::StyleGetCase, style));
	}

	CharacterSet TextEditor_Impl::StyleGetCharacterSet(int style) {
		return static_cast<KLWidgets::CharacterSet>(Call(Message::StyleGetCharacterSet, style));
	}

	bool TextEditor_Impl::StyleGetVisible(int style) {
		return Call(Message::StyleGetVisible, style);
	}

	bool TextEditor_Impl::StyleGetChangeable(int style) {
		return Call(Message::StyleGetChangeable, style);
	}

	bool TextEditor_Impl::StyleGetHotSpot(int style) {
		return Call(Message::StyleGetHotSpot, style);
	}

	void TextEditor_Impl::StyleSetCase(int style, KLWidgets::CaseVisible caseVisible) {
		Call(Message::StyleSetCase, style, static_cast<intptr_t>(caseVisible));
	}

	void TextEditor_Impl::StyleSetSizeFractional(int style, int sizeHundredthPoints) {
		Call(Message::StyleSetSizeFractional, style, sizeHundredthPoints);
	}

	int TextEditor_Impl::StyleGetSizeFractional(int style) {
		return static_cast<int>(Call(Message::StyleGetSizeFractional, style));
	}

	void TextEditor_Impl::StyleSetWeight(int style, KLWidgets::FontWeight weight) {
		Call(Message::StyleSetWeight, style, static_cast<intptr_t>(weight));
	}

	FontWeight TextEditor_Impl::StyleGetWeight(int style) {
		return static_cast<KLWidgets::FontWeight>(Call(Message::StyleGetWeight, style));
	}

	void TextEditor_Impl::StyleSetCharacterSet(int style, KLWidgets::CharacterSet characterSet) {
		Call(Message::StyleSetCharacterSet, style, static_cast<intptr_t>(characterSet));
	}

	void TextEditor_Impl::StyleSetHotSpot(int style, bool hotspot) {
		Call(Message::StyleSetHotSpot, style, hotspot);
	}

	void TextEditor_Impl::SetElementColour(KLWidgets::Element element, ColourAlpha colourElement) {
		Call(Message::SetElementColour, static_cast<uintptr_t>(element), colourElement);
	}

	ColourAlpha TextEditor_Impl::ElementColour(KLWidgets::Element element) {
		return static_cast<ColourAlpha>(Call(Message::GetElementColour, static_cast<uintptr_t>(element)));
	}

	void TextEditor_Impl::ResetElementColour(KLWidgets::Element element) {
		Call(Message::ResetElementColour, static_cast<uintptr_t>(element));
	}

	bool TextEditor_Impl::ElementIsSet(KLWidgets::Element element) {
		return Call(Message::GetElementIsSet, static_cast<uintptr_t>(element));
	}

	bool TextEditor_Impl::ElementAllowsTranslucent(KLWidgets::Element element) {
		return Call(Message::GetElementAllowsTranslucent, static_cast<uintptr_t>(element));
	}

	void TextEditor_Impl::SetSelFore(bool useSetting, Colour fore) {
		Call(Message::SetSelFore, useSetting, fore);
	}

	void TextEditor_Impl::SetSelBack(bool useSetting, Colour back) {
		Call(Message::SetSelBack, useSetting, back);
	}

	KLWidgets::Alpha TextEditor_Impl::SelAlpha() {
		return static_cast<KLWidgets::Alpha>(Call(Message::GetSelAlpha));
	}

	void TextEditor_Impl::SetSelAlpha(KLWidgets::Alpha alpha) {
		Call(Message::SetSelAlpha, static_cast<uintptr_t>(alpha));
	}

	bool TextEditor_Impl::SelEOLFilled() {
		return Call(Message::GetSelEOLFilled);
	}

	void TextEditor_Impl::SetSelEOLFilled(bool filled) {
		Call(Message::SetSelEOLFilled, filled);
	}

	void TextEditor_Impl::SetCaretFore(Colour fore) {
		Call(Message::SetCaretFore, fore);
	}

	void TextEditor_Impl::AssignCmdKey(int keyDefinition, int sciCommand) {
		Call(Message::AssignCmdKey, keyDefinition, sciCommand);
	}

	void TextEditor_Impl::ClearCmdKey(int keyDefinition) {
		Call(Message::ClearCmdKey, keyDefinition);
	}

	void TextEditor_Impl::ClearAllCmdKeys() {
		Call(Message::ClearAllCmdKeys);
	}

	void TextEditor_Impl::SetStylingEx(Position length, const char* styles) {
		CallString(Message::SetStylingEx, length, styles);
	}

	void TextEditor_Impl::StyleSetVisible(int style, bool visible) {
		Call(Message::StyleSetVisible, style, visible);
	}

	int TextEditor_Impl::CaretPeriod() {
		return static_cast<int>(Call(Message::GetCaretPeriod));
	}

	void TextEditor_Impl::SetCaretPeriod(int periodMilliseconds) {
		Call(Message::SetCaretPeriod, periodMilliseconds);
	}

	void TextEditor_Impl::SetWordChars(const char* characters) {
		CallString(Message::SetWordChars, 0, characters);
	}

	int TextEditor_Impl::WordChars(char* characters) {
		return static_cast<int>(CallPointer(Message::GetWordChars, 0, characters));
	}

	std::string TextEditor_Impl::WordChars() {
		return CallReturnString(Message::GetWordChars, 0);
	}

	void TextEditor_Impl::SetCharacterCategoryOptimization(int countCharacters) {
		Call(Message::SetCharacterCategoryOptimization, countCharacters);
	}

	int TextEditor_Impl::CharacterCategoryOptimization() {
		return static_cast<int>(Call(Message::GetCharacterCategoryOptimization));
	}

	void TextEditor_Impl::BeginUndoAction() {
		Call(Message::BeginUndoAction);
	}

	void TextEditor_Impl::EndUndoAction() {
		Call(Message::EndUndoAction);
	}

	void TextEditor_Impl::IndicSetStyle(int indicator, KLWidgets::IndicatorStyle indicatorStyle) {
		Call(Message::IndicSetStyle, indicator, static_cast<intptr_t>(indicatorStyle));
	}

	IndicatorStyle TextEditor_Impl::IndicGetStyle(int indicator) {
		return static_cast<KLWidgets::IndicatorStyle>(Call(Message::IndicGetStyle, indicator));
	}

	void TextEditor_Impl::IndicSetFore(int indicator, Colour fore) {
		Call(Message::IndicSetFore, indicator, fore);
	}

	Colour TextEditor_Impl::IndicGetFore(int indicator) {
		return static_cast<Colour>(Call(Message::IndicGetFore, indicator));
	}

	void TextEditor_Impl::IndicSetUnder(int indicator, bool under) {
		Call(Message::IndicSetUnder, indicator, under);
	}

	bool TextEditor_Impl::IndicGetUnder(int indicator) {
		return Call(Message::IndicGetUnder, indicator);
	}

	void TextEditor_Impl::IndicSetHoverStyle(int indicator, KLWidgets::IndicatorStyle indicatorStyle) {
		Call(Message::IndicSetHoverStyle, indicator, static_cast<intptr_t>(indicatorStyle));
	}

	IndicatorStyle TextEditor_Impl::IndicGetHoverStyle(int indicator) {
		return static_cast<KLWidgets::IndicatorStyle>(Call(Message::IndicGetHoverStyle, indicator));
	}

	void TextEditor_Impl::IndicSetHoverFore(int indicator, Colour fore) {
		Call(Message::IndicSetHoverFore, indicator, fore);
	}

	Colour TextEditor_Impl::IndicGetHoverFore(int indicator) {
		return static_cast<Colour>(Call(Message::IndicGetHoverFore, indicator));
	}

	void TextEditor_Impl::IndicSetFlags(int indicator, KLWidgets::IndicFlag flags) {
		Call(Message::IndicSetFlags, indicator, static_cast<intptr_t>(flags));
	}

	IndicFlag TextEditor_Impl::IndicGetFlags(int indicator) {
		return static_cast<KLWidgets::IndicFlag>(Call(Message::IndicGetFlags, indicator));
	}

	void TextEditor_Impl::IndicSetStrokeWidth(int indicator, int hundredths) {
		Call(Message::IndicSetStrokeWidth, indicator, hundredths);
	}

	int TextEditor_Impl::IndicGetStrokeWidth(int indicator) {
		return static_cast<int>(Call(Message::IndicGetStrokeWidth, indicator));
	}

	void TextEditor_Impl::SetWhitespaceFore(bool useSetting, Colour fore) {
		Call(Message::SetWhitespaceFore, useSetting, fore);
	}

	void TextEditor_Impl::SetWhitespaceBack(bool useSetting, Colour back) {
		Call(Message::SetWhitespaceBack, useSetting, back);
	}

	void TextEditor_Impl::SetWhitespaceSize(int size) {
		Call(Message::SetWhitespaceSize, size);
	}

	int TextEditor_Impl::WhitespaceSize() {
		return static_cast<int>(Call(Message::GetWhitespaceSize));
	}

	void TextEditor_Impl::SetLineState(Line line, int state) {
		Call(Message::SetLineState, line, state);
	}

	int TextEditor_Impl::LineState(Line line) {
		return static_cast<int>(Call(Message::GetLineState, line));
	}

	int TextEditor_Impl::MaxLineState() {
		return static_cast<int>(Call(Message::GetMaxLineState));
	}

	bool TextEditor_Impl::CaretLineVisible() {
		return Call(Message::GetCaretLineVisible);
	}

	void TextEditor_Impl::SetCaretLineVisible(bool show) {
		Call(Message::SetCaretLineVisible, show);
	}

	Colour TextEditor_Impl::CaretLineBack() {
		return static_cast<Colour>(Call(Message::GetCaretLineBack));
	}

	void TextEditor_Impl::SetCaretLineBack(Colour back) {
		Call(Message::SetCaretLineBack, back);
	}

	int TextEditor_Impl::CaretLineFrame() {
		return static_cast<int>(Call(Message::GetCaretLineFrame));
	}

	void TextEditor_Impl::SetCaretLineFrame(int width) {
		Call(Message::SetCaretLineFrame, width);
	}

	void TextEditor_Impl::StyleSetChangeable(int style, bool changeable) {
		Call(Message::StyleSetChangeable, style, changeable);
	}

	void TextEditor_Impl::AutoCShow(Position lengthEntered, const char* itemList) {
		CallString(Message::AutoCShow, lengthEntered, itemList);
	}

	void TextEditor_Impl::AutoCCancel() {
		Call(Message::AutoCCancel);
	}

	bool TextEditor_Impl::AutoCActive() {
		return Call(Message::AutoCActive);
	}

	Position TextEditor_Impl::AutoCPosStart() {
		return Call(Message::AutoCPosStart);
	}

	void TextEditor_Impl::AutoCComplete() {
		Call(Message::AutoCComplete);
	}

	void TextEditor_Impl::AutoCStops(const char* characterSet) {
		CallString(Message::AutoCStops, 0, characterSet);
	}

	void TextEditor_Impl::AutoCSetSeparator(int separatorCharacter) {
		Call(Message::AutoCSetSeparator, separatorCharacter);
	}

	int TextEditor_Impl::AutoCGetSeparator() {
		return static_cast<int>(Call(Message::AutoCGetSeparator));
	}

	void TextEditor_Impl::AutoCSelect(const char* select) {
		CallString(Message::AutoCSelect, 0, select);
	}

	void TextEditor_Impl::AutoCSetCancelAtStart(bool cancel) {
		Call(Message::AutoCSetCancelAtStart, cancel);
	}

	bool TextEditor_Impl::AutoCGetCancelAtStart() {
		return Call(Message::AutoCGetCancelAtStart);
	}

	void TextEditor_Impl::AutoCSetFillUps(const char* characterSet) {
		CallString(Message::AutoCSetFillUps, 0, characterSet);
	}

	void TextEditor_Impl::AutoCSetChooseSingle(bool chooseSingle) {
		Call(Message::AutoCSetChooseSingle, chooseSingle);
	}

	bool TextEditor_Impl::AutoCGetChooseSingle() {
		return Call(Message::AutoCGetChooseSingle);
	}

	void TextEditor_Impl::AutoCSetIgnoreCase(bool ignoreCase) {
		Call(Message::AutoCSetIgnoreCase, ignoreCase);
	}

	bool TextEditor_Impl::AutoCGetIgnoreCase() {
		return Call(Message::AutoCGetIgnoreCase);
	}

	void TextEditor_Impl::UserListShow(int listType, const char* itemList) {
		CallString(Message::UserListShow, listType, itemList);
	}

	void TextEditor_Impl::AutoCSetAutoHide(bool autoHide) {
		Call(Message::AutoCSetAutoHide, autoHide);
	}

	bool TextEditor_Impl::AutoCGetAutoHide() {
		return Call(Message::AutoCGetAutoHide);
	}

	void TextEditor_Impl::AutoCSetDropRestOfWord(bool dropRestOfWord) {
		Call(Message::AutoCSetDropRestOfWord, dropRestOfWord);
	}

	bool TextEditor_Impl::AutoCGetDropRestOfWord() {
		return Call(Message::AutoCGetDropRestOfWord);
	}

	void TextEditor_Impl::RegisterImage(int type, const char* xpmData) {
		CallString(Message::RegisterImage, type, xpmData);
	}

	void TextEditor_Impl::ClearRegisteredImages() {
		Call(Message::ClearRegisteredImages);
	}

	int TextEditor_Impl::AutoCGetTypeSeparator() {
		return static_cast<int>(Call(Message::AutoCGetTypeSeparator));
	}

	void TextEditor_Impl::AutoCSetTypeSeparator(int separatorCharacter) {
		Call(Message::AutoCSetTypeSeparator, separatorCharacter);
	}

	void TextEditor_Impl::AutoCSetMaxWidth(int characterCount) {
		Call(Message::AutoCSetMaxWidth, characterCount);
	}

	int TextEditor_Impl::AutoCGetMaxWidth() {
		return static_cast<int>(Call(Message::AutoCGetMaxWidth));
	}

	void TextEditor_Impl::AutoCSetMaxHeight(int rowCount) {
		Call(Message::AutoCSetMaxHeight, rowCount);
	}

	int TextEditor_Impl::AutoCGetMaxHeight() {
		return static_cast<int>(Call(Message::AutoCGetMaxHeight));
	}

	void TextEditor_Impl::SetIndent(int indentSize) {
		Call(Message::SetIndent, indentSize);
	}

	int TextEditor_Impl::Indent() {
		return static_cast<int>(Call(Message::GetIndent));
	}

	void TextEditor_Impl::SetUseTabs(bool useTabs) {
		Call(Message::SetUseTabs, useTabs);
	}

	bool TextEditor_Impl::UseTabs() {
		return Call(Message::GetUseTabs);
	}

	void TextEditor_Impl::SetLineIndentation(Line line, int indentation) {
		Call(Message::SetLineIndentation, line, indentation);
	}

	int TextEditor_Impl::LineIndentation(Line line) {
		return static_cast<int>(Call(Message::GetLineIndentation, line));
	}

	Position TextEditor_Impl::LineIndentPosition(Line line) {
		return Call(Message::GetLineIndentPosition, line);
	}

	Position TextEditor_Impl::Column(Position pos) {
		return Call(Message::GetColumn, pos);
	}

	Position TextEditor_Impl::CountCharacters(Position start, Position end) {
		return Call(Message::CountCharacters, start, end);
	}

	Position TextEditor_Impl::CountCodeUnits(Position start, Position end) {
		return Call(Message::CountCodeUnits, start, end);
	}

	void TextEditor_Impl::SetHScrollBar(bool visible) {
		Call(Message::SetHScrollBar, visible);
	}

	bool TextEditor_Impl::HScrollBar() {
		return Call(Message::GetHScrollBar);
	}

	void TextEditor_Impl::SetIndentationGuides(KLWidgets::IndentView indentView) {
		Call(Message::SetIndentationGuides, static_cast<uintptr_t>(indentView));
	}

	IndentView TextEditor_Impl::IndentationGuides() {
		return static_cast<KLWidgets::IndentView>(Call(Message::GetIndentationGuides));
	}

	void TextEditor_Impl::SetHighlightGuide(Position column) {
		Call(Message::SetHighlightGuide, column);
	}

	Position TextEditor_Impl::HighlightGuide() {
		return Call(Message::GetHighlightGuide);
	}

	Position TextEditor_Impl::LineEndPosition(Line line) {
		return Call(Message::GetLineEndPosition, line);
	}

	int TextEditor_Impl::CodePage() {
		return static_cast<int>(Call(Message::GetCodePage));
	}

	Colour TextEditor_Impl::CaretFore() {
		return static_cast<Colour>(Call(Message::GetCaretFore));
	}

	bool TextEditor_Impl::ReadOnly() {
		return Call(Message::GetReadOnly);
	}

	void TextEditor_Impl::SetCurrentPos(Position caret) {
		Call(Message::SetCurrentPos, caret);
	}

	void TextEditor_Impl::SetSelectionStart(Position anchor) {
		Call(Message::SetSelectionStart, anchor);
	}

	Position TextEditor_Impl::SelectionStart() {
		return Call(Message::GetSelectionStart);
	}

	void TextEditor_Impl::SetSelectionEnd(Position caret) {
		Call(Message::SetSelectionEnd, caret);
	}

	Position TextEditor_Impl::SelectionEnd() {
		return Call(Message::GetSelectionEnd);
	}

	void TextEditor_Impl::SetEmptySelection(Position caret) {
		Call(Message::SetEmptySelection, caret);
	}

	void TextEditor_Impl::SetPrintMagnification(int magnification) {
		Call(Message::SetPrintMagnification, magnification);
	}

	int TextEditor_Impl::PrintMagnification() {
		return static_cast<int>(Call(Message::GetPrintMagnification));
	}

	void TextEditor_Impl::SetPrintColourMode(KLWidgets::PrintOption mode) {
		Call(Message::SetPrintColourMode, static_cast<uintptr_t>(mode));
	}

	PrintOption TextEditor_Impl::PrintColourMode() {
		return static_cast<KLWidgets::PrintOption>(Call(Message::GetPrintColourMode));
	}

	Position TextEditor_Impl::FindText(KLWidgets::FindOption searchFlags, void* ft) {
		return CallPointer(Message::FindText, static_cast<uintptr_t>(searchFlags), ft);
	}

	Position TextEditor_Impl::FormatRange(bool draw, void* fr) {
		return CallPointer(Message::FormatRange, draw, fr);
	}

	Line TextEditor_Impl::FirstVisibleLine() {
		return Call(Message::GetFirstVisibleLine);
	}

	Position TextEditor_Impl::GetLine(Line line, char* text) {
		return CallPointer(Message::GetLine, line, text);
	}

	std::string TextEditor_Impl::GetLine(Line line) {
		return CallReturnString(Message::GetLine, line);
	}

	Line TextEditor_Impl::LineCount() {
		return Call(Message::GetLineCount);
	}

	void TextEditor_Impl::SetMarginLeft(int pixelWidth) {
		Call(Message::SetMarginLeft, 0, pixelWidth);
	}

	int TextEditor_Impl::MarginLeft() {
		return static_cast<int>(Call(Message::GetMarginLeft));
	}

	void TextEditor_Impl::SetMarginRight(int pixelWidth) {
		Call(Message::SetMarginRight, 0, pixelWidth);
	}

	int TextEditor_Impl::MarginRight() {
		return static_cast<int>(Call(Message::GetMarginRight));
	}

	bool TextEditor_Impl::Modify() {
		return Call(Message::GetModify);
	}

	void TextEditor_Impl::SetSel(Position anchor, Position caret) {
		Call(Message::SetSel, anchor, caret);
	}

	Position TextEditor_Impl::GetSelText(char* text) {
		return CallPointer(Message::GetSelText, 0, text);
	}

	std::string TextEditor_Impl::GetSelText() {
		return CallReturnString(Message::GetSelText, 0);
	}

	Position TextEditor_Impl::GetTextRange(void* tr) {
		return CallPointer(Message::GetTextRange, 0, tr);
	}

	void TextEditor_Impl::HideSelection(bool hide) {
		Call(Message::HideSelection, hide);
	}

	int TextEditor_Impl::PointXFromPosition(Position pos) {
		return static_cast<int>(Call(Message::PointXFromPosition, 0, pos));
	}

	int TextEditor_Impl::PointYFromPosition(Position pos) {
		return static_cast<int>(Call(Message::PointYFromPosition, 0, pos));
	}

	Line TextEditor_Impl::LineFromPosition(Position pos) {
		return Call(Message::LineFromPosition, pos);
	}

	Position TextEditor_Impl::PositionFromLine(Line line) {
		return Call(Message::PositionFromLine, line);
	}

	void TextEditor_Impl::LineScroll(Position columns, Line lines) {
		Call(Message::LineScroll, columns, lines);
	}

	void TextEditor_Impl::ScrollCaret() {
		Call(Message::ScrollCaret);
	}

	void TextEditor_Impl::ScrollRange(Position secondary, Position primary) {
		Call(Message::ScrollRange, secondary, primary);
	}

	void TextEditor_Impl::ReplaceSel(const char* text) {
		CallString(Message::ReplaceSel, 0, text);
	}

	void TextEditor_Impl::SetReadOnly(bool readOnly) {
		Call(Message::SetReadOnly, readOnly);
	}

	void TextEditor_Impl::Null() {
		Call(Message::Null);
	}

	bool TextEditor_Impl::CanPaste() {
		return Call(Message::CanPaste);
	}

	bool TextEditor_Impl::CanUndo() {
		return Call(Message::CanUndo);
	}

	void TextEditor_Impl::EmptyUndoBuffer() {
		Call(Message::EmptyUndoBuffer);
	}

	void TextEditor_Impl::Undo() {
		Call(Message::Undo);
	}

	void TextEditor_Impl::Cut() {
		Call(Message::Cut);
	}

	void TextEditor_Impl::Copy() {
		Call(Message::Copy);
	}

	void TextEditor_Impl::Paste() {
		Call(Message::Paste);
	}

	void TextEditor_Impl::Clear() {
		Call(Message::Clear);
	}

	void TextEditor_Impl::SetText(const char* text) {
		CallString(Message::SetText, 0, text);
	}

	Position TextEditor_Impl::GetText(Position length, char* text) {
		return CallPointer(Message::GetText, length, text);
	}

	std::string TextEditor_Impl::GetText(Position length) {
		return CallReturnString(Message::GetText, length);
	}

	Position TextEditor_Impl::TextLength() {
		return Call(Message::GetTextLength);
	}

	void* TextEditor_Impl::DirectFunction() {
		return reinterpret_cast<void*>(Call(Message::GetDirectFunction));
	}

	void* TextEditor_Impl::DirectPointer() {
		return reinterpret_cast<void*>(Call(Message::GetDirectPointer));
	}

	void TextEditor_Impl::SetOvertype(bool overType) {
		Call(Message::SetOvertype, overType);
	}

	bool TextEditor_Impl::Overtype() {
		return Call(Message::GetOvertype);
	}

	void TextEditor_Impl::SetCaretWidth(int pixelWidth) {
		Call(Message::SetCaretWidth, pixelWidth);
	}

	int TextEditor_Impl::CaretWidth() {
		return static_cast<int>(Call(Message::GetCaretWidth));
	}

	void TextEditor_Impl::SetTargetStart(Position start) {
		Call(Message::SetTargetStart, start);
	}

	Position TextEditor_Impl::TargetStart() {
		return Call(Message::GetTargetStart);
	}

	void TextEditor_Impl::SetTargetStartVirtualSpace(Position space) {
		Call(Message::SetTargetStartVirtualSpace, space);
	}

	Position TextEditor_Impl::TargetStartVirtualSpace() {
		return Call(Message::GetTargetStartVirtualSpace);
	}

	void TextEditor_Impl::SetTargetEnd(Position end) {
		Call(Message::SetTargetEnd, end);
	}

	Position TextEditor_Impl::TargetEnd() {
		return Call(Message::GetTargetEnd);
	}

	void TextEditor_Impl::SetTargetEndVirtualSpace(Position space) {
		Call(Message::SetTargetEndVirtualSpace, space);
	}

	Position TextEditor_Impl::TargetEndVirtualSpace() {
		return Call(Message::GetTargetEndVirtualSpace);
	}

	void TextEditor_Impl::SetTargetRange(Position start, Position end) {
		Call(Message::SetTargetRange, start, end);
	}

	Position TextEditor_Impl::TargetText(char* text) {
		return CallPointer(Message::GetTargetText, 0, text);
	}

	std::string TextEditor_Impl::TargetText() {
		return CallReturnString(Message::GetTargetText, 0);
	}

	void TextEditor_Impl::TargetFromSelection() {
		Call(Message::TargetFromSelection);
	}

	void TextEditor_Impl::TargetWholeDocument() {
		Call(Message::TargetWholeDocument);
	}

	Position TextEditor_Impl::ReplaceTarget(Position length, const char* text) {
		return CallString(Message::ReplaceTarget, length, text);
	}

	Position TextEditor_Impl::ReplaceTargetRE(Position length, const char* text) {
		return CallString(Message::ReplaceTargetRE, length, text);
	}

	Position TextEditor_Impl::SearchInTarget(Position length, const char* text) {
		return CallString(Message::SearchInTarget, length, text);
	}

	void TextEditor_Impl::SetSearchFlags(KLWidgets::FindOption searchFlags) {
		Call(Message::SetSearchFlags, static_cast<uintptr_t>(searchFlags));
	}

	FindOption TextEditor_Impl::SearchFlags() {
		return static_cast<KLWidgets::FindOption>(Call(Message::GetSearchFlags));
	}

	void TextEditor_Impl::CallTipShow(Position pos, const char* definition) {
		CallString(Message::CallTipShow, pos, definition);
	}

	void TextEditor_Impl::CallTipCancel() {
		Call(Message::CallTipCancel);
	}

	bool TextEditor_Impl::CallTipActive() {
		return Call(Message::CallTipActive);
	}

	Position TextEditor_Impl::CallTipPosStart() {
		return Call(Message::CallTipPosStart);
	}

	void TextEditor_Impl::CallTipSetPosStart(Position posStart) {
		Call(Message::CallTipSetPosStart, posStart);
	}

	void TextEditor_Impl::CallTipSetHlt(Position highlightStart, Position highlightEnd) {
		Call(Message::CallTipSetHlt, highlightStart, highlightEnd);
	}

	void TextEditor_Impl::CallTipSetBack(Colour back) {
		Call(Message::CallTipSetBack, back);
	}

	void TextEditor_Impl::CallTipSetFore(Colour fore) {
		Call(Message::CallTipSetFore, fore);
	}

	void TextEditor_Impl::CallTipSetForeHlt(Colour fore) {
		Call(Message::CallTipSetForeHlt, fore);
	}

	void TextEditor_Impl::CallTipUseStyle(int tabSize) {
		Call(Message::CallTipUseStyle, tabSize);
	}

	void TextEditor_Impl::CallTipSetPosition(bool above) {
		Call(Message::CallTipSetPosition, above);
	}

	Line TextEditor_Impl::VisibleFromDocLine(Line docLine) {
		return Call(Message::VisibleFromDocLine, docLine);
	}

	Line TextEditor_Impl::DocLineFromVisible(Line displayLine) {
		return Call(Message::DocLineFromVisible, displayLine);
	}

	Line TextEditor_Impl::WrapCount(Line docLine) {
		return Call(Message::WrapCount, docLine);
	}

	void TextEditor_Impl::SetFoldLevel(Line line, KLWidgets::FoldLevel level) {
		Call(Message::SetFoldLevel, line, static_cast<intptr_t>(level));
	}

	FoldLevel TextEditor_Impl::FoldLevel(Line line) {
		return static_cast<KLWidgets::FoldLevel>(Call(Message::GetFoldLevel, line));
	}

	Line TextEditor_Impl::LastChild(Line line, KLWidgets::FoldLevel level) {
		return Call(Message::GetLastChild, line, static_cast<intptr_t>(level));
	}

	Line TextEditor_Impl::FoldParent(Line line) {
		return Call(Message::GetFoldParent, line);
	}

	void TextEditor_Impl::ShowLines(Line lineStart, Line lineEnd) {
		Call(Message::ShowLines, lineStart, lineEnd);
	}

	void TextEditor_Impl::HideLines(Line lineStart, Line lineEnd) {
		Call(Message::HideLines, lineStart, lineEnd);
	}

	bool TextEditor_Impl::LineVisible(Line line) {
		return Call(Message::GetLineVisible, line);
	}

	bool TextEditor_Impl::AllLinesVisible() {
		return Call(Message::GetAllLinesVisible);
	}

	void TextEditor_Impl::SetFoldExpanded(Line line, bool expanded) {
		Call(Message::SetFoldExpanded, line, expanded);
	}

	bool TextEditor_Impl::FoldExpanded(Line line) {
		return Call(Message::GetFoldExpanded, line);
	}

	void TextEditor_Impl::ToggleFold(Line line) {
		Call(Message::ToggleFold, line);
	}

	void TextEditor_Impl::ToggleFoldShowText(Line line, const char* text) {
		CallString(Message::ToggleFoldShowText, line, text);
	}

	void TextEditor_Impl::FoldDisplayTextSetStyle(KLWidgets::FoldDisplayTextStyle style) {
		Call(Message::FoldDisplayTextSetStyle, static_cast<uintptr_t>(style));
	}

	FoldDisplayTextStyle TextEditor_Impl::FoldDisplayTextGetStyle() {
		return static_cast<KLWidgets::FoldDisplayTextStyle>(Call(Message::FoldDisplayTextGetStyle));
	}

	void TextEditor_Impl::SetDefaultFoldDisplayText(const char* text) {
		CallString(Message::SetDefaultFoldDisplayText, 0, text);
	}

	int TextEditor_Impl::GetDefaultFoldDisplayText(char* text) {
		return static_cast<int>(CallPointer(Message::GetDefaultFoldDisplayText, 0, text));
	}

	std::string TextEditor_Impl::GetDefaultFoldDisplayText() {
		return CallReturnString(Message::GetDefaultFoldDisplayText, 0);
	}

	void TextEditor_Impl::FoldLine(Line line, KLWidgets::FoldAction action) {
		Call(Message::FoldLine, line, static_cast<intptr_t>(action));
	}

	void TextEditor_Impl::FoldChildren(Line line, KLWidgets::FoldAction action) {
		Call(Message::FoldChildren, line, static_cast<intptr_t>(action));
	}

	void TextEditor_Impl::ExpandChildren(Line line, KLWidgets::FoldLevel level) {
		Call(Message::ExpandChildren, line, static_cast<intptr_t>(level));
	}

	void TextEditor_Impl::FoldAll(KLWidgets::FoldAction action) {
		Call(Message::FoldAll, static_cast<uintptr_t>(action));
	}

	void TextEditor_Impl::EnsureVisible(Line line) {
		Call(Message::EnsureVisible, line);
	}

	void TextEditor_Impl::SetAutomaticFold(KLWidgets::AutomaticFold automaticFold) {
		Call(Message::SetAutomaticFold, static_cast<uintptr_t>(automaticFold));
	}

	AutomaticFold TextEditor_Impl::AutomaticFold() {
		return static_cast<KLWidgets::AutomaticFold>(Call(Message::GetAutomaticFold));
	}

	void TextEditor_Impl::SetFoldFlags(KLWidgets::FoldFlag flags) {
		Call(Message::SetFoldFlags, static_cast<uintptr_t>(flags));
	}

	void TextEditor_Impl::EnsureVisibleEnforcePolicy(Line line) {
		Call(Message::EnsureVisibleEnforcePolicy, line);
	}

	void TextEditor_Impl::SetTabIndents(bool tabIndents) {
		Call(Message::SetTabIndents, tabIndents);
	}

	bool TextEditor_Impl::TabIndents() {
		return Call(Message::GetTabIndents);
	}

	void TextEditor_Impl::SetBackSpaceUnIndents(bool bsUnIndents) {
		Call(Message::SetBackSpaceUnIndents, bsUnIndents);
	}

	bool TextEditor_Impl::BackSpaceUnIndents() {
		return Call(Message::GetBackSpaceUnIndents);
	}

	void TextEditor_Impl::SetMouseDwellTime(int periodMilliseconds) {
		Call(Message::SetMouseDwellTime, periodMilliseconds);
	}

	int TextEditor_Impl::MouseDwellTime() {
		return static_cast<int>(Call(Message::GetMouseDwellTime));
	}

	Position TextEditor_Impl::WordStartPosition(Position pos, bool onlyWordCharacters) {
		return Call(Message::WordStartPosition, pos, onlyWordCharacters);
	}

	Position TextEditor_Impl::WordEndPosition(Position pos, bool onlyWordCharacters) {
		return Call(Message::WordEndPosition, pos, onlyWordCharacters);
	}

	bool TextEditor_Impl::IsRangeWord(Position start, Position end) {
		return Call(Message::IsRangeWord, start, end);
	}

	void TextEditor_Impl::SetIdleStyling(KLWidgets::IdleStyling idleStyling) {
		Call(Message::SetIdleStyling, static_cast<uintptr_t>(idleStyling));
	}

	IdleStyling TextEditor_Impl::IdleStyling() {
		return static_cast<KLWidgets::IdleStyling>(Call(Message::GetIdleStyling));
	}

	void TextEditor_Impl::SetWrapMode(KLWidgets::Wrap wrapMode) {
		Call(Message::SetWrapMode, static_cast<uintptr_t>(wrapMode));
	}

	Wrap TextEditor_Impl::WrapMode() {
		return static_cast<KLWidgets::Wrap>(Call(Message::GetWrapMode));
	}

	void TextEditor_Impl::SetWrapVisualFlags(KLWidgets::WrapVisualFlag wrapVisualFlags) {
		Call(Message::SetWrapVisualFlags, static_cast<uintptr_t>(wrapVisualFlags));
	}

	WrapVisualFlag TextEditor_Impl::WrapVisualFlags() {
		return static_cast<KLWidgets::WrapVisualFlag>(Call(Message::GetWrapVisualFlags));
	}

	void TextEditor_Impl::SetWrapVisualFlagsLocation(KLWidgets::WrapVisualLocation wrapVisualFlagsLocation) {
		Call(Message::SetWrapVisualFlagsLocation, static_cast<uintptr_t>(wrapVisualFlagsLocation));
	}

	WrapVisualLocation TextEditor_Impl::WrapVisualFlagsLocation() {
		return static_cast<KLWidgets::WrapVisualLocation>(Call(Message::GetWrapVisualFlagsLocation));
	}

	void TextEditor_Impl::SetWrapStartIndent(int indent) {
		Call(Message::SetWrapStartIndent, indent);
	}

	int TextEditor_Impl::WrapStartIndent() {
		return static_cast<int>(Call(Message::GetWrapStartIndent));
	}

	void TextEditor_Impl::SetWrapIndentMode(KLWidgets::WrapIndentMode wrapIndentMode) {
		Call(Message::SetWrapIndentMode, static_cast<uintptr_t>(wrapIndentMode));
	}

	WrapIndentMode TextEditor_Impl::WrapIndentMode() {
		return static_cast<KLWidgets::WrapIndentMode>(Call(Message::GetWrapIndentMode));
	}

	void TextEditor_Impl::SetLayoutCache(KLWidgets::LineCache cacheMode) {
		Call(Message::SetLayoutCache, static_cast<uintptr_t>(cacheMode));
	}

	LineCache TextEditor_Impl::LayoutCache() {
		return static_cast<KLWidgets::LineCache>(Call(Message::GetLayoutCache));
	}

	void TextEditor_Impl::SetScrollWidth(int pixelWidth) {
		Call(Message::SetScrollWidth, pixelWidth);
	}

	int TextEditor_Impl::ScrollWidth() {
		return static_cast<int>(Call(Message::GetScrollWidth));
	}

	void TextEditor_Impl::SetScrollWidthTracking(bool tracking) {
		Call(Message::SetScrollWidthTracking, tracking);
	}

	bool TextEditor_Impl::ScrollWidthTracking() {
		return Call(Message::GetScrollWidthTracking);
	}

	int TextEditor_Impl::TextWidth(int style, const char* text) {
		return static_cast<int>(CallString(Message::TextWidth, style, text));
	}

	void TextEditor_Impl::SetEndAtLastLine(bool endAtLastLine) {
		Call(Message::SetEndAtLastLine, endAtLastLine);
	}

	bool TextEditor_Impl::EndAtLastLine() {
		return Call(Message::GetEndAtLastLine);
	}

	int TextEditor_Impl::TextHeight(Line line) {
		return static_cast<int>(Call(Message::TextHeight, line));
	}

	void TextEditor_Impl::SetVScrollBar(bool visible) {
		Call(Message::SetVScrollBar, visible);
	}

	bool TextEditor_Impl::VScrollBar() {
		return Call(Message::GetVScrollBar);
	}

	void TextEditor_Impl::AppendText(Position length, const char* text) {
		CallString(Message::AppendText, length, text);
	}

	PhasesDraw TextEditor_Impl::PhasesDraw() {
		return static_cast<KLWidgets::PhasesDraw>(Call(Message::GetPhasesDraw));
	}

	void TextEditor_Impl::SetPhasesDraw(KLWidgets::PhasesDraw phases) {
		Call(Message::SetPhasesDraw, static_cast<uintptr_t>(phases));
	}

	void TextEditor_Impl::SetFontQuality(KLWidgets::FontQuality fontQuality) {
		Call(Message::SetFontQuality, static_cast<uintptr_t>(fontQuality));
	}

	FontQuality TextEditor_Impl::FontQuality() {
		return static_cast<KLWidgets::FontQuality>(Call(Message::GetFontQuality));
	}

	void TextEditor_Impl::SetFirstVisibleLine(Line displayLine) {
		Call(Message::SetFirstVisibleLine, displayLine);
	}

	void TextEditor_Impl::SetMultiPaste(KLWidgets::MultiPaste multiPaste) {
		Call(Message::SetMultiPaste, static_cast<uintptr_t>(multiPaste));
	}

	MultiPaste TextEditor_Impl::MultiPaste() {
		return static_cast<KLWidgets::MultiPaste>(Call(Message::GetMultiPaste));
	}

	int TextEditor_Impl::Tag(int tagNumber, char* tagValue) {
		return static_cast<int>(CallPointer(Message::GetTag, tagNumber, tagValue));
	}

	std::string TextEditor_Impl::Tag(int tagNumber) {
		return CallReturnString(Message::GetTag, tagNumber);
	}

	void TextEditor_Impl::LinesJoin() {
		Call(Message::LinesJoin);
	}

	void TextEditor_Impl::LinesSplit(int pixelWidth) {
		Call(Message::LinesSplit, pixelWidth);
	}

	void TextEditor_Impl::SetFoldMarginColour(bool useSetting, Colour back) {
		Call(Message::SetFoldMarginColour, useSetting, back);
	}

	void TextEditor_Impl::SetFoldMarginHiColour(bool useSetting, Colour fore) {
		Call(Message::SetFoldMarginHiColour, useSetting, fore);
	}

	void TextEditor_Impl::SetAccessibility(KLWidgets::Accessibility accessibility) {
		Call(Message::SetAccessibility, static_cast<uintptr_t>(accessibility));
	}

	Accessibility TextEditor_Impl::Accessibility() {
		return static_cast<KLWidgets::Accessibility>(Call(Message::GetAccessibility));
	}

	void TextEditor_Impl::LineDown() {
		Call(Message::LineDown);
	}

	void TextEditor_Impl::LineDownExtend() {
		Call(Message::LineDownExtend);
	}

	void TextEditor_Impl::LineUp() {
		Call(Message::LineUp);
	}

	void TextEditor_Impl::LineUpExtend() {
		Call(Message::LineUpExtend);
	}

	void TextEditor_Impl::CharLeft() {
		Call(Message::CharLeft);
	}

	void TextEditor_Impl::CharLeftExtend() {
		Call(Message::CharLeftExtend);
	}

	void TextEditor_Impl::CharRight() {
		Call(Message::CharRight);
	}

	void TextEditor_Impl::CharRightExtend() {
		Call(Message::CharRightExtend);
	}

	void TextEditor_Impl::WordLeft() {
		Call(Message::WordLeft);
	}

	void TextEditor_Impl::WordLeftExtend() {
		Call(Message::WordLeftExtend);
	}

	void TextEditor_Impl::WordRight() {
		Call(Message::WordRight);
	}

	void TextEditor_Impl::WordRightExtend() {
		Call(Message::WordRightExtend);
	}

	void TextEditor_Impl::Home() {
		Call(Message::Home);
	}

	void TextEditor_Impl::HomeExtend() {
		Call(Message::HomeExtend);
	}

	void TextEditor_Impl::LineEnd() {
		Call(Message::LineEnd);
	}

	void TextEditor_Impl::LineEndExtend() {
		Call(Message::LineEndExtend);
	}

	void TextEditor_Impl::DocumentStart() {
		Call(Message::DocumentStart);
	}

	void TextEditor_Impl::DocumentStartExtend() {
		Call(Message::DocumentStartExtend);
	}

	void TextEditor_Impl::DocumentEnd() {
		Call(Message::DocumentEnd);
	}

	void TextEditor_Impl::DocumentEndExtend() {
		Call(Message::DocumentEndExtend);
	}

	void TextEditor_Impl::PageUp() {
		Call(Message::PageUp);
	}

	void TextEditor_Impl::PageUpExtend() {
		Call(Message::PageUpExtend);
	}

	void TextEditor_Impl::PageDown() {
		Call(Message::PageDown);
	}

	void TextEditor_Impl::PageDownExtend() {
		Call(Message::PageDownExtend);
	}

	void TextEditor_Impl::EditToggleOvertype() {
		Call(Message::EditToggleOvertype);
	}

	void TextEditor_Impl::CancelN() {
		Call(Message::Cancel);
	}

	void TextEditor_Impl::DeleteBack() {
		Call(Message::DeleteBack);
	}

	void TextEditor_Impl::Tab() {
		Call(Message::Tab);
	}

	void TextEditor_Impl::BackTab() {
		Call(Message::BackTab);
	}

	void TextEditor_Impl::NewLine() {
		Call(Message::NewLine);
	}

	void TextEditor_Impl::FormFeed() {
		Call(Message::FormFeed);
	}

	void TextEditor_Impl::VCHome() {
		Call(Message::VCHome);
	}

	void TextEditor_Impl::VCHomeExtend() {
		Call(Message::VCHomeExtend);
	}

	void TextEditor_Impl::ZoomIn() {
		Call(Message::ZoomIn);
	}

	void TextEditor_Impl::ZoomOut() {
		Call(Message::ZoomOut);
	}

	void TextEditor_Impl::DelWordLeft() {
		Call(Message::DelWordLeft);
	}

	void TextEditor_Impl::DelWordRight() {
		Call(Message::DelWordRight);
	}

	void TextEditor_Impl::DelWordRightEnd() {
		Call(Message::DelWordRightEnd);
	}

	void TextEditor_Impl::LineCut() {
		Call(Message::LineCut);
	}

	void TextEditor_Impl::LineDelete() {
		Call(Message::LineDelete);
	}

	void TextEditor_Impl::LineTranspose() {
		Call(Message::LineTranspose);
	}

	void TextEditor_Impl::LineReverse() {
		Call(Message::LineReverse);
	}

	void TextEditor_Impl::LineDuplicate() {
		Call(Message::LineDuplicate);
	}

	void TextEditor_Impl::LowerCase() {
		Call(Message::LowerCase);
	}

	void TextEditor_Impl::UpperCase() {
		Call(Message::UpperCase);
	}

	void TextEditor_Impl::LineScrollDown() {
		Call(Message::LineScrollDown);
	}

	void TextEditor_Impl::LineScrollUp() {
		Call(Message::LineScrollUp);
	}

	void TextEditor_Impl::DeleteBackNotLine() {
		Call(Message::DeleteBackNotLine);
	}

	void TextEditor_Impl::HomeDisplay() {
		Call(Message::HomeDisplay);
	}

	void TextEditor_Impl::HomeDisplayExtend() {
		Call(Message::HomeDisplayExtend);
	}

	void TextEditor_Impl::LineEndDisplay() {
		Call(Message::LineEndDisplay);
	}

	void TextEditor_Impl::LineEndDisplayExtend() {
		Call(Message::LineEndDisplayExtend);
	}

	void TextEditor_Impl::HomeWrap() {
		Call(Message::HomeWrap);
	}

	void TextEditor_Impl::HomeWrapExtend() {
		Call(Message::HomeWrapExtend);
	}

	void TextEditor_Impl::LineEndWrap() {
		Call(Message::LineEndWrap);
	}

	void TextEditor_Impl::LineEndWrapExtend() {
		Call(Message::LineEndWrapExtend);
	}

	void TextEditor_Impl::VCHomeWrap() {
		Call(Message::VCHomeWrap);
	}

	void TextEditor_Impl::VCHomeWrapExtend() {
		Call(Message::VCHomeWrapExtend);
	}

	void TextEditor_Impl::LineCopy() {
		Call(Message::LineCopy);
	}

	void TextEditor_Impl::MoveCaretInsideView() {
		Call(Message::MoveCaretInsideView);
	}

	Position TextEditor_Impl::LineLength(Line line) {
		return Call(Message::LineLength, line);
	}

	void TextEditor_Impl::BraceHighlight(Position posA, Position posB) {
		Call(Message::BraceHighlight, posA, posB);
	}

	void TextEditor_Impl::BraceHighlightIndicator(bool useSetting, int indicator) {
		Call(Message::BraceHighlightIndicator, useSetting, indicator);
	}

	void TextEditor_Impl::BraceBadLight(Position pos) {
		Call(Message::BraceBadLight, pos);
	}

	void TextEditor_Impl::BraceBadLightIndicator(bool useSetting, int indicator) {
		Call(Message::BraceBadLightIndicator, useSetting, indicator);
	}

	Position TextEditor_Impl::BraceMatch(Position pos, int maxReStyle) {
		return Call(Message::BraceMatch, pos, maxReStyle);
	}

	Position TextEditor_Impl::BraceMatchNext(Position pos, Position startPos) {
		return Call(Message::BraceMatchNext, pos, startPos);
	}

	bool TextEditor_Impl::ViewEOL() {
		return Call(Message::GetViewEOL);
	}

	void TextEditor_Impl::SetViewEOL(bool visible) {
		Call(Message::SetViewEOL, visible);
	}

	void* TextEditor_Impl::DocPointer() {
		return reinterpret_cast<void*>(Call(Message::GetDocPointer));
	}

	void TextEditor_Impl::SetDocPointer(void* doc) {
		CallPointer(Message::SetDocPointer, 0, doc);
	}

	void TextEditor_Impl::SetModEventMask(KLWidgets::ModificationFlags eventMask) {
		Call(Message::SetModEventMask, static_cast<uintptr_t>(eventMask));
	}

	Position TextEditor_Impl::EdgeColumn() {
		return Call(Message::GetEdgeColumn);
	}

	void TextEditor_Impl::SetEdgeColumn(Position column) {
		Call(Message::SetEdgeColumn, column);
	}

	EdgeVisualStyle TextEditor_Impl::EdgeMode() {
		return static_cast<KLWidgets::EdgeVisualStyle>(Call(Message::GetEdgeMode));
	}

	void TextEditor_Impl::SetEdgeMode(KLWidgets::EdgeVisualStyle edgeMode) {
		Call(Message::SetEdgeMode, static_cast<uintptr_t>(edgeMode));
	}

	Colour TextEditor_Impl::EdgeColour() {
		return static_cast<Colour>(Call(Message::GetEdgeColour));
	}

	void TextEditor_Impl::SetEdgeColour(Colour edgeColour) {
		Call(Message::SetEdgeColour, edgeColour);
	}

	void TextEditor_Impl::MultiEdgeAddLine(Position column, Colour edgeColour) {
		Call(Message::MultiEdgeAddLine, column, edgeColour);
	}

	void TextEditor_Impl::MultiEdgeClearAll() {
		Call(Message::MultiEdgeClearAll);
	}

	Position TextEditor_Impl::MultiEdgeColumn(int which) {
		return Call(Message::GetMultiEdgeColumn, which);
	}

	void TextEditor_Impl::SearchAnchor() {
		Call(Message::SearchAnchor);
	}

	Position TextEditor_Impl::SearchNext(KLWidgets::FindOption searchFlags, const char* text) {
		return CallString(Message::SearchNext, static_cast<uintptr_t>(searchFlags), text);
	}

	Position TextEditor_Impl::SearchPrev(KLWidgets::FindOption searchFlags, const char* text) {
		return CallString(Message::SearchPrev, static_cast<uintptr_t>(searchFlags), text);
	}

	Line TextEditor_Impl::LinesOnScreen() {
		return Call(Message::LinesOnScreen);
	}

	void TextEditor_Impl::UsePopUp(KLWidgets::PopUp popUpMode) {
		Call(Message::UsePopUp, static_cast<uintptr_t>(popUpMode));
	}

	bool TextEditor_Impl::SelectionIsRectangle() {
		return Call(Message::SelectionIsRectangle);
	}

	void TextEditor_Impl::SetZoom(int zoomInPoints) {
		Call(Message::SetZoom, zoomInPoints);
	}

	int TextEditor_Impl::Zoom() {
		return static_cast<int>(Call(Message::GetZoom));
	}

	void* TextEditor_Impl::CreateDocument(Position bytes, KLWidgets::DocumentOption documentOptions) {
		return reinterpret_cast<void*>(Call(Message::CreateDocument, bytes, static_cast<intptr_t>(documentOptions)));
	}

	void TextEditor_Impl::AddRefDocument(void* doc) {
		CallPointer(Message::AddRefDocument, 0, doc);
	}

	void TextEditor_Impl::ReleaseDocument(void* doc) {
		CallPointer(Message::ReleaseDocument, 0, doc);
	}

	DocumentOption TextEditor_Impl::DocumentOptions() {
		return static_cast<KLWidgets::DocumentOption>(Call(Message::GetDocumentOptions));
	}

	ModificationFlags TextEditor_Impl::ModEventMask() {
		return static_cast<KLWidgets::ModificationFlags>(Call(Message::GetModEventMask));
	}

	void TextEditor_Impl::SetCommandEvents(bool commandEvents) {
		Call(Message::SetCommandEvents, commandEvents);
	}

	bool TextEditor_Impl::CommandEvents() {
		return Call(Message::GetCommandEvents);
	}

	void TextEditor_Impl::SetFocus(bool focus) {
		Call(Message::SetFocus, focus);
	}

	bool TextEditor_Impl::Focus() {
		return Call(Message::GetFocus);
	}

	void TextEditor_Impl::SetStatus(KLWidgets::Status status) {
		Call(Message::SetStatus, static_cast<uintptr_t>(status));
	}

	Status TextEditor_Impl::Status() {
		return static_cast<KLWidgets::Status>(Call(Message::GetStatus));
	}

	void TextEditor_Impl::SetMouseDownCaptures(bool captures) {
		Call(Message::SetMouseDownCaptures, captures);
	}

	bool TextEditor_Impl::MouseDownCaptures() {
		return Call(Message::GetMouseDownCaptures);
	}

	void TextEditor_Impl::SetMouseWheelCaptures(bool captures) {
		Call(Message::SetMouseWheelCaptures, captures);
	}

	bool TextEditor_Impl::MouseWheelCaptures() {
		return Call(Message::GetMouseWheelCaptures);
	}

	void TextEditor_Impl::SetCursor(KLWidgets::CursorShape cursorType) {
		Call(Message::SetCursor, static_cast<uintptr_t>(cursorType));
	}

	CursorShape TextEditor_Impl::Cursor() {
		return static_cast<KLWidgets::CursorShape>(Call(Message::GetCursor));
	}

	void TextEditor_Impl::SetControlCharSymbol(int symbol) {
		Call(Message::SetControlCharSymbol, symbol);
	}

	int TextEditor_Impl::ControlCharSymbol() {
		return static_cast<int>(Call(Message::GetControlCharSymbol));
	}

	void TextEditor_Impl::WordPartLeft() {
		Call(Message::WordPartLeft);
	}

	void TextEditor_Impl::WordPartLeftExtend() {
		Call(Message::WordPartLeftExtend);
	}

	void TextEditor_Impl::WordPartRight() {
		Call(Message::WordPartRight);
	}

	void TextEditor_Impl::WordPartRightExtend() {
		Call(Message::WordPartRightExtend);
	}

	void TextEditor_Impl::SetVisiblePolicy(KLWidgets::VisiblePolicy visiblePolicy, int visibleSlop) {
		Call(Message::SetVisiblePolicy, static_cast<uintptr_t>(visiblePolicy), visibleSlop);
	}

	void TextEditor_Impl::DelLineLeft() {
		Call(Message::DelLineLeft);
	}

	void TextEditor_Impl::DelLineRight() {
		Call(Message::DelLineRight);
	}

	void TextEditor_Impl::SetXOffset(int xOffset) {
		Call(Message::SetXOffset, xOffset);
	}

	int TextEditor_Impl::XOffset() {
		return static_cast<int>(Call(Message::GetXOffset));
	}

	void TextEditor_Impl::ChooseCaretX() {
		Call(Message::ChooseCaretX);
	}

	void TextEditor_Impl::GrabFocus() {
		Call(Message::GrabFocus);
	}

	void TextEditor_Impl::SetXCaretPolicy(KLWidgets::CaretPolicy caretPolicy, int caretSlop) {
		Call(Message::SetXCaretPolicy, static_cast<uintptr_t>(caretPolicy), caretSlop);
	}

	void TextEditor_Impl::SetYCaretPolicy(KLWidgets::CaretPolicy caretPolicy, int caretSlop) {
		Call(Message::SetYCaretPolicy, static_cast<uintptr_t>(caretPolicy), caretSlop);
	}

	void TextEditor_Impl::SetPrintWrapMode(KLWidgets::Wrap wrapMode) {
		Call(Message::SetPrintWrapMode, static_cast<uintptr_t>(wrapMode));
	}

	Wrap TextEditor_Impl::PrintWrapMode() {
		return static_cast<KLWidgets::Wrap>(Call(Message::GetPrintWrapMode));
	}

	void TextEditor_Impl::SetHotspotActiveFore(bool useSetting, Colour fore) {
		Call(Message::SetHotspotActiveFore, useSetting, fore);
	}

	Colour TextEditor_Impl::HotspotActiveFore() {
		return static_cast<Colour>(Call(Message::GetHotspotActiveFore));
	}

	void TextEditor_Impl::SetHotspotActiveBack(bool useSetting, Colour back) {
		Call(Message::SetHotspotActiveBack, useSetting, back);
	}

	Colour TextEditor_Impl::HotspotActiveBack() {
		return static_cast<Colour>(Call(Message::GetHotspotActiveBack));
	}

	void TextEditor_Impl::SetHotspotActiveUnderline(bool underline) {
		Call(Message::SetHotspotActiveUnderline, underline);
	}

	bool TextEditor_Impl::HotspotActiveUnderline() {
		return Call(Message::GetHotspotActiveUnderline);
	}

	void TextEditor_Impl::SetHotspotSingleLine(bool singleLine) {
		Call(Message::SetHotspotSingleLine, singleLine);
	}

	bool TextEditor_Impl::HotspotSingleLine() {
		return Call(Message::GetHotspotSingleLine);
	}

	void TextEditor_Impl::ParaDown() {
		Call(Message::ParaDown);
	}

	void TextEditor_Impl::ParaDownExtend() {
		Call(Message::ParaDownExtend);
	}

	void TextEditor_Impl::ParaUp() {
		Call(Message::ParaUp);
	}

	void TextEditor_Impl::ParaUpExtend() {
		Call(Message::ParaUpExtend);
	}

	Position TextEditor_Impl::PositionBefore(Position pos) {
		return Call(Message::PositionBefore, pos);
	}

	Position TextEditor_Impl::PositionAfter(Position pos) {
		return Call(Message::PositionAfter, pos);
	}

	Position TextEditor_Impl::PositionRelative(Position pos, Position relative) {
		return Call(Message::PositionRelative, pos, relative);
	}

	Position TextEditor_Impl::PositionRelativeCodeUnits(Position pos, Position relative) {
		return Call(Message::PositionRelativeCodeUnits, pos, relative);
	}

	void TextEditor_Impl::CopyRange(Position start, Position end) {
		Call(Message::CopyRange, start, end);
	}

	void TextEditor_Impl::CopyText(Position length, const char* text) {
		CallString(Message::CopyText, length, text);
	}

	void TextEditor_Impl::SetSelectionMode(KLWidgets::SelectionMode selectionMode) {
		Call(Message::SetSelectionMode, static_cast<uintptr_t>(selectionMode));
	}

	SelectionMode TextEditor_Impl::SelectionMode() {
		return static_cast<KLWidgets::SelectionMode>(Call(Message::GetSelectionMode));
	}

	bool TextEditor_Impl::MoveExtendsSelection() {
		return Call(Message::GetMoveExtendsSelection);
	}

	Position TextEditor_Impl::GetLineSelStartPosition(Line line) {
		return Call(Message::GetLineSelStartPosition, line);
	}

	Position TextEditor_Impl::GetLineSelEndPosition(Line line) {
		return Call(Message::GetLineSelEndPosition, line);
	}

	void TextEditor_Impl::LineDownRectExtend() {
		Call(Message::LineDownRectExtend);
	}

	void TextEditor_Impl::LineUpRectExtend() {
		Call(Message::LineUpRectExtend);
	}

	void TextEditor_Impl::CharLeftRectExtend() {
		Call(Message::CharLeftRectExtend);
	}

	void TextEditor_Impl::CharRightRectExtend() {
		Call(Message::CharRightRectExtend);
	}

	void TextEditor_Impl::HomeRectExtend() {
		Call(Message::HomeRectExtend);
	}

	void TextEditor_Impl::VCHomeRectExtend() {
		Call(Message::VCHomeRectExtend);
	}

	void TextEditor_Impl::LineEndRectExtend() {
		Call(Message::LineEndRectExtend);
	}

	void TextEditor_Impl::PageUpRectExtend() {
		Call(Message::PageUpRectExtend);
	}

	void TextEditor_Impl::PageDownRectExtend() {
		Call(Message::PageDownRectExtend);
	}

	void TextEditor_Impl::StutteredPageUp() {
		Call(Message::StutteredPageUp);
	}

	void TextEditor_Impl::StutteredPageUpExtend() {
		Call(Message::StutteredPageUpExtend);
	}

	void TextEditor_Impl::StutteredPageDown() {
		Call(Message::StutteredPageDown);
	}

	void TextEditor_Impl::StutteredPageDownExtend() {
		Call(Message::StutteredPageDownExtend);
	}

	void TextEditor_Impl::WordLeftEnd() {
		Call(Message::WordLeftEnd);
	}

	void TextEditor_Impl::WordLeftEndExtend() {
		Call(Message::WordLeftEndExtend);
	}

	void TextEditor_Impl::WordRightEnd() {
		Call(Message::WordRightEnd);
	}

	void TextEditor_Impl::WordRightEndExtend() {
		Call(Message::WordRightEndExtend);
	}

	void TextEditor_Impl::SetWhitespaceChars(const char* characters) {
		CallString(Message::SetWhitespaceChars, 0, characters);
	}

	int TextEditor_Impl::WhitespaceChars(char* characters) {
		return static_cast<int>(CallPointer(Message::GetWhitespaceChars, 0, characters));
	}

	std::string TextEditor_Impl::WhitespaceChars() {
		return CallReturnString(Message::GetWhitespaceChars, 0);
	}

	void TextEditor_Impl::SetPunctuationChars(const char* characters) {
		CallString(Message::SetPunctuationChars, 0, characters);
	}

	int TextEditor_Impl::PunctuationChars(char* characters) {
		return static_cast<int>(CallPointer(Message::GetPunctuationChars, 0, characters));
	}

	std::string TextEditor_Impl::PunctuationChars() {
		return CallReturnString(Message::GetPunctuationChars, 0);
	}

	void TextEditor_Impl::SetCharsDefault() {
		Call(Message::SetCharsDefault);
	}

	int TextEditor_Impl::AutoCGetCurrent() {
		return static_cast<int>(Call(Message::AutoCGetCurrent));
	}

	int TextEditor_Impl::AutoCGetCurrentText(char* text) {
		return static_cast<int>(CallPointer(Message::AutoCGetCurrentText, 0, text));
	}

	std::string TextEditor_Impl::AutoCGetCurrentText() {
		return CallReturnString(Message::AutoCGetCurrentText, 0);
	}

	void TextEditor_Impl::AutoCSetCaseInsensitiveBehaviour(KLWidgets::CaseInsensitiveBehaviour behaviour) {
		Call(Message::AutoCSetCaseInsensitiveBehaviour, static_cast<uintptr_t>(behaviour));
	}

	CaseInsensitiveBehaviour TextEditor_Impl::AutoCGetCaseInsensitiveBehaviour() {
		return static_cast<KLWidgets::CaseInsensitiveBehaviour>(Call(Message::AutoCGetCaseInsensitiveBehaviour));
	}

	void TextEditor_Impl::AutoCSetMulti(KLWidgets::MultiAutoComplete multi) {
		Call(Message::AutoCSetMulti, static_cast<uintptr_t>(multi));
	}

	MultiAutoComplete TextEditor_Impl::AutoCGetMulti() {
		return static_cast<KLWidgets::MultiAutoComplete>(Call(Message::AutoCGetMulti));
	}

	void TextEditor_Impl::AutoCSetOrder(KLWidgets::Ordering order) {
		Call(Message::AutoCSetOrder, static_cast<uintptr_t>(order));
	}

	Ordering TextEditor_Impl::AutoCGetOrder() {
		return static_cast<KLWidgets::Ordering>(Call(Message::AutoCGetOrder));
	}

	void TextEditor_Impl::Allocate(Position bytes) {
		Call(Message::Allocate, bytes);
	}

	Position TextEditor_Impl::TargetAsUTF8(char* s) {
		return CallPointer(Message::TargetAsUTF8, 0, s);
	}

	std::string TextEditor_Impl::TargetAsUTF8() {
		return CallReturnString(Message::TargetAsUTF8, 0);
	}

	void TextEditor_Impl::SetLengthForEncode(Position bytes) {
		Call(Message::SetLengthForEncode, bytes);
	}

	Position TextEditor_Impl::EncodedFromUTF8(const char* utf8, char* encoded) {
		return CallPointer(Message::EncodedFromUTF8, reinterpret_cast<uintptr_t>(utf8), encoded);
	}

	std::string TextEditor_Impl::EncodedFromUTF8(const char* utf8) {
		return CallReturnString(Message::EncodedFromUTF8, reinterpret_cast<uintptr_t>(utf8));
	}

	Position TextEditor_Impl::FindColumn(Line line, Position column) {
		return Call(Message::FindColumn, line, column);
	}

	CaretSticky TextEditor_Impl::CaretSticky() {
		return static_cast<KLWidgets::CaretSticky>(Call(Message::GetCaretSticky));
	}

	void TextEditor_Impl::SetCaretSticky(KLWidgets::CaretSticky useCaretStickyBehaviour) {
		Call(Message::SetCaretSticky, static_cast<uintptr_t>(useCaretStickyBehaviour));
	}

	void TextEditor_Impl::ToggleCaretSticky() {
		Call(Message::ToggleCaretSticky);
	}

	void TextEditor_Impl::SetPasteConvertEndings(bool convert) {
		Call(Message::SetPasteConvertEndings, convert);
	}

	bool TextEditor_Impl::PasteConvertEndings() {
		return Call(Message::GetPasteConvertEndings);
	}

	void TextEditor_Impl::SelectionDuplicate() {
		Call(Message::SelectionDuplicate);
	}

	void TextEditor_Impl::SetCaretLineBackAlpha(KLWidgets::Alpha alpha) {
		Call(Message::SetCaretLineBackAlpha, static_cast<uintptr_t>(alpha));
	}

	KLWidgets::Alpha TextEditor_Impl::CaretLineBackAlpha() {
		return static_cast<KLWidgets::Alpha>(Call(Message::GetCaretLineBackAlpha));
	}

	void TextEditor_Impl::SetCaretStyle(KLWidgets::CaretStyle caretStyle) {
		Call(Message::SetCaretStyle, static_cast<uintptr_t>(caretStyle));
	}

	CaretStyle TextEditor_Impl::CaretStyle() {
		return static_cast<KLWidgets::CaretStyle>(Call(Message::GetCaretStyle));
	}

	void TextEditor_Impl::SetIndicatorCurrent(int indicator) {
		Call(Message::SetIndicatorCurrent, indicator);
	}

	int TextEditor_Impl::IndicatorCurrent() {
		return static_cast<int>(Call(Message::GetIndicatorCurrent));
	}

	void TextEditor_Impl::SetIndicatorValue(int value) {
		Call(Message::SetIndicatorValue, value);
	}

	int TextEditor_Impl::IndicatorValue() {
		return static_cast<int>(Call(Message::GetIndicatorValue));
	}

	void TextEditor_Impl::IndicatorFillRange(Position start, Position lengthFill) {
		Call(Message::IndicatorFillRange, start, lengthFill);
	}

	void TextEditor_Impl::IndicatorClearRange(Position start, Position lengthClear) {
		Call(Message::IndicatorClearRange, start, lengthClear);
	}

	int TextEditor_Impl::IndicatorAllOnFor(Position pos) {
		return static_cast<int>(Call(Message::IndicatorAllOnFor, pos));
	}

	int TextEditor_Impl::IndicatorValueAt(int indicator, Position pos) {
		return static_cast<int>(Call(Message::IndicatorValueAt, indicator, pos));
	}

	Position TextEditor_Impl::IndicatorStart(int indicator, Position pos) {
		return Call(Message::IndicatorStart, indicator, pos);
	}

	Position TextEditor_Impl::IndicatorEnd(int indicator, Position pos) {
		return Call(Message::IndicatorEnd, indicator, pos);
	}

	void TextEditor_Impl::SetPositionCache(int size) {
		Call(Message::SetPositionCache, size);
	}

	int TextEditor_Impl::PositionCache() {
		return static_cast<int>(Call(Message::GetPositionCache));
	}

	void TextEditor_Impl::CopyAllowLine() {
		Call(Message::CopyAllowLine);
	}

	void* TextEditor_Impl::CharacterPointer() {
		return reinterpret_cast<void*>(Call(Message::GetCharacterPointer));
	}

	void* TextEditor_Impl::RangePointer(Position start, Position lengthRange) {
		return reinterpret_cast<void*>(Call(Message::GetRangePointer, start, lengthRange));
	}

	Position TextEditor_Impl::GapPosition() {
		return Call(Message::GetGapPosition);
	}

	void TextEditor_Impl::IndicSetAlpha(int indicator, KLWidgets::Alpha alpha) {
		Call(Message::IndicSetAlpha, indicator, static_cast<intptr_t>(alpha));
	}

	KLWidgets::Alpha TextEditor_Impl::IndicGetAlpha(int indicator) {
		return static_cast<KLWidgets::Alpha>(Call(Message::IndicGetAlpha, indicator));
	}

	void TextEditor_Impl::IndicSetOutlineAlpha(int indicator, KLWidgets::Alpha alpha) {
		Call(Message::IndicSetOutlineAlpha, indicator, static_cast<intptr_t>(alpha));
	}

	KLWidgets::Alpha TextEditor_Impl::IndicGetOutlineAlpha(int indicator) {
		return static_cast<KLWidgets::Alpha>(Call(Message::IndicGetOutlineAlpha, indicator));
	}

	void TextEditor_Impl::SetExtraAscent(int extraAscent) {
		Call(Message::SetExtraAscent, extraAscent);
	}

	int TextEditor_Impl::ExtraAscent() {
		return static_cast<int>(Call(Message::GetExtraAscent));
	}

	void TextEditor_Impl::SetExtraDescent(int extraDescent) {
		Call(Message::SetExtraDescent, extraDescent);
	}

	int TextEditor_Impl::ExtraDescent() {
		return static_cast<int>(Call(Message::GetExtraDescent));
	}

	int TextEditor_Impl::MarkerSymbolDefined(int markerNumber) {
		return static_cast<int>(Call(Message::MarkerSymbolDefined, markerNumber));
	}

	void TextEditor_Impl::MarginSetText(Line line, const char* text) {
		CallString(Message::MarginSetText, line, text);
	}

	int TextEditor_Impl::MarginGetText(Line line, char* text) {
		return static_cast<int>(CallPointer(Message::MarginGetText, line, text));
	}

	std::string TextEditor_Impl::MarginGetText(Line line) {
		return CallReturnString(Message::MarginGetText, line);
	}

	void TextEditor_Impl::MarginSetStyle(Line line, int style) {
		Call(Message::MarginSetStyle, line, style);
	}

	int TextEditor_Impl::MarginGetStyle(Line line) {
		return static_cast<int>(Call(Message::MarginGetStyle, line));
	}

	void TextEditor_Impl::MarginSetStyles(Line line, const char* styles) {
		CallString(Message::MarginSetStyles, line, styles);
	}

	int TextEditor_Impl::MarginGetStyles(Line line, char* styles) {
		return static_cast<int>(CallPointer(Message::MarginGetStyles, line, styles));
	}

	std::string TextEditor_Impl::MarginGetStyles(Line line) {
		return CallReturnString(Message::MarginGetStyles, line);
	}

	void TextEditor_Impl::MarginTextClearAll() {
		Call(Message::MarginTextClearAll);
	}

	void TextEditor_Impl::MarginSetStyleOffset(int style) {
		Call(Message::MarginSetStyleOffset, style);
	}

	int TextEditor_Impl::MarginGetStyleOffset() {
		return static_cast<int>(Call(Message::MarginGetStyleOffset));
	}

	void TextEditor_Impl::SetMarginOptions(KLWidgets::MarginOption marginOptions) {
		Call(Message::SetMarginOptions, static_cast<uintptr_t>(marginOptions));
	}

	MarginOption TextEditor_Impl::MarginOptions() {
		return static_cast<KLWidgets::MarginOption>(Call(Message::GetMarginOptions));
	}

	void TextEditor_Impl::AnnotationSetText(Line line, const char* text) {
		CallString(Message::AnnotationSetText, line, text);
	}

	int TextEditor_Impl::AnnotationGetText(Line line, char* text) {
		return static_cast<int>(CallPointer(Message::AnnotationGetText, line, text));
	}

	std::string TextEditor_Impl::AnnotationGetText(Line line) {
		return CallReturnString(Message::AnnotationGetText, line);
	}

	void TextEditor_Impl::AnnotationSetStyle(Line line, int style) {
		Call(Message::AnnotationSetStyle, line, style);
	}

	int TextEditor_Impl::AnnotationGetStyle(Line line) {
		return static_cast<int>(Call(Message::AnnotationGetStyle, line));
	}

	void TextEditor_Impl::AnnotationSetStyles(Line line, const char* styles) {
		CallString(Message::AnnotationSetStyles, line, styles);
	}

	int TextEditor_Impl::AnnotationGetStyles(Line line, char* styles) {
		return static_cast<int>(CallPointer(Message::AnnotationGetStyles, line, styles));
	}

	std::string TextEditor_Impl::AnnotationGetStyles(Line line) {
		return CallReturnString(Message::AnnotationGetStyles, line);
	}

	int TextEditor_Impl::AnnotationGetLines(Line line) {
		return static_cast<int>(Call(Message::AnnotationGetLines, line));
	}

	void TextEditor_Impl::AnnotationClearAll() {
		Call(Message::AnnotationClearAll);
	}

	void TextEditor_Impl::AnnotationSetVisible(KLWidgets::AnnotationVisible visible) {
		Call(Message::AnnotationSetVisible, static_cast<uintptr_t>(visible));
	}

	AnnotationVisible TextEditor_Impl::AnnotationGetVisible() {
		return static_cast<KLWidgets::AnnotationVisible>(Call(Message::AnnotationGetVisible));
	}

	void TextEditor_Impl::AnnotationSetStyleOffset(int style) {
		Call(Message::AnnotationSetStyleOffset, style);
	}

	int TextEditor_Impl::AnnotationGetStyleOffset() {
		return static_cast<int>(Call(Message::AnnotationGetStyleOffset));
	}

	void TextEditor_Impl::ReleaseAllExtendedStyles() {
		Call(Message::ReleaseAllExtendedStyles);
	}

	int TextEditor_Impl::AllocateExtendedStyles(int numberStyles) {
		return static_cast<int>(Call(Message::AllocateExtendedStyles, numberStyles));
	}

	void TextEditor_Impl::AddUndoAction(int token, KLWidgets::UndoFlags flags) {
		Call(Message::AddUndoAction, token, static_cast<intptr_t>(flags));
	}

	Position TextEditor_Impl::CharPositionFromPoint(int x, int y) {
		return Call(Message::CharPositionFromPoint, x, y);
	}

	Position TextEditor_Impl::CharPositionFromPointClose(int x, int y) {
		return Call(Message::CharPositionFromPointClose, x, y);
	}

	void TextEditor_Impl::SetMouseSelectionRectangularSwitch(bool mouseSelectionRectangularSwitch) {
		Call(Message::SetMouseSelectionRectangularSwitch, mouseSelectionRectangularSwitch);
	}

	bool TextEditor_Impl::MouseSelectionRectangularSwitch() {
		return Call(Message::GetMouseSelectionRectangularSwitch);
	}

	void TextEditor_Impl::SetMultipleSelection(bool multipleSelection) {
		Call(Message::SetMultipleSelection, multipleSelection);
	}

	bool TextEditor_Impl::MultipleSelection() {
		return Call(Message::GetMultipleSelection);
	}

	void TextEditor_Impl::SetAdditionalSelectionTyping(bool additionalSelectionTyping) {
		Call(Message::SetAdditionalSelectionTyping, additionalSelectionTyping);
	}

	bool TextEditor_Impl::AdditionalSelectionTyping() {
		return Call(Message::GetAdditionalSelectionTyping);
	}

	void TextEditor_Impl::SetAdditionalCaretsBlink(bool additionalCaretsBlink) {
		Call(Message::SetAdditionalCaretsBlink, additionalCaretsBlink);
	}

	bool TextEditor_Impl::AdditionalCaretsBlink() {
		return Call(Message::GetAdditionalCaretsBlink);
	}

	void TextEditor_Impl::SetAdditionalCaretsVisible(bool additionalCaretsVisible) {
		Call(Message::SetAdditionalCaretsVisible, additionalCaretsVisible);
	}

	bool TextEditor_Impl::AdditionalCaretsVisible() {
		return Call(Message::GetAdditionalCaretsVisible);
	}

	int TextEditor_Impl::Selections() {
		return static_cast<int>(Call(Message::GetSelections));
	}

	bool TextEditor_Impl::SelectionEmpty() {
		return Call(Message::GetSelectionEmpty);
	}

	void TextEditor_Impl::ClearSelections() {
		Call(Message::ClearSelections);
	}

	void TextEditor_Impl::SetSelection(Position caret, Position anchor) {
		Call(Message::SetSelection, caret, anchor);
	}

	void TextEditor_Impl::AddSelection(Position caret, Position anchor) {
		Call(Message::AddSelection, caret, anchor);
	}

	void TextEditor_Impl::DropSelectionN(int selection) {
		Call(Message::DropSelectionN, selection);
	}

	void TextEditor_Impl::SetMainSelection(int selection) {
		Call(Message::SetMainSelection, selection);
	}

	int TextEditor_Impl::MainSelection() {
		return static_cast<int>(Call(Message::GetMainSelection));
	}

	void TextEditor_Impl::SetSelectionNCaret(int selection, Position caret) {
		Call(Message::SetSelectionNCaret, selection, caret);
	}

	Position TextEditor_Impl::SelectionNCaret(int selection) {
		return Call(Message::GetSelectionNCaret, selection);
	}

	void TextEditor_Impl::SetSelectionNAnchor(int selection, Position anchor) {
		Call(Message::SetSelectionNAnchor, selection, anchor);
	}

	Position TextEditor_Impl::SelectionNAnchor(int selection) {
		return Call(Message::GetSelectionNAnchor, selection);
	}

	void TextEditor_Impl::SetSelectionNCaretVirtualSpace(int selection, Position space) {
		Call(Message::SetSelectionNCaretVirtualSpace, selection, space);
	}

	Position TextEditor_Impl::SelectionNCaretVirtualSpace(int selection) {
		return Call(Message::GetSelectionNCaretVirtualSpace, selection);
	}

	void TextEditor_Impl::SetSelectionNAnchorVirtualSpace(int selection, Position space) {
		Call(Message::SetSelectionNAnchorVirtualSpace, selection, space);
	}

	Position TextEditor_Impl::SelectionNAnchorVirtualSpace(int selection) {
		return Call(Message::GetSelectionNAnchorVirtualSpace, selection);
	}

	void TextEditor_Impl::SetSelectionNStart(int selection, Position anchor) {
		Call(Message::SetSelectionNStart, selection, anchor);
	}

	Position TextEditor_Impl::SelectionNStart(int selection) {
		return Call(Message::GetSelectionNStart, selection);
	}

	Position TextEditor_Impl::SelectionNStartVirtualSpace(int selection) {
		return Call(Message::GetSelectionNStartVirtualSpace, selection);
	}

	void TextEditor_Impl::SetSelectionNEnd(int selection, Position caret) {
		Call(Message::SetSelectionNEnd, selection, caret);
	}

	Position TextEditor_Impl::SelectionNEndVirtualSpace(int selection) {
		return Call(Message::GetSelectionNEndVirtualSpace, selection);
	}

	Position TextEditor_Impl::SelectionNEnd(int selection) {
		return Call(Message::GetSelectionNEnd, selection);
	}

	void TextEditor_Impl::SetRectangularSelectionCaret(Position caret) {
		Call(Message::SetRectangularSelectionCaret, caret);
	}

	Position TextEditor_Impl::RectangularSelectionCaret() {
		return Call(Message::GetRectangularSelectionCaret);
	}

	void TextEditor_Impl::SetRectangularSelectionAnchor(Position anchor) {
		Call(Message::SetRectangularSelectionAnchor, anchor);
	}

	Position TextEditor_Impl::RectangularSelectionAnchor() {
		return Call(Message::GetRectangularSelectionAnchor);
	}

	void TextEditor_Impl::SetRectangularSelectionCaretVirtualSpace(Position space) {
		Call(Message::SetRectangularSelectionCaretVirtualSpace, space);
	}

	Position TextEditor_Impl::RectangularSelectionCaretVirtualSpace() {
		return Call(Message::GetRectangularSelectionCaretVirtualSpace);
	}

	void TextEditor_Impl::SetRectangularSelectionAnchorVirtualSpace(Position space) {
		Call(Message::SetRectangularSelectionAnchorVirtualSpace, space);
	}

	Position TextEditor_Impl::RectangularSelectionAnchorVirtualSpace() {
		return Call(Message::GetRectangularSelectionAnchorVirtualSpace);
	}

	void TextEditor_Impl::SetVirtualSpaceOptions(KLWidgets::VirtualSpace virtualSpaceOptions) {
		Call(Message::SetVirtualSpaceOptions, static_cast<uintptr_t>(virtualSpaceOptions));
	}

	VirtualSpace TextEditor_Impl::VirtualSpaceOptions() {
		return static_cast<KLWidgets::VirtualSpace>(Call(Message::GetVirtualSpaceOptions));
	}

	void TextEditor_Impl::SetRectangularSelectionModifier(int modifier) {
		Call(Message::SetRectangularSelectionModifier, modifier);
	}

	int TextEditor_Impl::RectangularSelectionModifier() {
		return static_cast<int>(Call(Message::GetRectangularSelectionModifier));
	}

	void TextEditor_Impl::SetAdditionalSelFore(Colour fore) {
		Call(Message::SetAdditionalSelFore, fore);
	}

	void TextEditor_Impl::SetAdditionalSelBack(Colour back) {
		Call(Message::SetAdditionalSelBack, back);
	}

	void TextEditor_Impl::SetAdditionalSelAlpha(KLWidgets::Alpha alpha) {
		Call(Message::SetAdditionalSelAlpha, static_cast<uintptr_t>(alpha));
	}

	KLWidgets::Alpha TextEditor_Impl::AdditionalSelAlpha() {
		return static_cast<KLWidgets::Alpha>(Call(Message::GetAdditionalSelAlpha));
	}

	void TextEditor_Impl::SetAdditionalCaretFore(Colour fore) {
		Call(Message::SetAdditionalCaretFore, fore);
	}

	Colour TextEditor_Impl::AdditionalCaretFore() {
		return static_cast<Colour>(Call(Message::GetAdditionalCaretFore));
	}

	void TextEditor_Impl::RotateSelection() {
		Call(Message::RotateSelection);
	}

	void TextEditor_Impl::SwapMainAnchorCaret() {
		Call(Message::SwapMainAnchorCaret);
	}

	void TextEditor_Impl::MultipleSelectAddNext() {
		Call(Message::MultipleSelectAddNext);
	}

	void TextEditor_Impl::MultipleSelectAddEach() {
		Call(Message::MultipleSelectAddEach);
	}

	int TextEditor_Impl::ChangeLexerState(Position start, Position end) {
		return static_cast<int>(Call(Message::ChangeLexerState, start, end));
	}

	Line TextEditor_Impl::ContractedFoldNext(Line lineStart) {
		return Call(Message::ContractedFoldNext, lineStart);
	}

	void TextEditor_Impl::VerticalCentreCaret() {
		Call(Message::VerticalCentreCaret);
	}

	void TextEditor_Impl::MoveSelectedLinesUp() {
		Call(Message::MoveSelectedLinesUp);
	}

	void TextEditor_Impl::MoveSelectedLinesDown() {
		Call(Message::MoveSelectedLinesDown);
	}

	void TextEditor_Impl::SetIdentifier(int identifier) {
		Call(Message::SetIdentifier, identifier);
	}

	int TextEditor_Impl::Identifier() {
		return static_cast<int>(Call(Message::GetIdentifier));
	}

	void TextEditor_Impl::RGBAImageSetWidth(int width) {
		Call(Message::RGBAImageSetWidth, width);
	}

	void TextEditor_Impl::RGBAImageSetHeight(int height) {
		Call(Message::RGBAImageSetHeight, height);
	}

	void TextEditor_Impl::RGBAImageSetScale(int scalePercent) {
		Call(Message::RGBAImageSetScale, scalePercent);
	}

	void TextEditor_Impl::MarkerDefineRGBAImage(int markerNumber, const char* pixels) {
		CallString(Message::MarkerDefineRGBAImage, markerNumber, pixels);
	}

	void TextEditor_Impl::RegisterRGBAImage(int type, const char* pixels) {
		CallString(Message::RegisterRGBAImage, type, pixels);
	}

	void TextEditor_Impl::ScrollToStart() {
		Call(Message::ScrollToStart);
	}

	void TextEditor_Impl::ScrollToEnd() {
		Call(Message::ScrollToEnd);
	}

	void TextEditor_Impl::SetTechnology(KLWidgets::Technology technology) {
		Call(Message::SetTechnology, static_cast<uintptr_t>(technology));
	}

	Technology TextEditor_Impl::Technology() {
		return static_cast<KLWidgets::Technology>(Call(Message::GetTechnology));
	}

	void* TextEditor_Impl::CreateLoader(Position bytes, KLWidgets::DocumentOption documentOptions) {
		return reinterpret_cast<void*>(Call(Message::CreateLoader, bytes, static_cast<intptr_t>(documentOptions)));
	}

	void TextEditor_Impl::FindIndicatorShow(Position start, Position end) {
		Call(Message::FindIndicatorShow, start, end);
	}

	void TextEditor_Impl::FindIndicatorFlash(Position start, Position end) {
		Call(Message::FindIndicatorFlash, start, end);
	}

	void TextEditor_Impl::FindIndicatorHide() {
		Call(Message::FindIndicatorHide);
	}

	void TextEditor_Impl::VCHomeDisplay() {
		Call(Message::VCHomeDisplay);
	}

	void TextEditor_Impl::VCHomeDisplayExtend() {
		Call(Message::VCHomeDisplayExtend);
	}

	bool TextEditor_Impl::CaretLineVisibleAlways() {
		return Call(Message::GetCaretLineVisibleAlways);
	}

	void TextEditor_Impl::SetCaretLineVisibleAlways(bool alwaysVisible) {
		Call(Message::SetCaretLineVisibleAlways, alwaysVisible);
	}

	void TextEditor_Impl::SetLineEndTypesAllowed(KLWidgets::LineEndType lineEndBitSet) {
		Call(Message::SetLineEndTypesAllowed, static_cast<uintptr_t>(lineEndBitSet));
	}

	LineEndType TextEditor_Impl::LineEndTypesAllowed() {
		return static_cast<KLWidgets::LineEndType>(Call(Message::GetLineEndTypesAllowed));
	}

	LineEndType TextEditor_Impl::LineEndTypesActive() {
		return static_cast<KLWidgets::LineEndType>(Call(Message::GetLineEndTypesActive));
	}

	void TextEditor_Impl::SetRepresentation(const char* encodedCharacter, const char* representation) {
		CallString(Message::SetRepresentation, reinterpret_cast<uintptr_t>(encodedCharacter), representation);
	}

	int TextEditor_Impl::Representation(const char* encodedCharacter, char* representation) {
		return static_cast<int>(CallPointer(Message::GetRepresentation, reinterpret_cast<uintptr_t>(encodedCharacter), representation));
	}

	std::string TextEditor_Impl::Representation(const char* encodedCharacter) {
		return CallReturnString(Message::GetRepresentation, reinterpret_cast<uintptr_t>(encodedCharacter));
	}

	void TextEditor_Impl::ClearRepresentation(const char* encodedCharacter) {
		Call(Message::ClearRepresentation, reinterpret_cast<uintptr_t>(encodedCharacter));
	}

	void TextEditor_Impl::EOLAnnotationSetText(Line line, const char* text) {
		CallString(Message::EOLAnnotationSetText, line, text);
	}

	int TextEditor_Impl::EOLAnnotationGetText(Line line, char* text) {
		return static_cast<int>(CallPointer(Message::EOLAnnotationGetText, line, text));
	}

	std::string TextEditor_Impl::EOLAnnotationGetText(Line line) {
		return CallReturnString(Message::EOLAnnotationGetText, line);
	}

	void TextEditor_Impl::EOLAnnotationSetStyle(Line line, int style) {
		Call(Message::EOLAnnotationSetStyle, line, style);
	}

	int TextEditor_Impl::EOLAnnotationGetStyle(Line line) {
		return static_cast<int>(Call(Message::EOLAnnotationGetStyle, line));
	}

	void TextEditor_Impl::EOLAnnotationClearAll() {
		Call(Message::EOLAnnotationClearAll);
	}

	void TextEditor_Impl::EOLAnnotationSetVisible(KLWidgets::EOLAnnotationVisible visible) {
		Call(Message::EOLAnnotationSetVisible, static_cast<uintptr_t>(visible));
	}

	EOLAnnotationVisible TextEditor_Impl::EOLAnnotationGetVisible() {
		return static_cast<KLWidgets::EOLAnnotationVisible>(Call(Message::EOLAnnotationGetVisible));
	}

	void TextEditor_Impl::EOLAnnotationSetStyleOffset(int style) {
		Call(Message::EOLAnnotationSetStyleOffset, style);
	}

	int TextEditor_Impl::EOLAnnotationGetStyleOffset() {
		return static_cast<int>(Call(Message::EOLAnnotationGetStyleOffset));
	}

	bool TextEditor_Impl::SupportsFeature(KLWidgets::Supports feature) {
		return Call(Message::SupportsFeature, static_cast<uintptr_t>(feature));
	}

	void TextEditor_Impl::StartRecord() {
		Call(Message::StartRecord);
	}

	void TextEditor_Impl::StopRecord() {
		Call(Message::StopRecord);
	}

	int TextEditor_Impl::Lexer() {
		return static_cast<int>(Call(Message::GetLexer));
	}

	void TextEditor_Impl::Colourise(Position start, Position end) {
		Call(Message::Colourise, start, end);
	}

	void TextEditor_Impl::SetProperty(const char* key, const char* value) {
		CallString(Message::SetProperty, reinterpret_cast<uintptr_t>(key), value);
	}

	void TextEditor_Impl::SetKeyWords(int keyWordSet, const char* keyWords) {
		CallString(Message::SetKeyWords, keyWordSet, keyWords);
	}

	int TextEditor_Impl::Property(const char* key, char* value) {
		return static_cast<int>(CallPointer(Message::GetProperty, reinterpret_cast<uintptr_t>(key), value));
	}

	std::string TextEditor_Impl::Property(const char* key) {
		return CallReturnString(Message::GetProperty, reinterpret_cast<uintptr_t>(key));
	}

	int TextEditor_Impl::PropertyExpanded(const char* key, char* value) {
		return static_cast<int>(CallPointer(Message::GetPropertyExpanded, reinterpret_cast<uintptr_t>(key), value));
	}

	std::string TextEditor_Impl::PropertyExpanded(const char* key) {
		return CallReturnString(Message::GetPropertyExpanded, reinterpret_cast<uintptr_t>(key));
	}

	int TextEditor_Impl::PropertyInt(const char* key, int defaultValue) {
		return static_cast<int>(Call(Message::GetPropertyInt, reinterpret_cast<uintptr_t>(key), defaultValue));
	}

	int TextEditor_Impl::LexerLanguage(char* language) {
		return static_cast<int>(CallPointer(Message::GetLexerLanguage, 0, language));
	}

	std::string TextEditor_Impl::LexerLanguage() {
		return CallReturnString(Message::GetLexerLanguage, 0);
	}

	void* TextEditor_Impl::PrivateLexerCall(int operation, void* pointer) {
		return reinterpret_cast<void*>(CallPointer(Message::PrivateLexerCall, operation, pointer));
	}

	int TextEditor_Impl::PropertyNames(char* names) {
		return static_cast<int>(CallPointer(Message::PropertyNames, 0, names));
	}

	std::string TextEditor_Impl::PropertyNames() {
		return CallReturnString(Message::PropertyNames, 0);
	}

	TypeProperty TextEditor_Impl::PropertyType(const char* name) {
		return static_cast<KLWidgets::TypeProperty>(Call(Message::PropertyType, reinterpret_cast<uintptr_t>(name)));
	}

	int TextEditor_Impl::DescribeProperty(const char* name, char* description) {
		return static_cast<int>(CallPointer(Message::DescribeProperty, reinterpret_cast<uintptr_t>(name), description));
	}

	std::string TextEditor_Impl::DescribeProperty(const char* name) {
		return CallReturnString(Message::DescribeProperty, reinterpret_cast<uintptr_t>(name));
	}

	int TextEditor_Impl::DescribeKeyWordSets(char* descriptions) {
		return static_cast<int>(CallPointer(Message::DescribeKeyWordSets, 0, descriptions));
	}

	std::string TextEditor_Impl::DescribeKeyWordSets() {
		return CallReturnString(Message::DescribeKeyWordSets, 0);
	}

	int TextEditor_Impl::LineEndTypesSupported() {
		return static_cast<int>(Call(Message::GetLineEndTypesSupported));
	}

	int TextEditor_Impl::AllocateSubStyles(int styleBase, int numberStyles) {
		return static_cast<int>(Call(Message::AllocateSubStyles, styleBase, numberStyles));
	}

	int TextEditor_Impl::SubStylesStart(int styleBase) {
		return static_cast<int>(Call(Message::GetSubStylesStart, styleBase));
	}

	int TextEditor_Impl::SubStylesLength(int styleBase) {
		return static_cast<int>(Call(Message::GetSubStylesLength, styleBase));
	}

	int TextEditor_Impl::StyleFromSubStyle(int subStyle) {
		return static_cast<int>(Call(Message::GetStyleFromSubStyle, subStyle));
	}

	int TextEditor_Impl::PrimaryStyleFromStyle(int style) {
		return static_cast<int>(Call(Message::GetPrimaryStyleFromStyle, style));
	}

	void TextEditor_Impl::FreeSubStyles() {
		Call(Message::FreeSubStyles);
	}

	void TextEditor_Impl::SetIdentifiers(int style, const char* identifiers) {
		CallString(Message::SetIdentifiers, style, identifiers);
	}

	int TextEditor_Impl::DistanceToSecondaryStyles() {
		return static_cast<int>(Call(Message::DistanceToSecondaryStyles));
	}

	int TextEditor_Impl::SubStyleBases(char* styles) {
		return static_cast<int>(CallPointer(Message::GetSubStyleBases, 0, styles));
	}

	std::string TextEditor_Impl::SubStyleBases() {
		return CallReturnString(Message::GetSubStyleBases, 0);
	}

	int TextEditor_Impl::NamedStyles() {
		return static_cast<int>(Call(Message::GetNamedStyles));
	}

	int TextEditor_Impl::NameOfStyle(int style, char* name) {
		return static_cast<int>(CallPointer(Message::NameOfStyle, style, name));
	}

	std::string TextEditor_Impl::NameOfStyle(int style) {
		return CallReturnString(Message::NameOfStyle, style);
	}

	int TextEditor_Impl::TagsOfStyle(int style, char* tags) {
		return static_cast<int>(CallPointer(Message::TagsOfStyle, style, tags));
	}

	std::string TextEditor_Impl::TagsOfStyle(int style) {
		return CallReturnString(Message::TagsOfStyle, style);
	}

	int TextEditor_Impl::DescriptionOfStyle(int style, char* description) {
		return static_cast<int>(CallPointer(Message::DescriptionOfStyle, style, description));
	}

	std::string TextEditor_Impl::DescriptionOfStyle(int style) {
		return CallReturnString(Message::DescriptionOfStyle, style);
	}

	void TextEditor_Impl::SetILexer(void* ilexer) {
		CallPointer(Message::SetILexer, 0, ilexer);
	}

	Bidirectional TextEditor_Impl::Bidirectional() {
		return static_cast<KLWidgets::Bidirectional>(Call(Message::GetBidirectional));
	}

	void TextEditor_Impl::SetBidirectional(KLWidgets::Bidirectional bidirectional) {
		Call(Message::SetBidirectional, static_cast<uintptr_t>(bidirectional));
	}

	LineCharacterIndexType TextEditor_Impl::LineCharacterIndex() {
		return static_cast<KLWidgets::LineCharacterIndexType>(Call(Message::GetLineCharacterIndex));
	}

	void TextEditor_Impl::AllocateLineCharacterIndex(KLWidgets::LineCharacterIndexType lineCharacterIndex) {
		Call(Message::AllocateLineCharacterIndex, static_cast<uintptr_t>(lineCharacterIndex));
	}

	void TextEditor_Impl::ReleaseLineCharacterIndex(KLWidgets::LineCharacterIndexType lineCharacterIndex) {
		Call(Message::ReleaseLineCharacterIndex, static_cast<uintptr_t>(lineCharacterIndex));
	}

	Line TextEditor_Impl::LineFromIndexPosition(Position pos, KLWidgets::LineCharacterIndexType lineCharacterIndex) {
		return Call(Message::LineFromIndexPosition, pos, static_cast<intptr_t>(lineCharacterIndex));
	}

	Position TextEditor_Impl::IndexPositionFromLine(Line line, KLWidgets::LineCharacterIndexType lineCharacterIndex) {
		return Call(Message::IndexPositionFromLine, line, static_cast<intptr_t>(lineCharacterIndex));
	}
}
