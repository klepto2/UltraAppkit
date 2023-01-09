#include "UltraEngine.h"
#include "SyntaxWidget.h"

bool SyntaxEditor::SCI_REGISTERED = false;
vector<shared_ptr<SyntaxEditor>> SyntaxEditor::SytaxEditorMap;

EventID SyntaxEditor::EVENT_STYLENEEDED = AllocEventID();
EventID SyntaxEditor::EVENT_CHARADDED = AllocEventID();
EventID SyntaxEditor::EVENT_SAVEPOINTREACHED = AllocEventID();
EventID SyntaxEditor::EVENT_SAVEPOINTLEFT = AllocEventID();
EventID SyntaxEditor::EVENT_MODIFYATTEMPTRO = AllocEventID();
EventID SyntaxEditor::EVENT_KEY = AllocEventID();
EventID SyntaxEditor::EVENT_SCI_DOUBLECLICK = AllocEventID();
EventID SyntaxEditor::EVENT_UPDATEUI = AllocEventID();
EventID SyntaxEditor::EVENT_MODIFIED = AllocEventID();
EventID SyntaxEditor::EVENT_MACRORECORD = AllocEventID();
EventID SyntaxEditor::EVENT_MARGINCLICK = AllocEventID();
EventID SyntaxEditor::EVENT_NEEDSHOWN = AllocEventID();
EventID SyntaxEditor::EVENT_PAINTED = AllocEventID();
EventID SyntaxEditor::EVENT_USERLISTSELECTION = AllocEventID();
EventID SyntaxEditor::EVENT_URIDROPPED = AllocEventID();
EventID SyntaxEditor::EVENT_DWELLSTART = AllocEventID();
EventID SyntaxEditor::EVENT_DWELLEND = AllocEventID();
EventID SyntaxEditor::EVENT_ZOOM = AllocEventID();
EventID SyntaxEditor::EVENT_HOTSPOTCLICK = AllocEventID();
EventID SyntaxEditor::EVENT_HOTSPOTDOUBLECLICK = AllocEventID();
EventID SyntaxEditor::EVENT_HOTSPOTRELEASECLICK = AllocEventID();
EventID SyntaxEditor::EVENT_INDICATORCLICK = AllocEventID();
EventID SyntaxEditor::EVENT_INDICATORRELEASE = AllocEventID();
EventID SyntaxEditor::EVENT_CALLTIPCLICK = AllocEventID();
EventID SyntaxEditor::EVENT_AUTOCSELECTION = AllocEventID();
EventID SyntaxEditor::EVENT_AUTOCCANCELLED = AllocEventID();
EventID SyntaxEditor::EVENT_AUTOCCHARDELETED = AllocEventID();
EventID SyntaxEditor::EVENT_FOCUSIN = AllocEventID();
EventID SyntaxEditor::EVENT_FOCUSOUT = AllocEventID();
EventID SyntaxEditor::EVENT_AUTOCCOMPLETED = AllocEventID();
EventID SyntaxEditor::EVENT_MARGINRIGHTCLICK = AllocEventID();
EventID SyntaxEditor::EVENT_AUTOCSELECTIONCHANGE = AllocEventID();

shared_ptr<SyntaxEditor> CreateSyntaxEditor(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent)
{
	auto widget = make_shared<SyntaxEditor>();
	widget->Initialize("", x, y, width, height, parent, 0);
	float scale = widget->gui->scale;
	widget->_verticalSlider = CreateSlider(width - widget->slidersize * scale, 0, widget->slidersize * scale, height, widget, SLIDER_VERTICAL | SLIDER_SCROLLBAR);
	widget->_horizontalSlider = CreateSlider(0, width - widget->slidersize * scale, width, widget->slidersize * scale, widget, SLIDER_HORIZONTAL | SLIDER_SCROLLBAR);
	SyntaxEditor::SytaxEditorMap.push_back(widget);
	widget->UpdateLayout();
	return widget;
}

void SyntaxEditor::UpdateSliderLocation()
{
	int hscrollwidthlimiter = 0;
	int vscrollheightlimiter = 0;

	if (!_verticalSlider->Hidden())
	{
		hscrollwidthlimiter = slidersize * gui->scale;
	}

	if (!_horizontalSlider->Hidden())
	{
		vscrollheightlimiter = slidersize * gui->scale;
	}

	_horizontalSlider->SetShape(0, this->size.y - slidersize * gui->scale, this->size.x - hscrollwidthlimiter, slidersize * gui->scale);
	_verticalSlider->SetShape(this->size.x - slidersize * gui->scale - 0, 0, slidersize * gui->scale, this->size.y - vscrollheightlimiter);
}

void SyntaxEditor::UpdateSliderVisibility()
{
	if (GetScrollWidth() > GetSize().y)
	{
		_horizontalSlider->Show();
	}
	else
	{
		_horizontalSlider->Hide();
	}

	if (GetLineCount() > LinesOnScreen())
	{
		_verticalSlider->Show();
	}
	else
	{
		_verticalSlider->Hide();
	}
}

void SyntaxEditor::UpdateSliderValues(bool updateFromScintilla)
{
	auto shape = GetScintillaWindowShape();
	int margin_count = GetMargins();
	int msize = GetMarginLeft() + GetMarginRight();
	for (int m = 0; m < margin_count; m++)
	{
		msize += GetMarginWidthN(m);
	}

	auto range = _horizontalSlider->GetRange();
	if (range.x != (shape.width - msize) / 10 || range.y != GetScrollWidth() / 10)
	{
		_horizontalSlider->SetRange((shape.width - msize) / 10, GetScrollWidth() / 10);
		_horizontalSlider->SetValue(GetXOffset() / 10.0);
	}

	int lc = GetLineCount();
	int l = LinesOnScreen();
	range = _verticalSlider->GetRange();

	if (range.x != l || range.y != lc)
	{
		_verticalSlider->SetRange(LinesOnScreen(), GetLineCount());
		_verticalSlider->SetValue(GetFirstVisibleLine());
	}

	if (updateFromScintilla)
	{
		_horizontalSlider->SetValue(GetXOffset() / 10.0);
		_verticalSlider->SetValue(GetFirstVisibleLine());
	}
	else
	{
		SetXOffset(_horizontalSlider->GetValue() * 10.0);
		SetFirstVisibleLine(_verticalSlider->GetValue());
	}
}

iVec4 SyntaxEditor::GetScintillaWindowShape()
{
	auto pos = GetPosition(true);
	auto size = this->ClientSize();
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

	return iVec4(pos.x, pos.y, size.x - vScroll, size.y - hScroll);
}

void SyntaxEditor::UpdateLayout()
{
	Widget::UpdateLayout();
	UpdateSliderVisibility();
	UpdateSliderLocation();
	auto shape = GetScintillaWindowShape();
	SetWindowPos(hwnd, HWND_TOP, shape.x, shape.y, shape.width, shape.height, SWP_NOREDRAW);
}

void SyntaxEditor::Draw__(const int x, const int y, const int width, const int height)
{
	if (collapsed || hidestate)
	{
		return;
	}

	UpdateSliderValues();

	Widget::Draw__(x, y, width, height);

	if (!scintillaVisible)
	{
		UpdateLayout();
		ShowWindow(hwnd, SW_SHOW);
		scintillaVisible = true;
	}
	else
	{
		UpdateWindow(hwnd);
	}
}

void SyntaxEditor::ProcessNotification(SCNotification* n)
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
			UpdateSliderValues(true);
		}
		id = EVENT_UPDATEUI;
		break;
	case SCN_MODIFIED:
		UpdateSliderValues(true);
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

		for (auto w : SyntaxEditor::SytaxEditorMap)
		{
			auto editor = w.get();
			if (editor->hwnd == lpnmhdr->hwndFrom)
			{
				editor->ProcessNotification(reinterpret_cast<SCNotification*>(cwps.lParam));
				break;
			}
		}
		break;
	}

	return NULL;
}

bool SyntaxEditor::InitScintilla(const int x, const int y, const int width, const int height)
{
	auto parentWindow = gui->GetWindow();
	hInstance = (HINSTANCE)GetWindowLong(parentWindow->GetHandle(), GWLP_HINSTANCE);
	if (!SyntaxEditor::SCI_REGISTERED)
	{
		if (!Scintilla_RegisterClasses(hInstance))
		{
			return false;
		}

		SyntaxEditor::SCI_REGISTERED = true;
	}

	hwnd = CreateWindowA(
		"Scintilla",
		"Source",
		WS_CHILD | WS_TABSTOP | WS_CLIPCHILDREN,
		x, y,
		width, height,
		parentWindow->GetHandle(),
		0,
		hInstance,
		0);

	SciFnDirect fn_ = reinterpret_cast<SciFnDirect>(
		SendMessage(hwnd, SCI_GETDIRECTFUNCTION, 0, 0));
	const sptr_t ptr_ = SendMessage(hwnd, SCI_GETDIRECTPOINTER, 0, 0);
	SetFnPtr(fn_, ptr_);

	::SetWindowsHookEx(WH_CALLWNDPROC, HandleScintillaNotifications, hInstance, GetCurrentThreadId());
	SetVScrollBar(false);
	SetHScrollBar(false);
	SetBufferedDraw(true);
}

bool SyntaxEditor::Initialize(const WString& text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style)
{
	if (Widget::Initialize(text, x, y, width, height, parent, style))
	{
		return InitScintilla(x,y,width,height);
	}

	return false;
}

void SyntaxEditor::Hide()
{
	Widget::Hide();
	ShowWindow(hwnd, SW_HIDE);
}

void SyntaxEditor::Show()
{
	Widget::Show();
	scintillaVisible = false;
}void SyntaxEditor::SetFnPtr(FunctionDirect fn_, intptr_t ptr_) noexcept {
	fn = fn_;
	ptr = ptr_;
}

bool SyntaxEditor::IsValid() const noexcept {
	return fn && ptr;
}


intptr_t SyntaxEditor::Call(ScintillaFunction msg, uintptr_t wParam, intptr_t lParam) {
	if (!fn)
		throw Failure(Status::Failure);
	const intptr_t retVal = fn(ptr, static_cast<unsigned int>(msg), wParam, lParam);
	return retVal;
}

intptr_t SyntaxEditor::CallPointer(ScintillaFunction msg, uintptr_t wParam, void* s) {
	return Call(msg, wParam, reinterpret_cast<intptr_t>(s));
}

intptr_t SyntaxEditor::CallString(ScintillaFunction msg, uintptr_t wParam, const char* s) {
	return Call(msg, wParam, reinterpret_cast<intptr_t>(s));
}

std::string SyntaxEditor::CallReturnString(ScintillaFunction msg, uintptr_t wParam) {
	size_t len = CallPointer(msg, wParam, nullptr);
	if (len) {
		std::string value(len, '\0');
		CallPointer(msg, wParam, value.data());
		return value;
	}
	else {
		return std::string();
	}
}//PART:SCI_FUNCTION_IMPL
void SyntaxEditor::AddText(Position length,const char *text)
{
	CallString(ScintillaFunction::AddText, length, text);
}

void SyntaxEditor::AddStyledText(Position length,const char *c)
{
	CallString(ScintillaFunction::AddStyledText, length, c);
}

void SyntaxEditor::InsertText(Position pos,const char *text)
{
	CallString(ScintillaFunction::InsertText, pos, text);
}

void SyntaxEditor::ChangeInsertion(Position length,const char *text)
{
	CallString(ScintillaFunction::ChangeInsertion, length, text);
}

void SyntaxEditor::ClearAll()
{
	Call(ScintillaFunction::ClearAll);
}

void SyntaxEditor::DeleteRange(Position start,Position lengthDelete)
{
	Call(ScintillaFunction::DeleteRange, start, lengthDelete);
}

void SyntaxEditor::ClearDocumentStyle()
{
	Call(ScintillaFunction::ClearDocumentStyle);
}

Position SyntaxEditor::GetLength()
{
	return Call(ScintillaFunction::GetLength);
}

int SyntaxEditor::GetCharAt(Position pos)
{
	return static_cast<int>(Call(ScintillaFunction::GetCharAt, pos));
}

Position SyntaxEditor::GetCurrentPos()
{
	return Call(ScintillaFunction::GetCurrentPos);
}

Position SyntaxEditor::GetAnchor()
{
	return Call(ScintillaFunction::GetAnchor);
}

int SyntaxEditor::GetStyleAt(Position pos)
{
	return static_cast<int>(Call(ScintillaFunction::GetStyleAt, pos));
}

void SyntaxEditor::Redo()
{
	Call(ScintillaFunction::Redo);
}

void SyntaxEditor::SetUndoCollection(bool collectUndo)
{
	Call(ScintillaFunction::SetUndoCollection, collectUndo);
}

void SyntaxEditor::SelectAll()
{
	Call(ScintillaFunction::SelectAll);
}

void SyntaxEditor::SetSavePoint()
{
	Call(ScintillaFunction::SetSavePoint);
}

Position SyntaxEditor::GetStyledText(void *tr)
{
	return CallPointer(ScintillaFunction::GetStyledText, 0, tr);
}

bool SyntaxEditor::CanRedo()
{
	return Call(ScintillaFunction::CanRedo);
}

Line SyntaxEditor::MarkerLineFromHandle(int markerHandle)
{
	return Call(ScintillaFunction::MarkerLineFromHandle, markerHandle);
}

void SyntaxEditor::MarkerDeleteHandle(int markerHandle)
{
	Call(ScintillaFunction::MarkerDeleteHandle, markerHandle);
}

int SyntaxEditor::MarkerHandleFromLine(Line line,int which)
{
	return static_cast<int>(Call(ScintillaFunction::MarkerHandleFromLine, line, which));
}

int SyntaxEditor::MarkerNumberFromLine(Line line,int which)
{
	return static_cast<int>(Call(ScintillaFunction::MarkerNumberFromLine, line, which));
}

bool SyntaxEditor::GetUndoCollection()
{
	return Call(ScintillaFunction::GetUndoCollection);
}

WhiteSpace SyntaxEditor::GetViewWS()
{
	return static_cast<SyntaxEditorEnums::WhiteSpace>(Call(ScintillaFunction::GetViewWS));
}

void SyntaxEditor::SetViewWS(WhiteSpace viewWS)
{
	Call(ScintillaFunction::SetViewWS, static_cast<uintptr_t>(viewWS));
}

TabDrawMode SyntaxEditor::GetTabDrawMode()
{
	return static_cast<SyntaxEditorEnums::TabDrawMode>(Call(ScintillaFunction::GetTabDrawMode));
}

void SyntaxEditor::SetTabDrawMode(TabDrawMode tabDrawMode)
{
	Call(ScintillaFunction::SetTabDrawMode, static_cast<uintptr_t>(tabDrawMode));
}

Position SyntaxEditor::PositionFromPoint(int x,int y)
{
	return Call(ScintillaFunction::PositionFromPoint, x, y);
}

Position SyntaxEditor::PositionFromPointClose(int x,int y)
{
	return Call(ScintillaFunction::PositionFromPointClose, x, y);
}

void SyntaxEditor::GotoLine(Line line)
{
	Call(ScintillaFunction::GotoLine, line);
}

void SyntaxEditor::GotoPos(Position caret)
{
	Call(ScintillaFunction::GotoPos, caret);
}

void SyntaxEditor::SetAnchor(Position anchor)
{
	Call(ScintillaFunction::SetAnchor, anchor);
}

Position SyntaxEditor::GetCurLine(Position length,char *text)
{
	return CallPointer(ScintillaFunction::GetCurLine, length, text);
}

std::string SyntaxEditor::GetCurLine(Position length)
{
	return CallReturnString(ScintillaFunction::GetCurLine, length);
}

Position SyntaxEditor::GetEndStyled()
{
	return Call(ScintillaFunction::GetEndStyled);
}

void SyntaxEditor::ConvertEOLs(EndOfLine eolMode)
{
	Call(ScintillaFunction::ConvertEOLs, static_cast<uintptr_t>(eolMode));
}

EndOfLine SyntaxEditor::GetEOLMode()
{
	return static_cast<SyntaxEditorEnums::EndOfLine>(Call(ScintillaFunction::GetEOLMode));
}

void SyntaxEditor::SetEOLMode(EndOfLine eolMode)
{
	Call(ScintillaFunction::SetEOLMode, static_cast<uintptr_t>(eolMode));
}

