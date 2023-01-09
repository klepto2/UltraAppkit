#pragma once
#include "UltraEngine.h"
#include "include/KLWidgets.h"

using namespace UltraEngine;

using FunctionDirect = intptr_t(*)(intptr_t ptr, unsigned int iMessage, uintptr_t wParam, intptr_t lParam);

class TextEditorNotification : public Object {
public:
	Sci_Position position;
	int ch;
	int modifiers;
	int modificationType;
	String text;
	Sci_Position length;
	Sci_Position linesAdded;
	int message;
	Sci_Position line;
	int foldLevelNow;
	int foldLevelPrev;
	int margin;
	int listType;
	int x;
	int y;
	int token;
	Sci_Position annotationLinesAdded;
	int updated;
	int listCompletionMethod;
	int characterSource;
};


class SyntaxEditor : public Widget
{
private:
	static bool SCI_REGISTERED;
	static vector<shared_ptr<SyntaxEditor>> SytaxWidgetMap;

	HINSTANCE hInstance = 0;
	HWND hwnd = 0;
	FunctionDirect fn;
	intptr_t ptr;

	float slidersize = 20;
	shared_ptr<Widget> _horizontalSlider;
	shared_ptr<Widget> _verticalSlider;
	bool initialized = false;

	void UpdateSlider(bool updateFromScintilla = false);
	void ProcessNotification(SCNotification* n);
	intptr_t CallPointer(ScintillaFunction msg, uintptr_t wParam, void* s);
	intptr_t CallString(ScintillaFunction msg, uintptr_t wParam, const char* s);
	std::string CallReturnString(ScintillaFunction msg, uintptr_t wParam);

