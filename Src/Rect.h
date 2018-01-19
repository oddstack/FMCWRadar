//*******************************************************************
// IDrawable
//*******************************************************************

#ifndef __RECT
#define __RECT

//-------------------------------------------------------------------

#include "lib.h"

struct Rect
{
	int x;
	int y;
	int w;
	int h;

	Rect()
	{
		
	}
	
	Rect(int _x, int _y, int _w, int _h)
	{
		x = _x;
		y = _y;
		w = _w;
		h = _h;
	}
	
	bool isValid()
	{
		return w > 0 && h > 0;
	}
	
	bool intersects(Rect &_r2)
	{
		if (x+w < _r2.x || y+h < _r2.y || x >= _r2.x+_r2.w || y >= _r2.y+_r2.h) return false;
		return true;
	}
};

#define MINRECT(a, b) ((a.w+a.h<b.w+b.h) ? a : b)
#define MAXRECT(a, b) ((a.w+a.h<b.w+b.h) ? b : a)

#endif

//EOF
