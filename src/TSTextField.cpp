#include "TSTextField.hpp"
#include "widgets.hpp"
// for gVg
#include "gui.hpp"
// for key codes
#include <GLFW/glfw3.h>
using namespace rack;


// Draw if visible.
void TSTextField::draw(NVGcontext *vg) {
	if (visible)
	{
		/// TODO: Figure out how to limit the text inside the damn text box LOL.
		/// Probably implement scrolling of text and some cropping.
		TextField::draw(vg);
	}
} // end draw()

// Request focus on this field.
void TSTextField::requestFocus() {
	gFocusedWidget = this;	
	{
		EventFocus eFocus;
		this->onFocus(eFocus);
	}
	return;
} // end requestFocus()

// Remove invalid chars from input.
std::string TSTextField::cleanseString(std::string newText)
{
	if (allowedTextType == TextType::Any)
	{
		return newText.substr(0, maxLength);
	}
	else
	{
		// Remove invalid chars
		std::stringstream cleansedStr;
		// Issue: https://github.com/j4s0n-c/trowaSoft-VCV/issues/5. Changed from string constant (emtpy string "") to string object empty string ("") to older Linux compilers. Thx to @Chaircrusher.
		std::regex_replace(std::ostream_iterator<char>(cleansedStr), newText.begin(), newText.end(), regexInvalidChar, std::string(""));
		return cleansedStr.str().substr(0, maxLength);
	}
} // end cleanseString()

// Remove invalid chars
void TSTextField::insertText(std::string newText) {
	if (begin < end)
		text.erase(begin, end - begin);
	std::string cleansedStr = cleanseString(newText);
	text.insert(begin, cleansedStr);
	if (text.length() > maxLength)
		text = text.substr(0, maxLength);
	begin += cleansedStr.size();
	if (begin > maxLength - 1)
		begin = maxLength - 1;
	end = begin;
	onTextChange();
} // end insertText()
// When the text changes.
void TSTextField::onTextChange() {
	text = cleanseString(text);
	begin = mini(maxi(begin, 0), text.size());
	end = mini(maxi(end, 0), text.size());
	return;
} // end onTextChanged()
// On key press.
void TSTextField::onKey(EventKey &e) {
	if (!visible)
	{
		// Do not capture the keys.
		e.consumed = false;
		return;
	}
	// Flag if we need to validate/cleanse this character (only if printable and if we are doing validation).
	bool checkKey = (this->allowedTextType != TextType::Any) && isPrintableKey(e.key);
	switch (e.key) {
	case GLFW_KEY_BACKSPACE:
		if (begin < end) {
			text.erase(begin, end - begin);
			onTextChange();
		}
		else {
			begin--;
			if (begin >= 0) {
				text.erase(begin, 1);
				onTextChange();
			}
		}
		end = begin;
		break;
	case GLFW_KEY_DELETE:
		if (begin < end) {
			text.erase(begin, end - begin);
			onTextChange();
		}
		else {
			text.erase(begin, 1);
			onTextChange();
		}
		end = begin;
		break;
	case GLFW_KEY_LEFT:
		if (begin < end) {
		}
		else {
			begin--;
		}
		end = begin;
		break;
	case GLFW_KEY_RIGHT:
		if (begin < end) {
			begin = end;
		}
		else {
			begin++;
		}
		end = begin;
		break;
	case GLFW_KEY_HOME:
		end = begin = 0;
		break;
	case GLFW_KEY_END:
		end = begin = text.size();
		break;
	case GLFW_KEY_ENTER:
		if (multiline) {
			insertText("\n");
		}
		else {
			EventAction e;
			onAction(e);
		}
		break;
	case GLFW_KEY_TAB:
		// If we have an event to fire, then do it
		if (guiIsShiftPressed()) 
		{
			if (onShiftTabCallback != NULL)
				onShiftTabCallback(id);
			else if (prevField != NULL)
				prevField->requestFocus();
		}
		else if (onTabCallback != NULL)
		{
			onTabCallback(id);
		}
		else if (nextField != NULL)
		{
			nextField->requestFocus();
		}
		break;
	case GLFW_KEY_V:
		if (guiIsModPressed()) {
			// Paste (do not check character)
			checkKey = false;
			const char *newText = glfwGetClipboardString(gWindow);
			if (newText)
				insertText(newText);
		}	
		break;
	case GLFW_KEY_C:
		if (guiIsModPressed()) {
			// Copy (do not check character)
			checkKey = false;
			if (begin < end) {
				std::string selectedText = text.substr(begin, end - begin);
				glfwSetClipboardString(gWindow, selectedText.c_str());
			}
		}
		break;
	}
	if (checkKey)
	{
		this->onTextChange(); // Do some cleansing
	}
	else
	{
		begin = mini(maxi(begin, 0), text.size());
		end = mini(maxi(end, 0), text.size());
	}
	e.consumed = true;
	return;
} // end onKey()
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
// isPrintableKey()
// @keyCode : (IN) The key that is pressed.
// @returns: True if the key represents a printable character, false if not.
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
bool isPrintableKey(int key)
{
	bool isPrintable = false;
	switch (key)
	{
	case GLFW_KEY_SPACE:
	case GLFW_KEY_APOSTROPHE:
	case GLFW_KEY_COMMA:
	case GLFW_KEY_MINUS:
	case GLFW_KEY_PERIOD:
	case GLFW_KEY_SLASH:
	case GLFW_KEY_0:
	case GLFW_KEY_1:
	case GLFW_KEY_2:
	case GLFW_KEY_3:
	case GLFW_KEY_4:
	case GLFW_KEY_5:
	case GLFW_KEY_6:
	case GLFW_KEY_7:
	case GLFW_KEY_8:
	case GLFW_KEY_9:
	case GLFW_KEY_SEMICOLON:
	case GLFW_KEY_EQUAL:
	case GLFW_KEY_A:
	case GLFW_KEY_B:
	case GLFW_KEY_C:
	case GLFW_KEY_D:
	case GLFW_KEY_E:
	case GLFW_KEY_F:
	case GLFW_KEY_G:
	case GLFW_KEY_H:
	case GLFW_KEY_I:
	case GLFW_KEY_J:
	case GLFW_KEY_K:
	case GLFW_KEY_L:
	case GLFW_KEY_M:
	case GLFW_KEY_N:
	case GLFW_KEY_O:
	case GLFW_KEY_P:
	case GLFW_KEY_Q:
	case GLFW_KEY_R:
	case GLFW_KEY_S:
	case GLFW_KEY_T:
	case GLFW_KEY_U:
	case GLFW_KEY_V:
	case GLFW_KEY_W:
	case GLFW_KEY_X:
	case GLFW_KEY_Y:
	case GLFW_KEY_Z:
	case GLFW_KEY_LEFT_BRACKET:
	case GLFW_KEY_BACKSLASH:
	case GLFW_KEY_RIGHT_BRACKET:
	case GLFW_KEY_GRAVE_ACCENT:
	case GLFW_KEY_WORLD_1:
	case GLFW_KEY_WORLD_2:
		isPrintable = true;
		break;
	}
	return isPrintable;
} // end isPrintableKey()