void SyntaxEditor::StartStyling(Position start,int unused)
{
	Call(ScintillaFunction::StartStyling, start, unused);
}

void SyntaxEditor::SetStyling(Position length,int style)
{
	Call(ScintillaFunction::SetStyling, length, style);
}

bool SyntaxEditor::GetBufferedDraw()
{
	return Call(ScintillaFunction::GetBufferedDraw);
}

void SyntaxEditor::SetBufferedDraw(bool buffered)
{
	Call(ScintillaFunction::SetBufferedDraw, buffered);
}

void SyntaxEditor::SetTabWidth(int tabWidth)
{
	Call(ScintillaFunction::SetTabWidth, tabWidth);
}

int SyntaxEditor::GetTabWidth()
{
	return static_cast<int>(Call(ScintillaFunction::GetTabWidth));
}

void SyntaxEditor::SetTabMinimumWidth(int pixels)
{
	Call(ScintillaFunction::SetTabMinimumWidth, pixels);
}

int SyntaxEditor::GetTabMinimumWidth()
{
	return static_cast<int>(Call(ScintillaFunction::GetTabMinimumWidth));
}

void SyntaxEditor::ClearTabStops(Line line)
{
	Call(ScintillaFunction::ClearTabStops, line);
}

void SyntaxEditor::AddTabStop(Line line,int x)
{
	Call(ScintillaFunction::AddTabStop, line, x);
}

int SyntaxEditor::GetNextTabStop(Line line,int x)
{
	return static_cast<int>(Call(ScintillaFunction::GetNextTabStop, line, x));
}

void SyntaxEditor::SetCodePage(int codePage)
{
	Call(ScintillaFunction::SetCodePage, codePage);
}

void SyntaxEditor::SetFontLocale(const char *localeName)
{
	CallString(ScintillaFunction::SetFontLocale, 0, localeName);
}

int SyntaxEditor::GetFontLocale(char *localeName)
{
	return static_cast<int>(CallPointer(ScintillaFunction::GetFontLocale, 0, localeName));
}

std::string SyntaxEditor::GetFontLocale()
{
	return CallReturnString(ScintillaFunction::GetFontLocale, 0);
}

IMEInteraction SyntaxEditor::GetIMEInteraction()
{
	return static_cast<SyntaxEditorEnums::IMEInteraction>(Call(ScintillaFunction::GetIMEInteraction));
}

void SyntaxEditor::SetIMEInteraction(IMEInteraction imeInteraction)
{
	Call(ScintillaFunction::SetIMEInteraction, static_cast<uintptr_t>(imeInteraction));
}

void SyntaxEditor::MarkerDefine(int markerNumber,MarkerSymbol markerSymbol)
{
	Call(ScintillaFunction::MarkerDefine, markerNumber, static_cast<intptr_t>(markerSymbol));
}

void SyntaxEditor::MarkerSetFore(int markerNumber,Colour fore)
{
	Call(ScintillaFunction::MarkerSetFore, markerNumber, fore);
}

void SyntaxEditor::MarkerSetBack(int markerNumber,Colour back)
{
	Call(ScintillaFunction::MarkerSetBack, markerNumber, back);
}

void SyntaxEditor::MarkerSetBackSelected(int markerNumber,Colour back)
{
	Call(ScintillaFunction::MarkerSetBackSelected, markerNumber, back);
}

void SyntaxEditor::MarkerSetForeTranslucent(int markerNumber,ColourAlpha fore)
{
	Call(ScintillaFunction::MarkerSetForeTranslucent, markerNumber, fore);
}

void SyntaxEditor::MarkerSetBackTranslucent(int markerNumber,ColourAlpha back)
{
	Call(ScintillaFunction::MarkerSetBackTranslucent, markerNumber, back);
}

void SyntaxEditor::MarkerSetBackSelectedTranslucent(int markerNumber,ColourAlpha back)
{
	Call(ScintillaFunction::MarkerSetBackSelectedTranslucent, markerNumber, back);
}

void SyntaxEditor::MarkerSetStrokeWidth(int markerNumber,int hundredths)
{
	Call(ScintillaFunction::MarkerSetStrokeWidth, markerNumber, hundredths);
}

void SyntaxEditor::MarkerEnableHighlight(bool enabled)
{
	Call(ScintillaFunction::MarkerEnableHighlight, enabled);
}

int SyntaxEditor::MarkerAdd(Line line,int markerNumber)
{
	return static_cast<int>(Call(ScintillaFunction::MarkerAdd, line, markerNumber));
}

void SyntaxEditor::MarkerDelete(Line line,int markerNumber)
{
	Call(ScintillaFunction::MarkerDelete, line, markerNumber);
}

void SyntaxEditor::MarkerDeleteAll(int markerNumber)
{
	Call(ScintillaFunction::MarkerDeleteAll, markerNumber);
}

int SyntaxEditor::MarkerGet(Line line)
{
	return static_cast<int>(Call(ScintillaFunction::MarkerGet, line));
}

Line SyntaxEditor::MarkerNext(Line lineStart,int markerMask)
{
	return Call(ScintillaFunction::MarkerNext, lineStart, markerMask);
}

Line SyntaxEditor::MarkerPrevious(Line lineStart,int markerMask)
{
	return Call(ScintillaFunction::MarkerPrevious, lineStart, markerMask);
}

void SyntaxEditor::MarkerDefinePixmap(int markerNumber,const char *pixmap)
{
	CallString(ScintillaFunction::MarkerDefinePixmap, markerNumber, pixmap);
}

void SyntaxEditor::MarkerAddSet(Line line,int markerSet)
{
	Call(ScintillaFunction::MarkerAddSet, line, markerSet);
}

void SyntaxEditor::MarkerSetAlpha(int markerNumber,SyntaxEditorEnums::Alpha alpha)
{
	Call(ScintillaFunction::MarkerSetAlpha, markerNumber, static_cast<intptr_t>(alpha));
}

void SyntaxEditor::SetMarginTypeN(int margin,MarginType marginType)
{
	Call(ScintillaFunction::SetMarginTypeN, margin, static_cast<intptr_t>(marginType));
}

MarginType SyntaxEditor::GetMarginTypeN(int margin)
{
	return static_cast<SyntaxEditorEnums::MarginType>(Call(ScintillaFunction::GetMarginTypeN, margin));
}

void SyntaxEditor::SetMarginWidthN(int margin,int pixelWidth)
{
	Call(ScintillaFunction::SetMarginWidthN, margin, pixelWidth);
}

int SyntaxEditor::GetMarginWidthN(int margin)
{
	return static_cast<int>(Call(ScintillaFunction::GetMarginWidthN, margin));
}

void SyntaxEditor::SetMarginMaskN(int margin,int mask)
{
	Call(ScintillaFunction::SetMarginMaskN, margin, mask);
}

int SyntaxEditor::GetMarginMaskN(int margin)
{
	return static_cast<int>(Call(ScintillaFunction::GetMarginMaskN, margin));
}

void SyntaxEditor::SetMarginSensitiveN(int margin,bool sensitive)
{
	Call(ScintillaFunction::SetMarginSensitiveN, margin, sensitive);
}

bool SyntaxEditor::GetMarginSensitiveN(int margin)
{
	return Call(ScintillaFunction::GetMarginSensitiveN, margin);
}

void SyntaxEditor::SetMarginCursorN(int margin,CursorShape cursor)
{
	Call(ScintillaFunction::SetMarginCursorN, margin, static_cast<intptr_t>(cursor));
}

CursorShape SyntaxEditor::GetMarginCursorN(int margin)
{
	return static_cast<SyntaxEditorEnums::CursorShape>(Call(ScintillaFunction::GetMarginCursorN, margin));
}

void SyntaxEditor::SetMarginBackN(int margin,Colour back)
{
	Call(ScintillaFunction::SetMarginBackN, margin, back);
}

Colour SyntaxEditor::GetMarginBackN(int margin)
{
	return static_cast<Colour>(Call(ScintillaFunction::GetMarginBackN, margin));
}

void SyntaxEditor::SetMargins(int margins)
{
	Call(ScintillaFunction::SetMargins, margins);
}

int SyntaxEditor::GetMargins()
{
	return static_cast<int>(Call(ScintillaFunction::GetMargins));
}

void SyntaxEditor::StyleClearAll()
{
	Call(ScintillaFunction::StyleClearAll);
}

void SyntaxEditor::StyleSetFore(int style,Colour fore)
{
	Call(ScintillaFunction::StyleSetFore, style, fore);
}

void SyntaxEditor::StyleSetBack(int style,Colour back)
{
	Call(ScintillaFunction::StyleSetBack, style, back);
}

void SyntaxEditor::StyleSetBold(int style,bool bold)
{
	Call(ScintillaFunction::StyleSetBold, style, bold);
}

void SyntaxEditor::StyleSetItalic(int style,bool italic)
{
	Call(ScintillaFunction::StyleSetItalic, style, italic);
}

void SyntaxEditor::StyleSetSize(int style,int sizePoints)
{
	Call(ScintillaFunction::StyleSetSize, style, sizePoints);
}

void SyntaxEditor::StyleSetFont(int style,const char *fontName)
{
	CallString(ScintillaFunction::StyleSetFont, style, fontName);
}

void SyntaxEditor::StyleSetEOLFilled(int style,bool eolFilled)
{
	Call(ScintillaFunction::StyleSetEOLFilled, style, eolFilled);
}

void SyntaxEditor::StyleResetDefault()
{
	Call(ScintillaFunction::StyleResetDefault);
}

void SyntaxEditor::StyleSetUnderline(int style,bool underline)
{
	Call(ScintillaFunction::StyleSetUnderline, style, underline);
}

Colour SyntaxEditor::StyleGetFore(int style)
{
	return static_cast<Colour>(Call(ScintillaFunction::StyleGetFore, style));
}

Colour SyntaxEditor::StyleGetBack(int style)
{
	return static_cast<Colour>(Call(ScintillaFunction::StyleGetBack, style));
}

bool SyntaxEditor::StyleGetBold(int style)
{
	return Call(ScintillaFunction::StyleGetBold, style);
}

bool SyntaxEditor::StyleGetItalic(int style)
{
	return Call(ScintillaFunction::StyleGetItalic, style);
}

int SyntaxEditor::StyleGetSize(int style)
{
	return static_cast<int>(Call(ScintillaFunction::StyleGetSize, style));
}

int SyntaxEditor::StyleGetFont(int style,char *fontName)
{
	return static_cast<int>(CallPointer(ScintillaFunction::StyleGetFont, style, fontName));
}

std::string SyntaxEditor::StyleGetFont(int style)
{
	return CallReturnString(ScintillaFunction::StyleGetFont, style);
}

bool SyntaxEditor::StyleGetEOLFilled(int style)
{
	return Call(ScintillaFunction::StyleGetEOLFilled, style);
}

bool SyntaxEditor::StyleGetUnderline(int style)
{
	return Call(ScintillaFunction::StyleGetUnderline, style);
}

CaseVisible SyntaxEditor::StyleGetCase(int style)
{
	return static_cast<SyntaxEditorEnums::CaseVisible>(Call(ScintillaFunction::StyleGetCase, style));
}

CharacterSet SyntaxEditor::StyleGetCharacterSet(int style)
{
	return static_cast<SyntaxEditorEnums::CharacterSet>(Call(ScintillaFunction::StyleGetCharacterSet, style));
}

bool SyntaxEditor::StyleGetVisible(int style)
{
	return Call(ScintillaFunction::StyleGetVisible, style);
}

bool SyntaxEditor::StyleGetChangeable(int style)
{
	return Call(ScintillaFunction::StyleGetChangeable, style);
}

bool SyntaxEditor::StyleGetHotSpot(int style)
{
	return Call(ScintillaFunction::StyleGetHotSpot, style);
}

void SyntaxEditor::StyleSetCase(int style,CaseVisible caseVisible)
{
	Call(ScintillaFunction::StyleSetCase, style, static_cast<intptr_t>(caseVisible));
}

void SyntaxEditor::StyleSetSizeFractional(int style,int sizeHundredthPoints)
{
	Call(ScintillaFunction::StyleSetSizeFractional, style, sizeHundredthPoints);
}

int SyntaxEditor::StyleGetSizeFractional(int style)
{
	return static_cast<int>(Call(ScintillaFunction::StyleGetSizeFractional, style));
}

void SyntaxEditor::StyleSetWeight(int style,FontWeight weight)
{
	Call(ScintillaFunction::StyleSetWeight, style, static_cast<intptr_t>(weight));
}

FontWeight SyntaxEditor::StyleGetWeight(int style)
{
	return static_cast<SyntaxEditorEnums::FontWeight>(Call(ScintillaFunction::StyleGetWeight, style));
}

void SyntaxEditor::StyleSetCharacterSet(int style,CharacterSet characterSet)
{
	Call(ScintillaFunction::StyleSetCharacterSet, style, static_cast<intptr_t>(characterSet));
}

void SyntaxEditor::StyleSetHotSpot(int style,bool hotspot)
{
	Call(ScintillaFunction::StyleSetHotSpot, style, hotspot);
}

void SyntaxEditor::SetElementColour(Element element,ColourAlpha colourElement)
{
	Call(ScintillaFunction::SetElementColour, static_cast<uintptr_t>(element), colourElement);
}

ColourAlpha SyntaxEditor::GetElementColour(Element element)
{
	return static_cast<ColourAlpha>(Call(ScintillaFunction::GetElementColour, static_cast<uintptr_t>(element)));
}

void SyntaxEditor::ResetElementColour(Element element)
{
	Call(ScintillaFunction::ResetElementColour, static_cast<uintptr_t>(element));
}

bool SyntaxEditor::GetElementIsSet(Element element)
{
	return Call(ScintillaFunction::GetElementIsSet, static_cast<uintptr_t>(element));
}

bool SyntaxEditor::GetElementAllowsTranslucent(Element element)
{
	return Call(ScintillaFunction::GetElementAllowsTranslucent, static_cast<uintptr_t>(element));
}

void SyntaxEditor::SetSelFore(bool useSetting,Colour fore)
{
	Call(ScintillaFunction::SetSelFore, useSetting, fore);
}

void SyntaxEditor::SetSelBack(bool useSetting,Colour back)
{
	Call(ScintillaFunction::SetSelBack, useSetting, back);
}

SyntaxEditorEnums::Alpha SyntaxEditor::GetSelAlpha()
{
	return static_cast<SyntaxEditorEnums::Alpha>(Call(ScintillaFunction::GetSelAlpha));
}

void SyntaxEditor::SetSelAlpha(SyntaxEditorEnums::Alpha alpha)
{
	Call(ScintillaFunction::SetSelAlpha, static_cast<uintptr_t>(alpha));
}

bool SyntaxEditor::GetSelEOLFilled()
{
	return Call(ScintillaFunction::GetSelEOLFilled);
}

void SyntaxEditor::SetSelEOLFilled(bool filled)
{
	Call(ScintillaFunction::SetSelEOLFilled, filled);
}

void SyntaxEditor::SetCaretFore(Colour fore)
{
	Call(ScintillaFunction::SetCaretFore, fore);
}

void SyntaxEditor::AssignCmdKey(int keyDefinition,int sciCommand)
{
	Call(ScintillaFunction::AssignCmdKey, keyDefinition, sciCommand);
}

void SyntaxEditor::ClearCmdKey(int keyDefinition)
{
	Call(ScintillaFunction::ClearCmdKey, keyDefinition);
}

void SyntaxEditor::ClearAllCmdKeys()
{
	Call(ScintillaFunction::ClearAllCmdKeys);
}

void SyntaxEditor::SetStylingEx(Position length,const char *styles)
{
	CallString(ScintillaFunction::SetStylingEx, length, styles);
}

void SyntaxEditor::StyleSetVisible(int style,bool visible)
{
	Call(ScintillaFunction::StyleSetVisible, style, visible);
}

int SyntaxEditor::GetCaretPeriod()
{
	return static_cast<int>(Call(ScintillaFunction::GetCaretPeriod));
}

void SyntaxEditor::SetCaretPeriod(int periodMilliseconds)
{
	Call(ScintillaFunction::SetCaretPeriod, periodMilliseconds);
}

void SyntaxEditor::SetWordChars(const char *characters)
{
	CallString(ScintillaFunction::SetWordChars, 0, characters);
}

