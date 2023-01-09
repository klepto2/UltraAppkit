#include "UltraEngine.h"
#include "TextEditor.h"
#include <WinUser.h>
#include "Scintilla.h"
#include "SciLexer.h"

namespace KLWidgets
{
bool TextEditor::SCI_REGISTERED = false;
vector<shared_ptr<Widget>> TextEditor::TextEditorMap;

EventID TextEditor::EVENT_STYLENEEDED = AllocEventID();
EventID TextEditor::EVENT_CHARADDED = AllocEventID();
EventID TextEditor::EVENT_SAVEPOINTREACHED = AllocEventID();
EventID TextEditor::EVENT_SAVEPOINTLEFT = AllocEventID();
EventID TextEditor::EVENT_MODIFYATTEMPTRO = AllocEventID();
EventID TextEditor::EVENT_KEY = AllocEventID();
EventID TextEditor::EVENT_SCI_DOUBLECLICK = AllocEventID();
EventID TextEditor::EVENT_UPDATEUI = AllocEventID();
EventID TextEditor::EVENT_MODIFIED = AllocEventID();
EventID TextEditor::EVENT_MACRORECORD = AllocEventID();
EventID TextEditor::EVENT_MARGINCLICK = AllocEventID();
EventID TextEditor::EVENT_NEEDSHOWN = AllocEventID();
EventID TextEditor::EVENT_PAINTED = AllocEventID();
EventID TextEditor::EVENT_USERLISTSELECTION = AllocEventID();
EventID TextEditor::EVENT_URIDROPPED = AllocEventID();
EventID TextEditor::EVENT_DWELLSTART = AllocEventID();
EventID TextEditor::EVENT_DWELLEND = AllocEventID();
EventID TextEditor::EVENT_ZOOM = AllocEventID();
EventID TextEditor::EVENT_HOTSPOTCLICK = AllocEventID();
EventID TextEditor::EVENT_HOTSPOTDOUBLECLICK = AllocEventID();
EventID TextEditor::EVENT_HOTSPOTRELEASECLICK = AllocEventID();
EventID TextEditor::EVENT_INDICATORCLICK = AllocEventID();
EventID TextEditor::EVENT_INDICATORRELEASE = AllocEventID();
EventID TextEditor::EVENT_CALLTIPCLICK = AllocEventID();
EventID TextEditor::EVENT_AUTOCSELECTION = AllocEventID();
EventID TextEditor::EVENT_AUTOCCANCELLED = AllocEventID();
EventID TextEditor::EVENT_AUTOCCHARDELETED = AllocEventID();
EventID TextEditor::EVENT_FOCUSIN = AllocEventID();
EventID TextEditor::EVENT_FOCUSOUT = AllocEventID();
EventID TextEditor::EVENT_AUTOCCOMPLETED = AllocEventID();
EventID TextEditor::EVENT_MARGINRIGHTCLICK = AllocEventID();
EventID TextEditor::EVENT_AUTOCSELECTIONCHANGE = AllocEventID();

shared_ptr<TextEditor> CreateTextEditor(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent)
{
	auto widget = make_shared<TextEditor>();
	
	widget->Initialize("", x, y, width, height, parent,0);

	float scale = widget->gui->scale;
	widget->_verticalSlider = CreateSlider(width - widget->slidersize * scale, 0, widget->slidersize * scale, height, widget, SLIDER_VERTICAL | SLIDER_SCROLLBAR);
	widget->_horizontalSlider = CreateSlider(0, width - widget->slidersize * scale,width, widget->slidersize * scale, widget, SLIDER_HORIZONTAL | SLIDER_SCROLLBAR);
	//widget->m_kids.clear();
	TextEditor::TextEditorMap.push_back(widget);

	widget->UpdateLayout();
	return widget;
}

void TextEditor::Hide()
{
	Widget::Hide();
	::ShowWindow(hWND, SW_HIDE);
}
void TextEditor::Show()
{
	UpdateLayout();
	Widget::Show();
	isDirty = false;
}

void TextEditor::Draw(const int x, const int y, const int width, const int height)
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

void TextEditor::Draw__(const int x, const int y, const int width, const int height)
{
	if (collapsed || hidestate)
	{
		return;
	}

	auto pos = GetPosition(true);
	auto size = this->GetParent()->ClientSize();

	if (initialized)
	{
		UpdateSlider();
	}

	////if (!_horizontalSlider->Hidden())
	//{
	//	_horizontalSlider->Draw__(x, y, width, height);
	//}

	////if (!_verticalSlider->Hidden())
	//{
	//	//Print("VDraw");
	//	_verticalSlider->Draw__(x, y, width, height);
	//}

	if (!isDirty)
	{
		UpdateLayout();
		ShowWindow(hWND, SW_SHOW);
		isDirty = true;
	}

	UpdateWindow(hWND);
	Widget::Draw__(x, y, width, height);
	Print(WString(iVec4(_verticalSlider->GetPosition(true), _verticalSlider->GetSize())));
	//SetWindowPos(hWND, HWND_TOP, pos.x, pos.y, size.x - vScroll, size.y - hScroll, SWP_SHOWWINDOW | SWP_ASYNCWINDOWPOS);	
}

void TextEditor::UpdateLayout()
{
	Widget::UpdateLayout();

	UpdateSlider();

	auto pos = GetPosition(true);
	auto size = this->GetParent()->ClientSize();
	int vScroll = 0;
	int hScroll = 0;

	if (!_horizontalSlider->Hidden())
	{
		hScroll = _horizontalSlider->size.y;
	}

	if (!_verticalSlider->Hidden())
	{
		vScroll = _verticalSlider->size.x;
	}

	sciShape = iVec4(pos.x, pos.y, size.x - vScroll, size.y - hScroll);
	SetWindowPos(hWND, HWND_TOP, pos.x, pos.y, size.x - vScroll, size.y - hScroll, SWP_NOREDRAW);
	Redraw();
}

bool TextEditor::ProcessEvent(const Event& e)
{
	return true;
}

void TextEditor::UpdateSlider(bool updateFromScintilla)
{
	int hscrollwidthlimiter = 0;
	int vscrollheightlimiter = 0;
	if (ScrollWidth() > GetSize().y)
	{
		_horizontalSlider->Show();
		vscrollheightlimiter = slidersize * gui->scale;
	}
	else
	{
		_horizontalSlider->Hide();
	}

	int margin_count = Margins();
	int msize = MarginLeft() + MarginRight();
	for (int m = 0; m < margin_count; m++)
	{
		msize += MarginWidthN(m);
	}

	auto range = _horizontalSlider->GetRange();
	if (range.x != (sciShape.width - msize) / 10 || range.y != ScrollWidth() / 10)
	{
		_horizontalSlider->SetRange((sciShape.width - msize) / 10, ScrollWidth() / 10);
		_horizontalSlider->SetValue(XOffset() / 10.0);
	}

	if (LineCount() > LinesOnScreen())
	{
		_verticalSlider->Show();	
		hscrollwidthlimiter = slidersize * gui->scale;
		//Print("VShow");
	}
	else
	{
		_verticalSlider->Hide();
		//Print("VHide");
	}

	int lc = LineCount();
	int l = LinesOnScreen();
	range = _verticalSlider->GetRange();

	if (range.x != l || range.y != lc)
	{
		_verticalSlider->SetRange(LinesOnScreen(), LineCount());
		_verticalSlider->SetValue(FirstVisibleLine());
	}
	

	_horizontalSlider->SetShape(0, this->size.y - slidersize * gui->scale, this->size.x - hscrollwidthlimiter,slidersize * gui->scale);
	_verticalSlider->SetShape(this->size.x - slidersize * gui->scale - 0, 0, slidersize * gui->scale, this->size.y - vscrollheightlimiter);

	int hScrollValue = XOffset() / 10;
	int vScrollValue = FirstVisibleLine();


	if (updateFromScintilla)
	{
		_horizontalSlider->SetValue(XOffset() / 10.0);
		_verticalSlider->SetValue(FirstVisibleLine());
	}
	else
	{
		SetXOffset(_horizontalSlider->GetValue()* 10.0);
		SetFirstVisibleLine(_verticalSlider->GetValue());
	}
	
}

void TextEditor::ProcessNotification(SCNotification* n)
{
	

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
		if (n->updated & SC_UPDATE_V_SCROLL || n->updated & SC_UPDATE_H_SCROLL)
		{
			UpdateSlider(true);
		}
		id = EVENT_UPDATEUI;
		break;
	case SCN_MODIFIED:
		UpdateSlider(true);
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

		for (auto w : TextEditor::TextEditorMap)
		{
			auto editor = static_cast<TextEditor*>(w.get());
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

bool TextEditor::Initialize(const WString& text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style)
{
	if (Widget::Initialize(text, x, y, width, height, parent, style))
	{
		auto parentWindow = gui->window.lock();
		hInstance = (HINSTANCE)GetWindowLong(parentWindow->GetHandle(), GWLP_HINSTANCE);
		if (!TextEditor::SCI_REGISTERED)
		{
			if (!Scintilla_RegisterClasses(hInstance))
			{
				return false;
			}

			TextEditor::SCI_REGISTERED = true;
		}

		hWND = CreateWindowA(
			"Scintilla",
			"Source",
			WS_CHILD | WS_TABSTOP | WS_CLIPCHILDREN,
			x, y,
			width, height,
			parentWindow->GetHandle(),
			0,
			hInstance,
			0);

		
		//::ShowWindow(hWND, SW_SHOW);
		//::SetFocus(hWND);

		SciFnDirect fn_ = reinterpret_cast<SciFnDirect>(
			SendMessage(hWND,SCI_GETDIRECTFUNCTION, 0, 0));
		const sptr_t ptr_ = SendMessage(hWND, SCI_GETDIRECTPOINTER, 0, 0);
		SetFnPtr(fn_, ptr_);

		::SetWindowsHookEx(WH_CALLWNDPROC, HandleScintillaNotifications, hInstance, GetCurrentThreadId());
		SetVScrollBar(false);
		SetHScrollBar(false);
		SetBufferedDraw(true);
		//_horizontalSlider->Hide();
		//Redraw();
		initialized = true;

		return true;
	}
	return false;
}



//sptr_t TextEditor::Call(unsigned int iMessage, sptr_t wParam, sptr_t lParam)
//{
//	return SendMessage(hWND, iMessage, wParam, lParam);
//}

void TextEditor::SetFnPtr(FunctionDirect fn_, intptr_t ptr_) noexcept {
	fn = fn_;
	ptr = ptr_;
}

bool TextEditor::IsValid() const noexcept {
	return fn && ptr;
}


intptr_t TextEditor::Call(Message msg, uintptr_t wParam, intptr_t lParam) {
	if (!fn)
		throw Failure(Status::Failure);
	const intptr_t retVal = fn(ptr, static_cast<unsigned int>(msg), wParam, lParam);
	return retVal;
}

intptr_t TextEditor::CallPointer(Message msg, uintptr_t wParam, void* s) {
	return Call(msg, wParam, reinterpret_cast<intptr_t>(s));
}

intptr_t TextEditor::CallString(Message msg, uintptr_t wParam, const char* s) {
	return Call(msg, wParam, reinterpret_cast<intptr_t>(s));
}

std::string TextEditor::CallReturnString(Message msg, uintptr_t wParam) {
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

Position TextEditor::LineStart(Line line) {
	return Call(Message::PositionFromLine, line);
}

Position TextEditor::LineEnd(Line line) {
	return Call(Message::GetLineEndPosition, line);
}

Range TextEditor::SelectionRange() {
	return Range(
		Call(Message::GetSelectionStart),
		Call(Message::GetSelectionEnd));
}

Range TextEditor::TargetRange() {
	return Range(
		Call(Message::GetTargetStart),
		Call(Message::GetTargetEnd));
}

void TextEditor::SetTarget(Range range) {
	Call(Message::SetTargetRange, range.start, range.end);
}

void TextEditor::ColouriseAll() {
	Colourise(0, -1);
}

char TextEditor::CharacterAt(Position position) {
	return static_cast<char>(Call(Message::GetCharAt, position));
}

int TextEditor::UnsignedStyleAt(Position position) {
	// Returns signed value but easier to use as unsigned
	return static_cast<unsigned char>(Call(Message::GetStyleAt, position));
}

std::string TextEditor::StringOfRange(Range range) {
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

Position TextEditor::ReplaceTarget(std::string_view text) {
	return TextEditor::CallString(Message::ReplaceTarget, text.length(), text.data());
}

Position TextEditor::ReplaceTargetRE(std::string_view text) {
	return CallString(Message::ReplaceTargetRE, text.length(), text.data());
}

Position TextEditor::SearchInTarget(std::string_view text) {
	return CallString(Message::SearchInTarget, text.length(), text.data());
}

Range TextEditor::RangeSearchInTarget(std::string_view text) {
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
void TextEditor::AddText(Position length, const char* text) {
	CallString(Message::AddText, length, text);
}

void TextEditor::AddStyledText(Position length, const char* c) {
	CallString(Message::AddStyledText, length, c);
}

void TextEditor::InsertText(Position pos, const char* text) {
	CallString(Message::InsertText, pos, text);
}

void TextEditor::ChangeInsertion(Position length, const char* text) {
	CallString(Message::ChangeInsertion, length, text);
}

void TextEditor::ClearAll() {
	Call(Message::ClearAll);
}

void TextEditor::DeleteRange(Position start, Position lengthDelete) {
	Call(Message::DeleteRange, start, lengthDelete);
}

void TextEditor::ClearDocumentStyle() {
	Call(Message::ClearDocumentStyle);
}

Position TextEditor::Length() {
	return Call(Message::GetLength);
}

int TextEditor::CharAt(Position pos) {
	return static_cast<int>(Call(Message::GetCharAt, pos));
}

Position TextEditor::CurrentPos() {
	return Call(Message::GetCurrentPos);
}

Position TextEditor::Anchor() {
	return Call(Message::GetAnchor);
}

int TextEditor::StyleAt(Position pos) {
	return static_cast<int>(Call(Message::GetStyleAt, pos));
}

void TextEditor::Redo() {
	Call(Message::Redo);
}

void TextEditor::SetUndoCollection(bool collectUndo) {
	Call(Message::SetUndoCollection, collectUndo);
}

void TextEditor::SelectAll() {
	Call(Message::SelectAll);
}

void TextEditor::SetSavePoint() {
	Call(Message::SetSavePoint);
}

Position TextEditor::GetStyledText(void* tr) {
	return CallPointer(Message::GetStyledText, 0, tr);
}

bool TextEditor::CanRedo() {
	return Call(Message::CanRedo);
}

Line TextEditor::MarkerLineFromHandle(int markerHandle) {
	return Call(Message::MarkerLineFromHandle, markerHandle);
}

void TextEditor::MarkerDeleteHandle(int markerHandle) {
	Call(Message::MarkerDeleteHandle, markerHandle);
}

int TextEditor::MarkerHandleFromLine(Line line, int which) {
	return static_cast<int>(Call(Message::MarkerHandleFromLine, line, which));
}

int TextEditor::MarkerNumberFromLine(Line line, int which) {
	return static_cast<int>(Call(Message::MarkerNumberFromLine, line, which));
}

bool TextEditor::UndoCollection() {
	return Call(Message::GetUndoCollection);
}

WhiteSpace TextEditor::ViewWS() {
	return static_cast<KLWidgets::WhiteSpace>(Call(Message::GetViewWS));
}

void TextEditor::SetViewWS(KLWidgets::WhiteSpace viewWS) {
	Call(Message::SetViewWS, static_cast<uintptr_t>(viewWS));
}

TabDrawMode TextEditor::TabDrawMode() {
	return static_cast<KLWidgets::TabDrawMode>(Call(Message::GetTabDrawMode));
}

void TextEditor::SetTabDrawMode(KLWidgets::TabDrawMode tabDrawMode) {
	Call(Message::SetTabDrawMode, static_cast<uintptr_t>(tabDrawMode));
}

Position TextEditor::PositionFromPoint(int x, int y) {
	return Call(Message::PositionFromPoint, x, y);
}

Position TextEditor::PositionFromPointClose(int x, int y) {
	return Call(Message::PositionFromPointClose, x, y);
}

void TextEditor::GotoLine(Line line) {
	Call(Message::GotoLine, line);
}

void TextEditor::GotoPos(Position caret) {
	Call(Message::GotoPos, caret);
}

void TextEditor::SetAnchor(Position anchor) {
	Call(Message::SetAnchor, anchor);
}

Position TextEditor::GetCurLine(Position length, char* text) {
	return CallPointer(Message::GetCurLine, length, text);
}

std::string TextEditor::GetCurLine(Position length) {
	return CallReturnString(Message::GetCurLine, length);
}

Position TextEditor::EndStyled() {
	return Call(Message::GetEndStyled);
}

void TextEditor::ConvertEOLs(KLWidgets::EndOfLine eolMode) {
	Call(Message::ConvertEOLs, static_cast<uintptr_t>(eolMode));
}

EndOfLine TextEditor::EOLMode() {
	return static_cast<KLWidgets::EndOfLine>(Call(Message::GetEOLMode));
}

void TextEditor::SetEOLMode(KLWidgets::EndOfLine eolMode) {
	Call(Message::SetEOLMode, static_cast<uintptr_t>(eolMode));
}

void TextEditor::StartStyling(Position start, int unused) {
	Call(Message::StartStyling, start, unused);
}

void TextEditor::SetStyling(Position length, int style) {
	Call(Message::SetStyling, length, style);
}

bool TextEditor::BufferedDraw() {
	return Call(Message::GetBufferedDraw);
}

void TextEditor::SetBufferedDraw(bool buffered) {
	Call(Message::SetBufferedDraw, buffered);
}

void TextEditor::SetTabWidth(int tabWidth) {
	Call(Message::SetTabWidth, tabWidth);
}

int TextEditor::TabWidth() {
	return static_cast<int>(Call(Message::GetTabWidth));
}

void TextEditor::SetTabMinimumWidth(int pixels) {
	Call(Message::SetTabMinimumWidth, pixels);
}

int TextEditor::TabMinimumWidth() {
	return static_cast<int>(Call(Message::GetTabMinimumWidth));
}

void TextEditor::ClearTabStops(Line line) {
	Call(Message::ClearTabStops, line);
}

void TextEditor::AddTabStop(Line line, int x) {
	Call(Message::AddTabStop, line, x);
}

int TextEditor::GetNextTabStop(Line line, int x) {
	return static_cast<int>(Call(Message::GetNextTabStop, line, x));
}

void TextEditor::SetCodePage(int codePage) {
	Call(Message::SetCodePage, codePage);
}

void TextEditor::SetFontLocale(const char* localeName) {
	CallString(Message::SetFontLocale, 0, localeName);
}

int TextEditor::FontLocale(char* localeName) {
	return static_cast<int>(CallPointer(Message::GetFontLocale, 0, localeName));
}

std::string TextEditor::FontLocale() {
	return CallReturnString(Message::GetFontLocale, 0);
}

IMEInteraction TextEditor::IMEInteraction() {
	return static_cast<KLWidgets::IMEInteraction>(Call(Message::GetIMEInteraction));
}

void TextEditor::SetIMEInteraction(KLWidgets::IMEInteraction imeInteraction) {
	Call(Message::SetIMEInteraction, static_cast<uintptr_t>(imeInteraction));
}

void TextEditor::MarkerDefine(int markerNumber, KLWidgets::MarkerSymbol markerSymbol) {
	Call(Message::MarkerDefine, markerNumber, static_cast<intptr_t>(markerSymbol));
}

void TextEditor::MarkerSetFore(int markerNumber, Colour fore) {
	Call(Message::MarkerSetFore, markerNumber, fore);
}

void TextEditor::MarkerSetBack(int markerNumber, Colour back) {
	Call(Message::MarkerSetBack, markerNumber, back);
}

void TextEditor::MarkerSetBackSelected(int markerNumber, Colour back) {
	Call(Message::MarkerSetBackSelected, markerNumber, back);
}

void TextEditor::MarkerSetForeTranslucent(int markerNumber, ColourAlpha fore) {
	Call(Message::MarkerSetForeTranslucent, markerNumber, fore);
}

void TextEditor::MarkerSetBackTranslucent(int markerNumber, ColourAlpha back) {
	Call(Message::MarkerSetBackTranslucent, markerNumber, back);
}

void TextEditor::MarkerSetBackSelectedTranslucent(int markerNumber, ColourAlpha back) {
	Call(Message::MarkerSetBackSelectedTranslucent, markerNumber, back);
}

void TextEditor::MarkerSetStrokeWidth(int markerNumber, int hundredths) {
	Call(Message::MarkerSetStrokeWidth, markerNumber, hundredths);
}

void TextEditor::MarkerEnableHighlight(bool enabled) {
	Call(Message::MarkerEnableHighlight, enabled);
}

int TextEditor::MarkerAdd(Line line, int markerNumber) {
	return static_cast<int>(Call(Message::MarkerAdd, line, markerNumber));
}

void TextEditor::MarkerDelete(Line line, int markerNumber) {
	Call(Message::MarkerDelete, line, markerNumber);
}

void TextEditor::MarkerDeleteAll(int markerNumber) {
	Call(Message::MarkerDeleteAll, markerNumber);
}

int TextEditor::MarkerGet(Line line) {
	return static_cast<int>(Call(Message::MarkerGet, line));
}

Line TextEditor::MarkerNext(Line lineStart, int markerMask) {
	return Call(Message::MarkerNext, lineStart, markerMask);
}

Line TextEditor::MarkerPrevious(Line lineStart, int markerMask) {
	return Call(Message::MarkerPrevious, lineStart, markerMask);
}

void TextEditor::MarkerDefinePixmap(int markerNumber, const char* pixmap) {
	CallString(Message::MarkerDefinePixmap, markerNumber, pixmap);
}

void TextEditor::MarkerAddSet(Line line, int markerSet) {
	Call(Message::MarkerAddSet, line, markerSet);
}

void TextEditor::MarkerSetAlpha(int markerNumber, KLWidgets::Alpha alpha) {
	Call(Message::MarkerSetAlpha, markerNumber, static_cast<intptr_t>(alpha));
}

void TextEditor::SetMarginTypeN(int margin, KLWidgets::MarginType marginType) {
	Call(Message::SetMarginTypeN, margin, static_cast<intptr_t>(marginType));
}

MarginType TextEditor::MarginTypeN(int margin) {
	return static_cast<KLWidgets::MarginType>(Call(Message::GetMarginTypeN, margin));
}

void TextEditor::SetMarginWidthN(int margin, int pixelWidth) {
	Call(Message::SetMarginWidthN, margin, pixelWidth);
}

int TextEditor::MarginWidthN(int margin) {
	return static_cast<int>(Call(Message::GetMarginWidthN, margin));
}

void TextEditor::SetMarginMaskN(int margin, int mask) {
	Call(Message::SetMarginMaskN, margin, mask);
}

int TextEditor::MarginMaskN(int margin) {
	return static_cast<int>(Call(Message::GetMarginMaskN, margin));
}

void TextEditor::SetMarginSensitiveN(int margin, bool sensitive) {
	Call(Message::SetMarginSensitiveN, margin, sensitive);
}

bool TextEditor::MarginSensitiveN(int margin) {
	return Call(Message::GetMarginSensitiveN, margin);
}

void TextEditor::SetMarginCursorN(int margin, KLWidgets::CursorShape cursor) {
	Call(Message::SetMarginCursorN, margin, static_cast<intptr_t>(cursor));
}

CursorShape TextEditor::MarginCursorN(int margin) {
	return static_cast<KLWidgets::CursorShape>(Call(Message::GetMarginCursorN, margin));
}

void TextEditor::SetMarginBackN(int margin, Colour back) {
	Call(Message::SetMarginBackN, margin, back);
}

Colour TextEditor::MarginBackN(int margin) {
	return static_cast<Colour>(Call(Message::GetMarginBackN, margin));
}

void TextEditor::SetMargins(int margins) {
	Call(Message::SetMargins, margins);
}

int TextEditor::Margins() {
	return static_cast<int>(Call(Message::GetMargins));
}

void TextEditor::StyleClearAll() {
	Call(Message::StyleClearAll);
}

void TextEditor::StyleSetFore(int style, Colour fore) {
	Call(Message::StyleSetFore, style, fore);
}

void TextEditor::StyleSetBack(int style, Colour back) {
	Call(Message::StyleSetBack, style, back);
}

void TextEditor::StyleSetBold(int style, bool bold) {
	Call(Message::StyleSetBold, style, bold);
}

void TextEditor::StyleSetItalic(int style, bool italic) {
	Call(Message::StyleSetItalic, style, italic);
}

void TextEditor::StyleSetSize(int style, int sizePoints) {
	Call(Message::StyleSetSize, style, sizePoints);
}

void TextEditor::StyleSetFont(int style, const char* fontName) {
	CallString(Message::StyleSetFont, style, fontName);
}

void TextEditor::StyleSetEOLFilled(int style, bool eolFilled) {
	Call(Message::StyleSetEOLFilled, style, eolFilled);
}

void TextEditor::StyleResetDefault() {
	Call(Message::StyleResetDefault);
}

void TextEditor::StyleSetUnderline(int style, bool underline) {
	Call(Message::StyleSetUnderline, style, underline);
}

Colour TextEditor::StyleGetFore(int style) {
	return static_cast<Colour>(Call(Message::StyleGetFore, style));
}

Colour TextEditor::StyleGetBack(int style) {
	return static_cast<Colour>(Call(Message::StyleGetBack, style));
}

bool TextEditor::StyleGetBold(int style) {
	return Call(Message::StyleGetBold, style);
}

bool TextEditor::StyleGetItalic(int style) {
	return Call(Message::StyleGetItalic, style);
}

int TextEditor::StyleGetSize(int style) {
	return static_cast<int>(Call(Message::StyleGetSize, style));
}

int TextEditor::StyleGetFont(int style, char* fontName) {
	return static_cast<int>(CallPointer(Message::StyleGetFont, style, fontName));
}

std::string TextEditor::StyleGetFont(int style) {
	return CallReturnString(Message::StyleGetFont, style);
}

bool TextEditor::StyleGetEOLFilled(int style) {
	return Call(Message::StyleGetEOLFilled, style);
}

bool TextEditor::StyleGetUnderline(int style) {
	return Call(Message::StyleGetUnderline, style);
}

CaseVisible TextEditor::StyleGetCase(int style) {
	return static_cast<KLWidgets::CaseVisible>(Call(Message::StyleGetCase, style));
}

CharacterSet TextEditor::StyleGetCharacterSet(int style) {
	return static_cast<KLWidgets::CharacterSet>(Call(Message::StyleGetCharacterSet, style));
}

bool TextEditor::StyleGetVisible(int style) {
	return Call(Message::StyleGetVisible, style);
}

bool TextEditor::StyleGetChangeable(int style) {
	return Call(Message::StyleGetChangeable, style);
}

bool TextEditor::StyleGetHotSpot(int style) {
	return Call(Message::StyleGetHotSpot, style);
}

void TextEditor::StyleSetCase(int style, KLWidgets::CaseVisible caseVisible) {
	Call(Message::StyleSetCase, style, static_cast<intptr_t>(caseVisible));
}

void TextEditor::StyleSetSizeFractional(int style, int sizeHundredthPoints) {
	Call(Message::StyleSetSizeFractional, style, sizeHundredthPoints);
}

int TextEditor::StyleGetSizeFractional(int style) {
	return static_cast<int>(Call(Message::StyleGetSizeFractional, style));
}

void TextEditor::StyleSetWeight(int style, KLWidgets::FontWeight weight) {
	Call(Message::StyleSetWeight, style, static_cast<intptr_t>(weight));
}

FontWeight TextEditor::StyleGetWeight(int style) {
	return static_cast<KLWidgets::FontWeight>(Call(Message::StyleGetWeight, style));
}

void TextEditor::StyleSetCharacterSet(int style, KLWidgets::CharacterSet characterSet) {
	Call(Message::StyleSetCharacterSet, style, static_cast<intptr_t>(characterSet));
}

void TextEditor::StyleSetHotSpot(int style, bool hotspot) {
	Call(Message::StyleSetHotSpot, style, hotspot);
}

void TextEditor::SetElementColour(KLWidgets::Element element, ColourAlpha colourElement) {
	Call(Message::SetElementColour, static_cast<uintptr_t>(element), colourElement);
}

ColourAlpha TextEditor::ElementColour(KLWidgets::Element element) {
	return static_cast<ColourAlpha>(Call(Message::GetElementColour, static_cast<uintptr_t>(element)));
}

void TextEditor::ResetElementColour(KLWidgets::Element element) {
	Call(Message::ResetElementColour, static_cast<uintptr_t>(element));
}

bool TextEditor::ElementIsSet(KLWidgets::Element element) {
	return Call(Message::GetElementIsSet, static_cast<uintptr_t>(element));
}

bool TextEditor::ElementAllowsTranslucent(KLWidgets::Element element) {
	return Call(Message::GetElementAllowsTranslucent, static_cast<uintptr_t>(element));
}

void TextEditor::SetSelFore(bool useSetting, Colour fore) {
	Call(Message::SetSelFore, useSetting, fore);
}

void TextEditor::SetSelBack(bool useSetting, Colour back) {
	Call(Message::SetSelBack, useSetting, back);
}

KLWidgets::Alpha TextEditor::SelAlpha() {
	return static_cast<KLWidgets::Alpha>(Call(Message::GetSelAlpha));
}

void TextEditor::SetSelAlpha(KLWidgets::Alpha alpha) {
	Call(Message::SetSelAlpha, static_cast<uintptr_t>(alpha));
}

bool TextEditor::SelEOLFilled() {
	return Call(Message::GetSelEOLFilled);
}

void TextEditor::SetSelEOLFilled(bool filled) {
	Call(Message::SetSelEOLFilled, filled);
}

void TextEditor::SetCaretFore(Colour fore) {
	Call(Message::SetCaretFore, fore);
}

void TextEditor::AssignCmdKey(int keyDefinition, int sciCommand) {
	Call(Message::AssignCmdKey, keyDefinition, sciCommand);
}

void TextEditor::ClearCmdKey(int keyDefinition) {
	Call(Message::ClearCmdKey, keyDefinition);
}

void TextEditor::ClearAllCmdKeys() {
	Call(Message::ClearAllCmdKeys);
}

void TextEditor::SetStylingEx(Position length, const char* styles) {
	CallString(Message::SetStylingEx, length, styles);
}

void TextEditor::StyleSetVisible(int style, bool visible) {
	Call(Message::StyleSetVisible, style, visible);
}

int TextEditor::CaretPeriod() {
	return static_cast<int>(Call(Message::GetCaretPeriod));
}

void TextEditor::SetCaretPeriod(int periodMilliseconds) {
	Call(Message::SetCaretPeriod, periodMilliseconds);
}

void TextEditor::SetWordChars(const char* characters) {
	CallString(Message::SetWordChars, 0, characters);
}

int TextEditor::WordChars(char* characters) {
	return static_cast<int>(CallPointer(Message::GetWordChars, 0, characters));
}

std::string TextEditor::WordChars() {
	return CallReturnString(Message::GetWordChars, 0);
}

void TextEditor::SetCharacterCategoryOptimization(int countCharacters) {
	Call(Message::SetCharacterCategoryOptimization, countCharacters);
}

int TextEditor::CharacterCategoryOptimization() {
	return static_cast<int>(Call(Message::GetCharacterCategoryOptimization));
}

void TextEditor::BeginUndoAction() {
	Call(Message::BeginUndoAction);
}

void TextEditor::EndUndoAction() {
	Call(Message::EndUndoAction);
}

void TextEditor::IndicSetStyle(int indicator, KLWidgets::IndicatorStyle indicatorStyle) {
	Call(Message::IndicSetStyle, indicator, static_cast<intptr_t>(indicatorStyle));
}

IndicatorStyle TextEditor::IndicGetStyle(int indicator) {
	return static_cast<KLWidgets::IndicatorStyle>(Call(Message::IndicGetStyle, indicator));
}

void TextEditor::IndicSetFore(int indicator, Colour fore) {
	Call(Message::IndicSetFore, indicator, fore);
}

Colour TextEditor::IndicGetFore(int indicator) {
	return static_cast<Colour>(Call(Message::IndicGetFore, indicator));
}

void TextEditor::IndicSetUnder(int indicator, bool under) {
	Call(Message::IndicSetUnder, indicator, under);
}

bool TextEditor::IndicGetUnder(int indicator) {
	return Call(Message::IndicGetUnder, indicator);
}

void TextEditor::IndicSetHoverStyle(int indicator, KLWidgets::IndicatorStyle indicatorStyle) {
	Call(Message::IndicSetHoverStyle, indicator, static_cast<intptr_t>(indicatorStyle));
}

IndicatorStyle TextEditor::IndicGetHoverStyle(int indicator) {
	return static_cast<KLWidgets::IndicatorStyle>(Call(Message::IndicGetHoverStyle, indicator));
}

void TextEditor::IndicSetHoverFore(int indicator, Colour fore) {
	Call(Message::IndicSetHoverFore, indicator, fore);
}

Colour TextEditor::IndicGetHoverFore(int indicator) {
	return static_cast<Colour>(Call(Message::IndicGetHoverFore, indicator));
}

void TextEditor::IndicSetFlags(int indicator, KLWidgets::IndicFlag flags) {
	Call(Message::IndicSetFlags, indicator, static_cast<intptr_t>(flags));
}

IndicFlag TextEditor::IndicGetFlags(int indicator) {
	return static_cast<KLWidgets::IndicFlag>(Call(Message::IndicGetFlags, indicator));
}

void TextEditor::IndicSetStrokeWidth(int indicator, int hundredths) {
	Call(Message::IndicSetStrokeWidth, indicator, hundredths);
}

int TextEditor::IndicGetStrokeWidth(int indicator) {
	return static_cast<int>(Call(Message::IndicGetStrokeWidth, indicator));
}

void TextEditor::SetWhitespaceFore(bool useSetting, Colour fore) {
	Call(Message::SetWhitespaceFore, useSetting, fore);
}

void TextEditor::SetWhitespaceBack(bool useSetting, Colour back) {
	Call(Message::SetWhitespaceBack, useSetting, back);
}

void TextEditor::SetWhitespaceSize(int size) {
	Call(Message::SetWhitespaceSize, size);
}

int TextEditor::WhitespaceSize() {
	return static_cast<int>(Call(Message::GetWhitespaceSize));
}

void TextEditor::SetLineState(Line line, int state) {
	Call(Message::SetLineState, line, state);
}

int TextEditor::LineState(Line line) {
	return static_cast<int>(Call(Message::GetLineState, line));
}

int TextEditor::MaxLineState() {
	return static_cast<int>(Call(Message::GetMaxLineState));
}

bool TextEditor::CaretLineVisible() {
	return Call(Message::GetCaretLineVisible);
}

void TextEditor::SetCaretLineVisible(bool show) {
	Call(Message::SetCaretLineVisible, show);
}

Colour TextEditor::CaretLineBack() {
	return static_cast<Colour>(Call(Message::GetCaretLineBack));
}

void TextEditor::SetCaretLineBack(Colour back) {
	Call(Message::SetCaretLineBack, back);
}

int TextEditor::CaretLineFrame() {
	return static_cast<int>(Call(Message::GetCaretLineFrame));
}

void TextEditor::SetCaretLineFrame(int width) {
	Call(Message::SetCaretLineFrame, width);
}

void TextEditor::StyleSetChangeable(int style, bool changeable) {
	Call(Message::StyleSetChangeable, style, changeable);
}

void TextEditor::AutoCShow(Position lengthEntered, const char* itemList) {
	CallString(Message::AutoCShow, lengthEntered, itemList);
}

void TextEditor::AutoCCancel() {
	Call(Message::AutoCCancel);
}

bool TextEditor::AutoCActive() {
	return Call(Message::AutoCActive);
}

Position TextEditor::AutoCPosStart() {
	return Call(Message::AutoCPosStart);
}

void TextEditor::AutoCComplete() {
	Call(Message::AutoCComplete);
}

void TextEditor::AutoCStops(const char* characterSet) {
	CallString(Message::AutoCStops, 0, characterSet);
}

void TextEditor::AutoCSetSeparator(int separatorCharacter) {
	Call(Message::AutoCSetSeparator, separatorCharacter);
}

int TextEditor::AutoCGetSeparator() {
	return static_cast<int>(Call(Message::AutoCGetSeparator));
}

void TextEditor::AutoCSelect(const char* select) {
	CallString(Message::AutoCSelect, 0, select);
}

void TextEditor::AutoCSetCancelAtStart(bool cancel) {
	Call(Message::AutoCSetCancelAtStart, cancel);
}

bool TextEditor::AutoCGetCancelAtStart() {
	return Call(Message::AutoCGetCancelAtStart);
}

void TextEditor::AutoCSetFillUps(const char* characterSet) {
	CallString(Message::AutoCSetFillUps, 0, characterSet);
}

void TextEditor::AutoCSetChooseSingle(bool chooseSingle) {
	Call(Message::AutoCSetChooseSingle, chooseSingle);
}

bool TextEditor::AutoCGetChooseSingle() {
	return Call(Message::AutoCGetChooseSingle);
}

void TextEditor::AutoCSetIgnoreCase(bool ignoreCase) {
	Call(Message::AutoCSetIgnoreCase, ignoreCase);
}

bool TextEditor::AutoCGetIgnoreCase() {
	return Call(Message::AutoCGetIgnoreCase);
}

void TextEditor::UserListShow(int listType, const char* itemList) {
	CallString(Message::UserListShow, listType, itemList);
}

void TextEditor::AutoCSetAutoHide(bool autoHide) {
	Call(Message::AutoCSetAutoHide, autoHide);
}

bool TextEditor::AutoCGetAutoHide() {
	return Call(Message::AutoCGetAutoHide);
}

void TextEditor::AutoCSetDropRestOfWord(bool dropRestOfWord) {
	Call(Message::AutoCSetDropRestOfWord, dropRestOfWord);
}

bool TextEditor::AutoCGetDropRestOfWord() {
	return Call(Message::AutoCGetDropRestOfWord);
}

void TextEditor::RegisterImage(int type, const char* xpmData) {
	CallString(Message::RegisterImage, type, xpmData);
}

void TextEditor::ClearRegisteredImages() {
	Call(Message::ClearRegisteredImages);
}

int TextEditor::AutoCGetTypeSeparator() {
	return static_cast<int>(Call(Message::AutoCGetTypeSeparator));
}

void TextEditor::AutoCSetTypeSeparator(int separatorCharacter) {
	Call(Message::AutoCSetTypeSeparator, separatorCharacter);
}

void TextEditor::AutoCSetMaxWidth(int characterCount) {
	Call(Message::AutoCSetMaxWidth, characterCount);
}

int TextEditor::AutoCGetMaxWidth() {
	return static_cast<int>(Call(Message::AutoCGetMaxWidth));
}

void TextEditor::AutoCSetMaxHeight(int rowCount) {
	Call(Message::AutoCSetMaxHeight, rowCount);
}

int TextEditor::AutoCGetMaxHeight() {
	return static_cast<int>(Call(Message::AutoCGetMaxHeight));
}

void TextEditor::SetIndent(int indentSize) {
	Call(Message::SetIndent, indentSize);
}

int TextEditor::Indent() {
	return static_cast<int>(Call(Message::GetIndent));
}

void TextEditor::SetUseTabs(bool useTabs) {
	Call(Message::SetUseTabs, useTabs);
}

bool TextEditor::UseTabs() {
	return Call(Message::GetUseTabs);
}

void TextEditor::SetLineIndentation(Line line, int indentation) {
	Call(Message::SetLineIndentation, line, indentation);
}

int TextEditor::LineIndentation(Line line) {
	return static_cast<int>(Call(Message::GetLineIndentation, line));
}

Position TextEditor::LineIndentPosition(Line line) {
	return Call(Message::GetLineIndentPosition, line);
}

Position TextEditor::Column(Position pos) {
	return Call(Message::GetColumn, pos);
}

Position TextEditor::CountCharacters(Position start, Position end) {
	return Call(Message::CountCharacters, start, end);
}

Position TextEditor::CountCodeUnits(Position start, Position end) {
	return Call(Message::CountCodeUnits, start, end);
}

void TextEditor::SetHScrollBar(bool visible) {
	Call(Message::SetHScrollBar, visible);
}

bool TextEditor::HScrollBar() {
	return Call(Message::GetHScrollBar);
}

void TextEditor::SetIndentationGuides(KLWidgets::IndentView indentView) {
	Call(Message::SetIndentationGuides, static_cast<uintptr_t>(indentView));
}

IndentView TextEditor::IndentationGuides() {
	return static_cast<KLWidgets::IndentView>(Call(Message::GetIndentationGuides));
}

void TextEditor::SetHighlightGuide(Position column) {
	Call(Message::SetHighlightGuide, column);
}

Position TextEditor::HighlightGuide() {
	return Call(Message::GetHighlightGuide);
}

Position TextEditor::LineEndPosition(Line line) {
	return Call(Message::GetLineEndPosition, line);
}

int TextEditor::CodePage() {
	return static_cast<int>(Call(Message::GetCodePage));
}

Colour TextEditor::CaretFore() {
	return static_cast<Colour>(Call(Message::GetCaretFore));
}

bool TextEditor::ReadOnly() {
	return Call(Message::GetReadOnly);
}

void TextEditor::SetCurrentPos(Position caret) {
	Call(Message::SetCurrentPos, caret);
}

void TextEditor::SetSelectionStart(Position anchor) {
	Call(Message::SetSelectionStart, anchor);
}

Position TextEditor::SelectionStart() {
	return Call(Message::GetSelectionStart);
}

void TextEditor::SetSelectionEnd(Position caret) {
	Call(Message::SetSelectionEnd, caret);
}

Position TextEditor::SelectionEnd() {
	return Call(Message::GetSelectionEnd);
}

void TextEditor::SetEmptySelection(Position caret) {
	Call(Message::SetEmptySelection, caret);
}

void TextEditor::SetPrintMagnification(int magnification) {
	Call(Message::SetPrintMagnification, magnification);
}

int TextEditor::PrintMagnification() {
	return static_cast<int>(Call(Message::GetPrintMagnification));
}

void TextEditor::SetPrintColourMode(KLWidgets::PrintOption mode) {
	Call(Message::SetPrintColourMode, static_cast<uintptr_t>(mode));
}

PrintOption TextEditor::PrintColourMode() {
	return static_cast<KLWidgets::PrintOption>(Call(Message::GetPrintColourMode));
}

Position TextEditor::FindText(KLWidgets::FindOption searchFlags, void* ft) {
	return CallPointer(Message::FindText, static_cast<uintptr_t>(searchFlags), ft);
}

Position TextEditor::FormatRange(bool draw, void* fr) {
	return CallPointer(Message::FormatRange, draw, fr);
}

Line TextEditor::FirstVisibleLine() {
	return Call(Message::GetFirstVisibleLine);
}

Position TextEditor::GetLine(Line line, char* text) {
	return CallPointer(Message::GetLine, line, text);
}

std::string TextEditor::GetLine(Line line) {
	return CallReturnString(Message::GetLine, line);
}

Line TextEditor::LineCount() {
	return Call(Message::GetLineCount);
}

void TextEditor::SetMarginLeft(int pixelWidth) {
	Call(Message::SetMarginLeft, 0, pixelWidth);
}

int TextEditor::MarginLeft() {
	return static_cast<int>(Call(Message::GetMarginLeft));
}

void TextEditor::SetMarginRight(int pixelWidth) {
	Call(Message::SetMarginRight, 0, pixelWidth);
}

int TextEditor::MarginRight() {
	return static_cast<int>(Call(Message::GetMarginRight));
}

bool TextEditor::Modify() {
	return Call(Message::GetModify);
}

void TextEditor::SetSel(Position anchor, Position caret) {
	Call(Message::SetSel, anchor, caret);
}

Position TextEditor::GetSelText(char* text) {
	return CallPointer(Message::GetSelText, 0, text);
}

std::string TextEditor::GetSelText() {
	return CallReturnString(Message::GetSelText, 0);
}

Position TextEditor::GetTextRange(void* tr) {
	return CallPointer(Message::GetTextRange, 0, tr);
}

void TextEditor::HideSelection(bool hide) {
	Call(Message::HideSelection, hide);
}

int TextEditor::PointXFromPosition(Position pos) {
	return static_cast<int>(Call(Message::PointXFromPosition, 0, pos));
}

int TextEditor::PointYFromPosition(Position pos) {
	return static_cast<int>(Call(Message::PointYFromPosition, 0, pos));
}

Line TextEditor::LineFromPosition(Position pos) {
	return Call(Message::LineFromPosition, pos);
}

Position TextEditor::PositionFromLine(Line line) {
	return Call(Message::PositionFromLine, line);
}

void TextEditor::LineScroll(Position columns, Line lines) {
	Call(Message::LineScroll, columns, lines);
}

void TextEditor::ScrollCaret() {
	Call(Message::ScrollCaret);
}

void TextEditor::ScrollRange(Position secondary, Position primary) {
	Call(Message::ScrollRange, secondary, primary);
}

void TextEditor::ReplaceSel(const char* text) {
	CallString(Message::ReplaceSel, 0, text);
}

void TextEditor::SetReadOnly(bool readOnly) {
	Call(Message::SetReadOnly, readOnly);
}

void TextEditor::Null() {
	Call(Message::Null);
}

bool TextEditor::CanPaste() {
	return Call(Message::CanPaste);
}

bool TextEditor::CanUndo() {
	return Call(Message::CanUndo);
}

void TextEditor::EmptyUndoBuffer() {
	Call(Message::EmptyUndoBuffer);
}

void TextEditor::Undo() {
	Call(Message::Undo);
}

void TextEditor::Cut() {
	Call(Message::Cut);
}

void TextEditor::Copy() {
	Call(Message::Copy);
}

void TextEditor::Paste() {
	Call(Message::Paste);
}

void TextEditor::Clear() {
	Call(Message::Clear);
}

void TextEditor::SetText(const char* text) {
	CallString(Message::SetText, 0, text);
}

Position TextEditor::GetText(Position length, char* text) {
	return CallPointer(Message::GetText, length, text);
}

std::string TextEditor::GetText(Position length) {
	return CallReturnString(Message::GetText, length);
}

Position TextEditor::TextLength() {
	return Call(Message::GetTextLength);
}

void* TextEditor::DirectFunction() {
	return reinterpret_cast<void*>(Call(Message::GetDirectFunction));
}

void* TextEditor::DirectPointer() {
	return reinterpret_cast<void*>(Call(Message::GetDirectPointer));
}

void TextEditor::SetOvertype(bool overType) {
	Call(Message::SetOvertype, overType);
}

bool TextEditor::Overtype() {
	return Call(Message::GetOvertype);
}

void TextEditor::SetCaretWidth(int pixelWidth) {
	Call(Message::SetCaretWidth, pixelWidth);
}

int TextEditor::CaretWidth() {
	return static_cast<int>(Call(Message::GetCaretWidth));
}

void TextEditor::SetTargetStart(Position start) {
	Call(Message::SetTargetStart, start);
}

Position TextEditor::TargetStart() {
	return Call(Message::GetTargetStart);
}

void TextEditor::SetTargetStartVirtualSpace(Position space) {
	Call(Message::SetTargetStartVirtualSpace, space);
}

Position TextEditor::TargetStartVirtualSpace() {
	return Call(Message::GetTargetStartVirtualSpace);
}

void TextEditor::SetTargetEnd(Position end) {
	Call(Message::SetTargetEnd, end);
}

Position TextEditor::TargetEnd() {
	return Call(Message::GetTargetEnd);
}

void TextEditor::SetTargetEndVirtualSpace(Position space) {
	Call(Message::SetTargetEndVirtualSpace, space);
}

Position TextEditor::TargetEndVirtualSpace() {
	return Call(Message::GetTargetEndVirtualSpace);
}

void TextEditor::SetTargetRange(Position start, Position end) {
	Call(Message::SetTargetRange, start, end);
}

Position TextEditor::TargetText(char* text) {
	return CallPointer(Message::GetTargetText, 0, text);
}

std::string TextEditor::TargetText() {
	return CallReturnString(Message::GetTargetText, 0);
}

void TextEditor::TargetFromSelection() {
	Call(Message::TargetFromSelection);
}

void TextEditor::TargetWholeDocument() {
	Call(Message::TargetWholeDocument);
}

Position TextEditor::ReplaceTarget(Position length, const char* text) {
	return CallString(Message::ReplaceTarget, length, text);
}

Position TextEditor::ReplaceTargetRE(Position length, const char* text) {
	return CallString(Message::ReplaceTargetRE, length, text);
}

Position TextEditor::SearchInTarget(Position length, const char* text) {
	return CallString(Message::SearchInTarget, length, text);
}

void TextEditor::SetSearchFlags(KLWidgets::FindOption searchFlags) {
	Call(Message::SetSearchFlags, static_cast<uintptr_t>(searchFlags));
}

FindOption TextEditor::SearchFlags() {
	return static_cast<KLWidgets::FindOption>(Call(Message::GetSearchFlags));
}

void TextEditor::CallTipShow(Position pos, const char* definition) {
	CallString(Message::CallTipShow, pos, definition);
}

void TextEditor::CallTipCancel() {
	Call(Message::CallTipCancel);
}

bool TextEditor::CallTipActive() {
	return Call(Message::CallTipActive);
}

Position TextEditor::CallTipPosStart() {
	return Call(Message::CallTipPosStart);
}

void TextEditor::CallTipSetPosStart(Position posStart) {
	Call(Message::CallTipSetPosStart, posStart);
}

void TextEditor::CallTipSetHlt(Position highlightStart, Position highlightEnd) {
	Call(Message::CallTipSetHlt, highlightStart, highlightEnd);
}

void TextEditor::CallTipSetBack(Colour back) {
	Call(Message::CallTipSetBack, back);
}

void TextEditor::CallTipSetFore(Colour fore) {
	Call(Message::CallTipSetFore, fore);
}

void TextEditor::CallTipSetForeHlt(Colour fore) {
	Call(Message::CallTipSetForeHlt, fore);
}

void TextEditor::CallTipUseStyle(int tabSize) {
	Call(Message::CallTipUseStyle, tabSize);
}

void TextEditor::CallTipSetPosition(bool above) {
	Call(Message::CallTipSetPosition, above);
}

Line TextEditor::VisibleFromDocLine(Line docLine) {
	return Call(Message::VisibleFromDocLine, docLine);
}

Line TextEditor::DocLineFromVisible(Line displayLine) {
	return Call(Message::DocLineFromVisible, displayLine);
}

Line TextEditor::WrapCount(Line docLine) {
	return Call(Message::WrapCount, docLine);
}

void TextEditor::SetFoldLevel(Line line, KLWidgets::FoldLevel level) {
	Call(Message::SetFoldLevel, line, static_cast<intptr_t>(level));
}

FoldLevel TextEditor::FoldLevel(Line line) {
	return static_cast<KLWidgets::FoldLevel>(Call(Message::GetFoldLevel, line));
}

Line TextEditor::LastChild(Line line, KLWidgets::FoldLevel level) {
	return Call(Message::GetLastChild, line, static_cast<intptr_t>(level));
}

Line TextEditor::FoldParent(Line line) {
	return Call(Message::GetFoldParent, line);
}

void TextEditor::ShowLines(Line lineStart, Line lineEnd) {
	Call(Message::ShowLines, lineStart, lineEnd);
}

void TextEditor::HideLines(Line lineStart, Line lineEnd) {
	Call(Message::HideLines, lineStart, lineEnd);
}

bool TextEditor::LineVisible(Line line) {
	return Call(Message::GetLineVisible, line);
}

bool TextEditor::AllLinesVisible() {
	return Call(Message::GetAllLinesVisible);
}

void TextEditor::SetFoldExpanded(Line line, bool expanded) {
	Call(Message::SetFoldExpanded, line, expanded);
}

bool TextEditor::FoldExpanded(Line line) {
	return Call(Message::GetFoldExpanded, line);
}

void TextEditor::ToggleFold(Line line) {
	Call(Message::ToggleFold, line);
}

void TextEditor::ToggleFoldShowText(Line line, const char* text) {
	CallString(Message::ToggleFoldShowText, line, text);
}

void TextEditor::FoldDisplayTextSetStyle(KLWidgets::FoldDisplayTextStyle style) {
	Call(Message::FoldDisplayTextSetStyle, static_cast<uintptr_t>(style));
}

FoldDisplayTextStyle TextEditor::FoldDisplayTextGetStyle() {
	return static_cast<KLWidgets::FoldDisplayTextStyle>(Call(Message::FoldDisplayTextGetStyle));
}

void TextEditor::SetDefaultFoldDisplayText(const char* text) {
	CallString(Message::SetDefaultFoldDisplayText, 0, text);
}

int TextEditor::GetDefaultFoldDisplayText(char* text) {
	return static_cast<int>(CallPointer(Message::GetDefaultFoldDisplayText, 0, text));
}

std::string TextEditor::GetDefaultFoldDisplayText() {
	return CallReturnString(Message::GetDefaultFoldDisplayText, 0);
}

void TextEditor::FoldLine(Line line, KLWidgets::FoldAction action) {
	Call(Message::FoldLine, line, static_cast<intptr_t>(action));
}

void TextEditor::FoldChildren(Line line, KLWidgets::FoldAction action) {
	Call(Message::FoldChildren, line, static_cast<intptr_t>(action));
}

void TextEditor::ExpandChildren(Line line, KLWidgets::FoldLevel level) {
	Call(Message::ExpandChildren, line, static_cast<intptr_t>(level));
}

void TextEditor::FoldAll(KLWidgets::FoldAction action) {
	Call(Message::FoldAll, static_cast<uintptr_t>(action));
}

void TextEditor::EnsureVisible(Line line) {
	Call(Message::EnsureVisible, line);
}

void TextEditor::SetAutomaticFold(KLWidgets::AutomaticFold automaticFold) {
	Call(Message::SetAutomaticFold, static_cast<uintptr_t>(automaticFold));
}

AutomaticFold TextEditor::AutomaticFold() {
	return static_cast<KLWidgets::AutomaticFold>(Call(Message::GetAutomaticFold));
}

void TextEditor::SetFoldFlags(KLWidgets::FoldFlag flags) {
	Call(Message::SetFoldFlags, static_cast<uintptr_t>(flags));
}

void TextEditor::EnsureVisibleEnforcePolicy(Line line) {
	Call(Message::EnsureVisibleEnforcePolicy, line);
}

void TextEditor::SetTabIndents(bool tabIndents) {
	Call(Message::SetTabIndents, tabIndents);
}

bool TextEditor::TabIndents() {
	return Call(Message::GetTabIndents);
}

void TextEditor::SetBackSpaceUnIndents(bool bsUnIndents) {
	Call(Message::SetBackSpaceUnIndents, bsUnIndents);
}

bool TextEditor::BackSpaceUnIndents() {
	return Call(Message::GetBackSpaceUnIndents);
}

void TextEditor::SetMouseDwellTime(int periodMilliseconds) {
	Call(Message::SetMouseDwellTime, periodMilliseconds);
}

int TextEditor::MouseDwellTime() {
	return static_cast<int>(Call(Message::GetMouseDwellTime));
}

Position TextEditor::WordStartPosition(Position pos, bool onlyWordCharacters) {
	return Call(Message::WordStartPosition, pos, onlyWordCharacters);
}

Position TextEditor::WordEndPosition(Position pos, bool onlyWordCharacters) {
	return Call(Message::WordEndPosition, pos, onlyWordCharacters);
}

bool TextEditor::IsRangeWord(Position start, Position end) {
	return Call(Message::IsRangeWord, start, end);
}

void TextEditor::SetIdleStyling(KLWidgets::IdleStyling idleStyling) {
	Call(Message::SetIdleStyling, static_cast<uintptr_t>(idleStyling));
}

IdleStyling TextEditor::IdleStyling() {
	return static_cast<KLWidgets::IdleStyling>(Call(Message::GetIdleStyling));
}

void TextEditor::SetWrapMode(KLWidgets::Wrap wrapMode) {
	Call(Message::SetWrapMode, static_cast<uintptr_t>(wrapMode));
}

Wrap TextEditor::WrapMode() {
	return static_cast<KLWidgets::Wrap>(Call(Message::GetWrapMode));
}

void TextEditor::SetWrapVisualFlags(KLWidgets::WrapVisualFlag wrapVisualFlags) {
	Call(Message::SetWrapVisualFlags, static_cast<uintptr_t>(wrapVisualFlags));
}

WrapVisualFlag TextEditor::WrapVisualFlags() {
	return static_cast<KLWidgets::WrapVisualFlag>(Call(Message::GetWrapVisualFlags));
}

void TextEditor::SetWrapVisualFlagsLocation(KLWidgets::WrapVisualLocation wrapVisualFlagsLocation) {
	Call(Message::SetWrapVisualFlagsLocation, static_cast<uintptr_t>(wrapVisualFlagsLocation));
}

WrapVisualLocation TextEditor::WrapVisualFlagsLocation() {
	return static_cast<KLWidgets::WrapVisualLocation>(Call(Message::GetWrapVisualFlagsLocation));
}

void TextEditor::SetWrapStartIndent(int indent) {
	Call(Message::SetWrapStartIndent, indent);
}

int TextEditor::WrapStartIndent() {
	return static_cast<int>(Call(Message::GetWrapStartIndent));
}

void TextEditor::SetWrapIndentMode(KLWidgets::WrapIndentMode wrapIndentMode) {
	Call(Message::SetWrapIndentMode, static_cast<uintptr_t>(wrapIndentMode));
}

WrapIndentMode TextEditor::WrapIndentMode() {
	return static_cast<KLWidgets::WrapIndentMode>(Call(Message::GetWrapIndentMode));
}

void TextEditor::SetLayoutCache(KLWidgets::LineCache cacheMode) {
	Call(Message::SetLayoutCache, static_cast<uintptr_t>(cacheMode));
}

LineCache TextEditor::LayoutCache() {
	return static_cast<KLWidgets::LineCache>(Call(Message::GetLayoutCache));
}

void TextEditor::SetScrollWidth(int pixelWidth) {
	Call(Message::SetScrollWidth, pixelWidth);
}

int TextEditor::ScrollWidth() {
	return static_cast<int>(Call(Message::GetScrollWidth));
}

void TextEditor::SetScrollWidthTracking(bool tracking) {
	Call(Message::SetScrollWidthTracking, tracking);
}

bool TextEditor::ScrollWidthTracking() {
	return Call(Message::GetScrollWidthTracking);
}

int TextEditor::TextWidth(int style, const char* text) {
	return static_cast<int>(CallString(Message::TextWidth, style, text));
}

void TextEditor::SetEndAtLastLine(bool endAtLastLine) {
	Call(Message::SetEndAtLastLine, endAtLastLine);
}

bool TextEditor::EndAtLastLine() {
	return Call(Message::GetEndAtLastLine);
}

int TextEditor::TextHeight(Line line) {
	return static_cast<int>(Call(Message::TextHeight, line));
}

void TextEditor::SetVScrollBar(bool visible) {
	Call(Message::SetVScrollBar, visible);
}

bool TextEditor::VScrollBar() {
	return Call(Message::GetVScrollBar);
}

void TextEditor::AppendText(Position length, const char* text) {
	CallString(Message::AppendText, length, text);
}

PhasesDraw TextEditor::PhasesDraw() {
	return static_cast<KLWidgets::PhasesDraw>(Call(Message::GetPhasesDraw));
}

void TextEditor::SetPhasesDraw(KLWidgets::PhasesDraw phases) {
	Call(Message::SetPhasesDraw, static_cast<uintptr_t>(phases));
}

void TextEditor::SetFontQuality(KLWidgets::FontQuality fontQuality) {
	Call(Message::SetFontQuality, static_cast<uintptr_t>(fontQuality));
}

FontQuality TextEditor::FontQuality() {
	return static_cast<KLWidgets::FontQuality>(Call(Message::GetFontQuality));
}

void TextEditor::SetFirstVisibleLine(Line displayLine) {
	Call(Message::SetFirstVisibleLine, displayLine);
}

void TextEditor::SetMultiPaste(KLWidgets::MultiPaste multiPaste) {
	Call(Message::SetMultiPaste, static_cast<uintptr_t>(multiPaste));
}

MultiPaste TextEditor::MultiPaste() {
	return static_cast<KLWidgets::MultiPaste>(Call(Message::GetMultiPaste));
}

int TextEditor::Tag(int tagNumber, char* tagValue) {
	return static_cast<int>(CallPointer(Message::GetTag, tagNumber, tagValue));
}

std::string TextEditor::Tag(int tagNumber) {
	return CallReturnString(Message::GetTag, tagNumber);
}

void TextEditor::LinesJoin() {
	Call(Message::LinesJoin);
}

void TextEditor::LinesSplit(int pixelWidth) {
	Call(Message::LinesSplit, pixelWidth);
}

void TextEditor::SetFoldMarginColour(bool useSetting, Colour back) {
	Call(Message::SetFoldMarginColour, useSetting, back);
}

void TextEditor::SetFoldMarginHiColour(bool useSetting, Colour fore) {
	Call(Message::SetFoldMarginHiColour, useSetting, fore);
}

void TextEditor::SetAccessibility(KLWidgets::Accessibility accessibility) {
	Call(Message::SetAccessibility, static_cast<uintptr_t>(accessibility));
}

Accessibility TextEditor::Accessibility() {
	return static_cast<KLWidgets::Accessibility>(Call(Message::GetAccessibility));
}

void TextEditor::LineDown() {
	Call(Message::LineDown);
}

void TextEditor::LineDownExtend() {
	Call(Message::LineDownExtend);
}

void TextEditor::LineUp() {
	Call(Message::LineUp);
}

void TextEditor::LineUpExtend() {
	Call(Message::LineUpExtend);
}

void TextEditor::CharLeft() {
	Call(Message::CharLeft);
}

void TextEditor::CharLeftExtend() {
	Call(Message::CharLeftExtend);
}

void TextEditor::CharRight() {
	Call(Message::CharRight);
}

void TextEditor::CharRightExtend() {
	Call(Message::CharRightExtend);
}

void TextEditor::WordLeft() {
	Call(Message::WordLeft);
}

void TextEditor::WordLeftExtend() {
	Call(Message::WordLeftExtend);
}

void TextEditor::WordRight() {
	Call(Message::WordRight);
}

void TextEditor::WordRightExtend() {
	Call(Message::WordRightExtend);
}

void TextEditor::Home() {
	Call(Message::Home);
}

void TextEditor::HomeExtend() {
	Call(Message::HomeExtend);
}

void TextEditor::LineEnd() {
	Call(Message::LineEnd);
}

void TextEditor::LineEndExtend() {
	Call(Message::LineEndExtend);
}

void TextEditor::DocumentStart() {
	Call(Message::DocumentStart);
}

void TextEditor::DocumentStartExtend() {
	Call(Message::DocumentStartExtend);
}

void TextEditor::DocumentEnd() {
	Call(Message::DocumentEnd);
}

void TextEditor::DocumentEndExtend() {
	Call(Message::DocumentEndExtend);
}

void TextEditor::PageUp() {
	Call(Message::PageUp);
}

void TextEditor::PageUpExtend() {
	Call(Message::PageUpExtend);
}

void TextEditor::PageDown() {
	Call(Message::PageDown);
}

void TextEditor::PageDownExtend() {
	Call(Message::PageDownExtend);
}

void TextEditor::EditToggleOvertype() {
	Call(Message::EditToggleOvertype);
}

void TextEditor::CancelN() {
	Call(Message::Cancel);
}

void TextEditor::DeleteBack() {
	Call(Message::DeleteBack);
}

void TextEditor::Tab() {
	Call(Message::Tab);
}

void TextEditor::BackTab() {
	Call(Message::BackTab);
}

void TextEditor::NewLine() {
	Call(Message::NewLine);
}

void TextEditor::FormFeed() {
	Call(Message::FormFeed);
}

void TextEditor::VCHome() {
	Call(Message::VCHome);
}

void TextEditor::VCHomeExtend() {
	Call(Message::VCHomeExtend);
}

void TextEditor::ZoomIn() {
	Call(Message::ZoomIn);
}

void TextEditor::ZoomOut() {
	Call(Message::ZoomOut);
}

void TextEditor::DelWordLeft() {
	Call(Message::DelWordLeft);
}

void TextEditor::DelWordRight() {
	Call(Message::DelWordRight);
}

void TextEditor::DelWordRightEnd() {
	Call(Message::DelWordRightEnd);
}

void TextEditor::LineCut() {
	Call(Message::LineCut);
}

void TextEditor::LineDelete() {
	Call(Message::LineDelete);
}

void TextEditor::LineTranspose() {
	Call(Message::LineTranspose);
}

void TextEditor::LineReverse() {
	Call(Message::LineReverse);
}

void TextEditor::LineDuplicate() {
	Call(Message::LineDuplicate);
}

void TextEditor::LowerCase() {
	Call(Message::LowerCase);
}

void TextEditor::UpperCase() {
	Call(Message::UpperCase);
}

void TextEditor::LineScrollDown() {
	Call(Message::LineScrollDown);
}

void TextEditor::LineScrollUp() {
	Call(Message::LineScrollUp);
}

void TextEditor::DeleteBackNotLine() {
	Call(Message::DeleteBackNotLine);
}

void TextEditor::HomeDisplay() {
	Call(Message::HomeDisplay);
}

void TextEditor::HomeDisplayExtend() {
	Call(Message::HomeDisplayExtend);
}

void TextEditor::LineEndDisplay() {
	Call(Message::LineEndDisplay);
}

void TextEditor::LineEndDisplayExtend() {
	Call(Message::LineEndDisplayExtend);
}

void TextEditor::HomeWrap() {
	Call(Message::HomeWrap);
}

void TextEditor::HomeWrapExtend() {
	Call(Message::HomeWrapExtend);
}

void TextEditor::LineEndWrap() {
	Call(Message::LineEndWrap);
}

void TextEditor::LineEndWrapExtend() {
	Call(Message::LineEndWrapExtend);
}

void TextEditor::VCHomeWrap() {
	Call(Message::VCHomeWrap);
}

void TextEditor::VCHomeWrapExtend() {
	Call(Message::VCHomeWrapExtend);
}

void TextEditor::LineCopy() {
	Call(Message::LineCopy);
}

void TextEditor::MoveCaretInsideView() {
	Call(Message::MoveCaretInsideView);
}

Position TextEditor::LineLength(Line line) {
	return Call(Message::LineLength, line);
}

void TextEditor::BraceHighlight(Position posA, Position posB) {
	Call(Message::BraceHighlight, posA, posB);
}

void TextEditor::BraceHighlightIndicator(bool useSetting, int indicator) {
	Call(Message::BraceHighlightIndicator, useSetting, indicator);
}

void TextEditor::BraceBadLight(Position pos) {
	Call(Message::BraceBadLight, pos);
}

void TextEditor::BraceBadLightIndicator(bool useSetting, int indicator) {
	Call(Message::BraceBadLightIndicator, useSetting, indicator);
}

Position TextEditor::BraceMatch(Position pos, int maxReStyle) {
	return Call(Message::BraceMatch, pos, maxReStyle);
}

Position TextEditor::BraceMatchNext(Position pos, Position startPos) {
	return Call(Message::BraceMatchNext, pos, startPos);
}

bool TextEditor::ViewEOL() {
	return Call(Message::GetViewEOL);
}

void TextEditor::SetViewEOL(bool visible) {
	Call(Message::SetViewEOL, visible);
}

void* TextEditor::DocPointer() {
	return reinterpret_cast<void*>(Call(Message::GetDocPointer));
}

void TextEditor::SetDocPointer(void* doc) {
	CallPointer(Message::SetDocPointer, 0, doc);
}

void TextEditor::SetModEventMask(KLWidgets::ModificationFlags eventMask) {
	Call(Message::SetModEventMask, static_cast<uintptr_t>(eventMask));
}

Position TextEditor::EdgeColumn() {
	return Call(Message::GetEdgeColumn);
}

void TextEditor::SetEdgeColumn(Position column) {
	Call(Message::SetEdgeColumn, column);
}

EdgeVisualStyle TextEditor::EdgeMode() {
	return static_cast<KLWidgets::EdgeVisualStyle>(Call(Message::GetEdgeMode));
}

void TextEditor::SetEdgeMode(KLWidgets::EdgeVisualStyle edgeMode) {
	Call(Message::SetEdgeMode, static_cast<uintptr_t>(edgeMode));
}

Colour TextEditor::EdgeColour() {
	return static_cast<Colour>(Call(Message::GetEdgeColour));
}

void TextEditor::SetEdgeColour(Colour edgeColour) {
	Call(Message::SetEdgeColour, edgeColour);
}

void TextEditor::MultiEdgeAddLine(Position column, Colour edgeColour) {
	Call(Message::MultiEdgeAddLine, column, edgeColour);
}

void TextEditor::MultiEdgeClearAll() {
	Call(Message::MultiEdgeClearAll);
}

Position TextEditor::MultiEdgeColumn(int which) {
	return Call(Message::GetMultiEdgeColumn, which);
}

void TextEditor::SearchAnchor() {
	Call(Message::SearchAnchor);
}

Position TextEditor::SearchNext(KLWidgets::FindOption searchFlags, const char* text) {
	return CallString(Message::SearchNext, static_cast<uintptr_t>(searchFlags), text);
}

Position TextEditor::SearchPrev(KLWidgets::FindOption searchFlags, const char* text) {
	return CallString(Message::SearchPrev, static_cast<uintptr_t>(searchFlags), text);
}

Line TextEditor::LinesOnScreen() {
	return Call(Message::LinesOnScreen);
}

void TextEditor::UsePopUp(KLWidgets::PopUp popUpMode) {
	Call(Message::UsePopUp, static_cast<uintptr_t>(popUpMode));
}

bool TextEditor::SelectionIsRectangle() {
	return Call(Message::SelectionIsRectangle);
}

void TextEditor::SetZoom(int zoomInPoints) {
	Call(Message::SetZoom, zoomInPoints);
}

int TextEditor::Zoom() {
	return static_cast<int>(Call(Message::GetZoom));
}

void* TextEditor::CreateDocument(Position bytes, KLWidgets::DocumentOption documentOptions) {
	return reinterpret_cast<void*>(Call(Message::CreateDocument, bytes, static_cast<intptr_t>(documentOptions)));
}

void TextEditor::AddRefDocument(void* doc) {
	CallPointer(Message::AddRefDocument, 0, doc);
}

void TextEditor::ReleaseDocument(void* doc) {
	CallPointer(Message::ReleaseDocument, 0, doc);
}

DocumentOption TextEditor::DocumentOptions() {
	return static_cast<KLWidgets::DocumentOption>(Call(Message::GetDocumentOptions));
}

ModificationFlags TextEditor::ModEventMask() {
	return static_cast<KLWidgets::ModificationFlags>(Call(Message::GetModEventMask));
}

void TextEditor::SetCommandEvents(bool commandEvents) {
	Call(Message::SetCommandEvents, commandEvents);
}

bool TextEditor::CommandEvents() {
	return Call(Message::GetCommandEvents);
}

void TextEditor::SetFocus(bool focus) {
	Call(Message::SetFocus, focus);
}

bool TextEditor::Focus() {
	return Call(Message::GetFocus);
}

void TextEditor::SetStatus(KLWidgets::Status status) {
	Call(Message::SetStatus, static_cast<uintptr_t>(status));
}

Status TextEditor::Status() {
	return static_cast<KLWidgets::Status>(Call(Message::GetStatus));
}

void TextEditor::SetMouseDownCaptures(bool captures) {
	Call(Message::SetMouseDownCaptures, captures);
}

bool TextEditor::MouseDownCaptures() {
	return Call(Message::GetMouseDownCaptures);
}

void TextEditor::SetMouseWheelCaptures(bool captures) {
	Call(Message::SetMouseWheelCaptures, captures);
}

bool TextEditor::MouseWheelCaptures() {
	return Call(Message::GetMouseWheelCaptures);
}

void TextEditor::SetCursor(KLWidgets::CursorShape cursorType) {
	Call(Message::SetCursor, static_cast<uintptr_t>(cursorType));
}

CursorShape TextEditor::Cursor() {
	return static_cast<KLWidgets::CursorShape>(Call(Message::GetCursor));
}

void TextEditor::SetControlCharSymbol(int symbol) {
	Call(Message::SetControlCharSymbol, symbol);
}

int TextEditor::ControlCharSymbol() {
	return static_cast<int>(Call(Message::GetControlCharSymbol));
}

void TextEditor::WordPartLeft() {
	Call(Message::WordPartLeft);
}

void TextEditor::WordPartLeftExtend() {
	Call(Message::WordPartLeftExtend);
}

void TextEditor::WordPartRight() {
	Call(Message::WordPartRight);
}

void TextEditor::WordPartRightExtend() {
	Call(Message::WordPartRightExtend);
}

void TextEditor::SetVisiblePolicy(KLWidgets::VisiblePolicy visiblePolicy, int visibleSlop) {
	Call(Message::SetVisiblePolicy, static_cast<uintptr_t>(visiblePolicy), visibleSlop);
}

void TextEditor::DelLineLeft() {
	Call(Message::DelLineLeft);
}

void TextEditor::DelLineRight() {
	Call(Message::DelLineRight);
}

void TextEditor::SetXOffset(int xOffset) {
	Call(Message::SetXOffset, xOffset);
}

int TextEditor::XOffset() {
	return static_cast<int>(Call(Message::GetXOffset));
}

void TextEditor::ChooseCaretX() {
	Call(Message::ChooseCaretX);
}

void TextEditor::GrabFocus() {
	Call(Message::GrabFocus);
}

void TextEditor::SetXCaretPolicy(KLWidgets::CaretPolicy caretPolicy, int caretSlop) {
	Call(Message::SetXCaretPolicy, static_cast<uintptr_t>(caretPolicy), caretSlop);
}

void TextEditor::SetYCaretPolicy(KLWidgets::CaretPolicy caretPolicy, int caretSlop) {
	Call(Message::SetYCaretPolicy, static_cast<uintptr_t>(caretPolicy), caretSlop);
}

void TextEditor::SetPrintWrapMode(KLWidgets::Wrap wrapMode) {
	Call(Message::SetPrintWrapMode, static_cast<uintptr_t>(wrapMode));
}

Wrap TextEditor::PrintWrapMode() {
	return static_cast<KLWidgets::Wrap>(Call(Message::GetPrintWrapMode));
}

void TextEditor::SetHotspotActiveFore(bool useSetting, Colour fore) {
	Call(Message::SetHotspotActiveFore, useSetting, fore);
}

Colour TextEditor::HotspotActiveFore() {
	return static_cast<Colour>(Call(Message::GetHotspotActiveFore));
}

void TextEditor::SetHotspotActiveBack(bool useSetting, Colour back) {
	Call(Message::SetHotspotActiveBack, useSetting, back);
}

Colour TextEditor::HotspotActiveBack() {
	return static_cast<Colour>(Call(Message::GetHotspotActiveBack));
}

void TextEditor::SetHotspotActiveUnderline(bool underline) {
	Call(Message::SetHotspotActiveUnderline, underline);
}

bool TextEditor::HotspotActiveUnderline() {
	return Call(Message::GetHotspotActiveUnderline);
}

void TextEditor::SetHotspotSingleLine(bool singleLine) {
	Call(Message::SetHotspotSingleLine, singleLine);
}

bool TextEditor::HotspotSingleLine() {
	return Call(Message::GetHotspotSingleLine);
}

void TextEditor::ParaDown() {
	Call(Message::ParaDown);
}

void TextEditor::ParaDownExtend() {
	Call(Message::ParaDownExtend);
}

void TextEditor::ParaUp() {
	Call(Message::ParaUp);
}

void TextEditor::ParaUpExtend() {
	Call(Message::ParaUpExtend);
}

Position TextEditor::PositionBefore(Position pos) {
	return Call(Message::PositionBefore, pos);
}

Position TextEditor::PositionAfter(Position pos) {
	return Call(Message::PositionAfter, pos);
}

Position TextEditor::PositionRelative(Position pos, Position relative) {
	return Call(Message::PositionRelative, pos, relative);
}

Position TextEditor::PositionRelativeCodeUnits(Position pos, Position relative) {
	return Call(Message::PositionRelativeCodeUnits, pos, relative);
}

void TextEditor::CopyRange(Position start, Position end) {
	Call(Message::CopyRange, start, end);
}

void TextEditor::CopyText(Position length, const char* text) {
	CallString(Message::CopyText, length, text);
}

void TextEditor::SetSelectionMode(KLWidgets::SelectionMode selectionMode) {
	Call(Message::SetSelectionMode, static_cast<uintptr_t>(selectionMode));
}

SelectionMode TextEditor::SelectionMode() {
	return static_cast<KLWidgets::SelectionMode>(Call(Message::GetSelectionMode));
}

bool TextEditor::MoveExtendsSelection() {
	return Call(Message::GetMoveExtendsSelection);
}

Position TextEditor::GetLineSelStartPosition(Line line) {
	return Call(Message::GetLineSelStartPosition, line);
}

Position TextEditor::GetLineSelEndPosition(Line line) {
	return Call(Message::GetLineSelEndPosition, line);
}

void TextEditor::LineDownRectExtend() {
	Call(Message::LineDownRectExtend);
}

void TextEditor::LineUpRectExtend() {
	Call(Message::LineUpRectExtend);
}

void TextEditor::CharLeftRectExtend() {
	Call(Message::CharLeftRectExtend);
}

void TextEditor::CharRightRectExtend() {
	Call(Message::CharRightRectExtend);
}

void TextEditor::HomeRectExtend() {
	Call(Message::HomeRectExtend);
}

void TextEditor::VCHomeRectExtend() {
	Call(Message::VCHomeRectExtend);
}

void TextEditor::LineEndRectExtend() {
	Call(Message::LineEndRectExtend);
}

void TextEditor::PageUpRectExtend() {
	Call(Message::PageUpRectExtend);
}

void TextEditor::PageDownRectExtend() {
	Call(Message::PageDownRectExtend);
}

void TextEditor::StutteredPageUp() {
	Call(Message::StutteredPageUp);
}

void TextEditor::StutteredPageUpExtend() {
	Call(Message::StutteredPageUpExtend);
}

void TextEditor::StutteredPageDown() {
	Call(Message::StutteredPageDown);
}

void TextEditor::StutteredPageDownExtend() {
	Call(Message::StutteredPageDownExtend);
}

void TextEditor::WordLeftEnd() {
	Call(Message::WordLeftEnd);
}

void TextEditor::WordLeftEndExtend() {
	Call(Message::WordLeftEndExtend);
}

void TextEditor::WordRightEnd() {
	Call(Message::WordRightEnd);
}

void TextEditor::WordRightEndExtend() {
	Call(Message::WordRightEndExtend);
}

void TextEditor::SetWhitespaceChars(const char* characters) {
	CallString(Message::SetWhitespaceChars, 0, characters);
}

int TextEditor::WhitespaceChars(char* characters) {
	return static_cast<int>(CallPointer(Message::GetWhitespaceChars, 0, characters));
}

std::string TextEditor::WhitespaceChars() {
	return CallReturnString(Message::GetWhitespaceChars, 0);
}

void TextEditor::SetPunctuationChars(const char* characters) {
	CallString(Message::SetPunctuationChars, 0, characters);
}

int TextEditor::PunctuationChars(char* characters) {
	return static_cast<int>(CallPointer(Message::GetPunctuationChars, 0, characters));
}

std::string TextEditor::PunctuationChars() {
	return CallReturnString(Message::GetPunctuationChars, 0);
}

void TextEditor::SetCharsDefault() {
	Call(Message::SetCharsDefault);
}

int TextEditor::AutoCGetCurrent() {
	return static_cast<int>(Call(Message::AutoCGetCurrent));
}

int TextEditor::AutoCGetCurrentText(char* text) {
	return static_cast<int>(CallPointer(Message::AutoCGetCurrentText, 0, text));
}

std::string TextEditor::AutoCGetCurrentText() {
	return CallReturnString(Message::AutoCGetCurrentText, 0);
}

void TextEditor::AutoCSetCaseInsensitiveBehaviour(KLWidgets::CaseInsensitiveBehaviour behaviour) {
	Call(Message::AutoCSetCaseInsensitiveBehaviour, static_cast<uintptr_t>(behaviour));
}

CaseInsensitiveBehaviour TextEditor::AutoCGetCaseInsensitiveBehaviour() {
	return static_cast<KLWidgets::CaseInsensitiveBehaviour>(Call(Message::AutoCGetCaseInsensitiveBehaviour));
}

void TextEditor::AutoCSetMulti(KLWidgets::MultiAutoComplete multi) {
	Call(Message::AutoCSetMulti, static_cast<uintptr_t>(multi));
}

MultiAutoComplete TextEditor::AutoCGetMulti() {
	return static_cast<KLWidgets::MultiAutoComplete>(Call(Message::AutoCGetMulti));
}

void TextEditor::AutoCSetOrder(KLWidgets::Ordering order) {
	Call(Message::AutoCSetOrder, static_cast<uintptr_t>(order));
}

Ordering TextEditor::AutoCGetOrder() {
	return static_cast<KLWidgets::Ordering>(Call(Message::AutoCGetOrder));
}

void TextEditor::Allocate(Position bytes) {
	Call(Message::Allocate, bytes);
}

Position TextEditor::TargetAsUTF8(char* s) {
	return CallPointer(Message::TargetAsUTF8, 0, s);
}

std::string TextEditor::TargetAsUTF8() {
	return CallReturnString(Message::TargetAsUTF8, 0);
}

void TextEditor::SetLengthForEncode(Position bytes) {
	Call(Message::SetLengthForEncode, bytes);
}

Position TextEditor::EncodedFromUTF8(const char* utf8, char* encoded) {
	return CallPointer(Message::EncodedFromUTF8, reinterpret_cast<uintptr_t>(utf8), encoded);
}

std::string TextEditor::EncodedFromUTF8(const char* utf8) {
	return CallReturnString(Message::EncodedFromUTF8, reinterpret_cast<uintptr_t>(utf8));
}

Position TextEditor::FindColumn(Line line, Position column) {
	return Call(Message::FindColumn, line, column);
}

CaretSticky TextEditor::CaretSticky() {
	return static_cast<KLWidgets::CaretSticky>(Call(Message::GetCaretSticky));
}

void TextEditor::SetCaretSticky(KLWidgets::CaretSticky useCaretStickyBehaviour) {
	Call(Message::SetCaretSticky, static_cast<uintptr_t>(useCaretStickyBehaviour));
}

void TextEditor::ToggleCaretSticky() {
	Call(Message::ToggleCaretSticky);
}

void TextEditor::SetPasteConvertEndings(bool convert) {
	Call(Message::SetPasteConvertEndings, convert);
}

bool TextEditor::PasteConvertEndings() {
	return Call(Message::GetPasteConvertEndings);
}

void TextEditor::SelectionDuplicate() {
	Call(Message::SelectionDuplicate);
}

void TextEditor::SetCaretLineBackAlpha(KLWidgets::Alpha alpha) {
	Call(Message::SetCaretLineBackAlpha, static_cast<uintptr_t>(alpha));
}

KLWidgets::Alpha TextEditor::CaretLineBackAlpha() {
	return static_cast<KLWidgets::Alpha>(Call(Message::GetCaretLineBackAlpha));
}

void TextEditor::SetCaretStyle(KLWidgets::CaretStyle caretStyle) {
	Call(Message::SetCaretStyle, static_cast<uintptr_t>(caretStyle));
}

CaretStyle TextEditor::CaretStyle() {
	return static_cast<KLWidgets::CaretStyle>(Call(Message::GetCaretStyle));
}

void TextEditor::SetIndicatorCurrent(int indicator) {
	Call(Message::SetIndicatorCurrent, indicator);
}

int TextEditor::IndicatorCurrent() {
	return static_cast<int>(Call(Message::GetIndicatorCurrent));
}

void TextEditor::SetIndicatorValue(int value) {
	Call(Message::SetIndicatorValue, value);
}

int TextEditor::IndicatorValue() {
	return static_cast<int>(Call(Message::GetIndicatorValue));
}

void TextEditor::IndicatorFillRange(Position start, Position lengthFill) {
	Call(Message::IndicatorFillRange, start, lengthFill);
}

void TextEditor::IndicatorClearRange(Position start, Position lengthClear) {
	Call(Message::IndicatorClearRange, start, lengthClear);
}

int TextEditor::IndicatorAllOnFor(Position pos) {
	return static_cast<int>(Call(Message::IndicatorAllOnFor, pos));
}

int TextEditor::IndicatorValueAt(int indicator, Position pos) {
	return static_cast<int>(Call(Message::IndicatorValueAt, indicator, pos));
}

Position TextEditor::IndicatorStart(int indicator, Position pos) {
	return Call(Message::IndicatorStart, indicator, pos);
}

Position TextEditor::IndicatorEnd(int indicator, Position pos) {
	return Call(Message::IndicatorEnd, indicator, pos);
}

void TextEditor::SetPositionCache(int size) {
	Call(Message::SetPositionCache, size);
}

int TextEditor::PositionCache() {
	return static_cast<int>(Call(Message::GetPositionCache));
}

void TextEditor::CopyAllowLine() {
	Call(Message::CopyAllowLine);
}

void* TextEditor::CharacterPointer() {
	return reinterpret_cast<void*>(Call(Message::GetCharacterPointer));
}

void* TextEditor::RangePointer(Position start, Position lengthRange) {
	return reinterpret_cast<void*>(Call(Message::GetRangePointer, start, lengthRange));
}

Position TextEditor::GapPosition() {
	return Call(Message::GetGapPosition);
}

void TextEditor::IndicSetAlpha(int indicator, KLWidgets::Alpha alpha) {
	Call(Message::IndicSetAlpha, indicator, static_cast<intptr_t>(alpha));
}

KLWidgets::Alpha TextEditor::IndicGetAlpha(int indicator) {
	return static_cast<KLWidgets::Alpha>(Call(Message::IndicGetAlpha, indicator));
}

void TextEditor::IndicSetOutlineAlpha(int indicator, KLWidgets::Alpha alpha) {
	Call(Message::IndicSetOutlineAlpha, indicator, static_cast<intptr_t>(alpha));
}

KLWidgets::Alpha TextEditor::IndicGetOutlineAlpha(int indicator) {
	return static_cast<KLWidgets::Alpha>(Call(Message::IndicGetOutlineAlpha, indicator));
}

void TextEditor::SetExtraAscent(int extraAscent) {
	Call(Message::SetExtraAscent, extraAscent);
}

int TextEditor::ExtraAscent() {
	return static_cast<int>(Call(Message::GetExtraAscent));
}

void TextEditor::SetExtraDescent(int extraDescent) {
	Call(Message::SetExtraDescent, extraDescent);
}

int TextEditor::ExtraDescent() {
	return static_cast<int>(Call(Message::GetExtraDescent));
}

int TextEditor::MarkerSymbolDefined(int markerNumber) {
	return static_cast<int>(Call(Message::MarkerSymbolDefined, markerNumber));
}

void TextEditor::MarginSetText(Line line, const char* text) {
	CallString(Message::MarginSetText, line, text);
}

int TextEditor::MarginGetText(Line line, char* text) {
	return static_cast<int>(CallPointer(Message::MarginGetText, line, text));
}

std::string TextEditor::MarginGetText(Line line) {
	return CallReturnString(Message::MarginGetText, line);
}

void TextEditor::MarginSetStyle(Line line, int style) {
	Call(Message::MarginSetStyle, line, style);
}

int TextEditor::MarginGetStyle(Line line) {
	return static_cast<int>(Call(Message::MarginGetStyle, line));
}

void TextEditor::MarginSetStyles(Line line, const char* styles) {
	CallString(Message::MarginSetStyles, line, styles);
}

int TextEditor::MarginGetStyles(Line line, char* styles) {
	return static_cast<int>(CallPointer(Message::MarginGetStyles, line, styles));
}

std::string TextEditor::MarginGetStyles(Line line) {
	return CallReturnString(Message::MarginGetStyles, line);
}

void TextEditor::MarginTextClearAll() {
	Call(Message::MarginTextClearAll);
}

void TextEditor::MarginSetStyleOffset(int style) {
	Call(Message::MarginSetStyleOffset, style);
}

int TextEditor::MarginGetStyleOffset() {
	return static_cast<int>(Call(Message::MarginGetStyleOffset));
}

void TextEditor::SetMarginOptions(KLWidgets::MarginOption marginOptions) {
	Call(Message::SetMarginOptions, static_cast<uintptr_t>(marginOptions));
}

MarginOption TextEditor::MarginOptions() {
	return static_cast<KLWidgets::MarginOption>(Call(Message::GetMarginOptions));
}

void TextEditor::AnnotationSetText(Line line, const char* text) {
	CallString(Message::AnnotationSetText, line, text);
}

int TextEditor::AnnotationGetText(Line line, char* text) {
	return static_cast<int>(CallPointer(Message::AnnotationGetText, line, text));
}

std::string TextEditor::AnnotationGetText(Line line) {
	return CallReturnString(Message::AnnotationGetText, line);
}

void TextEditor::AnnotationSetStyle(Line line, int style) {
	Call(Message::AnnotationSetStyle, line, style);
}

int TextEditor::AnnotationGetStyle(Line line) {
	return static_cast<int>(Call(Message::AnnotationGetStyle, line));
}

void TextEditor::AnnotationSetStyles(Line line, const char* styles) {
	CallString(Message::AnnotationSetStyles, line, styles);
}

int TextEditor::AnnotationGetStyles(Line line, char* styles) {
	return static_cast<int>(CallPointer(Message::AnnotationGetStyles, line, styles));
}

std::string TextEditor::AnnotationGetStyles(Line line) {
	return CallReturnString(Message::AnnotationGetStyles, line);
}

int TextEditor::AnnotationGetLines(Line line) {
	return static_cast<int>(Call(Message::AnnotationGetLines, line));
}

void TextEditor::AnnotationClearAll() {
	Call(Message::AnnotationClearAll);
}

void TextEditor::AnnotationSetVisible(KLWidgets::AnnotationVisible visible) {
	Call(Message::AnnotationSetVisible, static_cast<uintptr_t>(visible));
}

AnnotationVisible TextEditor::AnnotationGetVisible() {
	return static_cast<KLWidgets::AnnotationVisible>(Call(Message::AnnotationGetVisible));
}

void TextEditor::AnnotationSetStyleOffset(int style) {
	Call(Message::AnnotationSetStyleOffset, style);
}

int TextEditor::AnnotationGetStyleOffset() {
	return static_cast<int>(Call(Message::AnnotationGetStyleOffset));
}

void TextEditor::ReleaseAllExtendedStyles() {
	Call(Message::ReleaseAllExtendedStyles);
}

int TextEditor::AllocateExtendedStyles(int numberStyles) {
	return static_cast<int>(Call(Message::AllocateExtendedStyles, numberStyles));
}

void TextEditor::AddUndoAction(int token, KLWidgets::UndoFlags flags) {
	Call(Message::AddUndoAction, token, static_cast<intptr_t>(flags));
}

Position TextEditor::CharPositionFromPoint(int x, int y) {
	return Call(Message::CharPositionFromPoint, x, y);
}

Position TextEditor::CharPositionFromPointClose(int x, int y) {
	return Call(Message::CharPositionFromPointClose, x, y);
}

void TextEditor::SetMouseSelectionRectangularSwitch(bool mouseSelectionRectangularSwitch) {
	Call(Message::SetMouseSelectionRectangularSwitch, mouseSelectionRectangularSwitch);
}

bool TextEditor::MouseSelectionRectangularSwitch() {
	return Call(Message::GetMouseSelectionRectangularSwitch);
}

void TextEditor::SetMultipleSelection(bool multipleSelection) {
	Call(Message::SetMultipleSelection, multipleSelection);
}

bool TextEditor::MultipleSelection() {
	return Call(Message::GetMultipleSelection);
}

void TextEditor::SetAdditionalSelectionTyping(bool additionalSelectionTyping) {
	Call(Message::SetAdditionalSelectionTyping, additionalSelectionTyping);
}

bool TextEditor::AdditionalSelectionTyping() {
	return Call(Message::GetAdditionalSelectionTyping);
}

void TextEditor::SetAdditionalCaretsBlink(bool additionalCaretsBlink) {
	Call(Message::SetAdditionalCaretsBlink, additionalCaretsBlink);
}

bool TextEditor::AdditionalCaretsBlink() {
	return Call(Message::GetAdditionalCaretsBlink);
}

void TextEditor::SetAdditionalCaretsVisible(bool additionalCaretsVisible) {
	Call(Message::SetAdditionalCaretsVisible, additionalCaretsVisible);
}

bool TextEditor::AdditionalCaretsVisible() {
	return Call(Message::GetAdditionalCaretsVisible);
}

int TextEditor::Selections() {
	return static_cast<int>(Call(Message::GetSelections));
}

bool TextEditor::SelectionEmpty() {
	return Call(Message::GetSelectionEmpty);
}

void TextEditor::ClearSelections() {
	Call(Message::ClearSelections);
}

void TextEditor::SetSelection(Position caret, Position anchor) {
	Call(Message::SetSelection, caret, anchor);
}

void TextEditor::AddSelection(Position caret, Position anchor) {
	Call(Message::AddSelection, caret, anchor);
}

void TextEditor::DropSelectionN(int selection) {
	Call(Message::DropSelectionN, selection);
}

void TextEditor::SetMainSelection(int selection) {
	Call(Message::SetMainSelection, selection);
}

int TextEditor::MainSelection() {
	return static_cast<int>(Call(Message::GetMainSelection));
}

void TextEditor::SetSelectionNCaret(int selection, Position caret) {
	Call(Message::SetSelectionNCaret, selection, caret);
}

Position TextEditor::SelectionNCaret(int selection) {
	return Call(Message::GetSelectionNCaret, selection);
}

void TextEditor::SetSelectionNAnchor(int selection, Position anchor) {
	Call(Message::SetSelectionNAnchor, selection, anchor);
}

Position TextEditor::SelectionNAnchor(int selection) {
	return Call(Message::GetSelectionNAnchor, selection);
}

void TextEditor::SetSelectionNCaretVirtualSpace(int selection, Position space) {
	Call(Message::SetSelectionNCaretVirtualSpace, selection, space);
}

Position TextEditor::SelectionNCaretVirtualSpace(int selection) {
	return Call(Message::GetSelectionNCaretVirtualSpace, selection);
}

void TextEditor::SetSelectionNAnchorVirtualSpace(int selection, Position space) {
	Call(Message::SetSelectionNAnchorVirtualSpace, selection, space);
}

Position TextEditor::SelectionNAnchorVirtualSpace(int selection) {
	return Call(Message::GetSelectionNAnchorVirtualSpace, selection);
}

void TextEditor::SetSelectionNStart(int selection, Position anchor) {
	Call(Message::SetSelectionNStart, selection, anchor);
}

Position TextEditor::SelectionNStart(int selection) {
	return Call(Message::GetSelectionNStart, selection);
}

Position TextEditor::SelectionNStartVirtualSpace(int selection) {
	return Call(Message::GetSelectionNStartVirtualSpace, selection);
}

void TextEditor::SetSelectionNEnd(int selection, Position caret) {
	Call(Message::SetSelectionNEnd, selection, caret);
}

Position TextEditor::SelectionNEndVirtualSpace(int selection) {
	return Call(Message::GetSelectionNEndVirtualSpace, selection);
}

Position TextEditor::SelectionNEnd(int selection) {
	return Call(Message::GetSelectionNEnd, selection);
}

void TextEditor::SetRectangularSelectionCaret(Position caret) {
	Call(Message::SetRectangularSelectionCaret, caret);
}

Position TextEditor::RectangularSelectionCaret() {
	return Call(Message::GetRectangularSelectionCaret);
}

void TextEditor::SetRectangularSelectionAnchor(Position anchor) {
	Call(Message::SetRectangularSelectionAnchor, anchor);
}

Position TextEditor::RectangularSelectionAnchor() {
	return Call(Message::GetRectangularSelectionAnchor);
}

void TextEditor::SetRectangularSelectionCaretVirtualSpace(Position space) {
	Call(Message::SetRectangularSelectionCaretVirtualSpace, space);
}

Position TextEditor::RectangularSelectionCaretVirtualSpace() {
	return Call(Message::GetRectangularSelectionCaretVirtualSpace);
}

void TextEditor::SetRectangularSelectionAnchorVirtualSpace(Position space) {
	Call(Message::SetRectangularSelectionAnchorVirtualSpace, space);
}

Position TextEditor::RectangularSelectionAnchorVirtualSpace() {
	return Call(Message::GetRectangularSelectionAnchorVirtualSpace);
}

void TextEditor::SetVirtualSpaceOptions(KLWidgets::VirtualSpace virtualSpaceOptions) {
	Call(Message::SetVirtualSpaceOptions, static_cast<uintptr_t>(virtualSpaceOptions));
}

VirtualSpace TextEditor::VirtualSpaceOptions() {
	return static_cast<KLWidgets::VirtualSpace>(Call(Message::GetVirtualSpaceOptions));
}

void TextEditor::SetRectangularSelectionModifier(int modifier) {
	Call(Message::SetRectangularSelectionModifier, modifier);
}

int TextEditor::RectangularSelectionModifier() {
	return static_cast<int>(Call(Message::GetRectangularSelectionModifier));
}

void TextEditor::SetAdditionalSelFore(Colour fore) {
	Call(Message::SetAdditionalSelFore, fore);
}

void TextEditor::SetAdditionalSelBack(Colour back) {
	Call(Message::SetAdditionalSelBack, back);
}

void TextEditor::SetAdditionalSelAlpha(KLWidgets::Alpha alpha) {
	Call(Message::SetAdditionalSelAlpha, static_cast<uintptr_t>(alpha));
}

KLWidgets::Alpha TextEditor::AdditionalSelAlpha() {
	return static_cast<KLWidgets::Alpha>(Call(Message::GetAdditionalSelAlpha));
}

void TextEditor::SetAdditionalCaretFore(Colour fore) {
	Call(Message::SetAdditionalCaretFore, fore);
}

Colour TextEditor::AdditionalCaretFore() {
	return static_cast<Colour>(Call(Message::GetAdditionalCaretFore));
}

void TextEditor::RotateSelection() {
	Call(Message::RotateSelection);
}

void TextEditor::SwapMainAnchorCaret() {
	Call(Message::SwapMainAnchorCaret);
}

void TextEditor::MultipleSelectAddNext() {
	Call(Message::MultipleSelectAddNext);
}

void TextEditor::MultipleSelectAddEach() {
	Call(Message::MultipleSelectAddEach);
}

int TextEditor::ChangeLexerState(Position start, Position end) {
	return static_cast<int>(Call(Message::ChangeLexerState, start, end));
}

Line TextEditor::ContractedFoldNext(Line lineStart) {
	return Call(Message::ContractedFoldNext, lineStart);
}

void TextEditor::VerticalCentreCaret() {
	Call(Message::VerticalCentreCaret);
}

void TextEditor::MoveSelectedLinesUp() {
	Call(Message::MoveSelectedLinesUp);
}

void TextEditor::MoveSelectedLinesDown() {
	Call(Message::MoveSelectedLinesDown);
}

void TextEditor::SetIdentifier(int identifier) {
	Call(Message::SetIdentifier, identifier);
}

int TextEditor::Identifier() {
	return static_cast<int>(Call(Message::GetIdentifier));
}

void TextEditor::RGBAImageSetWidth(int width) {
	Call(Message::RGBAImageSetWidth, width);
}

void TextEditor::RGBAImageSetHeight(int height) {
	Call(Message::RGBAImageSetHeight, height);
}

void TextEditor::RGBAImageSetScale(int scalePercent) {
	Call(Message::RGBAImageSetScale, scalePercent);
}

void TextEditor::MarkerDefineRGBAImage(int markerNumber, const char* pixels) {
	CallString(Message::MarkerDefineRGBAImage, markerNumber, pixels);
}

void TextEditor::RegisterRGBAImage(int type, const char* pixels) {
	CallString(Message::RegisterRGBAImage, type, pixels);
}

void TextEditor::ScrollToStart() {
	Call(Message::ScrollToStart);
}

void TextEditor::ScrollToEnd() {
	Call(Message::ScrollToEnd);
}

void TextEditor::SetTechnology(KLWidgets::Technology technology) {
	Call(Message::SetTechnology, static_cast<uintptr_t>(technology));
}

Technology TextEditor::Technology() {
	return static_cast<KLWidgets::Technology>(Call(Message::GetTechnology));
}

void* TextEditor::CreateLoader(Position bytes, KLWidgets::DocumentOption documentOptions) {
	return reinterpret_cast<void*>(Call(Message::CreateLoader, bytes, static_cast<intptr_t>(documentOptions)));
}

void TextEditor::FindIndicatorShow(Position start, Position end) {
	Call(Message::FindIndicatorShow, start, end);
}

void TextEditor::FindIndicatorFlash(Position start, Position end) {
	Call(Message::FindIndicatorFlash, start, end);
}

void TextEditor::FindIndicatorHide() {
	Call(Message::FindIndicatorHide);
}

void TextEditor::VCHomeDisplay() {
	Call(Message::VCHomeDisplay);
}

void TextEditor::VCHomeDisplayExtend() {
	Call(Message::VCHomeDisplayExtend);
}

bool TextEditor::CaretLineVisibleAlways() {
	return Call(Message::GetCaretLineVisibleAlways);
}

void TextEditor::SetCaretLineVisibleAlways(bool alwaysVisible) {
	Call(Message::SetCaretLineVisibleAlways, alwaysVisible);
}

void TextEditor::SetLineEndTypesAllowed(KLWidgets::LineEndType lineEndBitSet) {
	Call(Message::SetLineEndTypesAllowed, static_cast<uintptr_t>(lineEndBitSet));
}

LineEndType TextEditor::LineEndTypesAllowed() {
	return static_cast<KLWidgets::LineEndType>(Call(Message::GetLineEndTypesAllowed));
}

LineEndType TextEditor::LineEndTypesActive() {
	return static_cast<KLWidgets::LineEndType>(Call(Message::GetLineEndTypesActive));
}

void TextEditor::SetRepresentation(const char* encodedCharacter, const char* representation) {
	CallString(Message::SetRepresentation, reinterpret_cast<uintptr_t>(encodedCharacter), representation);
}

int TextEditor::Representation(const char* encodedCharacter, char* representation) {
	return static_cast<int>(CallPointer(Message::GetRepresentation, reinterpret_cast<uintptr_t>(encodedCharacter), representation));
}

std::string TextEditor::Representation(const char* encodedCharacter) {
	return CallReturnString(Message::GetRepresentation, reinterpret_cast<uintptr_t>(encodedCharacter));
}

void TextEditor::ClearRepresentation(const char* encodedCharacter) {
	Call(Message::ClearRepresentation, reinterpret_cast<uintptr_t>(encodedCharacter));
}

void TextEditor::EOLAnnotationSetText(Line line, const char* text) {
	CallString(Message::EOLAnnotationSetText, line, text);
}

int TextEditor::EOLAnnotationGetText(Line line, char* text) {
	return static_cast<int>(CallPointer(Message::EOLAnnotationGetText, line, text));
}

std::string TextEditor::EOLAnnotationGetText(Line line) {
	return CallReturnString(Message::EOLAnnotationGetText, line);
}

void TextEditor::EOLAnnotationSetStyle(Line line, int style) {
	Call(Message::EOLAnnotationSetStyle, line, style);
}

int TextEditor::EOLAnnotationGetStyle(Line line) {
	return static_cast<int>(Call(Message::EOLAnnotationGetStyle, line));
}

void TextEditor::EOLAnnotationClearAll() {
	Call(Message::EOLAnnotationClearAll);
}

void TextEditor::EOLAnnotationSetVisible(KLWidgets::EOLAnnotationVisible visible) {
	Call(Message::EOLAnnotationSetVisible, static_cast<uintptr_t>(visible));
}

EOLAnnotationVisible TextEditor::EOLAnnotationGetVisible() {
	return static_cast<KLWidgets::EOLAnnotationVisible>(Call(Message::EOLAnnotationGetVisible));
}

void TextEditor::EOLAnnotationSetStyleOffset(int style) {
	Call(Message::EOLAnnotationSetStyleOffset, style);
}

int TextEditor::EOLAnnotationGetStyleOffset() {
	return static_cast<int>(Call(Message::EOLAnnotationGetStyleOffset));
}

bool TextEditor::SupportsFeature(KLWidgets::Supports feature) {
	return Call(Message::SupportsFeature, static_cast<uintptr_t>(feature));
}

void TextEditor::StartRecord() {
	Call(Message::StartRecord);
}

void TextEditor::StopRecord() {
	Call(Message::StopRecord);
}

int TextEditor::Lexer() {
	return static_cast<int>(Call(Message::GetLexer));
}

void TextEditor::Colourise(Position start, Position end) {
	Call(Message::Colourise, start, end);
}

void TextEditor::SetProperty(const char* key, const char* value) {
	CallString(Message::SetProperty, reinterpret_cast<uintptr_t>(key), value);
}

void TextEditor::SetKeyWords(int keyWordSet, const char* keyWords) {
	CallString(Message::SetKeyWords, keyWordSet, keyWords);
}

int TextEditor::Property(const char* key, char* value) {
	return static_cast<int>(CallPointer(Message::GetProperty, reinterpret_cast<uintptr_t>(key), value));
}

std::string TextEditor::Property(const char* key) {
	return CallReturnString(Message::GetProperty, reinterpret_cast<uintptr_t>(key));
}

int TextEditor::PropertyExpanded(const char* key, char* value) {
	return static_cast<int>(CallPointer(Message::GetPropertyExpanded, reinterpret_cast<uintptr_t>(key), value));
}

std::string TextEditor::PropertyExpanded(const char* key) {
	return CallReturnString(Message::GetPropertyExpanded, reinterpret_cast<uintptr_t>(key));
}

int TextEditor::PropertyInt(const char* key, int defaultValue) {
	return static_cast<int>(Call(Message::GetPropertyInt, reinterpret_cast<uintptr_t>(key), defaultValue));
}

int TextEditor::LexerLanguage(char* language) {
	return static_cast<int>(CallPointer(Message::GetLexerLanguage, 0, language));
}

std::string TextEditor::LexerLanguage() {
	return CallReturnString(Message::GetLexerLanguage, 0);
}

void* TextEditor::PrivateLexerCall(int operation, void* pointer) {
	return reinterpret_cast<void*>(CallPointer(Message::PrivateLexerCall, operation, pointer));
}

int TextEditor::PropertyNames(char* names) {
	return static_cast<int>(CallPointer(Message::PropertyNames, 0, names));
}

std::string TextEditor::PropertyNames() {
	return CallReturnString(Message::PropertyNames, 0);
}

TypeProperty TextEditor::PropertyType(const char* name) {
	return static_cast<KLWidgets::TypeProperty>(Call(Message::PropertyType, reinterpret_cast<uintptr_t>(name)));
}

int TextEditor::DescribeProperty(const char* name, char* description) {
	return static_cast<int>(CallPointer(Message::DescribeProperty, reinterpret_cast<uintptr_t>(name), description));
}

std::string TextEditor::DescribeProperty(const char* name) {
	return CallReturnString(Message::DescribeProperty, reinterpret_cast<uintptr_t>(name));
}

int TextEditor::DescribeKeyWordSets(char* descriptions) {
	return static_cast<int>(CallPointer(Message::DescribeKeyWordSets, 0, descriptions));
}

std::string TextEditor::DescribeKeyWordSets() {
	return CallReturnString(Message::DescribeKeyWordSets, 0);
}

int TextEditor::LineEndTypesSupported() {
	return static_cast<int>(Call(Message::GetLineEndTypesSupported));
}

int TextEditor::AllocateSubStyles(int styleBase, int numberStyles) {
	return static_cast<int>(Call(Message::AllocateSubStyles, styleBase, numberStyles));
}

int TextEditor::SubStylesStart(int styleBase) {
	return static_cast<int>(Call(Message::GetSubStylesStart, styleBase));
}

int TextEditor::SubStylesLength(int styleBase) {
	return static_cast<int>(Call(Message::GetSubStylesLength, styleBase));
}

int TextEditor::StyleFromSubStyle(int subStyle) {
	return static_cast<int>(Call(Message::GetStyleFromSubStyle, subStyle));
}

int TextEditor::PrimaryStyleFromStyle(int style) {
	return static_cast<int>(Call(Message::GetPrimaryStyleFromStyle, style));
}

void TextEditor::FreeSubStyles() {
	Call(Message::FreeSubStyles);
}

void TextEditor::SetIdentifiers(int style, const char* identifiers) {
	CallString(Message::SetIdentifiers, style, identifiers);
}

int TextEditor::DistanceToSecondaryStyles() {
	return static_cast<int>(Call(Message::DistanceToSecondaryStyles));
}

int TextEditor::SubStyleBases(char* styles) {
	return static_cast<int>(CallPointer(Message::GetSubStyleBases, 0, styles));
}

std::string TextEditor::SubStyleBases() {
	return CallReturnString(Message::GetSubStyleBases, 0);
}

int TextEditor::NamedStyles() {
	return static_cast<int>(Call(Message::GetNamedStyles));
}

int TextEditor::NameOfStyle(int style, char* name) {
	return static_cast<int>(CallPointer(Message::NameOfStyle, style, name));
}

std::string TextEditor::NameOfStyle(int style) {
	return CallReturnString(Message::NameOfStyle, style);
}

int TextEditor::TagsOfStyle(int style, char* tags) {
	return static_cast<int>(CallPointer(Message::TagsOfStyle, style, tags));
}

std::string TextEditor::TagsOfStyle(int style) {
	return CallReturnString(Message::TagsOfStyle, style);
}

int TextEditor::DescriptionOfStyle(int style, char* description) {
	return static_cast<int>(CallPointer(Message::DescriptionOfStyle, style, description));
}

std::string TextEditor::DescriptionOfStyle(int style) {
	return CallReturnString(Message::DescriptionOfStyle, style);
}

void TextEditor::SetILexer(void* ilexer) {
	CallPointer(Message::SetILexer, 0, ilexer);
}

Bidirectional TextEditor::Bidirectional() {
	return static_cast<KLWidgets::Bidirectional>(Call(Message::GetBidirectional));
}

void TextEditor::SetBidirectional(KLWidgets::Bidirectional bidirectional) {
	Call(Message::SetBidirectional, static_cast<uintptr_t>(bidirectional));
}

LineCharacterIndexType TextEditor::LineCharacterIndex() {
	return static_cast<KLWidgets::LineCharacterIndexType>(Call(Message::GetLineCharacterIndex));
}

void TextEditor::AllocateLineCharacterIndex(KLWidgets::LineCharacterIndexType lineCharacterIndex) {
	Call(Message::AllocateLineCharacterIndex, static_cast<uintptr_t>(lineCharacterIndex));
}

void TextEditor::ReleaseLineCharacterIndex(KLWidgets::LineCharacterIndexType lineCharacterIndex) {
	Call(Message::ReleaseLineCharacterIndex, static_cast<uintptr_t>(lineCharacterIndex));
}

Line TextEditor::LineFromIndexPosition(Position pos, KLWidgets::LineCharacterIndexType lineCharacterIndex) {
	return Call(Message::LineFromIndexPosition, pos, static_cast<intptr_t>(lineCharacterIndex));
}

Position TextEditor::IndexPositionFromLine(Line line, KLWidgets::LineCharacterIndexType lineCharacterIndex) {
	return Call(Message::IndexPositionFromLine, line, static_cast<intptr_t>(lineCharacterIndex));
}
}