	virtual void UpdateLayout();
	virtual void Draw__(const int x, const int y, const int width, const int height);
	virtual bool Initialize(const WString& text, const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style);

public:
	friend shared_ptr<SyntaxEditor> CreateSyntaxEditor(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent);
	friend LRESULT CALLBACK HandleScintillaNotifications(int nCode, WPARAM wParam, LPARAM lParam);
	virtual void Hide(), Show();

//PART:SCI_FUNCTION	/**
	* Add text to the document at current position.
	*/
	void AddText(position length,string text);
	/**
	* Add array of cells to document.
	*/
	void AddStyledText(position length,cells c);
	/**
	* Insert string at a position.
	*/
	void InsertText(position pos,string text);
	/**
	* Change the text that is being inserted in response to SC_MOD_INSERTCHECK
	*/
	void ChangeInsertion(position length,string text);
	/**
	* Delete all text in the document.
	*/
	void ClearAll();
	/**
	* Delete a range of text in the document.
	*/
	void DeleteRange(position start,position lengthDelete);
	/**
	* Set all style bytes to 0, remove all folding information.
	*/
	void ClearDocumentStyle();
	/**
	* Returns the number of bytes in the document.
	*/
	position GetLength();
	/**
	* Returns the character byte at the position.
	*/
	int GetCharAt(position pos);
	/**
	* Returns the position of the caret.
	*/
	position GetCurrentPos();
	/**
	* Returns the position of the opposite end of the selection to the caret.
	*/
	position GetAnchor();
	/**
	* Returns the style byte at the position.
	*/
	int GetStyleAt(position pos);
	/**
	* Redoes the next action on the undo history.
	*/
	void Redo();
	/**
	* Choose between collecting actions into the undo
	* history and discarding them.
	*/
	void SetUndoCollection(bool collectUndo);
	/**
	* Select all the text in the document.
	*/
	void SelectAll();
	/**
	* Remember the current position in the undo history as the position
	* at which the document was saved.
	*/
	void SetSavePoint();
	/**
	* Retrieve a buffer of cells.
	* Returns the number of bytes in the buffer not including terminating NULs.
	*/
	position GetStyledText(textrange tr);
	/**
	* Are there any redoable actions in the undo history?
	*/
	bool CanRedo();
	/**
	* Retrieve the line number at which a particular marker is located.
	*/
	line MarkerLineFromHandle(int markerHandle);
	/**
	* Delete a marker.
	*/
	void MarkerDeleteHandle(int markerHandle);
	/**
	* Retrieve marker handles of a line
	*/
	int MarkerHandleFromLine(line line,int which);
	/**
	* Retrieve marker number of a marker handle
	*/
	int MarkerNumberFromLine(line line,int which);
	/**
	* Is undo history being collected?
	*/
	bool GetUndoCollection();
	/**
	* Are white space characters currently visible?
	* Returns one of SCWS_* constants.
	*/
	WhiteSpace GetViewWS();
	/**
	* Make white space characters invisible, always visible or visible outside indentation.
	*/
	void SetViewWS(WhiteSpace viewWS);
	/**
	* Retrieve the current tab draw mode.
	* Returns one of SCTD_* constants.
	*/
	TabDrawMode GetTabDrawMode();
	/**
	* Set how tabs are drawn when visible.
	*/
	void SetTabDrawMode(TabDrawMode tabDrawMode);
	/**
	* Find the position from a point within the window.
	*/
	position PositionFromPoint(int x,int y);
	/**
	* Find the position from a point within the window but return
	* INVALID_POSITION if not close to text.
	*/
	position PositionFromPointClose(int x,int y);
	/**
	* Set caret to start of a line and ensure it is visible.
	*/
	void GotoLine(line line);
	/**
	* Set caret to a position and ensure it is visible.
	*/
	void GotoPos(position caret);
	/**
	* Set the selection anchor to a position. The anchor is the opposite
	* end of the selection from the caret.
	*/
	void SetAnchor(position anchor);
	/**
	* Retrieve the text of the line containing the caret.
	* Returns the index of the caret on the line.
	* Result is NUL-terminated.
	*/
	position GetCurLine(position length,stringresult text);
	/**
	* Retrieve the position of the last correctly styled character.
	*/
	position GetEndStyled();
	/**
	* Convert all line endings in the document to one mode.
	*/
	void ConvertEOLs(EndOfLine eolMode);
	/**
	* Retrieve the current end of line mode - one of CRLF, CR, or LF.
	*/
	EndOfLine GetEOLMode();
	/**
	* Set the current end of line mode.
	*/
	void SetEOLMode(EndOfLine eolMode);
	/**
	* Set the current styling position to start.
	* The unused parameter is no longer used and should be set to 0.
	*/
	void StartStyling(position start,int unused);
	/**
	* Change style from current styling position for length characters to a style
	* and move the current styling position to after this newly styled segment.
	*/
	void SetStyling(position length,int style);
	/**
	* Is drawing done first into a buffer or direct to the screen?
	*/
	bool GetBufferedDraw();
	/**
	* If drawing is buffered then each line of text is drawn into a bitmap buffer
	* before drawing it to the screen to avoid flicker.
	*/
	void SetBufferedDraw(bool buffered);
	/**
	* Change the visible size of a tab to be a multiple of the width of a space character.
	*/
	void SetTabWidth(int tabWidth);
	/**
	* Retrieve the visible size of a tab.
	*/
	int GetTabWidth();
	/**
	* Set the minimum visual width of a tab.
	*/
	void SetTabMinimumWidth(int pixels);
	/**
	* Get the minimum visual width of a tab.
	*/
	int GetTabMinimumWidth();
	/**
	* Clear explicit tabstops on a line.
	*/
	void ClearTabStops(line line);
	/**
	* Add an explicit tab stop for a line.
	*/
	void AddTabStop(line line,int x);
	/**
	* Find the next explicit tab stop position on a line after a position.
	*/
	int GetNextTabStop(line line,int x);
	/**
	* Set the code page used to interpret the bytes of the document as characters.
	* The SC_CP_UTF8 value can be used to enter Unicode mode.
	*/
	void SetCodePage(int codePage);
	/**
	* Set the locale for displaying text.
	*/
	void SetFontLocale(string localeName);
	/**
	* Get the locale for displaying text.
	*/
	int GetFontLocale(stringresult localeName);
	/**
	* Is the IME displayed in a window or inline?
	*/
	IMEInteraction GetIMEInteraction();
	/**
	* Choose to display the IME in a window or inline.
	*/
	void SetIMEInteraction(IMEInteraction imeInteraction);
	/**
	* Set the symbol used for a particular marker number.
	*/
	void MarkerDefine(int markerNumber,MarkerSymbol markerSymbol);
	/**
	* Set the foreground colour used for a particular marker number.
	*/
	void MarkerSetFore(int markerNumber,colour fore);
	/**
	* Set the background colour used for a particular marker number.
	*/
	void MarkerSetBack(int markerNumber,colour back);
	/**
	* Set the background colour used for a particular marker number when its folding block is selected.
	*/
	void MarkerSetBackSelected(int markerNumber,colour back);
	/**
	* Set the foreground colour used for a particular marker number.
	*/
	void MarkerSetForeTranslucent(int markerNumber,colouralpha fore);
	/**
	* Set the background colour used for a particular marker number.
	*/
	void MarkerSetBackTranslucent(int markerNumber,colouralpha back);
	/**
	* Set the background colour used for a particular marker number when its folding block is selected.
	*/
	void MarkerSetBackSelectedTranslucent(int markerNumber,colouralpha back);
	/**
	* Set the width of strokes used in .01 pixels so 50  = 1/2 pixel width.
	*/
	void MarkerSetStrokeWidth(int markerNumber,int hundredths);
	/**
	* Enable/disable highlight for current folding block (smallest one that contains the caret)
	*/
	void MarkerEnableHighlight(bool enabled);
	/**
	* Add a marker to a line, returning an ID which can be used to find or delete the marker.
	*/
	int MarkerAdd(line line,int markerNumber);
	/**
	* Delete a marker from a line.
	*/
	void MarkerDelete(line line,int markerNumber);
	/**
	* Delete all markers with a particular number from all lines.
	*/
	void MarkerDeleteAll(int markerNumber);
	/**
	* Get a bit mask of all the markers set on a line.
	*/
	int MarkerGet(line line);
	/**
	* Find the next line at or after lineStart that includes a marker in mask.
	* Return -1 when no more lines.
	*/
	line MarkerNext(line lineStart,int markerMask);
	/**
	* Find the previous line before lineStart that includes a marker in mask.
	*/
	line MarkerPrevious(line lineStart,int markerMask);
	/**
	* Define a marker from a pixmap.
	*/
	void MarkerDefinePixmap(int markerNumber,string pixmap);
	/**
	* Add a set of markers to a line.
	*/
	void MarkerAddSet(line line,int markerSet);
	/**
	* Set the alpha used for a marker that is drawn in the text area, not the margin.
	*/
	void MarkerSetAlpha(int markerNumber,Alpha alpha);
	/**
	* Set a margin to be either numeric or symbolic.
	*/
	void SetMarginTypeN(int margin,MarginType marginType);
	/**
	* Retrieve the type of a margin.
	*/
	MarginType GetMarginTypeN(int margin);
	/**
	* Set the width of a margin to a width expressed in pixels.
	*/
	void SetMarginWidthN(int margin,int pixelWidth);
	/**
	* Retrieve the width of a margin in pixels.
	*/
	int GetMarginWidthN(int margin);
	/**
	* Set a mask that determines which markers are displayed in a margin.
	*/
	void SetMarginMaskN(int margin,int mask);
	/**
	* Retrieve the marker mask of a margin.
	*/
	int GetMarginMaskN(int margin);
	/**
	* Make a margin sensitive or insensitive to mouse clicks.
	*/
	void SetMarginSensitiveN(int margin,bool sensitive);
	/**
	* Retrieve the mouse click sensitivity of a margin.
	*/
	bool GetMarginSensitiveN(int margin);
	/**
	* Set the cursor shown when the mouse is inside a margin.
	*/
	void SetMarginCursorN(int margin,CursorShape cursor);
	/**
	* Retrieve the cursor shown in a margin.
	*/
	CursorShape GetMarginCursorN(int margin);
	/**
	* Set the background colour of a margin. Only visible for SC_MARGIN_COLOUR.
	*/
	void SetMarginBackN(int margin,colour back);
	/**
	* Retrieve the background colour of a margin
	*/
	colour GetMarginBackN(int margin);
	/**
	* Allocate a non-standard number of margins.
	*/
	void SetMargins(int margins);
	/**
	* How many margins are there?.
	*/
	int GetMargins();
	/**
	* Clear all the styles and make equivalent to the global default style.
	*/
	void StyleClearAll();
	/**
	* Set the foreground colour of a style.
	*/
	void StyleSetFore(int style,colour fore);
	/**
	* Set the background colour of a style.
	*/
	void StyleSetBack(int style,colour back);
	/**
	* Set a style to be bold or not.
	*/
	void StyleSetBold(int style,bool bold);
	/**
	* Set a style to be italic or not.
	*/
	void StyleSetItalic(int style,bool italic);
	/**
	* Set the size of characters of a style.
	*/
	void StyleSetSize(int style,int sizePoints);
	/**
	* Set the font of a style.
	*/
	void StyleSetFont(int style,string fontName);
	/**
	* Set a style to have its end of line filled or not.
	*/
	void StyleSetEOLFilled(int style,bool eolFilled);
	/**
	* Reset the default style to its state at startup
	*/
	void StyleResetDefault();
	/**
	* Set a style to be underlined or not.
	*/
	void StyleSetUnderline(int style,bool underline);
	/**
	* Get the foreground colour of a style.
	*/
	colour StyleGetFore(int style);
	/**
	* Get the background colour of a style.
	*/
	colour StyleGetBack(int style);
	/**
	* Get is a style bold or not.
	*/
	bool StyleGetBold(int style);
	/**
	* Get is a style italic or not.
	*/
	bool StyleGetItalic(int style);
	/**
	* Get the size of characters of a style.
	*/
	int StyleGetSize(int style);
	/**
	* Get the font of a style.
	* Returns the length of the fontName
	* Result is NUL-terminated.
	*/
	int StyleGetFont(int style,stringresult fontName);
	/**
	* Get is a style to have its end of line filled or not.
	*/
	bool StyleGetEOLFilled(int style);
	/**
	* Get is a style underlined or not.
	*/
	bool StyleGetUnderline(int style);
	/**
	* Get is a style mixed case, or to force upper or lower case.
	*/
	CaseVisible StyleGetCase(int style);
	/**
	* Get the character get of the font in a style.
	*/
	CharacterSet StyleGetCharacterSet(int style);
	/**
	* Get is a style visible or not.
	*/
	bool StyleGetVisible(int style);
	/**
	* Get is a style changeable or not (read only).
	* Experimental feature, currently buggy.
	*/
	bool StyleGetChangeable(int style);
	/**
	* Get is a style a hotspot or not.
	*/
	bool StyleGetHotSpot(int style);
	/**
	* Set a style to be mixed case, or to force upper or lower case.
	*/
	void StyleSetCase(int style,CaseVisible caseVisible);
	/**
	* Set the size of characters of a style. Size is in points multiplied by 100.
	*/
	void StyleSetSizeFractional(int style,int sizeHundredthPoints);
	/**
	* Get the size of characters of a style in points multiplied by 100
	*/
	int StyleGetSizeFractional(int style);
	/**
	* Set the weight of characters of a style.
	*/
	void StyleSetWeight(int style,FontWeight weight);
	/**
	* Get the weight of characters of a style.
	*/
	FontWeight StyleGetWeight(int style);
	/**
	* Set the character set of the font in a style.
	*/
	void StyleSetCharacterSet(int style,CharacterSet characterSet);
	/**
	* Set a style to be a hotspot or not.
	*/
	void StyleSetHotSpot(int style,bool hotspot);
	/**
	* Set the colour of an element. Translucency (alpha) may or may not be significant
	* and this may depend on the platform. The alpha byte should commonly be 0xff for opaque.
	*/
	void SetElementColour(Element element,colouralpha colourElement);
	/**
	* Get the colour of an element.
	*/
	colouralpha GetElementColour(Element element);
	/**
	* Use the default or platform-defined colour for an element.
	*/
	void ResetElementColour(Element element);
	/**
	* Get whether an element has been set by SetElementColour.
	* When false, a platform-defined or default colour is used.
	*/
	bool GetElementIsSet(Element element);
	/**
	* Get whether an element supports translucency.
	*/
	bool GetElementAllowsTranslucent(Element element);
	/**
	* Set the foreground colour of the main and additional selections and whether to use this setting.
	*/
	void SetSelFore(bool useSetting,colour fore);
	/**
	* Set the background colour of the main and additional selections and whether to use this setting.
	*/
	void SetSelBack(bool useSetting,colour back);
	/**
	* Get the alpha of the selection.
	*/
	Alpha GetSelAlpha();
	/**
	* Set the alpha of the selection.
	*/
	void SetSelAlpha(Alpha alpha);
	/**
	* Is the selection end of line filled?
	*/
	bool GetSelEOLFilled();
	/**
	* Set the selection to have its end of line filled or not.
	*/
	void SetSelEOLFilled(bool filled);
	/**
	* Set the foreground colour of the caret.
	*/
	void SetCaretFore(colour fore);
	/**
	* When key+modifier combination keyDefinition is pressed perform sciCommand.
	*/
	void AssignCmdKey(keymod keyDefinition,int sciCommand);
	/**
	* When key+modifier combination keyDefinition is pressed do nothing.
	*/
	void ClearCmdKey(keymod keyDefinition);
	/**
	* Drop all key mappings.
	*/
	void ClearAllCmdKeys();
	/**
	* Set the styles for a segment of the document.
	*/
	void SetStylingEx(position length,string styles);
	/**
	* Set a style to be visible or not.
	*/
	void StyleSetVisible(int style,bool visible);
	/**
	* Get the time in milliseconds that the caret is on and off.
	*/
	int GetCaretPeriod();
	/**
	* Get the time in milliseconds that the caret is on and off. 0 = steady on.
	*/
	void SetCaretPeriod(int periodMilliseconds);
	/**
	* Set the set of characters making up words for when moving or selecting by word.
	* First sets defaults like SetCharsDefault.
	*/
	void SetWordChars(string characters);
	/**
	* Get the set of characters making up words for when moving or selecting by word.
	* Returns the number of characters
	*/
	int GetWordChars(stringresult characters);
	/**
	* Set the number of characters to have directly indexed categories
	*/
	void SetCharacterCategoryOptimization(int countCharacters);
	/**
	* Get the number of characters to have directly indexed categories
	*/
	int GetCharacterCategoryOptimization();
	/**
	* Start a sequence of actions that is undone and redone as a unit.
	* May be nested.
	*/
	void BeginUndoAction();
	/**
	* End a sequence of actions that is undone and redone as a unit.
	*/
	void EndUndoAction();
	/**
	* Set an indicator to plain, squiggle or TT.
	*/
	void IndicSetStyle(int indicator,IndicatorStyle indicatorStyle);
	/**
	* Retrieve the style of an indicator.
	*/
	IndicatorStyle IndicGetStyle(int indicator);
	/**
	* Set the foreground colour of an indicator.
	*/
	void IndicSetFore(int indicator,colour fore);
	/**
	* Retrieve the foreground colour of an indicator.
	*/
	colour IndicGetFore(int indicator);
	/**
	* Set an indicator to draw under text or over(default).
	*/
	void IndicSetUnder(int indicator,bool under);
	/**
	* Retrieve whether indicator drawn under or over text.
	*/
	bool IndicGetUnder(int indicator);
	/**
	* Set a hover indicator to plain, squiggle or TT.
	*/
	void IndicSetHoverStyle(int indicator,IndicatorStyle indicatorStyle);
	/**
	* Retrieve the hover style of an indicator.
	*/
	IndicatorStyle IndicGetHoverStyle(int indicator);
	/**
	* Set the foreground hover colour of an indicator.
	*/
	void IndicSetHoverFore(int indicator,colour fore);
	/**
	* Retrieve the foreground hover colour of an indicator.
	*/
	colour IndicGetHoverFore(int indicator);
	/**
	* Set the attributes of an indicator.
	*/
	void IndicSetFlags(int indicator,IndicFlag flags);
	/**
	* Retrieve the attributes of an indicator.
	*/
	IndicFlag IndicGetFlags(int indicator);
	/**
	* Set the stroke width of an indicator in hundredths of a pixel.
	*/
	void IndicSetStrokeWidth(int indicator,int hundredths);
	/**
	* Retrieve the stroke width of an indicator.
	*/
	int IndicGetStrokeWidth(int indicator);
	/**
	* Set the foreground colour of all whitespace and whether to use this setting.
	*/
	void SetWhitespaceFore(bool useSetting,colour fore);
	/**
	* Set the background colour of all whitespace and whether to use this setting.
	*/
	void SetWhitespaceBack(bool useSetting,colour back);
	/**
	* Set the size of the dots used to mark space characters.
	*/
	void SetWhitespaceSize(int size);
	/**
	* Get the size of the dots used to mark space characters.
	*/
	int GetWhitespaceSize();
	/**
	* Used to hold extra styling information for each line.
	*/
	void SetLineState(line line,int state);
	/**
	* Retrieve the extra styling information for a line.
	*/
	int GetLineState(line line);
	/**
	* Retrieve the last line number that has line state.
	*/
	int GetMaxLineState();
	/**
	* Is the background of the line containing the caret in a different colour?
	*/
	bool GetCaretLineVisible();
	/**
	* Display the background of the line containing the caret in a different colour.
	*/
	void SetCaretLineVisible(bool show);
	/**
	* Get the colour of the background of the line containing the caret.
	*/
	colour GetCaretLineBack();
	/**
	* Set the colour of the background of the line containing the caret.
	*/
	void SetCaretLineBack(colour back);
	/**
	* Retrieve the caret line frame width.
	* Width = 0 means this option is disabled.
	*/
	int GetCaretLineFrame();
	/**
	* Display the caret line framed.
	* Set width != 0 to enable this option and width = 0 to disable it.
	*/
	void SetCaretLineFrame(int width);
	/**
	* Set a style to be changeable or not (read only).
	* Experimental feature, currently buggy.
	*/
	void StyleSetChangeable(int style,bool changeable);
	/**
	* Display a auto-completion list.
	* The lengthEntered parameter indicates how many characters before
	* the caret should be used to provide context.
	*/
	void AutoCShow(position lengthEntered,string itemList);
	/**
	* Remove the auto-completion list from the screen.
	*/
	void AutoCCancel();
	/**
	* Is there an auto-completion list visible?
	*/
	bool AutoCActive();
	/**
	* Retrieve the position of the caret when the auto-completion list was displayed.
	*/
	position AutoCPosStart();
	/**
	* User has selected an item so remove the list and insert the selection.
	*/
	void AutoCComplete();
	/**
	* Define a set of character that when typed cancel the auto-completion list.
	*/
	void AutoCStops(string characterSet);
	/**
	* Change the separator character in the string setting up an auto-completion list.
	* Default is space but can be changed if items contain space.
	*/
	void AutoCSetSeparator(int separatorCharacter);
	/**
	* Retrieve the auto-completion list separator character.
	*/
	int AutoCGetSeparator();
	/**
	* Select the item in the auto-completion list that starts with a string.
	*/
	void AutoCSelect(string select);
	/**
	* Should the auto-completion list be cancelled if the user backspaces to a
	* position before where the box was created.
	*/
	void AutoCSetCancelAtStart(bool cancel);
	/**
	* Retrieve whether auto-completion cancelled by backspacing before start.
	*/
	bool AutoCGetCancelAtStart();
	/**
	* Define a set of characters that when typed will cause the autocompletion to
	* choose the selected item.
	*/
	void AutoCSetFillUps(string characterSet);
	/**
	* Should a single item auto-completion list automatically choose the item.
	*/
	void AutoCSetChooseSingle(bool chooseSingle);
	/**
	* Retrieve whether a single item auto-completion list automatically choose the item.
	*/
	bool AutoCGetChooseSingle();
	/**
	* Set whether case is significant when performing auto-completion searches.
	*/
	void AutoCSetIgnoreCase(bool ignoreCase);
	/**
	* Retrieve state of ignore case flag.
	*/
	bool AutoCGetIgnoreCase();
	/**
	* Display a list of strings and send notification when user chooses one.
	*/
	void UserListShow(int listType,string itemList);
	/**
	* Set whether or not autocompletion is hidden automatically when nothing matches.
	*/
	void AutoCSetAutoHide(bool autoHide);
	/**
	* Retrieve whether or not autocompletion is hidden automatically when nothing matches.
	*/
	bool AutoCGetAutoHide();
	/**
	* Set whether or not autocompletion deletes any word characters
	* after the inserted text upon completion.
	*/
	void AutoCSetDropRestOfWord(bool dropRestOfWord);
	/**
	* Retrieve whether or not autocompletion deletes any word characters
	* after the inserted text upon completion.
	*/
	bool AutoCGetDropRestOfWord();
	/**
	* Register an XPM image for use in autocompletion lists.
	*/
	void RegisterImage(int type,string xpmData);
	/**
	* Clear all the registered XPM images.
	*/
	void ClearRegisteredImages();
	/**
	* Retrieve the auto-completion list type-separator character.
	*/
	int AutoCGetTypeSeparator();
	/**
	* Change the type-separator character in the string setting up an auto-completion list.
	* Default is '?' but can be changed if items contain '?'.
	*/
	void AutoCSetTypeSeparator(int separatorCharacter);
	/**
	* Set the maximum width, in characters, of auto-completion and user lists.
	* Set to 0 to autosize to fit longest item, which is the default.
	*/
	void AutoCSetMaxWidth(int characterCount);
	/**
	* Get the maximum width, in characters, of auto-completion and user lists.
	*/
	int AutoCGetMaxWidth();
	/**
	* Set the maximum height, in rows, of auto-completion and user lists.
	* The default is 5 rows.
	*/
	void AutoCSetMaxHeight(int rowCount);
	/**
	* Set the maximum height, in rows, of auto-completion and user lists.
	*/
	int AutoCGetMaxHeight();
	/**
	* Set the number of spaces used for one level of indentation.
	*/
	void SetIndent(int indentSize);
	/**
	* Retrieve indentation size.
	*/
	int GetIndent();
	/**
	* Indentation will only use space characters if useTabs is false, otherwise
	* it will use a combination of tabs and spaces.
	*/
	void SetUseTabs(bool useTabs);
	/**
	* Retrieve whether tabs will be used in indentation.
	*/
	bool GetUseTabs();
	/**
	* Change the indentation of a line to a number of columns.
	*/
	void SetLineIndentation(line line,int indentation);
	/**
	* Retrieve the number of columns that a line is indented.
	*/
	int GetLineIndentation(line line);
	/**
	* Retrieve the position before the first non indentation character on a line.
	*/
	position GetLineIndentPosition(line line);
	/**
	* Retrieve the column number of a position, taking tab width into account.
	*/
	position GetColumn(position pos);
	/**
	* Count characters between two positions.
	*/
	position CountCharacters(position start,position end);
	/**
	* Count code units between two positions.
	*/
	position CountCodeUnits(position start,position end);
	/**
	* Show or hide the horizontal scroll bar.
	*/
	void SetHScrollBar(bool visible);
	/**
	* Is the horizontal scroll bar visible?
	*/
	bool GetHScrollBar();
	/**
	* Show or hide indentation guides.
	*/
	void SetIndentationGuides(IndentView indentView);
	/**
	* Are the indentation guides visible?
	*/
	IndentView GetIndentationGuides();
	/**
	* Set the highlighted indentation guide column.
	* 0 = no highlighted guide.
	*/
	void SetHighlightGuide(position column);
	/**
	* Get the highlighted indentation guide column.
	*/
	position GetHighlightGuide();
	/**
	* Get the position after the last visible characters on a line.
	*/
	position GetLineEndPosition(line line);
	/**
	* Get the code page used to interpret the bytes of the document as characters.
	*/
	int GetCodePage();
	/**
	* Get the foreground colour of the caret.
	*/
	colour GetCaretFore();
	/**
	* In read-only mode?
	*/
	bool GetReadOnly();
	/**
	* Sets the position of the caret.
	*/
	void SetCurrentPos(position caret);
	/**
	* Sets the position that starts the selection - this becomes the anchor.
	*/
	void SetSelectionStart(position anchor);
	/**
	* Returns the position at the start of the selection.
	*/
	position GetSelectionStart();
	/**
	* Sets the position that ends the selection - this becomes the caret.
	*/
	void SetSelectionEnd(position caret);
	/**
	* Returns the position at the end of the selection.
	*/
	position GetSelectionEnd();
	/**
	* Set caret to a position, while removing any existing selection.
	*/
	void SetEmptySelection(position caret);
	/**
	* Sets the print magnification added to the point size of each style for printing.
	*/
	void SetPrintMagnification(int magnification);
	/**
	* Returns the print magnification.
	*/
	int GetPrintMagnification();
	/**
	* Modify colours when printing for clearer printed text.
	*/
	void SetPrintColourMode(PrintOption mode);
	/**
	* Returns the print colour mode.
	*/
	PrintOption GetPrintColourMode();
	/**
	* Find some text in the document.
	*/
	position FindText(FindOption searchFlags,findtext ft);
	/**
	* On Windows, will draw the document into a display context such as a printer.
	*/
	position FormatRange(bool draw,formatrange fr);
	/**
	* Retrieve the display line at the top of the display.
	*/
	line GetFirstVisibleLine();
	/**
	* Retrieve the contents of a line.
	* Returns the length of the line.
	*/
	position GetLine(line line,stringresult text);
	/**
	* Returns the number of lines in the document. There is always at least one.
	*/
	line GetLineCount();
	/**
	* Sets the size in pixels of the left margin.
	*/
	void SetMarginLeft(int pixelWidth);
	/**
	* Returns the size in pixels of the left margin.
	*/
	int GetMarginLeft();
	/**
	* Sets the size in pixels of the right margin.
	*/
	void SetMarginRight(int pixelWidth);
	/**
	* Returns the size in pixels of the right margin.
	*/
	int GetMarginRight();
	/**
	* Is the document different from when it was last saved?
	*/
	bool GetModify();
	/**
	* Select a range of text.
	*/
	void SetSel(position anchor,position caret);
	/**
	* Retrieve the selected text.
	* Return the length of the text.
	* Result is NUL-terminated.
	*/
	position GetSelText(stringresult text);
	/**
	* Retrieve a range of text.
	* Return the length of the text.
	*/
	position GetTextRange(textrange tr);
	/**
	* Draw the selection either highlighted or in normal (non-highlighted) style.
	*/
	void HideSelection(bool hide);
	/**
	* Retrieve the x value of the point in the window where a position is displayed.
	*/
	int PointXFromPosition(position pos);
	/**
	* Retrieve the y value of the point in the window where a position is displayed.
	*/
	int PointYFromPosition(position pos);
	/**
	* Retrieve the line containing a position.
	*/
	line LineFromPosition(position pos);
	/**
	* Retrieve the position at the start of a line.
	*/
	position PositionFromLine(line line);
	/**
	* Scroll horizontally and vertically.
	*/
	void LineScroll(position columns,line lines);
	/**
	* Ensure the caret is visible.
	*/
	void ScrollCaret();
	/**
	* Scroll the argument positions and the range between them into view giving
	* priority to the primary position then the secondary position.
	* This may be used to make a search match visible.
	*/
	void ScrollRange(position secondary,position primary);
	/**
	* Replace the selected text with the argument text.
	*/
	void ReplaceSel(string text);
	/**
	* Set to read only or read write.
	*/
	void SetReadOnly(bool readOnly);
	/**
	* Null operation.
	*/
	void Null();
	/**
	* Will a paste succeed?
	*/
	bool CanPaste();
	/**
	* Are there any undoable actions in the undo history?
	*/
	bool CanUndo();
	/**
	* Delete the undo history.
	*/
	void EmptyUndoBuffer();
	/**
	* Undo one action in the undo history.
	*/
	void Undo();
	/**
	* Cut the selection to the clipboard.
	*/
	void Cut();
	/**
	* Copy the selection to the clipboard.
	*/
	void Copy();
	/**
	* Paste the contents of the clipboard into the document replacing the selection.
	*/
	void Paste();
	/**
	* Clear the selection.
	*/
	void Clear();
	/**
	* Replace the contents of the document with the argument text.
	*/
	void SetText(string text);
	/**
	* Retrieve all the text in the document.
	* Returns number of characters retrieved.
	* Result is NUL-terminated.
	*/
	position GetText(position length,stringresult text);
	/**
	* Retrieve the number of characters in the document.
	*/
	position GetTextLength();
	/**
	* Retrieve a pointer to a function that processes messages for this Scintilla.
	*/
	pointer GetDirectFunction();
	/**
	* Retrieve a pointer value to use as the first argument when calling
	* the function returned by GetDirectFunction.
	*/
	pointer GetDirectPointer();
	/**
	* Set to overtype (true) or insert mode.
	*/
	void SetOvertype(bool overType);
	/**
	* Returns true if overtype mode is active otherwise false is returned.
	*/
	bool GetOvertype();
	/**
	* Set the width of the insert mode caret.
	*/
	void SetCaretWidth(int pixelWidth);
	/**
	* Returns the width of the insert mode caret.
	*/
	int GetCaretWidth();
	/**
	* Sets the position that starts the target which is used for updating the
	* document without affecting the scroll position.
	*/
	void SetTargetStart(position start);
	/**
	* Get the position that starts the target.
	*/
	position GetTargetStart();
	/**
	* Sets the virtual space of the target start
	*/
	void SetTargetStartVirtualSpace(position space);
	/**
	* Get the virtual space of the target start
	*/
	position GetTargetStartVirtualSpace();
	/**
	* Sets the position that ends the target which is used for updating the
	* document without affecting the scroll position.
	*/
	void SetTargetEnd(position end);
	/**
	* Get the position that ends the target.
	*/
	position GetTargetEnd();
	/**
	* Sets the virtual space of the target end
	*/
	void SetTargetEndVirtualSpace(position space);
	/**
	* Get the virtual space of the target end
	*/
	position GetTargetEndVirtualSpace();
	/**
	* Sets both the start and end of the target in one call.
	*/
	void SetTargetRange(position start,position end);
	/**
	* Retrieve the text in the target.
	*/
	position GetTargetText(stringresult text);
	/**
	* Make the target range start and end be the same as the selection range start and end.
	*/
	void TargetFromSelection();
	/**
	* Sets the target to the whole document.
	*/
	void TargetWholeDocument();
	/**
	* Replace the target text with the argument text.
	* Text is counted so it can contain NULs.
	* Returns the length of the replacement text.
	*/
	position ReplaceTarget(position length,string text);
	/**
	* Replace the target text with the argument text after \d processing.
	* Text is counted so it can contain NULs.
	* Looks for \d where d is between 1 and 9 and replaces these with the strings
	* matched in the last search operation which were surrounded by \( and \).
	* Returns the length of the replacement text including any change
	* caused by processing the \d patterns.
	*/
	position ReplaceTargetRE(position length,string text);
	/**
	* Search for a counted string in the target and set the target to the found
	* range. Text is counted so it can contain NULs.
	* Returns start of found range or -1 for failure in which case target is not moved.
	*/
	position SearchInTarget(position length,string text);
	/**
	* Set the search flags used by SearchInTarget.
	*/
	void SetSearchFlags(FindOption searchFlags);
	/**
	* Get the search flags used by SearchInTarget.
	*/
	FindOption GetSearchFlags();
	/**
	* Show a call tip containing a definition near position pos.
	*/
	void CallTipShow(position pos,string definition);
	/**
	* Remove the call tip from the screen.
	*/
	void CallTipCancel();
	/**
	* Is there an active call tip?
	*/
	bool CallTipActive();
	/**
	* Retrieve the position where the caret was before displaying the call tip.
	*/
	position CallTipPosStart();
	/**
	* Set the start position in order to change when backspacing removes the calltip.
	*/
	void CallTipSetPosStart(position posStart);
	/**
	* Highlight a segment of the definition.
	*/
	void CallTipSetHlt(position highlightStart,position highlightEnd);
	/**
	* Set the background colour for the call tip.
	*/
	void CallTipSetBack(colour back);
	/**
	* Set the foreground colour for the call tip.
	*/
	void CallTipSetFore(colour fore);
	/**
	* Set the foreground colour for the highlighted part of the call tip.
	*/
	void CallTipSetForeHlt(colour fore);
	/**
	* Enable use of STYLE_CALLTIP and set call tip tab size in pixels.
	*/
	void CallTipUseStyle(int tabSize);
	/**
	* Set position of calltip, above or below text.
	*/
	void CallTipSetPosition(bool above);
	/**
	* Find the display line of a document line taking hidden lines into account.
	*/
	line VisibleFromDocLine(line docLine);
	/**
	* Find the document line of a display line taking hidden lines into account.
	*/
	line DocLineFromVisible(line displayLine);
	/**
	* The number of display lines needed to wrap a document line
	*/
	line WrapCount(line docLine);
	/**
	* Set the fold level of a line.
	* This encodes an integer level along with flags indicating whether the
	* line is a header and whether it is effectively white space.
	*/
	void SetFoldLevel(line line,FoldLevel level);
	/**
	* Retrieve the fold level of a line.
	*/
	FoldLevel GetFoldLevel(line line);
	/**
	* Find the last child line of a header line.
	*/
	line GetLastChild(line line,FoldLevel level);
	/**
	* Find the parent line of a child line.
	*/
	line GetFoldParent(line line);
	/**
	* Make a range of lines visible.
	*/
	void ShowLines(line lineStart,line lineEnd);
	/**
	* Make a range of lines invisible.
	*/
	void HideLines(line lineStart,line lineEnd);
	/**
	* Is a line visible?
	*/
	bool GetLineVisible(line line);
	/**
	* Are all lines visible?
	*/
	bool GetAllLinesVisible();
	/**
	* Show the children of a header line.
	*/
	void SetFoldExpanded(line line,bool expanded);
	/**
	* Is a header line expanded?
	*/
	bool GetFoldExpanded(line line);
	/**
	* Switch a header line between expanded and contracted.
	*/
	void ToggleFold(line line);
	/**
	* Switch a header line between expanded and contracted and show some text after the line.
	*/
	void ToggleFoldShowText(line line,string text);
	/**
	* Set the style of fold display text.
	*/
	void FoldDisplayTextSetStyle(FoldDisplayTextStyle style);
	/**
	* Get the style of fold display text.
	*/
	FoldDisplayTextStyle FoldDisplayTextGetStyle();
	/**
	* Set the default fold display text.
	*/
	void SetDefaultFoldDisplayText(string text);
	/**
	* Get the default fold display text.
	*/
	int GetDefaultFoldDisplayText(stringresult text);
	/**
	* Expand or contract a fold header.
	*/
	void FoldLine(line line,FoldAction action);
	/**
	* Expand or contract a fold header and its children.
	*/
	void FoldChildren(line line,FoldAction action);
	/**
	* Expand a fold header and all children. Use the level argument instead of the line's current level.
	*/
	void ExpandChildren(line line,FoldLevel level);
	/**
	* Expand or contract all fold headers.
	*/
	void FoldAll(FoldAction action);
	/**
	* Ensure a particular line is visible by expanding any header line hiding it.
	*/
	void EnsureVisible(line line);
	/**
	* Set automatic folding behaviours.
	*/
	void SetAutomaticFold(AutomaticFold automaticFold);
	/**
	* Get automatic folding behaviours.
	*/
	AutomaticFold GetAutomaticFold();
	/**
	* Set some style options for folding.
	*/
	void SetFoldFlags(FoldFlag flags);
	/**
	* Ensure a particular line is visible by expanding any header line hiding it.
	* Use the currently set visibility policy to determine which range to display.
	*/
	void EnsureVisibleEnforcePolicy(line line);
	/**
	* Sets whether a tab pressed when caret is within indentation indents.
	*/
	void SetTabIndents(bool tabIndents);
	/**
	* Does a tab pressed when caret is within indentation indent?
	*/
	bool GetTabIndents();
	/**
	* Sets whether a backspace pressed when caret is within indentation unindents.
	*/
	void SetBackSpaceUnIndents(bool bsUnIndents);
	/**
	* Does a backspace pressed when caret is within indentation unindent?
	*/
	bool GetBackSpaceUnIndents();
	/**
	* Sets the time the mouse must sit still to generate a mouse dwell event.
	*/
	void SetMouseDwellTime(int periodMilliseconds);
	/**
	* Retrieve the time the mouse must sit still to generate a mouse dwell event.
	*/
	int GetMouseDwellTime();
	/**
	* Get position of start of word.
	*/
	position WordStartPosition(position pos,bool onlyWordCharacters);
	/**
	* Get position of end of word.
	*/
	position WordEndPosition(position pos,bool onlyWordCharacters);
	/**
	* Is the range start..end considered a word?
	*/
	bool IsRangeWord(position start,position end);
	/**
	* Sets limits to idle styling.
	*/
	void SetIdleStyling(IdleStyling idleStyling);
	/**
	* Retrieve the limits to idle styling.
	*/
	IdleStyling GetIdleStyling();
	/**
	* Sets whether text is word wrapped.
	*/
	void SetWrapMode(Wrap wrapMode);
	/**
	* Retrieve whether text is word wrapped.
	*/
	Wrap GetWrapMode();
	/**
	* Set the display mode of visual flags for wrapped lines.
	*/
	void SetWrapVisualFlags(WrapVisualFlag wrapVisualFlags);
	/**
	* Retrive the display mode of visual flags for wrapped lines.
	*/
	WrapVisualFlag GetWrapVisualFlags();
	/**
	* Set the location of visual flags for wrapped lines.
	*/
	void SetWrapVisualFlagsLocation(WrapVisualLocation wrapVisualFlagsLocation);
	/**
	* Retrive the location of visual flags for wrapped lines.
	*/
	WrapVisualLocation GetWrapVisualFlagsLocation();
	/**
	* Set the start indent for wrapped lines.
	*/
	void SetWrapStartIndent(int indent);
	/**
	* Retrive the start indent for wrapped lines.
	*/
	int GetWrapStartIndent();
	/**
	* Sets how wrapped sublines are placed. Default is fixed.
	*/
	void SetWrapIndentMode(WrapIndentMode wrapIndentMode);
	/**
	* Retrieve how wrapped sublines are placed. Default is fixed.
	*/
	WrapIndentMode GetWrapIndentMode();
	/**
	* Sets the degree of caching of layout information.
	*/
	void SetLayoutCache(LineCache cacheMode);
	/**
	* Retrieve the degree of caching of layout information.
	*/
	LineCache GetLayoutCache();
	/**
	* Sets the document width assumed for scrolling.
	*/
	void SetScrollWidth(int pixelWidth);
	/**
	* Retrieve the document width assumed for scrolling.
	*/
	int GetScrollWidth();
	/**
	* Sets whether the maximum width line displayed is used to set scroll width.
	*/
	void SetScrollWidthTracking(bool tracking);
	/**
	* Retrieve whether the scroll width tracks wide lines.
	*/
	bool GetScrollWidthTracking();
	/**
	* Measure the pixel width of some text in a particular style.
	* NUL terminated text argument.
	* Does not handle tab or control characters.
	*/
	int TextWidth(int style,string text);
	/**
	* Sets the scroll range so that maximum scroll position has
	* the last line at the bottom of the view (default).
	* Setting this to false allows scrolling one page below the last line.
	*/
	void SetEndAtLastLine(bool endAtLastLine);
	/**
	* Retrieve whether the maximum scroll position has the last
	* line at the bottom of the view.
	*/
	bool GetEndAtLastLine();
	/**
	* Retrieve the height of a particular line of text in pixels.
	*/
	int TextHeight(line line);
	/**
	* Show or hide the vertical scroll bar.
	*/
	void SetVScrollBar(bool visible);
	/**
	* Is the vertical scroll bar visible?
	*/
	bool GetVScrollBar();
	/**
	* Append a string to the end of the document without changing the selection.
	*/
	void AppendText(position length,string text);
	/**
	* How many phases is drawing done in?
	*/
	PhasesDraw GetPhasesDraw();
	/**
	* In one phase draw, text is drawn in a series of rectangular blocks with no overlap.
	* In two phase draw, text is drawn in a series of lines allowing runs to overlap horizontally.
	* In multiple phase draw, each element is drawn over the whole drawing area, allowing text
	* to overlap from one line to the next.
	*/
	void SetPhasesDraw(PhasesDraw phases);
	/**
	* Choose the quality level for text from the FontQuality enumeration.
	*/
	void SetFontQuality(FontQuality fontQuality);
	/**
	* Retrieve the quality level for text.
	*/
	FontQuality GetFontQuality();
	/**
	* Scroll so that a display line is at the top of the display.
	*/
	void SetFirstVisibleLine(line displayLine);
	/**
	* Change the effect of pasting when there are multiple selections.
	*/
	void SetMultiPaste(MultiPaste multiPaste);
	/**
	* Retrieve the effect of pasting when there are multiple selections.
	*/
	MultiPaste GetMultiPaste();
	/**
	* Retrieve the value of a tag from a regular expression search.
	* Result is NUL-terminated.
	*/
	int GetTag(int tagNumber,stringresult tagValue);
	/**
	* Join the lines in the target.
	*/
	void LinesJoin();
	/**
	* Split the lines in the target into lines that are less wide than pixelWidth
	* where possible.
	*/
	void LinesSplit(int pixelWidth);
	/**
	* Set one of the colours used as a chequerboard pattern in the fold margin
	*/
	void SetFoldMarginColour(bool useSetting,colour back);
	/**
	* Set the other colour used as a chequerboard pattern in the fold margin
	*/
	void SetFoldMarginHiColour(bool useSetting,colour fore);
	/**
	* Enable or disable accessibility.
	*/
	void SetAccessibility(Accessibility accessibility);
	/**
	* Report accessibility status.
	*/
	Accessibility GetAccessibility();
	/**
	* Move caret down one line.
	*/
	void LineDown();
	/**
	* Move caret down one line extending selection to new caret position.
	*/
	void LineDownExtend();
	/**
	* Move caret up one line.
	*/
	void LineUp();
	/**
	* Move caret up one line extending selection to new caret position.
	*/
	void LineUpExtend();
	/**
	* Move caret left one character.
	*/
	void CharLeft();
	/**
	* Move caret left one character extending selection to new caret position.
	*/
	void CharLeftExtend();
	/**
	* Move caret right one character.
	*/
	void CharRight();
	/**
	* Move caret right one character extending selection to new caret position.
	*/
	void CharRightExtend();
	/**
	* Move caret left one word.
	*/
	void WordLeft();
	/**
	* Move caret left one word extending selection to new caret position.
	*/
	void WordLeftExtend();
	/**
	* Move caret right one word.
	*/
	void WordRight();
	/**
	* Move caret right one word extending selection to new caret position.
	*/
	void WordRightExtend();
	/**
	* Move caret to first position on line.
	*/
	void Home();
	/**
	* Move caret to first position on line extending selection to new caret position.
	*/
	void HomeExtend();
	/**
	* Move caret to last position on line.
	*/
	void LineEnd();
	/**
	* Move caret to last position on line extending selection to new caret position.
	*/
	void LineEndExtend();
	/**
	* Move caret to first position in document.
	*/
	void DocumentStart();
	/**
	* Move caret to first position in document extending selection to new caret position.
	*/
	void DocumentStartExtend();
	/**
	* Move caret to last position in document.
	*/
	void DocumentEnd();
	/**
	* Move caret to last position in document extending selection to new caret position.
	*/
	void DocumentEndExtend();
	/**
	* Move caret one page up.
	*/
	void PageUp();
	/**
	* Move caret one page up extending selection to new caret position.
	*/
	void PageUpExtend();
	/**
	* Move caret one page down.
	*/
	void PageDown();
	/**
	* Move caret one page down extending selection to new caret position.
	*/
	void PageDownExtend();
	/**
	* Switch from insert to overtype mode or the reverse.
	*/
	void EditToggleOvertype();
	/**
	* Cancel any modes such as call tip or auto-completion list display.
	*/
	void Cancel();
	/**
	* Delete the selection or if no selection, the character before the caret.
	*/
	void DeleteBack();
	/**
	* If selection is empty or all on one line replace the selection with a tab character.
	* If more than one line selected, indent the lines.
	*/
	void Tab();
	/**
	* Dedent the selected lines.
	*/
	void BackTab();
	/**
	* Insert a new line, may use a CRLF, CR or LF depending on EOL mode.
	*/
	void NewLine();
	/**
	* Insert a Form Feed character.
	*/
	void FormFeed();
	/**
	* Move caret to before first visible character on line.
	* If already there move to first character on line.
	*/
	void VCHome();
	/**
	* Like VCHome but extending selection to new caret position.
	*/
	void VCHomeExtend();
	/**
	* Magnify the displayed text by increasing the sizes by 1 point.
	*/
	void ZoomIn();
	/**
	* Make the displayed text smaller by decreasing the sizes by 1 point.
	*/
	void ZoomOut();
	/**
	* Delete the word to the left of the caret.
	*/
	void DelWordLeft();
	/**
	* Delete the word to the right of the caret.
	*/
	void DelWordRight();
	/**
	* Delete the word to the right of the caret, but not the trailing non-word characters.
	*/
	void DelWordRightEnd();
	/**
	* Cut the line containing the caret.
	*/
	void LineCut();
	/**
	* Delete the line containing the caret.
	*/
	void LineDelete();
	/**
	* Switch the current line with the previous.
	*/
	void LineTranspose();
	/**
	* Reverse order of selected lines.
	*/
	void LineReverse();
	/**
	* Duplicate the current line.
	*/
	void LineDuplicate();
	/**
	* Transform the selection to lower case.
	*/
	void LowerCase();
	/**
	* Transform the selection to upper case.
	*/
	void UpperCase();
	/**
	* Scroll the document down, keeping the caret visible.
	*/
	void LineScrollDown();
	/**
	* Scroll the document up, keeping the caret visible.
	*/
	void LineScrollUp();
	/**
	* Delete the selection or if no selection, the character before the caret.
	* Will not delete the character before at the start of a line.
	*/
	void DeleteBackNotLine();
	/**
	* Move caret to first position on display line.
	*/
	void HomeDisplay();
	/**
	* Move caret to first position on display line extending selection to
	* new caret position.
	*/
	void HomeDisplayExtend();
	/**
	* Move caret to last position on display line.
	*/
	void LineEndDisplay();
	/**
	* Move caret to last position on display line extending selection to new
	* caret position.
	*/
	void LineEndDisplayExtend();
	/**
	* Like Home but when word-wrap is enabled goes first to start of display line
	* HomeDisplay, then to start of document line Home.
	*/
	void HomeWrap();
	/**
	* Like HomeExtend but when word-wrap is enabled extends first to start of display line
	* HomeDisplayExtend, then to start of document line HomeExtend.
	*/
	void HomeWrapExtend();
	/**
	* Like LineEnd but when word-wrap is enabled goes first to end of display line
	* LineEndDisplay, then to start of document line LineEnd.
	*/
	void LineEndWrap();
	/**
	* Like LineEndExtend but when word-wrap is enabled extends first to end of display line
	* LineEndDisplayExtend, then to start of document line LineEndExtend.
	*/
	void LineEndWrapExtend();
	/**
	* Like VCHome but when word-wrap is enabled goes first to start of display line
	* VCHomeDisplay, then behaves like VCHome.
	*/
	void VCHomeWrap();
	/**
	* Like VCHomeExtend but when word-wrap is enabled extends first to start of display line
	* VCHomeDisplayExtend, then behaves like VCHomeExtend.
	*/
	void VCHomeWrapExtend();
	/**
	* Copy the line containing the caret.
	*/
	void LineCopy();
	/**
	* Move the caret inside current view if it's not there already.
	*/
	void MoveCaretInsideView();
	/**
	* How many characters are on a line, including end of line characters?
	*/
	position LineLength(line line);
	/**
	* Highlight the characters at two positions.
	*/
	void BraceHighlight(position posA,position posB);
	/**
	* Use specified indicator to highlight matching braces instead of changing their style.
	*/
	void BraceHighlightIndicator(bool useSetting,int indicator);
	/**
	* Highlight the character at a position indicating there is no matching brace.
	*/
	void BraceBadLight(position pos);
	/**
	* Use specified indicator to highlight non matching brace instead of changing its style.
	*/
	void BraceBadLightIndicator(bool useSetting,int indicator);
	/**
	* Find the position of a matching brace or INVALID_POSITION if no match.
	* The maxReStyle must be 0 for now. It may be defined in a future release.
	*/
	position BraceMatch(position pos,int maxReStyle);
	/**
	* Similar to BraceMatch, but matching starts at the explicit start position.
	*/
	position BraceMatchNext(position pos,position startPos);
	/**
	* Are the end of line characters visible?
	*/
	bool GetViewEOL();
	/**
	* Make the end of line characters visible or invisible.
	*/
	void SetViewEOL(bool visible);
	/**
	* Retrieve a pointer to the document object.
	*/
	pointer GetDocPointer();
	/**
	* Change the document object used.
	*/
	void SetDocPointer(pointer doc);
	/**
	* Set which document modification events are sent to the container.
	*/
	void SetModEventMask(ModificationFlags eventMask);
	/**
	* Retrieve the column number which text should be kept within.
	*/
	position GetEdgeColumn();
	/**
	* Set the column number of the edge.
	* If text goes past the edge then it is highlighted.
	*/
	void SetEdgeColumn(position column);
	/**
	* Retrieve the edge highlight mode.
	*/
	EdgeVisualStyle GetEdgeMode();
	/**
	* The edge may be displayed by a line (EDGE_LINE/EDGE_MULTILINE) or by highlighting text that
	* goes beyond it (EDGE_BACKGROUND) or not displayed at all (EDGE_NONE).
	*/
	void SetEdgeMode(EdgeVisualStyle edgeMode);
	/**
	* Retrieve the colour used in edge indication.
	*/
	colour GetEdgeColour();
	/**
	* Change the colour used in edge indication.
	*/
	void SetEdgeColour(colour edgeColour);
	/**
	* Add a new vertical edge to the view.
	*/
	void MultiEdgeAddLine(position column,colour edgeColour);
	/**
	* Clear all vertical edges.
	*/
	void MultiEdgeClearAll();
	/**
	* Get multi edge positions.
	*/
	position GetMultiEdgeColumn(int which);
	/**
	* Sets the current caret position to be the search anchor.
	*/
	void SearchAnchor();
	/**
	* Find some text starting at the search anchor.
	* Does not ensure the selection is visible.
	*/
	position SearchNext(FindOption searchFlags,string text);
	/**
	* Find some text starting at the search anchor and moving backwards.
	* Does not ensure the selection is visible.
	*/
	position SearchPrev(FindOption searchFlags,string text);
	/**
	* Retrieves the number of lines completely visible.
	*/
	line LinesOnScreen();
	/**
	* Set whether a pop up menu is displayed automatically when the user presses
	* the wrong mouse button on certain areas.
	*/
	void UsePopUp(PopUp popUpMode);
	/**
	* Is the selection rectangular? The alternative is the more common stream selection.
	*/
	bool SelectionIsRectangle();
	/**
	* Set the zoom level. This number of points is added to the size of all fonts.
	* It may be positive to magnify or negative to reduce.
	*/
	void SetZoom(int zoomInPoints);
	/**
	* Retrieve the zoom level.
	*/
	int GetZoom();
	/**
	* Create a new document object.
	* Starts with reference count of 1 and not selected into editor.
	*/
	pointer CreateDocument(position bytes,DocumentOption documentOptions);
	/**
	* Extend life of document.
	*/
	void AddRefDocument(pointer doc);
	/**
	* Release a reference to the document, deleting document if it fades to black.
	*/
	void ReleaseDocument(pointer doc);
	/**
	* Get which document options are set.
	*/
	DocumentOption GetDocumentOptions();
	/**
	* Get which document modification events are sent to the container.
	*/
	ModificationFlags GetModEventMask();
	/**
	* Set whether command events are sent to the container.
	*/
	void SetCommandEvents(bool commandEvents);
	/**
	* Get whether command events are sent to the container.
	*/
	bool GetCommandEvents();
	/**
	* Change internal focus flag.
	*/
	void SetFocus(bool focus);
	/**
	* Get internal focus flag.
	*/
	bool GetFocus();
	/**
	* Change error status - 0 = OK.
	*/
	void SetStatus(Status status);
	/**
	* Get error status.
	*/
	Status GetStatus();
	/**
	* Set whether the mouse is captured when its button is pressed.
	*/
	void SetMouseDownCaptures(bool captures);
	/**
	* Get whether mouse gets captured.
	*/
	bool GetMouseDownCaptures();
	/**
	* Set whether the mouse wheel can be active outside the window.
	*/
	void SetMouseWheelCaptures(bool captures);
	/**
	* Get whether mouse wheel can be active outside the window.
	*/
	bool GetMouseWheelCaptures();
	/**
	* Sets the cursor to one of the SC_CURSOR* values.
	*/
	void SetCursor(CursorShape cursorType);
	/**
	* Get cursor type.
	*/
	CursorShape GetCursor();
	/**
	* Change the way control characters are displayed:
	* If symbol is < 32, keep the drawn way, else, use the given character.
	*/
	void SetControlCharSymbol(int symbol);
	/**
	* Get the way control characters are displayed.
	*/
	int GetControlCharSymbol();
	/**
	* Move to the previous change in capitalisation.
	*/
	void WordPartLeft();
	/**
	* Move to the previous change in capitalisation extending selection
	* to new caret position.
	*/
	void WordPartLeftExtend();
	/**
	* Move to the change next in capitalisation.
	*/
	void WordPartRight();
	/**
	* Move to the next change in capitalisation extending selection
	* to new caret position.
	*/
	void WordPartRightExtend();
	/**
	* Set the way the display area is determined when a particular line
	* is to be moved to by Find, FindNext, GotoLine, etc.
	*/
	void SetVisiblePolicy(VisiblePolicy visiblePolicy,int visibleSlop);
	/**
	* Delete back from the current position to the start of the line.
	*/
	void DelLineLeft();
	/**
	* Delete forwards from the current position to the end of the line.
	*/
	void DelLineRight();
	/**
	* Set the xOffset (ie, horizontal scroll position).
	*/
	void SetXOffset(int xOffset);
	/**
	* Get the xOffset (ie, horizontal scroll position).
	*/
	int GetXOffset();
	/**
	* Set the last x chosen value to be the caret x position.
	*/
	void ChooseCaretX();
	/**
	* Set the focus to this Scintilla widget.
	*/
	void GrabFocus();
	/**
	* Set the way the caret is kept visible when going sideways.
	* The exclusion zone is given in pixels.
	*/
	void SetXCaretPolicy(CaretPolicy caretPolicy,int caretSlop);
	/**
	* Set the way the line the caret is on is kept visible.
	* The exclusion zone is given in lines.
	*/
	void SetYCaretPolicy(CaretPolicy caretPolicy,int caretSlop);
	/**
	* Set printing to line wrapped (SC_WRAP_WORD) or not line wrapped (SC_WRAP_NONE).
	*/
	void SetPrintWrapMode(Wrap wrapMode);
	/**
	* Is printing line wrapped?
	*/
	Wrap GetPrintWrapMode();
	/**
	* Set a fore colour for active hotspots.
	*/
	void SetHotspotActiveFore(bool useSetting,colour fore);
	/**
	* Get the fore colour for active hotspots.
	*/
	colour GetHotspotActiveFore();
	/**
	* Set a back colour for active hotspots.
	*/
	void SetHotspotActiveBack(bool useSetting,colour back);
	/**
	* Get the back colour for active hotspots.
	*/
	colour GetHotspotActiveBack();
	/**
	* Enable / Disable underlining active hotspots.
	*/
	void SetHotspotActiveUnderline(bool underline);
	/**
	* Get whether underlining for active hotspots.
	*/
	bool GetHotspotActiveUnderline();
	/**
	* Limit hotspots to single line so hotspots on two lines don't merge.
	*/
	void SetHotspotSingleLine(bool singleLine);
	/**
	* Get the HotspotSingleLine property
	*/
	bool GetHotspotSingleLine();
	/**
	* Move caret down one paragraph (delimited by empty lines).
	*/
	void ParaDown();
	/**
	* Extend selection down one paragraph (delimited by empty lines).
	*/
	void ParaDownExtend();
	/**
	* Move caret up one paragraph (delimited by empty lines).
	*/
	void ParaUp();
	/**
	* Extend selection up one paragraph (delimited by empty lines).
	*/
	void ParaUpExtend();
	/**
	* Given a valid document position, return the previous position taking code
	* page into account. Returns 0 if passed 0.
	*/
	position PositionBefore(position pos);
	/**
	* Given a valid document position, return the next position taking code
	* page into account. Maximum value returned is the last position in the document.
	*/
	position PositionAfter(position pos);
	/**
	* Given a valid document position, return a position that differs in a number
	* of characters. Returned value is always between 0 and last position in document.
	*/
	position PositionRelative(position pos,position relative);
	/**
	* Given a valid document position, return a position that differs in a number
	* of UTF-16 code units. Returned value is always between 0 and last position in document.
	* The result may point half way (2 bytes) inside a non-BMP character.
	*/
	position PositionRelativeCodeUnits(position pos,position relative);
	/**
	* Copy a range of text to the clipboard. Positions are clipped into the document.
	*/
	void CopyRange(position start,position end);
	/**
	* Copy argument text to the clipboard.
	*/
	void CopyText(position length,string text);
	/**
	* Set the selection mode to stream (SC_SEL_STREAM) or rectangular (SC_SEL_RECTANGLE/SC_SEL_THIN) or
	* by lines (SC_SEL_LINES).
	*/
	void SetSelectionMode(SelectionMode selectionMode);
	/**
	* Get the mode of the current selection.
	*/
	SelectionMode GetSelectionMode();
	/**
	* Get whether or not regular caret moves will extend or reduce the selection.
	*/
	bool GetMoveExtendsSelection();
	/**
	* Retrieve the position of the start of the selection at the given line (INVALID_POSITION if no selection on this line).
	*/
	position GetLineSelStartPosition(line line);
	/**
	* Retrieve the position of the end of the selection at the given line (INVALID_POSITION if no selection on this line).
	*/
	position GetLineSelEndPosition(line line);
	/**
	* Move caret down one line, extending rectangular selection to new caret position.
	*/
	void LineDownRectExtend();
	/**
	* Move caret up one line, extending rectangular selection to new caret position.
	*/
	void LineUpRectExtend();
	/**
	* Move caret left one character, extending rectangular selection to new caret position.
	*/
	void CharLeftRectExtend();
	/**
	* Move caret right one character, extending rectangular selection to new caret position.
	*/
	void CharRightRectExtend();
	/**
	* Move caret to first position on line, extending rectangular selection to new caret position.
	*/
	void HomeRectExtend();
	/**
	* Move caret to before first visible character on line.
	* If already there move to first character on line.
	* In either case, extend rectangular selection to new caret position.
	*/
	void VCHomeRectExtend();
	/**
	* Move caret to last position on line, extending rectangular selection to new caret position.
	*/
	void LineEndRectExtend();
	/**
	* Move caret one page up, extending rectangular selection to new caret position.
	*/
	void PageUpRectExtend();
	/**
	* Move caret one page down, extending rectangular selection to new caret position.
	*/
	void PageDownRectExtend();
	/**
	* Move caret to top of page, or one page up if already at top of page.
	*/
	void StutteredPageUp();
	/**
	* Move caret to top of page, or one page up if already at top of page, extending selection to new caret position.
	*/
	void StutteredPageUpExtend();
	/**
	* Move caret to bottom of page, or one page down if already at bottom of page.
	*/
	void StutteredPageDown();
	/**
	* Move caret to bottom of page, or one page down if already at bottom of page, extending selection to new caret position.
	*/
	void StutteredPageDownExtend();
	/**
	* Move caret left one word, position cursor at end of word.
	*/
	void WordLeftEnd();
	/**
	* Move caret left one word, position cursor at end of word, extending selection to new caret position.
	*/
	void WordLeftEndExtend();
	/**
	* Move caret right one word, position cursor at end of word.
	*/
	void WordRightEnd();
	/**
	* Move caret right one word, position cursor at end of word, extending selection to new caret position.
	*/
	void WordRightEndExtend();
	/**
	* Set the set of characters making up whitespace for when moving or selecting by word.
	* Should be called after SetWordChars.
	*/
	void SetWhitespaceChars(string characters);
	/**
	* Get the set of characters making up whitespace for when moving or selecting by word.
	*/
	int GetWhitespaceChars(stringresult characters);
	/**
	* Set the set of characters making up punctuation characters
	* Should be called after SetWordChars.
	*/
	void SetPunctuationChars(string characters);
	/**
	* Get the set of characters making up punctuation characters
	*/
	int GetPunctuationChars(stringresult characters);
	/**
	* Reset the set of characters for whitespace and word characters to the defaults.
	*/
	void SetCharsDefault();
	/**
	* Get currently selected item position in the auto-completion list
	*/
	int AutoCGetCurrent();
	/**
	* Get currently selected item text in the auto-completion list
	* Returns the length of the item text
	* Result is NUL-terminated.
	*/
	int AutoCGetCurrentText(stringresult text);
	/**
	* Set auto-completion case insensitive behaviour to either prefer case-sensitive matches or have no preference.
	*/
	void AutoCSetCaseInsensitiveBehaviour(CaseInsensitiveBehaviour behaviour);
	/**
	* Get auto-completion case insensitive behaviour.
	*/
	CaseInsensitiveBehaviour AutoCGetCaseInsensitiveBehaviour();
	/**
	* Change the effect of autocompleting when there are multiple selections.
	*/
	void AutoCSetMulti(MultiAutoComplete multi);
	/**
	* Retrieve the effect of autocompleting when there are multiple selections.
	*/
	MultiAutoComplete AutoCGetMulti();
	/**
	* Set the way autocompletion lists are ordered.
	*/
	void AutoCSetOrder(Ordering order);
	/**
	* Get the way autocompletion lists are ordered.
	*/
	Ordering AutoCGetOrder();
	/**
	* Enlarge the document to a particular size of text bytes.
	*/
	void Allocate(position bytes);
	/**
	* Returns the target converted to UTF8.
	* Return the length in bytes.
	*/
	position TargetAsUTF8(stringresult s);
	/**
	* Set the length of the utf8 argument for calling EncodedFromUTF8.
	* Set to -1 and the string will be measured to the first nul.
	*/
	void SetLengthForEncode(position bytes);
	/**
	* Translates a UTF8 string into the document encoding.
	* Return the length of the result in bytes.
	* On error return 0.
	*/
	position EncodedFromUTF8(string utf8,stringresult encoded);
	/**
	* Find the position of a column on a line taking into account tabs and
	* multi-byte characters. If beyond end of line, return line end position.
	*/
	position FindColumn(line line,position column);
	/**
	* Can the caret preferred x position only be changed by explicit movement commands?
	*/
	CaretSticky GetCaretSticky();
	/**
	* Stop the caret preferred x position changing when the user types.
	*/
	void SetCaretSticky(CaretSticky useCaretStickyBehaviour);
	/**
	* Switch between sticky and non-sticky: meant to be bound to a key.
	*/
	void ToggleCaretSticky();
	/**
	* Enable/Disable convert-on-paste for line endings
	*/
	void SetPasteConvertEndings(bool convert);
	/**
	* Get convert-on-paste setting
	*/
	bool GetPasteConvertEndings();
	/**
	* Duplicate the selection. If selection empty duplicate the line containing the caret.
	*/
	void SelectionDuplicate();
	/**
	* Set background alpha of the caret line.
	*/
	void SetCaretLineBackAlpha(Alpha alpha);
	/**
	* Get the background alpha of the caret line.
	*/
	Alpha GetCaretLineBackAlpha();
	/**
	* Set the style of the caret to be drawn.
	*/
	void SetCaretStyle(CaretStyle caretStyle);
	/**
	* Returns the current style of the caret.
	*/
	CaretStyle GetCaretStyle();
	/**
	* Set the indicator used for IndicatorFillRange and IndicatorClearRange
	*/
	void SetIndicatorCurrent(int indicator);
	/**
	* Get the current indicator
	*/
	int GetIndicatorCurrent();
	/**
	* Set the value used for IndicatorFillRange
	*/
	void SetIndicatorValue(int value);
	/**
	* Get the current indicator value
	*/
	int GetIndicatorValue();
	/**
	* Turn a indicator on over a range.
	*/
	void IndicatorFillRange(position start,position lengthFill);
	/**
	* Turn a indicator off over a range.
	*/
	void IndicatorClearRange(position start,position lengthClear);
	/**
	* Are any indicators present at pos?
	*/
	int IndicatorAllOnFor(position pos);
	/**
	* What value does a particular indicator have at a position?
	*/
	int IndicatorValueAt(int indicator,position pos);
	/**
	* Where does a particular indicator start?
	*/
	position IndicatorStart(int indicator,position pos);
	/**
	* Where does a particular indicator end?
	*/
	position IndicatorEnd(int indicator,position pos);
	/**
	* Set number of entries in position cache
	*/
	void SetPositionCache(int size);
	/**
	* How many entries are allocated to the position cache?
	*/
	int GetPositionCache();
	/**
	* Copy the selection, if selection empty copy the line with the caret
	*/
	void CopyAllowLine();
	/**
	* Compact the document buffer and return a read-only pointer to the
	* characters in the document.
	*/
	pointer GetCharacterPointer();
	/**
	* Return a read-only pointer to a range of characters in the document.
	* May move the gap so that the range is contiguous, but will only move up
	* to lengthRange bytes.
	*/
	pointer GetRangePointer(position start,position lengthRange);
	/**
	* Return a position which, to avoid performance costs, should not be within
	* the range of a call to GetRangePointer.
	*/
	position GetGapPosition();
	/**
	* Set the alpha fill colour of the given indicator.
	*/
	void IndicSetAlpha(int indicator,Alpha alpha);
	/**
	* Get the alpha fill colour of the given indicator.
	*/
	Alpha IndicGetAlpha(int indicator);
	/**
	* Set the alpha outline colour of the given indicator.
	*/
	void IndicSetOutlineAlpha(int indicator,Alpha alpha);
	/**
	* Get the alpha outline colour of the given indicator.
	*/
	Alpha IndicGetOutlineAlpha(int indicator);
	/**
	* Set extra ascent for each line
	*/
	void SetExtraAscent(int extraAscent);
	/**
	* Get extra ascent for each line
	*/
	int GetExtraAscent();
	/**
	* Set extra descent for each line
	*/
	void SetExtraDescent(int extraDescent);
	/**
	* Get extra descent for each line
	*/
	int GetExtraDescent();
	/**
	* Which symbol was defined for markerNumber with MarkerDefine
	*/
	int MarkerSymbolDefined(int markerNumber);
	/**
	* Set the text in the text margin for a line
	*/
	void MarginSetText(line line,string text);
	/**
	* Get the text in the text margin for a line
	*/
	int MarginGetText(line line,stringresult text);
	/**
	* Set the style number for the text margin for a line
	*/
	void MarginSetStyle(line line,int style);
	/**
	* Get the style number for the text margin for a line
	*/
	int MarginGetStyle(line line);
	/**
	* Set the style in the text margin for a line
	*/
	void MarginSetStyles(line line,string styles);
	/**
	* Get the styles in the text margin for a line
	*/
	int MarginGetStyles(line line,stringresult styles);
	/**
	* Clear the margin text on all lines
	*/
	void MarginTextClearAll();
	/**
	* Get the start of the range of style numbers used for margin text
	*/
	void MarginSetStyleOffset(int style);
	/**
	* Get the start of the range of style numbers used for margin text
	*/
	int MarginGetStyleOffset();
	/**
	* Set the margin options.
	*/
	void SetMarginOptions(MarginOption marginOptions);
	/**
	* Get the margin options.
	*/
	MarginOption GetMarginOptions();
	/**
	* Set the annotation text for a line
	*/
	void AnnotationSetText(line line,string text);
	/**
	* Get the annotation text for a line
	*/
	int AnnotationGetText(line line,stringresult text);
	/**
	* Set the style number for the annotations for a line
	*/
	void AnnotationSetStyle(line line,int style);
	/**
	* Get the style number for the annotations for a line
	*/
	int AnnotationGetStyle(line line);
	/**
	* Set the annotation styles for a line
	*/
	void AnnotationSetStyles(line line,string styles);
	/**
	* Get the annotation styles for a line
	*/
	int AnnotationGetStyles(line line,stringresult styles);
	/**
	* Get the number of annotation lines for a line
	*/
	int AnnotationGetLines(line line);
	/**
	* Clear the annotations from all lines
	*/
	void AnnotationClearAll();
	/**
	* Set the visibility for the annotations for a view
	*/
	void AnnotationSetVisible(AnnotationVisible visible);
	/**
	* Get the visibility for the annotations for a view
	*/
	AnnotationVisible AnnotationGetVisible();
	/**
	* Get the start of the range of style numbers used for annotations
	*/
	void AnnotationSetStyleOffset(int style);
	/**
	* Get the start of the range of style numbers used for annotations
	*/
	int AnnotationGetStyleOffset();
	/**
	* Release all extended (>255) style numbers
	*/
	void ReleaseAllExtendedStyles();
	/**
	* Allocate some extended (>255) style numbers and return the start of the range
	*/
	int AllocateExtendedStyles(int numberStyles);
	/**
	* Add a container action to the undo stack
	*/
	void AddUndoAction(int token,UndoFlags flags);
	/**
	* Find the position of a character from a point within the window.
	*/
	position CharPositionFromPoint(int x,int y);
	/**
	* Find the position of a character from a point within the window.
	* Return INVALID_POSITION if not close to text.
	*/
	position CharPositionFromPointClose(int x,int y);
	/**
	* Set whether switching to rectangular mode while selecting with the mouse is allowed.
	*/
	void SetMouseSelectionRectangularSwitch(bool mouseSelectionRectangularSwitch);
	/**
	* Whether switching to rectangular mode while selecting with the mouse is allowed.
	*/
	bool GetMouseSelectionRectangularSwitch();
	/**
	* Set whether multiple selections can be made
	*/
	void SetMultipleSelection(bool multipleSelection);
	/**
	* Whether multiple selections can be made
	*/
	bool GetMultipleSelection();
	/**
	* Set whether typing can be performed into multiple selections
	*/
	void SetAdditionalSelectionTyping(bool additionalSelectionTyping);
	/**
	* Whether typing can be performed into multiple selections
	*/
	bool GetAdditionalSelectionTyping();
	/**
	* Set whether additional carets will blink
	*/
	void SetAdditionalCaretsBlink(bool additionalCaretsBlink);
	/**
	* Whether additional carets will blink
	*/
	bool GetAdditionalCaretsBlink();
	/**
	* Set whether additional carets are visible
	*/
	void SetAdditionalCaretsVisible(bool additionalCaretsVisible);
	/**
	* Whether additional carets are visible
	*/
	bool GetAdditionalCaretsVisible();
	/**
	* How many selections are there?
	*/
	int GetSelections();
	/**
	* Is every selected range empty?
	*/
	bool GetSelectionEmpty();
	/**
	* Clear selections to a single empty stream selection
	*/
	void ClearSelections();
	/**
	* Set a simple selection
	*/
	void SetSelection(position caret,position anchor);
	/**
	* Add a selection
	*/
	void AddSelection(position caret,position anchor);
	/**
	* Drop one selection
	*/
	void DropSelectionN(int selection);
	/**
	* Set the main selection
	*/
	void SetMainSelection(int selection);
	/**
	* Which selection is the main selection
	*/
	int GetMainSelection();
	/**
	* Set the caret position of the nth selection.
	*/
	void SetSelectionNCaret(int selection,position caret);
	/**
	* Return the caret position of the nth selection.
	*/
	position GetSelectionNCaret(int selection);
	/**
	* Set the anchor position of the nth selection.
	*/
	void SetSelectionNAnchor(int selection,position anchor);
	/**
	* Return the anchor position of the nth selection.
	*/
	position GetSelectionNAnchor(int selection);
	/**
	* Set the virtual space of the caret of the nth selection.
	*/
	void SetSelectionNCaretVirtualSpace(int selection,position space);
	/**
	* Return the virtual space of the caret of the nth selection.
	*/
	position GetSelectionNCaretVirtualSpace(int selection);
	/**
	* Set the virtual space of the anchor of the nth selection.
	*/
	void SetSelectionNAnchorVirtualSpace(int selection,position space);
	/**
	* Return the virtual space of the anchor of the nth selection.
	*/
	position GetSelectionNAnchorVirtualSpace(int selection);
	/**
	* Sets the position that starts the selection - this becomes the anchor.
	*/
	void SetSelectionNStart(int selection,position anchor);
	/**
	* Returns the position at the start of the selection.
	*/
	position GetSelectionNStart(int selection);
	/**
	* Returns the virtual space at the start of the selection.
	*/
	position GetSelectionNStartVirtualSpace(int selection);
	/**
	* Sets the position that ends the selection - this becomes the currentPosition.
	*/
	void SetSelectionNEnd(int selection,position caret);
	/**
	* Returns the virtual space at the end of the selection.
	*/
	position GetSelectionNEndVirtualSpace(int selection);
	/**
	* Returns the position at the end of the selection.
	*/
	position GetSelectionNEnd(int selection);
	/**
	* Set the caret position of the rectangular selection.
	*/
	void SetRectangularSelectionCaret(position caret);
	/**
	* Return the caret position of the rectangular selection.
	*/
	position GetRectangularSelectionCaret();
	/**
	* Set the anchor position of the rectangular selection.
	*/
	void SetRectangularSelectionAnchor(position anchor);
	/**
	* Return the anchor position of the rectangular selection.
	*/
	position GetRectangularSelectionAnchor();
	/**
	* Set the virtual space of the caret of the rectangular selection.
	*/
	void SetRectangularSelectionCaretVirtualSpace(position space);
	/**
	* Return the virtual space of the caret of the rectangular selection.
	*/
	position GetRectangularSelectionCaretVirtualSpace();
	/**
	* Set the virtual space of the anchor of the rectangular selection.
	*/
	void SetRectangularSelectionAnchorVirtualSpace(position space);
	/**
	* Return the virtual space of the anchor of the rectangular selection.
	*/
	position GetRectangularSelectionAnchorVirtualSpace();
	/**
	* Set options for virtual space behaviour.
	*/
	void SetVirtualSpaceOptions(VirtualSpace virtualSpaceOptions);
	/**
	* Return options for virtual space behaviour.
	*/
	VirtualSpace GetVirtualSpaceOptions();
	/**
	* On GTK, allow selecting the modifier key to use for mouse-based
	* rectangular selection. Often the window manager requires Alt+Mouse Drag
	* for moving windows.
	* Valid values are SCMOD_CTRL(default), SCMOD_ALT, or SCMOD_SUPER.
	*/
	void SetRectangularSelectionModifier(int modifier);
	/**
	* Get the modifier key used for rectangular selection.
	*/
	int GetRectangularSelectionModifier();
	/**
	* Set the foreground colour of additional selections.
	* Must have previously called SetSelFore with non-zero first argument for this to have an effect.
	*/
	void SetAdditionalSelFore(colour fore);
	/**
	* Set the background colour of additional selections.
	* Must have previously called SetSelBack with non-zero first argument for this to have an effect.
	*/
	void SetAdditionalSelBack(colour back);
	/**
	* Set the alpha of the selection.
	*/
	void SetAdditionalSelAlpha(Alpha alpha);
	/**
	* Get the alpha of the selection.
	*/
	Alpha GetAdditionalSelAlpha();
	/**
	* Set the foreground colour of additional carets.
	*/
	void SetAdditionalCaretFore(colour fore);
	/**
	* Get the foreground colour of additional carets.
	*/
	colour GetAdditionalCaretFore();
	/**
	* Set the main selection to the next selection.
	*/
	void RotateSelection();
	/**
	* Swap that caret and anchor of the main selection.
	*/
	void SwapMainAnchorCaret();
	/**
	* Add the next occurrence of the main selection to the set of selections as main.
	* If the current selection is empty then select word around caret.
	*/
	void MultipleSelectAddNext();
	/**
	* Add each occurrence of the main selection in the target to the set of selections.
	* If the current selection is empty then select word around caret.
	*/
	void MultipleSelectAddEach();
	/**
	* Indicate that the internal state of a lexer has changed over a range and therefore
	* there may be a need to redraw.
	*/
	int ChangeLexerState(position start,position end);
	/**
	* Find the next line at or after lineStart that is a contracted fold header line.
	* Return -1 when no more lines.
	*/
	line ContractedFoldNext(line lineStart);
	/**
	* Centre current line in window.
	*/
	void VerticalCentreCaret();
	/**
	* Move the selected lines up one line, shifting the line above after the selection
	*/
	void MoveSelectedLinesUp();
	/**
	* Move the selected lines down one line, shifting the line below before the selection
	*/
	void MoveSelectedLinesDown();
	/**
	* Set the identifier reported as idFrom in notification messages.
	*/
	void SetIdentifier(int identifier);
	/**
	* Get the identifier.
	*/
	int GetIdentifier();
	/**
	* Set the width for future RGBA image data.
	*/
	void RGBAImageSetWidth(int width);
	/**
	* Set the height for future RGBA image data.
	*/
	void RGBAImageSetHeight(int height);
	/**
	* Set the scale factor in percent for future RGBA image data.
	*/
	void RGBAImageSetScale(int scalePercent);
	/**
	* Define a marker from RGBA data.
	* It has the width and height from RGBAImageSetWidth/Height
	*/
	void MarkerDefineRGBAImage(int markerNumber,string pixels);
	/**
	* Register an RGBA image for use in autocompletion lists.
	* It has the width and height from RGBAImageSetWidth/Height
	*/
	void RegisterRGBAImage(int type,string pixels);
	/**
	* Scroll to start of document.
	*/
	void ScrollToStart();
	/**
	* Scroll to end of document.
	*/
	void ScrollToEnd();
	/**
	* Set the technology used.
	*/
	void SetTechnology(Technology technology);
	/**
	* Get the tech.
	*/
	Technology GetTechnology();
	/**
	* Create an ILoader*.
	*/
	pointer CreateLoader(position bytes,DocumentOption documentOptions);
	/**
	* On OS X, show a find indicator.
	*/
	void FindIndicatorShow(position start,position end);
	/**
	* On OS X, flash a find indicator, then fade out.
	*/
	void FindIndicatorFlash(position start,position end);
	/**
	* On OS X, hide the find indicator.
	*/
	void FindIndicatorHide();
	/**
	* Move caret to before first visible character on display line.
	* If already there move to first character on display line.
	*/
	void VCHomeDisplay();
	/**
	* Like VCHomeDisplay but extending selection to new caret position.
	*/
	void VCHomeDisplayExtend();
	/**
	* Is the caret line always visible?
	*/
	bool GetCaretLineVisibleAlways();
	/**
	* Sets the caret line to always visible.
	*/
	void SetCaretLineVisibleAlways(bool alwaysVisible);
	/**
	* Set the line end types that the application wants to use. May not be used if incompatible with lexer or encoding.
	*/
	void SetLineEndTypesAllowed(LineEndType lineEndBitSet);
	/**
	* Get the line end types currently allowed.
	*/
	LineEndType GetLineEndTypesAllowed();
	/**
	* Get the line end types currently recognised. May be a subset of the allowed types due to lexer limitation.
	*/
	LineEndType GetLineEndTypesActive();
	/**
	* Set the way a character is drawn.
	*/
	void SetRepresentation(string encodedCharacter,string representation);
	/**
	* Set the way a character is drawn.
	* Result is NUL-terminated.
	*/
	int GetRepresentation(string encodedCharacter,stringresult representation);
	/**
	* Remove a character representation.
	*/
	void ClearRepresentation(string encodedCharacter);
	/**
	* Set the end of line annotation text for a line
	*/
	void EOLAnnotationSetText(line line,string text);
	/**
	* Get the end of line annotation text for a line
	*/
	int EOLAnnotationGetText(line line,stringresult text);
	/**
	* Set the style number for the end of line annotations for a line
	*/
	void EOLAnnotationSetStyle(line line,int style);
	/**
	* Get the style number for the end of line annotations for a line
	*/
	int EOLAnnotationGetStyle(line line);
	/**
	* Clear the end of annotations from all lines
	*/
	void EOLAnnotationClearAll();
	/**
	* Set the visibility for the end of line annotations for a view
	*/
	void EOLAnnotationSetVisible(EOLAnnotationVisible visible);
	/**
	* Get the visibility for the end of line annotations for a view
	*/
	EOLAnnotationVisible EOLAnnotationGetVisible();
	/**
	* Get the start of the range of style numbers used for end of line annotations
	*/
	void EOLAnnotationSetStyleOffset(int style);
	/**
	* Get the start of the range of style numbers used for end of line annotations
	*/
	int EOLAnnotationGetStyleOffset();
	/**
	* Get whether a feature is supported
	*/
	bool SupportsFeature(Supports feature);
	/**
	* Start notifying the container of all key presses and commands.
	*/
	void StartRecord();
	/**
	* Stop notifying the container of all key presses and commands.
	*/
	void StopRecord();
	/**
	* Retrieve the lexing language of the document.
	*/
	int GetLexer();
	/**
	* Colourise a segment of the document using the current lexing language.
	*/
	void Colourise(position start,position end);
	/**
	* Set up a value that may be used by a lexer for some optional feature.
	*/
	void SetProperty(string key,string value);
	/**
	* Set up the key words used by the lexer.
	*/
	void SetKeyWords(int keyWordSet,string keyWords);
	/**
	* Retrieve a "property" value previously set with SetProperty.
	* Result is NUL-terminated.
	*/
	int GetProperty(string key,stringresult value);
	/**
	* Retrieve a "property" value previously set with SetProperty,
	* with "$()" variable replacement on returned buffer.
	* Result is NUL-terminated.
	*/
	int GetPropertyExpanded(string key,stringresult value);
	/**
	* Retrieve a "property" value previously set with SetProperty,
	* interpreted as an int AFTER any "$()" variable replacement.
	*/
	int GetPropertyInt(string key,int defaultValue);
	/**
	* Retrieve the name of the lexer.
	* Return the length of the text.
	* Result is NUL-terminated.
	*/
	int GetLexerLanguage(stringresult language);
	/**
	* For private communication between an application and a known lexer.
	*/
	pointer PrivateLexerCall(int operation,pointer pointer);
	/**
	* Retrieve a '\n' separated list of properties understood by the current lexer.
	* Result is NUL-terminated.
	*/
	int PropertyNames(stringresult names);
	/**
	* Retrieve the type of a property.
	*/
	TypeProperty PropertyType(string name);
	/**
	* Describe a property.
	* Result is NUL-terminated.
	*/
	int DescribeProperty(string name,stringresult description);
	/**
	* Retrieve a '\n' separated list of descriptions of the keyword sets understood by the current lexer.
	* Result is NUL-terminated.
	*/
	int DescribeKeyWordSets(stringresult descriptions);
	/**
	* Bit set of LineEndType enumertion for which line ends beyond the standard
	* LF, CR, and CRLF are supported by the lexer.
	*/
	int GetLineEndTypesSupported();
	/**
	* Allocate a set of sub styles for a particular base style, returning start of range
	*/
	int AllocateSubStyles(int styleBase,int numberStyles);
	/**
	* The starting style number for the sub styles associated with a base style
	*/
	int GetSubStylesStart(int styleBase);
	/**
	* The number of sub styles associated with a base style
	*/
	int GetSubStylesLength(int styleBase);
	/**
	* For a sub style, return the base style, else return the argument.
	*/
	int GetStyleFromSubStyle(int subStyle);
	/**
	* For a secondary style, return the primary style, else return the argument.
	*/
	int GetPrimaryStyleFromStyle(int style);
	/**
	* Free allocated sub styles
	*/
	void FreeSubStyles();
	/**
	* Set the identifiers that are shown in a particular style
	*/
	void SetIdentifiers(int style,string identifiers);
	/**
	* Where styles are duplicated by a feature such as active/inactive code
	* return the distance between the two types.
	*/
	int DistanceToSecondaryStyles();
	/**
	* Get the set of base styles that can be extended with sub styles
	* Result is NUL-terminated.
	*/
	int GetSubStyleBases(stringresult styles);
	/**
	* Retrieve the number of named styles for the lexer.
	*/
	int GetNamedStyles();
	/**
	* Retrieve the name of a style.
	* Result is NUL-terminated.
	*/
	int NameOfStyle(int style,stringresult name);
	/**
	* Retrieve a ' ' separated list of style tags like "literal quoted string".
	* Result is NUL-terminated.
	*/
	int TagsOfStyle(int style,stringresult tags);
	/**
	* Retrieve a description of a style.
	* Result is NUL-terminated.
	*/
	int DescriptionOfStyle(int style,stringresult description);
	/**
	* Set the lexer from an ILexer*.
	*/
	void SetILexer(pointer ilexer);
	/**
	* Retrieve bidirectional text display state.
	*/
	Bidirectional GetBidirectional();
	/**
	* Set bidirectional text display state.
	*/
	void SetBidirectional(Bidirectional bidirectional);
	/**
	* Retrieve line character index state.
	*/
	LineCharacterIndexType GetLineCharacterIndex();
	/**
	* Request line character index be created or its use count increased.
	*/
	void AllocateLineCharacterIndex(LineCharacterIndexType lineCharacterIndex);
	/**
	* Decrease use count of line character index and remove if 0.
	*/
	void ReleaseLineCharacterIndex(LineCharacterIndexType lineCharacterIndex);
	/**
	* Retrieve the document line containing a position measured in index units.
	*/
	line LineFromIndexPosition(position pos,LineCharacterIndexType lineCharacterIndex);
	/**
	* Retrieve the position measured in index units at the start of a document line.
	*/
	position IndexPositionFromLine(line line,LineCharacterIndexType lineCharacterIndex);
//ENDPART

};