int SyntaxEditor::GetWordChars(char *characters)
{
	return static_cast<int>(CallPointer(ScintillaFunction::GetWordChars, 0, characters));
}

std::string SyntaxEditor::GetWordChars()
{
	return CallReturnString(ScintillaFunction::GetWordChars, 0);
}

void SyntaxEditor::SetCharacterCategoryOptimization(int countCharacters)
{
	Call(ScintillaFunction::SetCharacterCategoryOptimization, countCharacters);
}

int SyntaxEditor::GetCharacterCategoryOptimization()
{
	return static_cast<int>(Call(ScintillaFunction::GetCharacterCategoryOptimization));
}

void SyntaxEditor::BeginUndoAction()
{
	Call(ScintillaFunction::BeginUndoAction);
}

void SyntaxEditor::EndUndoAction()
{
	Call(ScintillaFunction::EndUndoAction);
}

void SyntaxEditor::IndicSetStyle(int indicator,IndicatorStyle indicatorStyle)
{
	Call(ScintillaFunction::IndicSetStyle, indicator, static_cast<intptr_t>(indicatorStyle));
}

IndicatorStyle SyntaxEditor::IndicGetStyle(int indicator)
{
	return static_cast<SyntaxEditorEnums::IndicatorStyle>(Call(ScintillaFunction::IndicGetStyle, indicator));
}

void SyntaxEditor::IndicSetFore(int indicator,Colour fore)
{
	Call(ScintillaFunction::IndicSetFore, indicator, fore);
}

Colour SyntaxEditor::IndicGetFore(int indicator)
{
	return static_cast<Colour>(Call(ScintillaFunction::IndicGetFore, indicator));
}

void SyntaxEditor::IndicSetUnder(int indicator,bool under)
{
	Call(ScintillaFunction::IndicSetUnder, indicator, under);
}

bool SyntaxEditor::IndicGetUnder(int indicator)
{
	return Call(ScintillaFunction::IndicGetUnder, indicator);
}

void SyntaxEditor::IndicSetHoverStyle(int indicator,IndicatorStyle indicatorStyle)
{
	Call(ScintillaFunction::IndicSetHoverStyle, indicator, static_cast<intptr_t>(indicatorStyle));
}

IndicatorStyle SyntaxEditor::IndicGetHoverStyle(int indicator)
{
	return static_cast<SyntaxEditorEnums::IndicatorStyle>(Call(ScintillaFunction::IndicGetHoverStyle, indicator));
}

void SyntaxEditor::IndicSetHoverFore(int indicator,Colour fore)
{
	Call(ScintillaFunction::IndicSetHoverFore, indicator, fore);
}

Colour SyntaxEditor::IndicGetHoverFore(int indicator)
{
	return static_cast<Colour>(Call(ScintillaFunction::IndicGetHoverFore, indicator));
}

void SyntaxEditor::IndicSetFlags(int indicator,IndicFlag flags)
{
	Call(ScintillaFunction::IndicSetFlags, indicator, static_cast<intptr_t>(flags));
}

IndicFlag SyntaxEditor::IndicGetFlags(int indicator)
{
	return static_cast<SyntaxEditorEnums::IndicFlag>(Call(ScintillaFunction::IndicGetFlags, indicator));
}

void SyntaxEditor::IndicSetStrokeWidth(int indicator,int hundredths)
{
	Call(ScintillaFunction::IndicSetStrokeWidth, indicator, hundredths);
}

int SyntaxEditor::IndicGetStrokeWidth(int indicator)
{
	return static_cast<int>(Call(ScintillaFunction::IndicGetStrokeWidth, indicator));
}

void SyntaxEditor::SetWhitespaceFore(bool useSetting,Colour fore)
{
	Call(ScintillaFunction::SetWhitespaceFore, useSetting, fore);
}

void SyntaxEditor::SetWhitespaceBack(bool useSetting,Colour back)
{
	Call(ScintillaFunction::SetWhitespaceBack, useSetting, back);
}

void SyntaxEditor::SetWhitespaceSize(int size)
{
	Call(ScintillaFunction::SetWhitespaceSize, size);
}

int SyntaxEditor::GetWhitespaceSize()
{
	return static_cast<int>(Call(ScintillaFunction::GetWhitespaceSize));
}

void SyntaxEditor::SetLineState(Line line,int state)
{
	Call(ScintillaFunction::SetLineState, line, state);
}

int SyntaxEditor::GetLineState(Line line)
{
	return static_cast<int>(Call(ScintillaFunction::GetLineState, line));
}

int SyntaxEditor::GetMaxLineState()
{
	return static_cast<int>(Call(ScintillaFunction::GetMaxLineState));
}

bool SyntaxEditor::GetCaretLineVisible()
{
	return Call(ScintillaFunction::GetCaretLineVisible);
}

void SyntaxEditor::SetCaretLineVisible(bool show)
{
	Call(ScintillaFunction::SetCaretLineVisible, show);
}

Colour SyntaxEditor::GetCaretLineBack()
{
	return static_cast<Colour>(Call(ScintillaFunction::GetCaretLineBack));
}

void SyntaxEditor::SetCaretLineBack(Colour back)
{
	Call(ScintillaFunction::SetCaretLineBack, back);
}

int SyntaxEditor::GetCaretLineFrame()
{
	return static_cast<int>(Call(ScintillaFunction::GetCaretLineFrame));
}

void SyntaxEditor::SetCaretLineFrame(int width)
{
	Call(ScintillaFunction::SetCaretLineFrame, width);
}

void SyntaxEditor::StyleSetChangeable(int style,bool changeable)
{
	Call(ScintillaFunction::StyleSetChangeable, style, changeable);
}

void SyntaxEditor::AutoCShow(Position lengthEntered,const char *itemList)
{
	CallString(ScintillaFunction::AutoCShow, lengthEntered, itemList);
}

void SyntaxEditor::AutoCCancel()
{
	Call(ScintillaFunction::AutoCCancel);
}

bool SyntaxEditor::AutoCActive()
{
	return Call(ScintillaFunction::AutoCActive);
}

Position SyntaxEditor::AutoCPosStart()
{
	return Call(ScintillaFunction::AutoCPosStart);
}

void SyntaxEditor::AutoCComplete()
{
	Call(ScintillaFunction::AutoCComplete);
}

void SyntaxEditor::AutoCStops(const char *characterSet)
{
	CallString(ScintillaFunction::AutoCStops, 0, characterSet);
}

void SyntaxEditor::AutoCSetSeparator(int separatorCharacter)
{
	Call(ScintillaFunction::AutoCSetSeparator, separatorCharacter);
}

int SyntaxEditor::AutoCGetSeparator()
{
	return static_cast<int>(Call(ScintillaFunction::AutoCGetSeparator));
}

void SyntaxEditor::AutoCSelect(const char *select)
{
	CallString(ScintillaFunction::AutoCSelect, 0, select);
}

void SyntaxEditor::AutoCSetCancelAtStart(bool cancel)
{
	Call(ScintillaFunction::AutoCSetCancelAtStart, cancel);
}

bool SyntaxEditor::AutoCGetCancelAtStart()
{
	return Call(ScintillaFunction::AutoCGetCancelAtStart);
}

void SyntaxEditor::AutoCSetFillUps(const char *characterSet)
{
	CallString(ScintillaFunction::AutoCSetFillUps, 0, characterSet);
}

void SyntaxEditor::AutoCSetChooseSingle(bool chooseSingle)
{
	Call(ScintillaFunction::AutoCSetChooseSingle, chooseSingle);
}

bool SyntaxEditor::AutoCGetChooseSingle()
{
	return Call(ScintillaFunction::AutoCGetChooseSingle);
}

void SyntaxEditor::AutoCSetIgnoreCase(bool ignoreCase)
{
	Call(ScintillaFunction::AutoCSetIgnoreCase, ignoreCase);
}

bool SyntaxEditor::AutoCGetIgnoreCase()
{
	return Call(ScintillaFunction::AutoCGetIgnoreCase);
}

void SyntaxEditor::UserListShow(int listType,const char *itemList)
{
	CallString(ScintillaFunction::UserListShow, listType, itemList);
}

void SyntaxEditor::AutoCSetAutoHide(bool autoHide)
{
	Call(ScintillaFunction::AutoCSetAutoHide, autoHide);
}

bool SyntaxEditor::AutoCGetAutoHide()
{
	return Call(ScintillaFunction::AutoCGetAutoHide);
}

void SyntaxEditor::AutoCSetDropRestOfWord(bool dropRestOfWord)
{
	Call(ScintillaFunction::AutoCSetDropRestOfWord, dropRestOfWord);
}

bool SyntaxEditor::AutoCGetDropRestOfWord()
{
	return Call(ScintillaFunction::AutoCGetDropRestOfWord);
}

void SyntaxEditor::RegisterImage(int type,const char *xpmData)
{
	CallString(ScintillaFunction::RegisterImage, type, xpmData);
}

void SyntaxEditor::ClearRegisteredImages()
{
	Call(ScintillaFunction::ClearRegisteredImages);
}

int SyntaxEditor::AutoCGetTypeSeparator()
{
	return static_cast<int>(Call(ScintillaFunction::AutoCGetTypeSeparator));
}

void SyntaxEditor::AutoCSetTypeSeparator(int separatorCharacter)
{
	Call(ScintillaFunction::AutoCSetTypeSeparator, separatorCharacter);
}

void SyntaxEditor::AutoCSetMaxWidth(int characterCount)
{
	Call(ScintillaFunction::AutoCSetMaxWidth, characterCount);
}

int SyntaxEditor::AutoCGetMaxWidth()
{
	return static_cast<int>(Call(ScintillaFunction::AutoCGetMaxWidth));
}

void SyntaxEditor::AutoCSetMaxHeight(int rowCount)
{
	Call(ScintillaFunction::AutoCSetMaxHeight, rowCount);
}

int SyntaxEditor::AutoCGetMaxHeight()
{
	return static_cast<int>(Call(ScintillaFunction::AutoCGetMaxHeight));
}

void SyntaxEditor::SetIndent(int indentSize)
{
	Call(ScintillaFunction::SetIndent, indentSize);
}

int SyntaxEditor::GetIndent()
{
	return static_cast<int>(Call(ScintillaFunction::GetIndent));
}

void SyntaxEditor::SetUseTabs(bool useTabs)
{
	Call(ScintillaFunction::SetUseTabs, useTabs);
}

bool SyntaxEditor::GetUseTabs()
{
	return Call(ScintillaFunction::GetUseTabs);
}

void SyntaxEditor::SetLineIndentation(Line line,int indentation)
{
	Call(ScintillaFunction::SetLineIndentation, line, indentation);
}

int SyntaxEditor::GetLineIndentation(Line line)
{
	return static_cast<int>(Call(ScintillaFunction::GetLineIndentation, line));
}

Position SyntaxEditor::GetLineIndentPosition(Line line)
{
	return Call(ScintillaFunction::GetLineIndentPosition, line);
}

Position SyntaxEditor::GetColumn(Position pos)
{
	return Call(ScintillaFunction::GetColumn, pos);
}

Position SyntaxEditor::CountCharacters(Position start,Position end)
{
	return Call(ScintillaFunction::CountCharacters, start, end);
}

Position SyntaxEditor::CountCodeUnits(Position start,Position end)
{
	return Call(ScintillaFunction::CountCodeUnits, start, end);
}

void SyntaxEditor::SetHScrollBar(bool visible)
{
	Call(ScintillaFunction::SetHScrollBar, visible);
}

bool SyntaxEditor::GetHScrollBar()
{
	return Call(ScintillaFunction::GetHScrollBar);
}

void SyntaxEditor::SetIndentationGuides(IndentView indentView)
{
	Call(ScintillaFunction::SetIndentationGuides, static_cast<uintptr_t>(indentView));
}

IndentView SyntaxEditor::GetIndentationGuides()
{
	return static_cast<SyntaxEditorEnums::IndentView>(Call(ScintillaFunction::GetIndentationGuides));
}

void SyntaxEditor::SetHighlightGuide(Position column)
{
	Call(ScintillaFunction::SetHighlightGuide, column);
}

Position SyntaxEditor::GetHighlightGuide()
{
	return Call(ScintillaFunction::GetHighlightGuide);
}

Position SyntaxEditor::GetLineEndPosition(Line line)
{
	return Call(ScintillaFunction::GetLineEndPosition, line);
}

int SyntaxEditor::GetCodePage()
{
	return static_cast<int>(Call(ScintillaFunction::GetCodePage));
}

Colour SyntaxEditor::GetCaretFore()
{
	return static_cast<Colour>(Call(ScintillaFunction::GetCaretFore));
}

bool SyntaxEditor::GetReadOnly()
{
	return Call(ScintillaFunction::GetReadOnly);
}

void SyntaxEditor::SetCurrentPos(Position caret)
{
	Call(ScintillaFunction::SetCurrentPos, caret);
}

void SyntaxEditor::SetSelectionStart(Position anchor)
{
	Call(ScintillaFunction::SetSelectionStart, anchor);
}

Position SyntaxEditor::GetSelectionStart()
{
	return Call(ScintillaFunction::GetSelectionStart);
}

void SyntaxEditor::SetSelectionEnd(Position caret)
{
	Call(ScintillaFunction::SetSelectionEnd, caret);
}

Position SyntaxEditor::GetSelectionEnd()
{
	return Call(ScintillaFunction::GetSelectionEnd);
}

void SyntaxEditor::SetEmptySelection(Position caret)
{
	Call(ScintillaFunction::SetEmptySelection, caret);
}

void SyntaxEditor::SetPrintMagnification(int magnification)
{
	Call(ScintillaFunction::SetPrintMagnification, magnification);
}

int SyntaxEditor::GetPrintMagnification()
{
	return static_cast<int>(Call(ScintillaFunction::GetPrintMagnification));
}

void SyntaxEditor::SetPrintColourMode(PrintOption mode)
{
	Call(ScintillaFunction::SetPrintColourMode, static_cast<uintptr_t>(mode));
}

PrintOption SyntaxEditor::GetPrintColourMode()
{
	return static_cast<SyntaxEditorEnums::PrintOption>(Call(ScintillaFunction::GetPrintColourMode));
}

Position SyntaxEditor::FindText(FindOption searchFlags,void *ft)
{
	return CallPointer(ScintillaFunction::FindText, static_cast<uintptr_t>(searchFlags), ft);
}

Position SyntaxEditor::FormatRange(bool draw,void *fr)
{
	return CallPointer(ScintillaFunction::FormatRange, draw, fr);
}

Line SyntaxEditor::GetFirstVisibleLine()
{
	return Call(ScintillaFunction::GetFirstVisibleLine);
}

Position SyntaxEditor::GetLine(Line line,char *text)
{
	return CallPointer(ScintillaFunction::GetLine, line, text);
}

std::string SyntaxEditor::GetLine(Line line)
{
	return CallReturnString(ScintillaFunction::GetLine, line);
}

Line SyntaxEditor::GetLineCount()
{
	return Call(ScintillaFunction::GetLineCount);
}

void SyntaxEditor::SetMarginLeft(int pixelWidth)
{
	Call(ScintillaFunction::SetMarginLeft, 0, pixelWidth);
}

int SyntaxEditor::GetMarginLeft()
{
	return static_cast<int>(Call(ScintillaFunction::GetMarginLeft));
}

void SyntaxEditor::SetMarginRight(int pixelWidth)
{
	Call(ScintillaFunction::SetMarginRight, 0, pixelWidth);
}

int SyntaxEditor::GetMarginRight()
{
	return static_cast<int>(Call(ScintillaFunction::GetMarginRight));
}

bool SyntaxEditor::GetModify()
{
	return Call(ScintillaFunction::GetModify);
}

void SyntaxEditor::SetSel(Position anchor,Position caret)
{
	Call(ScintillaFunction::SetSel, anchor, caret);
}

Position SyntaxEditor::GetSelText(char *text)
{
	return CallPointer(ScintillaFunction::GetSelText, 0, text);
}

std::string SyntaxEditor::GetSelText()
{
	return CallReturnString(ScintillaFunction::GetSelText, 0);
}

Position SyntaxEditor::GetTextRange(void *tr)
{
	return CallPointer(ScintillaFunction::GetTextRange, 0, tr);
}

void SyntaxEditor::HideSelection(bool hide)
{
	Call(ScintillaFunction::HideSelection, hide);
}

