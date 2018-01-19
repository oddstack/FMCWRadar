//*******************************************************************
// IDrawable
//*******************************************************************

#ifndef __GUI
#define __GUI

//-------------------------------------------------------------------

#include "lib.h"
#include "Rect.h"
#include <stdarg.h>

#define COLOR WORD

#define BLACK cHwDisplayGraphic::Black
#define NAVY cHwDisplayGraphic::Navy
#define DARKGREEN cHwDisplayGraphic::DarkGreen
#define DARKCYAN cHwDisplayGraphic::DarkCyan
#define MAROON cHwDisplayGraphic::Maroon
#define PURPLE cHwDisplayGraphic::Purple
#define OLIVE cHwDisplayGraphic::Olive
#define GREY cHwDisplayGraphic::Grey
#define DARKGREY cHwDisplayGraphic::DarkGrey
#define BLUE cHwDisplayGraphic::Blue
#define GREEN cHwDisplayGraphic::Green
#define CYAN cHwDisplayGraphic::Cyan
#define RED cHwDisplayGraphic::Red
#define MAGENTA cHwDisplayGraphic::Magenta
#define YELLOW cHwDisplayGraphic::Yellow
#define WHITE cHwDisplayGraphic::White

#define LCD_BUFFER_SIZE 256

class GUI 
{
protected:
	cDevDisplayGraphic *disp;	
public:
	static Rect screenRect;

	static GUI& getInstance()
	{
		static GUI instance;
		return instance;
	}
	
	GUI() 
	{
		
	}
	
	static void init(cDevDisplayGraphic *_disp, int _width, int _height)
	{
		getInstance().disp = _disp;
		GUI::screenRect = Rect(0, 0, _width, _height);
	}
	
	GUI(GUI const&);
	void operator=(GUI const&);
	
	static void drawRect(Rect _rect, COLOR _color)
	{
		getInstance().disp->drawRectangle(_rect.x, _rect.y, _rect.w, _rect.h, _color );
	}
	
	static void drawRectOutline(Rect _rect, COLOR _color, BYTE _th = 1)
	{
		getInstance().disp->drawFrame(_rect.x, _rect.y, _rect.w, _rect.h, _th, _color);
	}
	
	static void drawString(Rect _r, COLOR _tColor, COLOR _bgColor, const char *_format,...)
	{
		char str[LCD_BUFFER_SIZE+1]; // ein paar Zeichen als Reserve

		va_list ptr;
		va_start(ptr, _format);
		vsprintf(str, _format, ptr);
		va_end(ptr);
		
		cDevDisplayGraphic *disp = getInstance().disp;
		disp->setTextColor(_tColor);
    disp->setBackColor(_bgColor);
		disp->drawText(_r.x, _r.y, str);
	}
};

#endif

//EOF