int SyntaxEditor::PointXFromPosition(Position pos)
{
	return static_cast<int>(Call(ScintillaFunction::PointXFromPosition, 0, pos));
}

int SyntaxEditor::PointYFromPosition(Position pos)
{
	return static_cast<int>(Call(ScintillaFunction::PointYFromPosition, 0, pos));
}

Line SyntaxEditor::LineFromPosition(Position pos)
{
	return Call(ScintillaFunction::LineFromPosition, pos);
}

Position SyntaxEditor::PositionFromLine(Line line)
{
	return Call(ScintillaFunction::PositionFromLine, line);
}

void SyntaxEditor::LineScroll(Position columns,Line lines)
{
	Call(ScintillaFunction::LineScroll, columns, lines);
}

void SyntaxEditor::ScrollCaret()
{
	Call(ScintillaFunction::ScrollCaret);
}

void SyntaxEditor::ScrollRange(Position secondary,Position primary)
{
	Call(ScintillaFunction::ScrollRange, secondary, primary);
}

void SyntaxEditor::ReplaceSel(const char *text)
{
	CallString(ScintillaFunction::ReplaceSel, 0, text);
}

void SyntaxEditor::SetReadOnly(bool readOnly)
{
	Call(ScintillaFunction::SetReadOnly, readOnly);
}

void SyntaxEditor::Null()
{
	Call(ScintillaFunction::Null);
}

bool SyntaxEditor::CanPaste()
{
	return Call(ScintillaFunction::CanPaste);
}

bool SyntaxEditor::CanUndo()
{
	return Call(ScintillaFunction::CanUndo);
}

void SyntaxEditor::EmptyUndoBuffer()
{
	Call(ScintillaFunction::EmptyUndoBuffer);
}

void SyntaxEditor::Undo()
{
	Call(ScintillaFunction::Undo);
}

void SyntaxEditor::Cut()
{
	Call(ScintillaFunction::Cut);
}

void SyntaxEditor::Copy()
{
	Call(ScintillaFunction::Copy);
}

void SyntaxEditor::Paste()
{
	Call(ScintillaFunction::Paste);
}

void SyntaxEditor::Clear()
{
	Call(ScintillaFunction::Clear);
}

void SyntaxEditor::SetText(const char *text)
{
	CallString(ScintillaFunction::SetText, 0, text);
}

Position SyntaxEditor::GetText(Position length,char *text)
{
	return CallPointer(ScintillaFunction::GetText, length, text);
}

std::string SyntaxEditor::GetText(Position length)
{
	return CallReturnString(ScintillaFunction::GetText, length);
}

Position SyntaxEditor::GetTextLength()
{
	return Call(ScintillaFunction::GetTextLength);
}

void *SyntaxEditor::GetDirectFunction()
{
	return reinterpret_cast<void *>(Call(ScintillaFunction::GetDirectFunction));
}

void *SyntaxEditor::GetDirectPointer()
{
	return reinterpret_cast<void *>(Call(ScintillaFunction::GetDirectPointer));
}

void SyntaxEditor::SetOvertype(bool overType)
{
	Call(ScintillaFunction::SetOvertype, overType);
}

bool SyntaxEditor::GetOvertype()
{
	return Call(ScintillaFunction::GetOvertype);
}

void SyntaxEditor::SetCaretWidth(int pixelWidth)
{
	Call(ScintillaFunction::SetCaretWidth, pixelWidth);
}

int SyntaxEditor::GetCaretWidth()
{
	return static_cast<int>(Call(ScintillaFunction::GetCaretWidth));
}

void SyntaxEditor::SetTargetStart(Position start)
{
	Call(ScintillaFunction::SetTargetStart, start);
}

Position SyntaxEditor::GetTargetStart()
{
	return Call(ScintillaFunction::GetTargetStart);
}

void SyntaxEditor::SetTargetStartVirtualSpace(Position space)
{
	Call(ScintillaFunction::SetTargetStartVirtualSpace, space);
}

Position SyntaxEditor::GetTargetStartVirtualSpace()
{
	return Call(ScintillaFunction::GetTargetStartVirtualSpace);
}

void SyntaxEditor::SetTargetEnd(Position end)
{
	Call(ScintillaFunction::SetTargetEnd, end);
}

Position SyntaxEditor::GetTargetEnd()
{
	return Call(ScintillaFunction::GetTargetEnd);
}

void SyntaxEditor::SetTargetEndVirtualSpace(Position space)
{
	Call(ScintillaFunction::SetTargetEndVirtualSpace, space);
}

Position SyntaxEditor::GetTargetEndVirtualSpace()
{
	return Call(ScintillaFunction::GetTargetEndVirtualSpace);
}

void SyntaxEditor::SetTargetRange(Position start,Position end)
{
	Call(ScintillaFunction::SetTargetRange, start, end);
}

Position SyntaxEditor::GetTargetText(char *text)
{
	return CallPointer(ScintillaFunction::GetTargetText, 0, text);
}

std::string SyntaxEditor::GetTargetText()
{
	return CallReturnString(ScintillaFunction::GetTargetText, 0);
}

void SyntaxEditor::TargetFromSelection()
{
	Call(ScintillaFunction::TargetFromSelection);
}

void SyntaxEditor::TargetWholeDocument()
{
	Call(ScintillaFunction::TargetWholeDocument);
}

Position SyntaxEditor::ReplaceTarget(Position length,const char *text)
{
	return CallString(ScintillaFunction::ReplaceTarget, length, text);
}

Position SyntaxEditor::ReplaceTargetRE(Position length,const char *text)
{
	return CallString(ScintillaFunction::ReplaceTargetRE, length, text);
}

Position SyntaxEditor::SearchInTarget(Position length,const char *text)
{
	return CallString(ScintillaFunction::SearchInTarget, length, text);
}

void SyntaxEditor::SetSearchFlags(FindOption searchFlags)
{
	Call(ScintillaFunction::SetSearchFlags, static_cast<uintptr_t>(searchFlags));
}

FindOption SyntaxEditor::GetSearchFlags()
{
	return static_cast<SyntaxEditorEnums::FindOption>(Call(ScintillaFunction::GetSearchFlags));
}

void SyntaxEditor::CallTipShow(Position pos,const char *definition)
{
	CallString(ScintillaFunction::CallTipShow, pos, definition);
}

void SyntaxEditor::CallTipCancel()
{
	Call(ScintillaFunction::CallTipCancel);
}

bool SyntaxEditor::CallTipActive()
{
	return Call(ScintillaFunction::CallTipActive);
}

Position SyntaxEditor::CallTipPosStart()
{
	return Call(ScintillaFunction::CallTipPosStart);
}

void SyntaxEditor::CallTipSetPosStart(Position posStart)
{
	Call(ScintillaFunction::CallTipSetPosStart, posStart);
}

void SyntaxEditor::CallTipSetHlt(Position highlightStart,Position highlightEnd)
{
	Call(ScintillaFunction::CallTipSetHlt, highlightStart, highlightEnd);
}

void SyntaxEditor::CallTipSetBack(Colour back)
{
	Call(ScintillaFunction::CallTipSetBack, back);
}

void SyntaxEditor::CallTipSetFore(Colour fore)
{
	Call(ScintillaFunction::CallTipSetFore, fore);
}

void SyntaxEditor::CallTipSetForeHlt(Colour fore)
{
	Call(ScintillaFunction::CallTipSetForeHlt, fore);
}

void SyntaxEditor::CallTipUseStyle(int tabSize)
{
	Call(ScintillaFunction::CallTipUseStyle, tabSize);
}

void SyntaxEditor::CallTipSetPosition(bool above)
{
	Call(ScintillaFunction::CallTipSetPosition, above);
}

Line SyntaxEditor::VisibleFromDocLine(Line docLine)
{
	return Call(ScintillaFunction::VisibleFromDocLine, docLine);
}

Line SyntaxEditor::DocLineFromVisible(Line displayLine)
{
	return Call(ScintillaFunction::DocLineFromVisible, displayLine);
}

Line SyntaxEditor::WrapCount(Line docLine)
{
	return Call(ScintillaFunction::WrapCount, docLine);
}

void SyntaxEditor::SetFoldLevel(Line line,FoldLevel level)
{
	Call(ScintillaFunction::SetFoldLevel, line, static_cast<intptr_t>(level));
}

FoldLevel SyntaxEditor::GetFoldLevel(Line line)
{
	return static_cast<SyntaxEditorEnums::FoldLevel>(Call(ScintillaFunction::GetFoldLevel, line));
}

Line SyntaxEditor::GetLastChild(Line line,FoldLevel level)
{
	return Call(ScintillaFunction::GetLastChild, line, static_cast<intptr_t>(level));
}

Line SyntaxEditor::GetFoldParent(Line line)
{
	return Call(ScintillaFunction::GetFoldParent, line);
}

void SyntaxEditor::ShowLines(Line lineStart,Line lineEnd)
{
	Call(ScintillaFunction::ShowLines, lineStart, lineEnd);
}

void SyntaxEditor::HideLines(Line lineStart,Line lineEnd)
{
	Call(ScintillaFunction::HideLines, lineStart, lineEnd);
}

bool SyntaxEditor::GetLineVisible(Line line)
{
	return Call(ScintillaFunction::GetLineVisible, line);
}

bool SyntaxEditor::GetAllLinesVisible()
{
	return Call(ScintillaFunction::GetAllLinesVisible);
}

void SyntaxEditor::SetFoldExpanded(Line line,bool expanded)
{
	Call(ScintillaFunction::SetFoldExpanded, line, expanded);
}

bool SyntaxEditor::GetFoldExpanded(Line line)
{
	return Call(ScintillaFunction::GetFoldExpanded, line);
}

void SyntaxEditor::ToggleFold(Line line)
{
	Call(ScintillaFunction::ToggleFold, line);
}

void SyntaxEditor::ToggleFoldShowText(Line line,const char *text)
{
	CallString(ScintillaFunction::ToggleFoldShowText, line, text);
}

void SyntaxEditor::FoldDisplayTextSetStyle(FoldDisplayTextStyle style)
{
	Call(ScintillaFunction::FoldDisplayTextSetStyle, static_cast<uintptr_t>(style));
}

FoldDisplayTextStyle SyntaxEditor::FoldDisplayTextGetStyle()
{
	return static_cast<SyntaxEditorEnums::FoldDisplayTextStyle>(Call(ScintillaFunction::FoldDisplayTextGetStyle));
}

void SyntaxEditor::SetDefaultFoldDisplayText(const char *text)
{
	CallString(ScintillaFunction::SetDefaultFoldDisplayText, 0, text);
}

int SyntaxEditor::GetDefaultFoldDisplayText(char *text)
{
	return static_cast<int>(CallPointer(ScintillaFunction::GetDefaultFoldDisplayText, 0, text));
}

std::string SyntaxEditor::GetDefaultFoldDisplayText()
{
	return CallReturnString(ScintillaFunction::GetDefaultFoldDisplayText, 0);
}

void SyntaxEditor::FoldLine(Line line,FoldAction action)
{
	Call(ScintillaFunction::FoldLine, line, static_cast<intptr_t>(action));
}

void SyntaxEditor::FoldChildren(Line line,FoldAction action)
{
	Call(ScintillaFunction::FoldChildren, line, static_cast<intptr_t>(action));
}

void SyntaxEditor::ExpandChildren(Line line,FoldLevel level)
{
	Call(ScintillaFunction::ExpandChildren, line, static_cast<intptr_t>(level));
}

void SyntaxEditor::FoldAll(FoldAction action)
{
	Call(ScintillaFunction::FoldAll, static_cast<uintptr_t>(action));
}

void SyntaxEditor::EnsureVisible(Line line)
{
	Call(ScintillaFunction::EnsureVisible, line);
}

void SyntaxEditor::SetAutomaticFold(AutomaticFold automaticFold)
{
	Call(ScintillaFunction::SetAutomaticFold, static_cast<uintptr_t>(automaticFold));
}

AutomaticFold SyntaxEditor::GetAutomaticFold()
{
	return static_cast<SyntaxEditorEnums::AutomaticFold>(Call(ScintillaFunction::GetAutomaticFold));
}

void SyntaxEditor::SetFoldFlags(FoldFlag flags)
{
	Call(ScintillaFunction::SetFoldFlags, static_cast<uintptr_t>(flags));
}

void SyntaxEditor::EnsureVisibleEnforcePolicy(Line line)
{
	Call(ScintillaFunction::EnsureVisibleEnforcePolicy, line);
}

void SyntaxEditor::SetTabIndents(bool tabIndents)
{
	Call(ScintillaFunction::SetTabIndents, tabIndents);
}

bool SyntaxEditor::GetTabIndents()
{
	return Call(ScintillaFunction::GetTabIndents);
}

void SyntaxEditor::SetBackSpaceUnIndents(bool bsUnIndents)
{
	Call(ScintillaFunction::SetBackSpaceUnIndents, bsUnIndents);
}

bool SyntaxEditor::GetBackSpaceUnIndents()
{
	return Call(ScintillaFunction::GetBackSpaceUnIndents);
}

void SyntaxEditor::SetMouseDwellTime(int periodMilliseconds)
{
	Call(ScintillaFunction::SetMouseDwellTime, periodMilliseconds);
}

int SyntaxEditor::GetMouseDwellTime()
{
	return static_cast<int>(Call(ScintillaFunction::GetMouseDwellTime));
}

Position SyntaxEditor::WordStartPosition(Position pos,bool onlyWordCharacters)
{
	return Call(ScintillaFunction::WordStartPosition, pos, onlyWordCharacters);
}

Position SyntaxEditor::WordEndPosition(Position pos,bool onlyWordCharacters)
{
	return Call(ScintillaFunction::WordEndPosition, pos, onlyWordCharacters);
}

bool SyntaxEditor::IsRangeWord(Position start,Position end)
{
	return Call(ScintillaFunction::IsRangeWord, start, end);
}

void SyntaxEditor::SetIdleStyling(IdleStyling idleStyling)
{
	Call(ScintillaFunction::SetIdleStyling, static_cast<uintptr_t>(idleStyling));
}

IdleStyling SyntaxEditor::GetIdleStyling()
{
	return static_cast<SyntaxEditorEnums::IdleStyling>(Call(ScintillaFunction::GetIdleStyling));
}

void SyntaxEditor::SetWrapMode(Wrap wrapMode)
{
	Call(ScintillaFunction::SetWrapMode, static_cast<uintptr_t>(wrapMode));
}

Wrap SyntaxEditor::GetWrapMode()
{
	return static_cast<SyntaxEditorEnums::Wrap>(Call(ScintillaFunction::GetWrapMode));
}

void SyntaxEditor::SetWrapVisualFlags(WrapVisualFlag wrapVisualFlags)
{
	Call(ScintillaFunction::SetWrapVisualFlags, static_cast<uintptr_t>(wrapVisualFlags));
}

WrapVisualFlag SyntaxEditor::GetWrapVisualFlags()
{
	return static_cast<SyntaxEditorEnums::WrapVisualFlag>(Call(ScintillaFunction::GetWrapVisualFlags));
}

void SyntaxEditor::SetWrapVisualFlagsLocation(WrapVisualLocation wrapVisualFlagsLocation)
{
	Call(ScintillaFunction::SetWrapVisualFlagsLocation, static_cast<uintptr_t>(wrapVisualFlagsLocation));
}

WrapVisualLocation SyntaxEditor::GetWrapVisualFlagsLocation()
{
	return static_cast<SyntaxEditorEnums::WrapVisualLocation>(Call(ScintillaFunction::GetWrapVisualFlagsLocation));
}

void SyntaxEditor::SetWrapStartIndent(int indent)
{
	Call(ScintillaFunction::SetWrapStartIndent, indent);
}

int SyntaxEditor::GetWrapStartIndent()
{
	return static_cast<int>(Call(ScintillaFunction::GetWrapStartIndent));
}

void SyntaxEditor::SetWrapIndentMode(WrapIndentMode wrapIndentMode)
{
	Call(ScintillaFunction::SetWrapIndentMode, static_cast<uintptr_t>(wrapIndentMode));
}

WrapIndentMode SyntaxEditor::GetWrapIndentMode()
{
	return static_cast<SyntaxEditorEnums::WrapIndentMode>(Call(ScintillaFunction::GetWrapIndentMode));
}

void SyntaxEditor::SetLayoutCache(LineCache cacheMode)
{
	Call(ScintillaFunction::SetLayoutCache, static_cast<uintptr_t>(cacheMode));
}

LineCache SyntaxEditor::GetLayoutCache()
{
	return static_cast<SyntaxEditorEnums::LineCache>(Call(ScintillaFunction::GetLayoutCache));
}

void SyntaxEditor::SetScrollWidth(int pixelWidth)
{
	Call(ScintillaFunction::SetScrollWidth, pixelWidth);
}

int SyntaxEditor::GetScrollWidth()
{
	return static_cast<int>(Call(ScintillaFunction::GetScrollWidth));
}

void SyntaxEditor::SetScrollWidthTracking(bool tracking)
{
	Call(ScintillaFunction::SetScrollWidthTracking, tracking);
}

bool SyntaxEditor::GetScrollWidthTracking()
{
	return Call(ScintillaFunction::GetScrollWidthTracking);
}

int SyntaxEditor::TextWidth(int style,const char *text)
{
	return static_cast<int>(CallString(ScintillaFunction::TextWidth, style, text));
}

void SyntaxEditor::SetEndAtLastLine(bool endAtLastLine)
{
	Call(ScintillaFunction::SetEndAtLastLine, endAtLastLine);
}

bool SyntaxEditor::GetEndAtLastLine()
{
	return Call(ScintillaFunction::GetEndAtLastLine);
}

int SyntaxEditor::TextHeight(Line line)
{
	return static_cast<int>(Call(ScintillaFunction::TextHeight, line));
}

void SyntaxEditor::SetVScrollBar(bool visible)
{
	Call(ScintillaFunction::SetVScrollBar, visible);
}

bool SyntaxEditor::GetVScrollBar()
{
	return Call(ScintillaFunction::GetVScrollBar);
}

void SyntaxEditor::AppendText(Position length,const char *text)
{
	CallString(ScintillaFunction::AppendText, length, text);
}

PhasesDraw SyntaxEditor::GetPhasesDraw()
{
	return static_cast<SyntaxEditorEnums::PhasesDraw>(Call(ScintillaFunction::GetPhasesDraw));
}

void SyntaxEditor::SetPhasesDraw(PhasesDraw phases)
{
	Call(ScintillaFunction::SetPhasesDraw, static_cast<uintptr_t>(phases));
}

void SyntaxEditor::SetFontQuality(FontQuality fontQuality)
{
	Call(ScintillaFunction::SetFontQuality, static_cast<uintptr_t>(fontQuality));
}

FontQuality SyntaxEditor::GetFontQuality()
{
	return static_cast<SyntaxEditorEnums::FontQuality>(Call(ScintillaFunction::GetFontQuality));
}

void SyntaxEditor::SetFirstVisibleLine(Line displayLine)
{
	Call(ScintillaFunction::SetFirstVisibleLine, displayLine);
}

void SyntaxEditor::SetMultiPaste(MultiPaste multiPaste)
{
	Call(ScintillaFunction::SetMultiPaste, static_cast<uintptr_t>(multiPaste));
}

MultiPaste SyntaxEditor::GetMultiPaste()
{
	return static_cast<SyntaxEditorEnums::MultiPaste>(Call(ScintillaFunction::GetMultiPaste));
}

int SyntaxEditor::GetTag(int tagNumber,char *tagValue)
{
	return static_cast<int>(CallPointer(ScintillaFunction::GetTag, tagNumber, tagValue));
}

std::string SyntaxEditor::GetTag(int tagNumber)
{
	return CallReturnString(ScintillaFunction::GetTag, tagNumber);
}

void SyntaxEditor::LinesJoin()
{
	Call(ScintillaFunction::LinesJoin);
}

void SyntaxEditor::LinesSplit(int pixelWidth)
{
	Call(ScintillaFunction::LinesSplit, pixelWidth);
}

void SyntaxEditor::SetFoldMarginColour(bool useSetting,Colour back)
{
	Call(ScintillaFunction::SetFoldMarginColour, useSetting, back);
}

void SyntaxEditor::SetFoldMarginHiColour(bool useSetting,Colour fore)
{
	Call(ScintillaFunction::SetFoldMarginHiColour, useSetting, fore);
}

void SyntaxEditor::SetAccessibility(Accessibility accessibility)
{
	Call(ScintillaFunction::SetAccessibility, static_cast<uintptr_t>(accessibility));
}

Accessibility SyntaxEditor::GetAccessibility()
{
	return static_cast<SyntaxEditorEnums::Accessibility>(Call(ScintillaFunction::GetAccessibility));
}

void SyntaxEditor::LineDown()
{
	Call(ScintillaFunction::LineDown);
}

void SyntaxEditor::LineDownExtend()
{
	Call(ScintillaFunction::LineDownExtend);
}

void SyntaxEditor::LineUp()
{
	Call(ScintillaFunction::LineUp);
}

void SyntaxEditor::LineUpExtend()
{
	Call(ScintillaFunction::LineUpExtend);
}

void SyntaxEditor::CharLeft()
{
	Call(ScintillaFunction::CharLeft);
}

void SyntaxEditor::CharLeftExtend()
{
	Call(ScintillaFunction::CharLeftExtend);
}

void SyntaxEditor::CharRight()
{
	Call(ScintillaFunction::CharRight);
}

void SyntaxEditor::CharRightExtend()
{
	Call(ScintillaFunction::CharRightExtend);
}

void SyntaxEditor::WordLeft()
{
	Call(ScintillaFunction::WordLeft);
}

void SyntaxEditor::WordLeftExtend()
{
	Call(ScintillaFunction::WordLeftExtend);
}

void SyntaxEditor::WordRight()
{
	Call(ScintillaFunction::WordRight);
}

void SyntaxEditor::WordRightExtend()
{
	Call(ScintillaFunction::WordRightExtend);
}

void SyntaxEditor::Home()
{
	Call(ScintillaFunction::Home);
}

void SyntaxEditor::HomeExtend()
{
	Call(ScintillaFunction::HomeExtend);
}

void SyntaxEditor::LineEnd()
{
	Call(ScintillaFunction::LineEnd);
}

void SyntaxEditor::LineEndExtend()
{
	Call(ScintillaFunction::LineEndExtend);
}

void SyntaxEditor::DocumentStart()
{
	Call(ScintillaFunction::DocumentStart);
}

void SyntaxEditor::DocumentStartExtend()
{
	Call(ScintillaFunction::DocumentStartExtend);
}

void SyntaxEditor::DocumentEnd()
{
	Call(ScintillaFunction::DocumentEnd);
}

void SyntaxEditor::DocumentEndExtend()
{
	Call(ScintillaFunction::DocumentEndExtend);
}

void SyntaxEditor::PageUp()
{
	Call(ScintillaFunction::PageUp);
}

void SyntaxEditor::PageUpExtend()
{
	Call(ScintillaFunction::PageUpExtend);
}

void SyntaxEditor::PageDown()
{
	Call(ScintillaFunction::PageDown);
}

void SyntaxEditor::PageDownExtend()
{
	Call(ScintillaFunction::PageDownExtend);
}

void SyntaxEditor::EditToggleOvertype()
{
	Call(ScintillaFunction::EditToggleOvertype);
}

void SyntaxEditor::CancelModes()
{
	Call(ScintillaFunction::CancelModes);
}

void SyntaxEditor::DeleteBack()
{
	Call(ScintillaFunction::DeleteBack);
}

void SyntaxEditor::Tab()
{
	Call(ScintillaFunction::Tab);
}

void SyntaxEditor::BackTab()
{
	Call(ScintillaFunction::BackTab);
}

void SyntaxEditor::NewLine()
{
	Call(ScintillaFunction::NewLine);
}

void SyntaxEditor::FormFeed()
{
	Call(ScintillaFunction::FormFeed);
}

void SyntaxEditor::VCHome()
{
	Call(ScintillaFunction::VCHome);
}

void SyntaxEditor::VCHomeExtend()
{
	Call(ScintillaFunction::VCHomeExtend);
}

void SyntaxEditor::ZoomIn()
{
	Call(ScintillaFunction::ZoomIn);
}

void SyntaxEditor::ZoomOut()
{
	Call(ScintillaFunction::ZoomOut);
}

void SyntaxEditor::DelWordLeft()
{
	Call(ScintillaFunction::DelWordLeft);
}

void SyntaxEditor::DelWordRight()
{
	Call(ScintillaFunction::DelWordRight);
}

void SyntaxEditor::DelWordRightEnd()
{
	Call(ScintillaFunction::DelWordRightEnd);
}

void SyntaxEditor::LineCut()
{
	Call(ScintillaFunction::LineCut);
}

void SyntaxEditor::LineDelete()
{
	Call(ScintillaFunction::LineDelete);
}

void SyntaxEditor::LineTranspose()
{
	Call(ScintillaFunction::LineTranspose);
}

void SyntaxEditor::LineReverse()
{
	Call(ScintillaFunction::LineReverse);
}

void SyntaxEditor::LineDuplicate()
{
	Call(ScintillaFunction::LineDuplicate);
}

void SyntaxEditor::LowerCase()
{
	Call(ScintillaFunction::LowerCase);
}

void SyntaxEditor::UpperCase()
{
	Call(ScintillaFunction::UpperCase);
}

void SyntaxEditor::LineScrollDown()
{
	Call(ScintillaFunction::LineScrollDown);
}

void SyntaxEditor::LineScrollUp()
{
	Call(ScintillaFunction::LineScrollUp);
}

void SyntaxEditor::DeleteBackNotLine()
{
	Call(ScintillaFunction::DeleteBackNotLine);
}

void SyntaxEditor::HomeDisplay()
{
	Call(ScintillaFunction::HomeDisplay);
}

void SyntaxEditor::HomeDisplayExtend()
{
	Call(ScintillaFunction::HomeDisplayExtend);
}

void SyntaxEditor::LineEndDisplay()
{
	Call(ScintillaFunction::LineEndDisplay);
}

void SyntaxEditor::LineEndDisplayExtend()
{
	Call(ScintillaFunction::LineEndDisplayExtend);
}

void SyntaxEditor::HomeWrap()
{
	Call(ScintillaFunction::HomeWrap);
}

void SyntaxEditor::HomeWrapExtend()
{
	Call(ScintillaFunction::HomeWrapExtend);
}

void SyntaxEditor::LineEndWrap()
{
	Call(ScintillaFunction::LineEndWrap);
}

void SyntaxEditor::LineEndWrapExtend()
{
	Call(ScintillaFunction::LineEndWrapExtend);
}

void SyntaxEditor::VCHomeWrap()
{
	Call(ScintillaFunction::VCHomeWrap);
}

void SyntaxEditor::VCHomeWrapExtend()
{
	Call(ScintillaFunction::VCHomeWrapExtend);
}

void SyntaxEditor::LineCopy()
{
	Call(ScintillaFunction::LineCopy);
}

void SyntaxEditor::MoveCaretInsideView()
{
	Call(ScintillaFunction::MoveCaretInsideView);
}

Position SyntaxEditor::LineLength(Line line)
{
	return Call(ScintillaFunction::LineLength, line);
}

void SyntaxEditor::BraceHighlight(Position posA,Position posB)
{
	Call(ScintillaFunction::BraceHighlight, posA, posB);
}

void SyntaxEditor::BraceHighlightIndicator(bool useSetting,int indicator)
{
	Call(ScintillaFunction::BraceHighlightIndicator, useSetting, indicator);
}

void SyntaxEditor::BraceBadLight(Position pos)
{
	Call(ScintillaFunction::BraceBadLight, pos);
}

void SyntaxEditor::BraceBadLightIndicator(bool useSetting,int indicator)
{
	Call(ScintillaFunction::BraceBadLightIndicator, useSetting, indicator);
}

Position SyntaxEditor::BraceMatch(Position pos,int maxReStyle)
{
	return Call(ScintillaFunction::BraceMatch, pos, maxReStyle);
}

Position SyntaxEditor::BraceMatchNext(Position pos,Position startPos)
{
	return Call(ScintillaFunction::BraceMatchNext, pos, startPos);
}

bool SyntaxEditor::GetViewEOL()
{
	return Call(ScintillaFunction::GetViewEOL);
}

void SyntaxEditor::SetViewEOL(bool visible)
{
	Call(ScintillaFunction::SetViewEOL, visible);
}

void *SyntaxEditor::GetDocPointer()
{
	return reinterpret_cast<void *>(Call(ScintillaFunction::GetDocPointer));
}

void SyntaxEditor::SetDocPointer(void *doc)
{
	CallPointer(ScintillaFunction::SetDocPointer, 0, doc);
}

void SyntaxEditor::SetModEventMask(ModificationFlags eventMask)
{
	Call(ScintillaFunction::SetModEventMask, static_cast<uintptr_t>(eventMask));
}

Position SyntaxEditor::GetEdgeColumn()
{
	return Call(ScintillaFunction::GetEdgeColumn);
}

void SyntaxEditor::SetEdgeColumn(Position column)
{
	Call(ScintillaFunction::SetEdgeColumn, column);
}

EdgeVisualStyle SyntaxEditor::GetEdgeMode()
{
	return static_cast<SyntaxEditorEnums::EdgeVisualStyle>(Call(ScintillaFunction::GetEdgeMode));
}

void SyntaxEditor::SetEdgeMode(EdgeVisualStyle edgeMode)
{
	Call(ScintillaFunction::SetEdgeMode, static_cast<uintptr_t>(edgeMode));
}

Colour SyntaxEditor::GetEdgeColour()
{
	return static_cast<Colour>(Call(ScintillaFunction::GetEdgeColour));
}

void SyntaxEditor::SetEdgeColour(Colour edgeColour)
{
	Call(ScintillaFunction::SetEdgeColour, edgeColour);
}

void SyntaxEditor::MultiEdgeAddLine(Position column,Colour edgeColour)
{
	Call(ScintillaFunction::MultiEdgeAddLine, column, edgeColour);
}

void SyntaxEditor::MultiEdgeClearAll()
{
	Call(ScintillaFunction::MultiEdgeClearAll);
}

Position SyntaxEditor::GetMultiEdgeColumn(int which)
{
	return Call(ScintillaFunction::GetMultiEdgeColumn, which);
}

void SyntaxEditor::SearchAnchor()
{
	Call(ScintillaFunction::SearchAnchor);
}

Position SyntaxEditor::SearchNext(FindOption searchFlags,const char *text)
{
	return CallString(ScintillaFunction::SearchNext, static_cast<uintptr_t>(searchFlags), text);
}

Position SyntaxEditor::SearchPrev(FindOption searchFlags,const char *text)
{
	return CallString(ScintillaFunction::SearchPrev, static_cast<uintptr_t>(searchFlags), text);
}

Line SyntaxEditor::LinesOnScreen()
{
	return Call(ScintillaFunction::LinesOnScreen);
}

void SyntaxEditor::UsePopUp(PopUp popUpMode)
{
	Call(ScintillaFunction::UsePopUp, static_cast<uintptr_t>(popUpMode));
}

bool SyntaxEditor::SelectionIsRectangle()
{
	return Call(ScintillaFunction::SelectionIsRectangle);
}

void SyntaxEditor::SetZoom(int zoomInPoints)
{
	Call(ScintillaFunction::SetZoom, zoomInPoints);
}

int SyntaxEditor::GetZoom()
{
	return static_cast<int>(Call(ScintillaFunction::GetZoom));
}

void *SyntaxEditor::CreateDocument(Position bytes,DocumentOption documentOptions)
{
	return reinterpret_cast<void *>(Call(ScintillaFunction::CreateDocument, bytes, static_cast<intptr_t>(documentOptions)));
}

void SyntaxEditor::AddRefDocument(void *doc)
{
	CallPointer(ScintillaFunction::AddRefDocument, 0, doc);
}

void SyntaxEditor::ReleaseDocument(void *doc)
{
	CallPointer(ScintillaFunction::ReleaseDocument, 0, doc);
}

DocumentOption SyntaxEditor::GetDocumentOptions()
{
	return static_cast<SyntaxEditorEnums::DocumentOption>(Call(ScintillaFunction::GetDocumentOptions));
}

ModificationFlags SyntaxEditor::GetModEventMask()
{
	return static_cast<SyntaxEditorEnums::ModificationFlags>(Call(ScintillaFunction::GetModEventMask));
}

void SyntaxEditor::SetCommandEvents(bool commandEvents)
{
	Call(ScintillaFunction::SetCommandEvents, commandEvents);
}

bool SyntaxEditor::GetCommandEvents()
{
	return Call(ScintillaFunction::GetCommandEvents);
}

void SyntaxEditor::SetFocus(bool focus)
{
	Call(ScintillaFunction::SetFocus, focus);
}

bool SyntaxEditor::GetFocus()
{
	return Call(ScintillaFunction::GetFocus);
}

void SyntaxEditor::SetStatus(Status status)
{
	Call(ScintillaFunction::SetStatus, static_cast<uintptr_t>(status));
}

Status SyntaxEditor::GetStatus()
{
	return static_cast<SyntaxEditorEnums::Status>(Call(ScintillaFunction::GetStatus));
}

void SyntaxEditor::SetMouseDownCaptures(bool captures)
{
	Call(ScintillaFunction::SetMouseDownCaptures, captures);
}

bool SyntaxEditor::GetMouseDownCaptures()
{
	return Call(ScintillaFunction::GetMouseDownCaptures);
}

void SyntaxEditor::SetMouseWheelCaptures(bool captures)
{
	Call(ScintillaFunction::SetMouseWheelCaptures, captures);
}

bool SyntaxEditor::GetMouseWheelCaptures()
{
	return Call(ScintillaFunction::GetMouseWheelCaptures);
}

void SyntaxEditor::SetCursor(CursorShape cursorType)
{
	Call(ScintillaFunction::SetCursor, static_cast<uintptr_t>(cursorType));
}

CursorShape SyntaxEditor::GetCursor()
{
	return static_cast<SyntaxEditorEnums::CursorShape>(Call(ScintillaFunction::GetCursor));
}

void SyntaxEditor::SetControlCharSymbol(int symbol)
{
	Call(ScintillaFunction::SetControlCharSymbol, symbol);
}

int SyntaxEditor::GetControlCharSymbol()
{
	return static_cast<int>(Call(ScintillaFunction::GetControlCharSymbol));
}

void SyntaxEditor::WordPartLeft()
{
	Call(ScintillaFunction::WordPartLeft);
}

void SyntaxEditor::WordPartLeftExtend()
{
	Call(ScintillaFunction::WordPartLeftExtend);
}

void SyntaxEditor::WordPartRight()
{
	Call(ScintillaFunction::WordPartRight);
}

void SyntaxEditor::WordPartRightExtend()
{
	Call(ScintillaFunction::WordPartRightExtend);
}

void SyntaxEditor::SetVisiblePolicy(VisiblePolicy visiblePolicy,int visibleSlop)
{
	Call(ScintillaFunction::SetVisiblePolicy, static_cast<uintptr_t>(visiblePolicy), visibleSlop);
}

void SyntaxEditor::DelLineLeft()
{
	Call(ScintillaFunction::DelLineLeft);
}

void SyntaxEditor::DelLineRight()
{
	Call(ScintillaFunction::DelLineRight);
}

void SyntaxEditor::SetXOffset(int xOffset)
{
	Call(ScintillaFunction::SetXOffset, xOffset);
}

int SyntaxEditor::GetXOffset()
{
	return static_cast<int>(Call(ScintillaFunction::GetXOffset));
}

void SyntaxEditor::ChooseCaretX()
{
	Call(ScintillaFunction::ChooseCaretX);
}

void SyntaxEditor::GrabFocus()
{
	Call(ScintillaFunction::GrabFocus);
}

void SyntaxEditor::SetXCaretPolicy(CaretPolicy caretPolicy,int caretSlop)
{
	Call(ScintillaFunction::SetXCaretPolicy, static_cast<uintptr_t>(caretPolicy), caretSlop);
}

void SyntaxEditor::SetYCaretPolicy(CaretPolicy caretPolicy,int caretSlop)
{
	Call(ScintillaFunction::SetYCaretPolicy, static_cast<uintptr_t>(caretPolicy), caretSlop);
}

void SyntaxEditor::SetPrintWrapMode(Wrap wrapMode)
{
	Call(ScintillaFunction::SetPrintWrapMode, static_cast<uintptr_t>(wrapMode));
}

Wrap SyntaxEditor::GetPrintWrapMode()
{
	return static_cast<SyntaxEditorEnums::Wrap>(Call(ScintillaFunction::GetPrintWrapMode));
}

void SyntaxEditor::SetHotspotActiveFore(bool useSetting,Colour fore)
{
	Call(ScintillaFunction::SetHotspotActiveFore, useSetting, fore);
}

Colour SyntaxEditor::GetHotspotActiveFore()
{
	return static_cast<Colour>(Call(ScintillaFunction::GetHotspotActiveFore));
}

void SyntaxEditor::SetHotspotActiveBack(bool useSetting,Colour back)
{
	Call(ScintillaFunction::SetHotspotActiveBack, useSetting, back);
}

Colour SyntaxEditor::GetHotspotActiveBack()
{
	return static_cast<Colour>(Call(ScintillaFunction::GetHotspotActiveBack));
}

void SyntaxEditor::SetHotspotActiveUnderline(bool underline)
{
	Call(ScintillaFunction::SetHotspotActiveUnderline, underline);
}

bool SyntaxEditor::GetHotspotActiveUnderline()
{
	return Call(ScintillaFunction::GetHotspotActiveUnderline);
}

void SyntaxEditor::SetHotspotSingleLine(bool singleLine)
{
	Call(ScintillaFunction::SetHotspotSingleLine, singleLine);
}

bool SyntaxEditor::GetHotspotSingleLine()
{
	return Call(ScintillaFunction::GetHotspotSingleLine);
}

void SyntaxEditor::ParaDown()
{
	Call(ScintillaFunction::ParaDown);
}

void SyntaxEditor::ParaDownExtend()
{
	Call(ScintillaFunction::ParaDownExtend);
}

void SyntaxEditor::ParaUp()
{
	Call(ScintillaFunction::ParaUp);
}

void SyntaxEditor::ParaUpExtend()
{
	Call(ScintillaFunction::ParaUpExtend);
}

Position SyntaxEditor::PositionBefore(Position pos)
{
	return Call(ScintillaFunction::PositionBefore, pos);
}

Position SyntaxEditor::PositionAfter(Position pos)
{
	return Call(ScintillaFunction::PositionAfter, pos);
}

Position SyntaxEditor::PositionRelative(Position pos,Position relative)
{
	return Call(ScintillaFunction::PositionRelative, pos, relative);
}

Position SyntaxEditor::PositionRelativeCodeUnits(Position pos,Position relative)
{
	return Call(ScintillaFunction::PositionRelativeCodeUnits, pos, relative);
}

void SyntaxEditor::CopyRange(Position start,Position end)
{
	Call(ScintillaFunction::CopyRange, start, end);
}

void SyntaxEditor::CopyText(Position length,const char *text)
{
	CallString(ScintillaFunction::CopyText, length, text);
}

void SyntaxEditor::SetSelectionMode(SelectionMode selectionMode)
{
	Call(ScintillaFunction::SetSelectionMode, static_cast<uintptr_t>(selectionMode));
}

SelectionMode SyntaxEditor::GetSelectionMode()
{
	return static_cast<SyntaxEditorEnums::SelectionMode>(Call(ScintillaFunction::GetSelectionMode));
}

bool SyntaxEditor::GetMoveExtendsSelection()
{
	return Call(ScintillaFunction::GetMoveExtendsSelection);
}

Position SyntaxEditor::GetLineSelStartPosition(Line line)
{
	return Call(ScintillaFunction::GetLineSelStartPosition, line);
}

Position SyntaxEditor::GetLineSelEndPosition(Line line)
{
	return Call(ScintillaFunction::GetLineSelEndPosition, line);
}

void SyntaxEditor::LineDownRectExtend()
{
	Call(ScintillaFunction::LineDownRectExtend);
}

void SyntaxEditor::LineUpRectExtend()
{
	Call(ScintillaFunction::LineUpRectExtend);
}

void SyntaxEditor::CharLeftRectExtend()
{
	Call(ScintillaFunction::CharLeftRectExtend);
}

void SyntaxEditor::CharRightRectExtend()
{
	Call(ScintillaFunction::CharRightRectExtend);
}

void SyntaxEditor::HomeRectExtend()
{
	Call(ScintillaFunction::HomeRectExtend);
}

void SyntaxEditor::VCHomeRectExtend()
{
	Call(ScintillaFunction::VCHomeRectExtend);
}

void SyntaxEditor::LineEndRectExtend()
{
	Call(ScintillaFunction::LineEndRectExtend);
}

void SyntaxEditor::PageUpRectExtend()
{
	Call(ScintillaFunction::PageUpRectExtend);
}

void SyntaxEditor::PageDownRectExtend()
{
	Call(ScintillaFunction::PageDownRectExtend);
}

void SyntaxEditor::StutteredPageUp()
{
	Call(ScintillaFunction::StutteredPageUp);
}

void SyntaxEditor::StutteredPageUpExtend()
{
	Call(ScintillaFunction::StutteredPageUpExtend);
}

void SyntaxEditor::StutteredPageDown()
{
	Call(ScintillaFunction::StutteredPageDown);
}

void SyntaxEditor::StutteredPageDownExtend()
{
	Call(ScintillaFunction::StutteredPageDownExtend);
}

void SyntaxEditor::WordLeftEnd()
{
	Call(ScintillaFunction::WordLeftEnd);
}

void SyntaxEditor::WordLeftEndExtend()
{
	Call(ScintillaFunction::WordLeftEndExtend);
}

void SyntaxEditor::WordRightEnd()
{
	Call(ScintillaFunction::WordRightEnd);
}

void SyntaxEditor::WordRightEndExtend()
{
	Call(ScintillaFunction::WordRightEndExtend);
}

void SyntaxEditor::SetWhitespaceChars(const char *characters)
{
	CallString(ScintillaFunction::SetWhitespaceChars, 0, characters);
}

int SyntaxEditor::GetWhitespaceChars(char *characters)
{
	return static_cast<int>(CallPointer(ScintillaFunction::GetWhitespaceChars, 0, characters));
}

std::string SyntaxEditor::GetWhitespaceChars()
{
	return CallReturnString(ScintillaFunction::GetWhitespaceChars, 0);
}

void SyntaxEditor::SetPunctuationChars(const char *characters)
{
	CallString(ScintillaFunction::SetPunctuationChars, 0, characters);
}

int SyntaxEditor::GetPunctuationChars(char *characters)
{
	return static_cast<int>(CallPointer(ScintillaFunction::GetPunctuationChars, 0, characters));
}

std::string SyntaxEditor::GetPunctuationChars()
{
	return CallReturnString(ScintillaFunction::GetPunctuationChars, 0);
}

void SyntaxEditor::SetCharsDefault()
{
	Call(ScintillaFunction::SetCharsDefault);
}

int SyntaxEditor::AutoCGetCurrent()
{
	return static_cast<int>(Call(ScintillaFunction::AutoCGetCurrent));
}

int SyntaxEditor::AutoCGetCurrentText(char *text)
{
	return static_cast<int>(CallPointer(ScintillaFunction::AutoCGetCurrentText, 0, text));
}

std::string SyntaxEditor::AutoCGetCurrentText()
{
	return CallReturnString(ScintillaFunction::AutoCGetCurrentText, 0);
}

void SyntaxEditor::AutoCSetCaseInsensitiveBehaviour(CaseInsensitiveBehaviour behaviour)
{
	Call(ScintillaFunction::AutoCSetCaseInsensitiveBehaviour, static_cast<uintptr_t>(behaviour));
}

CaseInsensitiveBehaviour SyntaxEditor::AutoCGetCaseInsensitiveBehaviour()
{
	return static_cast<SyntaxEditorEnums::CaseInsensitiveBehaviour>(Call(ScintillaFunction::AutoCGetCaseInsensitiveBehaviour));
}

void SyntaxEditor::AutoCSetMulti(MultiAutoComplete multi)
{
	Call(ScintillaFunction::AutoCSetMulti, static_cast<uintptr_t>(multi));
}

MultiAutoComplete SyntaxEditor::AutoCGetMulti()
{
	return static_cast<SyntaxEditorEnums::MultiAutoComplete>(Call(ScintillaFunction::AutoCGetMulti));
}

void SyntaxEditor::AutoCSetOrder(Ordering order)
{
	Call(ScintillaFunction::AutoCSetOrder, static_cast<uintptr_t>(order));
}

Ordering SyntaxEditor::AutoCGetOrder()
{
	return static_cast<SyntaxEditorEnums::Ordering>(Call(ScintillaFunction::AutoCGetOrder));
}

void SyntaxEditor::Allocate(Position bytes)
{
	Call(ScintillaFunction::Allocate, bytes);
}

Position SyntaxEditor::TargetAsUTF8(char *s)
{
	return CallPointer(ScintillaFunction::TargetAsUTF8, 0, s);
}

std::string SyntaxEditor::TargetAsUTF8()
{
	return CallReturnString(ScintillaFunction::TargetAsUTF8, 0);
}

void SyntaxEditor::SetLengthForEncode(Position bytes)
{
	Call(ScintillaFunction::SetLengthForEncode, bytes);
}

Position SyntaxEditor::EncodedFromUTF8(const char *utf8,char *encoded)
{
	return CallPointer(ScintillaFunction::EncodedFromUTF8, reinterpret_cast<uintptr_t>(utf8), encoded);
}

std::string SyntaxEditor::EncodedFromUTF8(const char *utf8)
{
	return CallReturnString(ScintillaFunction::EncodedFromUTF8, reinterpret_cast<uintptr_t>(utf8));
}

Position SyntaxEditor::FindColumn(Line line,Position column)
{
	return Call(ScintillaFunction::FindColumn, line, column);
}

CaretSticky SyntaxEditor::GetCaretSticky()
{
	return static_cast<SyntaxEditorEnums::CaretSticky>(Call(ScintillaFunction::GetCaretSticky));
}

void SyntaxEditor::SetCaretSticky(CaretSticky useCaretStickyBehaviour)
{
	Call(ScintillaFunction::SetCaretSticky, static_cast<uintptr_t>(useCaretStickyBehaviour));
}

void SyntaxEditor::ToggleCaretSticky()
{
	Call(ScintillaFunction::ToggleCaretSticky);
}

void SyntaxEditor::SetPasteConvertEndings(bool convert)
{
	Call(ScintillaFunction::SetPasteConvertEndings, convert);
}

bool SyntaxEditor::GetPasteConvertEndings()
{
	return Call(ScintillaFunction::GetPasteConvertEndings);
}

void SyntaxEditor::SelectionDuplicate()
{
	Call(ScintillaFunction::SelectionDuplicate);
}

void SyntaxEditor::SetCaretLineBackAlpha(SyntaxEditorEnums::Alpha alpha)
{
	Call(ScintillaFunction::SetCaretLineBackAlpha, static_cast<uintptr_t>(alpha));
}

SyntaxEditorEnums::Alpha SyntaxEditor::GetCaretLineBackAlpha()
{
	return static_cast<SyntaxEditorEnums::Alpha>(Call(ScintillaFunction::GetCaretLineBackAlpha));
}

void SyntaxEditor::SetCaretStyle(CaretStyle caretStyle)
{
	Call(ScintillaFunction::SetCaretStyle, static_cast<uintptr_t>(caretStyle));
}

CaretStyle SyntaxEditor::GetCaretStyle()
{
	return static_cast<SyntaxEditorEnums::CaretStyle>(Call(ScintillaFunction::GetCaretStyle));
}

void SyntaxEditor::SetIndicatorCurrent(int indicator)
{
	Call(ScintillaFunction::SetIndicatorCurrent, indicator);
}

int SyntaxEditor::GetIndicatorCurrent()
{
	return static_cast<int>(Call(ScintillaFunction::GetIndicatorCurrent));
}

void SyntaxEditor::SetIndicatorValue(int value)
{
	Call(ScintillaFunction::SetIndicatorValue, value);
}

int SyntaxEditor::GetIndicatorValue()
{
	return static_cast<int>(Call(ScintillaFunction::GetIndicatorValue));
}

void SyntaxEditor::IndicatorFillRange(Position start,Position lengthFill)
{
	Call(ScintillaFunction::IndicatorFillRange, start, lengthFill);
}

void SyntaxEditor::IndicatorClearRange(Position start,Position lengthClear)
{
	Call(ScintillaFunction::IndicatorClearRange, start, lengthClear);
}

int SyntaxEditor::IndicatorAllOnFor(Position pos)
{
	return static_cast<int>(Call(ScintillaFunction::IndicatorAllOnFor, pos));
}

int SyntaxEditor::IndicatorValueAt(int indicator,Position pos)
{
	return static_cast<int>(Call(ScintillaFunction::IndicatorValueAt, indicator, pos));
}

Position SyntaxEditor::IndicatorStart(int indicator,Position pos)
{
	return Call(ScintillaFunction::IndicatorStart, indicator, pos);
}

Position SyntaxEditor::IndicatorEnd(int indicator,Position pos)
{
	return Call(ScintillaFunction::IndicatorEnd, indicator, pos);
}

void SyntaxEditor::SetPositionCache(int size)
{
	Call(ScintillaFunction::SetPositionCache, size);
}

int SyntaxEditor::GetPositionCache()
{
	return static_cast<int>(Call(ScintillaFunction::GetPositionCache));
}

void SyntaxEditor::CopyAllowLine()
{
	Call(ScintillaFunction::CopyAllowLine);
}

void *SyntaxEditor::GetCharacterPointer()
{
	return reinterpret_cast<void *>(Call(ScintillaFunction::GetCharacterPointer));
}

void *SyntaxEditor::GetRangePointer(Position start,Position lengthRange)
{
	return reinterpret_cast<void *>(Call(ScintillaFunction::GetRangePointer, start, lengthRange));
}

Position SyntaxEditor::GetGapPosition()
{
	return Call(ScintillaFunction::GetGapPosition);
}

void SyntaxEditor::IndicSetAlpha(int indicator,SyntaxEditorEnums::Alpha alpha)
{
	Call(ScintillaFunction::IndicSetAlpha, indicator, static_cast<intptr_t>(alpha));
}

SyntaxEditorEnums::Alpha SyntaxEditor::IndicGetAlpha(int indicator)
{
	return static_cast<SyntaxEditorEnums::Alpha>(Call(ScintillaFunction::IndicGetAlpha, indicator));
}

void SyntaxEditor::IndicSetOutlineAlpha(int indicator,SyntaxEditorEnums::Alpha alpha)
{
	Call(ScintillaFunction::IndicSetOutlineAlpha, indicator, static_cast<intptr_t>(alpha));
}

SyntaxEditorEnums::Alpha SyntaxEditor::IndicGetOutlineAlpha(int indicator)
{
	return static_cast<SyntaxEditorEnums::Alpha>(Call(ScintillaFunction::IndicGetOutlineAlpha, indicator));
}

void SyntaxEditor::SetExtraAscent(int extraAscent)
{
	Call(ScintillaFunction::SetExtraAscent, extraAscent);
}

int SyntaxEditor::GetExtraAscent()
{
	return static_cast<int>(Call(ScintillaFunction::GetExtraAscent));
}

void SyntaxEditor::SetExtraDescent(int extraDescent)
{
	Call(ScintillaFunction::SetExtraDescent, extraDescent);
}

int SyntaxEditor::GetExtraDescent()
{
	return static_cast<int>(Call(ScintillaFunction::GetExtraDescent));
}

int SyntaxEditor::MarkerSymbolDefined(int markerNumber)
{
	return static_cast<int>(Call(ScintillaFunction::MarkerSymbolDefined, markerNumber));
}

void SyntaxEditor::MarginSetText(Line line,const char *text)
{
	CallString(ScintillaFunction::MarginSetText, line, text);
}

int SyntaxEditor::MarginGetText(Line line,char *text)
{
	return static_cast<int>(CallPointer(ScintillaFunction::MarginGetText, line, text));
}

std::string SyntaxEditor::MarginGetText(Line line)
{
	return CallReturnString(ScintillaFunction::MarginGetText, line);
}

void SyntaxEditor::MarginSetStyle(Line line,int style)
{
	Call(ScintillaFunction::MarginSetStyle, line, style);
}

int SyntaxEditor::MarginGetStyle(Line line)
{
	return static_cast<int>(Call(ScintillaFunction::MarginGetStyle, line));
}

void SyntaxEditor::MarginSetStyles(Line line,const char *styles)
{
	CallString(ScintillaFunction::MarginSetStyles, line, styles);
}

int SyntaxEditor::MarginGetStyles(Line line,char *styles)
{
	return static_cast<int>(CallPointer(ScintillaFunction::MarginGetStyles, line, styles));
}

std::string SyntaxEditor::MarginGetStyles(Line line)
{
	return CallReturnString(ScintillaFunction::MarginGetStyles, line);
}

void SyntaxEditor::MarginTextClearAll()
{
	Call(ScintillaFunction::MarginTextClearAll);
}

void SyntaxEditor::MarginSetStyleOffset(int style)
{
	Call(ScintillaFunction::MarginSetStyleOffset, style);
}

int SyntaxEditor::MarginGetStyleOffset()
{
	return static_cast<int>(Call(ScintillaFunction::MarginGetStyleOffset));
}

void SyntaxEditor::SetMarginOptions(MarginOption marginOptions)
{
	Call(ScintillaFunction::SetMarginOptions, static_cast<uintptr_t>(marginOptions));
}

MarginOption SyntaxEditor::GetMarginOptions()
{
	return static_cast<SyntaxEditorEnums::MarginOption>(Call(ScintillaFunction::GetMarginOptions));
}

void SyntaxEditor::AnnotationSetText(Line line,const char *text)
{
	CallString(ScintillaFunction::AnnotationSetText, line, text);
}

int SyntaxEditor::AnnotationGetText(Line line,char *text)
{
	return static_cast<int>(CallPointer(ScintillaFunction::AnnotationGetText, line, text));
}

std::string SyntaxEditor::AnnotationGetText(Line line)
{
	return CallReturnString(ScintillaFunction::AnnotationGetText, line);
}

void SyntaxEditor::AnnotationSetStyle(Line line,int style)
{
	Call(ScintillaFunction::AnnotationSetStyle, line, style);
}

int SyntaxEditor::AnnotationGetStyle(Line line)
{
	return static_cast<int>(Call(ScintillaFunction::AnnotationGetStyle, line));
}

void SyntaxEditor::AnnotationSetStyles(Line line,const char *styles)
{
	CallString(ScintillaFunction::AnnotationSetStyles, line, styles);
}

int SyntaxEditor::AnnotationGetStyles(Line line,char *styles)
{
	return static_cast<int>(CallPointer(ScintillaFunction::AnnotationGetStyles, line, styles));
}

std::string SyntaxEditor::AnnotationGetStyles(Line line)
{
	return CallReturnString(ScintillaFunction::AnnotationGetStyles, line);
}

int SyntaxEditor::AnnotationGetLines(Line line)
{
	return static_cast<int>(Call(ScintillaFunction::AnnotationGetLines, line));
}

void SyntaxEditor::AnnotationClearAll()
{
	Call(ScintillaFunction::AnnotationClearAll);
}

void SyntaxEditor::AnnotationSetVisible(AnnotationVisible visible)
{
	Call(ScintillaFunction::AnnotationSetVisible, static_cast<uintptr_t>(visible));
}

AnnotationVisible SyntaxEditor::AnnotationGetVisible()
{
	return static_cast<SyntaxEditorEnums::AnnotationVisible>(Call(ScintillaFunction::AnnotationGetVisible));
}

void SyntaxEditor::AnnotationSetStyleOffset(int style)
{
	Call(ScintillaFunction::AnnotationSetStyleOffset, style);
}

int SyntaxEditor::AnnotationGetStyleOffset()
{
	return static_cast<int>(Call(ScintillaFunction::AnnotationGetStyleOffset));
}

void SyntaxEditor::ReleaseAllExtendedStyles()
{
	Call(ScintillaFunction::ReleaseAllExtendedStyles);
}

int SyntaxEditor::AllocateExtendedStyles(int numberStyles)
{
	return static_cast<int>(Call(ScintillaFunction::AllocateExtendedStyles, numberStyles));
}

void SyntaxEditor::AddUndoAction(int token,UndoFlags flags)
{
	Call(ScintillaFunction::AddUndoAction, token, static_cast<intptr_t>(flags));
}

Position SyntaxEditor::CharPositionFromPoint(int x,int y)
{
	return Call(ScintillaFunction::CharPositionFromPoint, x, y);
}

Position SyntaxEditor::CharPositionFromPointClose(int x,int y)
{
	return Call(ScintillaFunction::CharPositionFromPointClose, x, y);
}

void SyntaxEditor::SetMouseSelectionRectangularSwitch(bool mouseSelectionRectangularSwitch)
{
	Call(ScintillaFunction::SetMouseSelectionRectangularSwitch, mouseSelectionRectangularSwitch);
}

bool SyntaxEditor::GetMouseSelectionRectangularSwitch()
{
	return Call(ScintillaFunction::GetMouseSelectionRectangularSwitch);
}

void SyntaxEditor::SetMultipleSelection(bool multipleSelection)
{
	Call(ScintillaFunction::SetMultipleSelection, multipleSelection);
}

bool SyntaxEditor::GetMultipleSelection()
{
	return Call(ScintillaFunction::GetMultipleSelection);
}

void SyntaxEditor::SetAdditionalSelectionTyping(bool additionalSelectionTyping)
{
	Call(ScintillaFunction::SetAdditionalSelectionTyping, additionalSelectionTyping);
}

bool SyntaxEditor::GetAdditionalSelectionTyping()
{
	return Call(ScintillaFunction::GetAdditionalSelectionTyping);
}

void SyntaxEditor::SetAdditionalCaretsBlink(bool additionalCaretsBlink)
{
	Call(ScintillaFunction::SetAdditionalCaretsBlink, additionalCaretsBlink);
}

bool SyntaxEditor::GetAdditionalCaretsBlink()
{
	return Call(ScintillaFunction::GetAdditionalCaretsBlink);
}

void SyntaxEditor::SetAdditionalCaretsVisible(bool additionalCaretsVisible)
{
	Call(ScintillaFunction::SetAdditionalCaretsVisible, additionalCaretsVisible);
}

bool SyntaxEditor::GetAdditionalCaretsVisible()
{
	return Call(ScintillaFunction::GetAdditionalCaretsVisible);
}

int SyntaxEditor::GetSelections()
{
	return static_cast<int>(Call(ScintillaFunction::GetSelections));
}

bool SyntaxEditor::GetSelectionEmpty()
{
	return Call(ScintillaFunction::GetSelectionEmpty);
}

void SyntaxEditor::ClearSelections()
{
	Call(ScintillaFunction::ClearSelections);
}

void SyntaxEditor::SetSelection(Position caret,Position anchor)
{
	Call(ScintillaFunction::SetSelection, caret, anchor);
}

void SyntaxEditor::AddSelection(Position caret,Position anchor)
{
	Call(ScintillaFunction::AddSelection, caret, anchor);
}

void SyntaxEditor::DropSelectionN(int selection)
{
	Call(ScintillaFunction::DropSelectionN, selection);
}

void SyntaxEditor::SetMainSelection(int selection)
{
	Call(ScintillaFunction::SetMainSelection, selection);
}

int SyntaxEditor::GetMainSelection()
{
	return static_cast<int>(Call(ScintillaFunction::GetMainSelection));
}

void SyntaxEditor::SetSelectionNCaret(int selection,Position caret)
{
	Call(ScintillaFunction::SetSelectionNCaret, selection, caret);
}

Position SyntaxEditor::GetSelectionNCaret(int selection)
{
	return Call(ScintillaFunction::GetSelectionNCaret, selection);
}

void SyntaxEditor::SetSelectionNAnchor(int selection,Position anchor)
{
	Call(ScintillaFunction::SetSelectionNAnchor, selection, anchor);
}

Position SyntaxEditor::GetSelectionNAnchor(int selection)
{
	return Call(ScintillaFunction::GetSelectionNAnchor, selection);
}

void SyntaxEditor::SetSelectionNCaretVirtualSpace(int selection,Position space)
{
	Call(ScintillaFunction::SetSelectionNCaretVirtualSpace, selection, space);
}

Position SyntaxEditor::GetSelectionNCaretVirtualSpace(int selection)
{
	return Call(ScintillaFunction::GetSelectionNCaretVirtualSpace, selection);
}

void SyntaxEditor::SetSelectionNAnchorVirtualSpace(int selection,Position space)
{
	Call(ScintillaFunction::SetSelectionNAnchorVirtualSpace, selection, space);
}

Position SyntaxEditor::GetSelectionNAnchorVirtualSpace(int selection)
{
	return Call(ScintillaFunction::GetSelectionNAnchorVirtualSpace, selection);
}

void SyntaxEditor::SetSelectionNStart(int selection,Position anchor)
{
	Call(ScintillaFunction::SetSelectionNStart, selection, anchor);
}

Position SyntaxEditor::GetSelectionNStart(int selection)
{
	return Call(ScintillaFunction::GetSelectionNStart, selection);
}

Position SyntaxEditor::GetSelectionNStartVirtualSpace(int selection)
{
	return Call(ScintillaFunction::GetSelectionNStartVirtualSpace, selection);
}

void SyntaxEditor::SetSelectionNEnd(int selection,Position caret)
{
	Call(ScintillaFunction::SetSelectionNEnd, selection, caret);
}

Position SyntaxEditor::GetSelectionNEndVirtualSpace(int selection)
{
	return Call(ScintillaFunction::GetSelectionNEndVirtualSpace, selection);
}

Position SyntaxEditor::GetSelectionNEnd(int selection)
{
	return Call(ScintillaFunction::GetSelectionNEnd, selection);
}

void SyntaxEditor::SetRectangularSelectionCaret(Position caret)
{
	Call(ScintillaFunction::SetRectangularSelectionCaret, caret);
}

Position SyntaxEditor::GetRectangularSelectionCaret()
{
	return Call(ScintillaFunction::GetRectangularSelectionCaret);
}

void SyntaxEditor::SetRectangularSelectionAnchor(Position anchor)
{
	Call(ScintillaFunction::SetRectangularSelectionAnchor, anchor);
}

Position SyntaxEditor::GetRectangularSelectionAnchor()
{
	return Call(ScintillaFunction::GetRectangularSelectionAnchor);
}

void SyntaxEditor::SetRectangularSelectionCaretVirtualSpace(Position space)
{
	Call(ScintillaFunction::SetRectangularSelectionCaretVirtualSpace, space);
}

Position SyntaxEditor::GetRectangularSelectionCaretVirtualSpace()
{
	return Call(ScintillaFunction::GetRectangularSelectionCaretVirtualSpace);
}

void SyntaxEditor::SetRectangularSelectionAnchorVirtualSpace(Position space)
{
	Call(ScintillaFunction::SetRectangularSelectionAnchorVirtualSpace, space);
}

Position SyntaxEditor::GetRectangularSelectionAnchorVirtualSpace()
{
	return Call(ScintillaFunction::GetRectangularSelectionAnchorVirtualSpace);
}

void SyntaxEditor::SetVirtualSpaceOptions(VirtualSpace virtualSpaceOptions)
{
	Call(ScintillaFunction::SetVirtualSpaceOptions, static_cast<uintptr_t>(virtualSpaceOptions));
}

VirtualSpace SyntaxEditor::GetVirtualSpaceOptions()
{
	return static_cast<SyntaxEditorEnums::VirtualSpace>(Call(ScintillaFunction::GetVirtualSpaceOptions));
}

void SyntaxEditor::SetRectangularSelectionModifier(int modifier)
{
	Call(ScintillaFunction::SetRectangularSelectionModifier, modifier);
}

int SyntaxEditor::GetRectangularSelectionModifier()
{
	return static_cast<int>(Call(ScintillaFunction::GetRectangularSelectionModifier));
}

void SyntaxEditor::SetAdditionalSelFore(Colour fore)
{
	Call(ScintillaFunction::SetAdditionalSelFore, fore);
}

void SyntaxEditor::SetAdditionalSelBack(Colour back)
{
	Call(ScintillaFunction::SetAdditionalSelBack, back);
}

void SyntaxEditor::SetAdditionalSelAlpha(SyntaxEditorEnums::Alpha alpha)
{
	Call(ScintillaFunction::SetAdditionalSelAlpha, static_cast<uintptr_t>(alpha));
}

SyntaxEditorEnums::Alpha SyntaxEditor::GetAdditionalSelAlpha()
{
	return static_cast<SyntaxEditorEnums::Alpha>(Call(ScintillaFunction::GetAdditionalSelAlpha));
}

void SyntaxEditor::SetAdditionalCaretFore(Colour fore)
{
	Call(ScintillaFunction::SetAdditionalCaretFore, fore);
}

Colour SyntaxEditor::GetAdditionalCaretFore()
{
	return static_cast<Colour>(Call(ScintillaFunction::GetAdditionalCaretFore));
}

void SyntaxEditor::RotateSelection()
{
	Call(ScintillaFunction::RotateSelection);
}

void SyntaxEditor::SwapMainAnchorCaret()
{
	Call(ScintillaFunction::SwapMainAnchorCaret);
}

void SyntaxEditor::MultipleSelectAddNext()
{
	Call(ScintillaFunction::MultipleSelectAddNext);
}

void SyntaxEditor::MultipleSelectAddEach()
{
	Call(ScintillaFunction::MultipleSelectAddEach);
}

int SyntaxEditor::ChangeLexerState(Position start,Position end)
{
	return static_cast<int>(Call(ScintillaFunction::ChangeLexerState, start, end));
}

Line SyntaxEditor::ContractedFoldNext(Line lineStart)
{
	return Call(ScintillaFunction::ContractedFoldNext, lineStart);
}

void SyntaxEditor::VerticalCentreCaret()
{
	Call(ScintillaFunction::VerticalCentreCaret);
}

void SyntaxEditor::MoveSelectedLinesUp()
{
	Call(ScintillaFunction::MoveSelectedLinesUp);
}

void SyntaxEditor::MoveSelectedLinesDown()
{
	Call(ScintillaFunction::MoveSelectedLinesDown);
}

void SyntaxEditor::SetIdentifier(int identifier)
{
	Call(ScintillaFunction::SetIdentifier, identifier);
}

int SyntaxEditor::GetIdentifier()
{
	return static_cast<int>(Call(ScintillaFunction::GetIdentifier));
}

void SyntaxEditor::RGBAImageSetWidth(int width)
{
	Call(ScintillaFunction::RGBAImageSetWidth, width);
}

void SyntaxEditor::RGBAImageSetHeight(int height)
{
	Call(ScintillaFunction::RGBAImageSetHeight, height);
}

void SyntaxEditor::RGBAImageSetScale(int scalePercent)
{
	Call(ScintillaFunction::RGBAImageSetScale, scalePercent);
}

void SyntaxEditor::MarkerDefineRGBAImage(int markerNumber,const char *pixels)
{
	CallString(ScintillaFunction::MarkerDefineRGBAImage, markerNumber, pixels);
}

void SyntaxEditor::RegisterRGBAImage(int type,const char *pixels)
{
	CallString(ScintillaFunction::RegisterRGBAImage, type, pixels);
}

void SyntaxEditor::ScrollToStart()
{
	Call(ScintillaFunction::ScrollToStart);
}

void SyntaxEditor::ScrollToEnd()
{
	Call(ScintillaFunction::ScrollToEnd);
}

void SyntaxEditor::SetTechnology(Technology technology)
{
	Call(ScintillaFunction::SetTechnology, static_cast<uintptr_t>(technology));
}

Technology SyntaxEditor::GetTechnology()
{
	return static_cast<SyntaxEditorEnums::Technology>(Call(ScintillaFunction::GetTechnology));
}

void *SyntaxEditor::CreateLoader(Position bytes,DocumentOption documentOptions)
{
	return reinterpret_cast<void *>(Call(ScintillaFunction::CreateLoader, bytes, static_cast<intptr_t>(documentOptions)));
}

void SyntaxEditor::FindIndicatorShow(Position start,Position end)
{
	Call(ScintillaFunction::FindIndicatorShow, start, end);
}

void SyntaxEditor::FindIndicatorFlash(Position start,Position end)
{
	Call(ScintillaFunction::FindIndicatorFlash, start, end);
}

void SyntaxEditor::FindIndicatorHide()
{
	Call(ScintillaFunction::FindIndicatorHide);
}

void SyntaxEditor::VCHomeDisplay()
{
	Call(ScintillaFunction::VCHomeDisplay);
}

void SyntaxEditor::VCHomeDisplayExtend()
{
	Call(ScintillaFunction::VCHomeDisplayExtend);
}

bool SyntaxEditor::GetCaretLineVisibleAlways()
{
	return Call(ScintillaFunction::GetCaretLineVisibleAlways);
}

void SyntaxEditor::SetCaretLineVisibleAlways(bool alwaysVisible)
{
	Call(ScintillaFunction::SetCaretLineVisibleAlways, alwaysVisible);
}

void SyntaxEditor::SetLineEndTypesAllowed(LineEndType lineEndBitSet)
{
	Call(ScintillaFunction::SetLineEndTypesAllowed, static_cast<uintptr_t>(lineEndBitSet));
}

LineEndType SyntaxEditor::GetLineEndTypesAllowed()
{
	return static_cast<SyntaxEditorEnums::LineEndType>(Call(ScintillaFunction::GetLineEndTypesAllowed));
}

LineEndType SyntaxEditor::GetLineEndTypesActive()
{
	return static_cast<SyntaxEditorEnums::LineEndType>(Call(ScintillaFunction::GetLineEndTypesActive));
}

void SyntaxEditor::SetRepresentation(const char *encodedCharacter,const char *representation)
{
	CallString(ScintillaFunction::SetRepresentation, reinterpret_cast<uintptr_t>(encodedCharacter), representation);
}

int SyntaxEditor::GetRepresentation(const char *encodedCharacter,char *representation)
{
	return static_cast<int>(CallPointer(ScintillaFunction::GetRepresentation, reinterpret_cast<uintptr_t>(encodedCharacter), representation));
}

std::string SyntaxEditor::GetRepresentation(const char *encodedCharacter)
{
	return CallReturnString(ScintillaFunction::GetRepresentation, reinterpret_cast<uintptr_t>(encodedCharacter));
}

void SyntaxEditor::ClearRepresentation(const char *encodedCharacter)
{
	Call(ScintillaFunction::ClearRepresentation, reinterpret_cast<uintptr_t>(encodedCharacter));
}

void SyntaxEditor::EOLAnnotationSetText(Line line,const char *text)
{
	CallString(ScintillaFunction::EOLAnnotationSetText, line, text);
}

int SyntaxEditor::EOLAnnotationGetText(Line line,char *text)
{
	return static_cast<int>(CallPointer(ScintillaFunction::EOLAnnotationGetText, line, text));
}

std::string SyntaxEditor::EOLAnnotationGetText(Line line)
{
	return CallReturnString(ScintillaFunction::EOLAnnotationGetText, line);
}

void SyntaxEditor::EOLAnnotationSetStyle(Line line,int style)
{
	Call(ScintillaFunction::EOLAnnotationSetStyle, line, style);
}

int SyntaxEditor::EOLAnnotationGetStyle(Line line)
{
	return static_cast<int>(Call(ScintillaFunction::EOLAnnotationGetStyle, line));
}

void SyntaxEditor::EOLAnnotationClearAll()
{
	Call(ScintillaFunction::EOLAnnotationClearAll);
}

void SyntaxEditor::EOLAnnotationSetVisible(EOLAnnotationVisible visible)
{
	Call(ScintillaFunction::EOLAnnotationSetVisible, static_cast<uintptr_t>(visible));
}

EOLAnnotationVisible SyntaxEditor::EOLAnnotationGetVisible()
{
	return static_cast<SyntaxEditorEnums::EOLAnnotationVisible>(Call(ScintillaFunction::EOLAnnotationGetVisible));
}

void SyntaxEditor::EOLAnnotationSetStyleOffset(int style)
{
	Call(ScintillaFunction::EOLAnnotationSetStyleOffset, style);
}

int SyntaxEditor::EOLAnnotationGetStyleOffset()
{
	return static_cast<int>(Call(ScintillaFunction::EOLAnnotationGetStyleOffset));
}

bool SyntaxEditor::SupportsFeature(Supports feature)
{
	return Call(ScintillaFunction::SupportsFeature, static_cast<uintptr_t>(feature));
}

void SyntaxEditor::StartRecord()
{
	Call(ScintillaFunction::StartRecord);
}

void SyntaxEditor::StopRecord()
{
	Call(ScintillaFunction::StopRecord);
}

int SyntaxEditor::GetLexer()
{
	return static_cast<int>(Call(ScintillaFunction::GetLexer));
}

void SyntaxEditor::Colourise(Position start,Position end)
{
	Call(ScintillaFunction::Colourise, start, end);
}

void SyntaxEditor::SetProperty(const char *key,const char *value)
{
	CallString(ScintillaFunction::SetProperty, reinterpret_cast<uintptr_t>(key), value);
}

void SyntaxEditor::SetKeyWords(int keyWordSet,const char *keyWords)
{
	CallString(ScintillaFunction::SetKeyWords, keyWordSet, keyWords);
}

int SyntaxEditor::GetProperty(const char *key,char *value)
{
	return static_cast<int>(CallPointer(ScintillaFunction::GetProperty, reinterpret_cast<uintptr_t>(key), value));
}

std::string SyntaxEditor::GetProperty(const char *key)
{
	return CallReturnString(ScintillaFunction::GetProperty, reinterpret_cast<uintptr_t>(key));
}

int SyntaxEditor::GetPropertyExpanded(const char *key,char *value)
{
	return static_cast<int>(CallPointer(ScintillaFunction::GetPropertyExpanded, reinterpret_cast<uintptr_t>(key), value));
}

std::string SyntaxEditor::GetPropertyExpanded(const char *key)
{
	return CallReturnString(ScintillaFunction::GetPropertyExpanded, reinterpret_cast<uintptr_t>(key));
}

int SyntaxEditor::GetPropertyInt(const char *key,int defaultValue)
{
	return static_cast<int>(Call(ScintillaFunction::GetPropertyInt, reinterpret_cast<uintptr_t>(key), defaultValue));
}

int SyntaxEditor::GetLexerLanguage(char *language)
{
	return static_cast<int>(CallPointer(ScintillaFunction::GetLexerLanguage, 0, language));
}

std::string SyntaxEditor::GetLexerLanguage()
{
	return CallReturnString(ScintillaFunction::GetLexerLanguage, 0);
}

void *SyntaxEditor::PrivateLexerCall(int operation,void *pointer)
{
	return reinterpret_cast<void *>(CallPointer(ScintillaFunction::PrivateLexerCall, operation, pointer));
}

int SyntaxEditor::PropertyNames(char *names)
{
	return static_cast<int>(CallPointer(ScintillaFunction::PropertyNames, 0, names));
}

std::string SyntaxEditor::PropertyNames()
{
	return CallReturnString(ScintillaFunction::PropertyNames, 0);
}

TypeProperty SyntaxEditor::PropertyType(const char *name)
{
	return static_cast<SyntaxEditorEnums::TypeProperty>(Call(ScintillaFunction::PropertyType, reinterpret_cast<uintptr_t>(name)));
}

int SyntaxEditor::DescribeProperty(const char *name,char *description)
{
	return static_cast<int>(CallPointer(ScintillaFunction::DescribeProperty, reinterpret_cast<uintptr_t>(name), description));
}

std::string SyntaxEditor::DescribeProperty(const char *name)
{
	return CallReturnString(ScintillaFunction::DescribeProperty, reinterpret_cast<uintptr_t>(name));
}

int SyntaxEditor::DescribeKeyWordSets(char *descriptions)
{
	return static_cast<int>(CallPointer(ScintillaFunction::DescribeKeyWordSets, 0, descriptions));
}

std::string SyntaxEditor::DescribeKeyWordSets()
{
	return CallReturnString(ScintillaFunction::DescribeKeyWordSets, 0);
}

int SyntaxEditor::GetLineEndTypesSupported()
{
	return static_cast<int>(Call(ScintillaFunction::GetLineEndTypesSupported));
}

int SyntaxEditor::AllocateSubStyles(int styleBase,int numberStyles)
{
	return static_cast<int>(Call(ScintillaFunction::AllocateSubStyles, styleBase, numberStyles));
}

int SyntaxEditor::GetSubStylesStart(int styleBase)
{
	return static_cast<int>(Call(ScintillaFunction::GetSubStylesStart, styleBase));
}

int SyntaxEditor::GetSubStylesLength(int styleBase)
{
	return static_cast<int>(Call(ScintillaFunction::GetSubStylesLength, styleBase));
}

int SyntaxEditor::GetStyleFromSubStyle(int subStyle)
{
	return static_cast<int>(Call(ScintillaFunction::GetStyleFromSubStyle, subStyle));
}

int SyntaxEditor::GetPrimaryStyleFromStyle(int style)
{
	return static_cast<int>(Call(ScintillaFunction::GetPrimaryStyleFromStyle, style));
}

void SyntaxEditor::FreeSubStyles()
{
	Call(ScintillaFunction::FreeSubStyles);
}

void SyntaxEditor::SetIdentifiers(int style,const char *identifiers)
{
	CallString(ScintillaFunction::SetIdentifiers, style, identifiers);
}

int SyntaxEditor::DistanceToSecondaryStyles()
{
	return static_cast<int>(Call(ScintillaFunction::DistanceToSecondaryStyles));
}

int SyntaxEditor::GetSubStyleBases(char *styles)
{
	return static_cast<int>(CallPointer(ScintillaFunction::GetSubStyleBases, 0, styles));
}

std::string SyntaxEditor::GetSubStyleBases()
{
	return CallReturnString(ScintillaFunction::GetSubStyleBases, 0);
}

int SyntaxEditor::GetNamedStyles()
{
	return static_cast<int>(Call(ScintillaFunction::GetNamedStyles));
}

int SyntaxEditor::NameOfStyle(int style,char *name)
{
	return static_cast<int>(CallPointer(ScintillaFunction::NameOfStyle, style, name));
}

std::string SyntaxEditor::NameOfStyle(int style)
{
	return CallReturnString(ScintillaFunction::NameOfStyle, style);
}

int SyntaxEditor::TagsOfStyle(int style,char *tags)
{
	return static_cast<int>(CallPointer(ScintillaFunction::TagsOfStyle, style, tags));
}

std::string SyntaxEditor::TagsOfStyle(int style)
{
	return CallReturnString(ScintillaFunction::TagsOfStyle, style);
}

int SyntaxEditor::DescriptionOfStyle(int style,char *description)
{
	return static_cast<int>(CallPointer(ScintillaFunction::DescriptionOfStyle, style, description));
}

std::string SyntaxEditor::DescriptionOfStyle(int style)
{
	return CallReturnString(ScintillaFunction::DescriptionOfStyle, style);
}

void SyntaxEditor::SetILexer(void *ilexer)
{
	CallPointer(ScintillaFunction::SetILexer, 0, ilexer);
}

Bidirectional SyntaxEditor::GetBidirectional()
{
	return static_cast<SyntaxEditorEnums::Bidirectional>(Call(ScintillaFunction::GetBidirectional));
}

void SyntaxEditor::SetBidirectional(Bidirectional bidirectional)
{
	Call(ScintillaFunction::SetBidirectional, static_cast<uintptr_t>(bidirectional));
}

LineCharacterIndexType SyntaxEditor::GetLineCharacterIndex()
{
	return static_cast<SyntaxEditorEnums::LineCharacterIndexType>(Call(ScintillaFunction::GetLineCharacterIndex));
}

void SyntaxEditor::AllocateLineCharacterIndex(LineCharacterIndexType lineCharacterIndex)
{
	Call(ScintillaFunction::AllocateLineCharacterIndex, static_cast<uintptr_t>(lineCharacterIndex));
}

void SyntaxEditor::ReleaseLineCharacterIndex(LineCharacterIndexType lineCharacterIndex)
{
	Call(ScintillaFunction::ReleaseLineCharacterIndex, static_cast<uintptr_t>(lineCharacterIndex));
}

Line SyntaxEditor::LineFromIndexPosition(Position pos,LineCharacterIndexType lineCharacterIndex)
{
	return Call(ScintillaFunction::LineFromIndexPosition, pos, static_cast<intptr_t>(lineCharacterIndex));
}

Position SyntaxEditor::IndexPositionFromLine(Line line,LineCharacterIndexType lineCharacterIndex)
{
	return Call(ScintillaFunction::IndexPositionFromLine, line, static_cast<intptr_t>(lineCharacterIndex));
}

//ENDPART


