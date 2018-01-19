//*******************************************************************
#include "lib.h"
#include "Module/RTOS.h"

//*******************************************************************

#include "configLPC17xx.h"
#include "GUI.h"
#include <math.h>

int currentValueX;
int pixels[320];

#define BUFFER 16
#define M 4
double scratchX[BUFFER];
double scratchY[BUFFER];

void DFT(int dir,int m,double *x1,double *y1, double *x2, double *y2)
{
   long i,k;
   double arg;
   double cosarg,sinarg;
	
   for (i=0;i<m;i++) {
      x2[i] = 0;
      y2[i] = 0;
      arg = - dir * 2.0 * 3.141592654 * (double)i / (double)m;
      for (k=0;k<m;k++) {
         cosarg = cos(k * arg);
         sinarg = sin(k * arg);
         x2[i] += (x1[k] * cosarg - y1[k] * sinarg);
         y2[i] += (x1[k] * sinarg + y1[k] * cosarg);
      }
   }

   /* Copy the data back */
   if (dir == 1) {
      for (i=0;i<m;i++) {
         x1[i] = x2[i] / (double)m;
         y1[i] = y2[i] / (double)m;
      }
   } else {
      for (i=0;i<m;i++) {
         x1[i] = x2[i];
         y1[i] = y2[i];
      }
   }
}


void displayValue(float _volts, int zoom = 0, int reset = 0, int fill = 0)
{
	currentValueX++;
	if (currentValueX >= 320 || reset)
	{
		currentValueX = 0;
	}
	_volts = _volts < 0 ? 0 : (_volts >= 5000 ? 5000 : _volts);
	
	if (zoom)
	{
		_volts *= 2.0f;
		_volts -= 2500.0f;
		_volts *= 4.0f;
	}
	
	int y = 240 - (int)(_volts / 5000.0f * 240.0f);
	if (y == 240) y = 239;
	if (y == 0) y = 1;
	
	int lastY = pixels[currentValueX];
	
	GUI::drawRect(Rect(currentValueX, lastY, 1, fill ? (240 - lastY) : 1), BLACK);
	
	GUI::drawRect(Rect(currentValueX, y, 1, fill ? (240 - y) : 1), WHITE);
	pixels[currentValueX] = y;
}

float max = 0;
float min = 100000;

//rc pass
//56k and 47nF
//op amp 
//(in -> 8.2k -> 5.6k, opamp out) (3.9k 47k to positive)

#define M_PI 3.1415
#define M_C 299792568
#define FREQ 10.525e9

class TimerHandler : public cList::Item
{
public:
	float buffer[BUFFER];
	int sliding;
	bool dirty;

	float lastY;
	float lastX;
	float f, b1, a0, a1;
	TimerHandler()
	{
		f = 70.0f;
		b1 = (float)exp(-2.0f * 3.141f * f * 1.0f / 100.0f);
		a0 = (1.0f - b1) / 2.0f;
		a1 = a0;
	}
  
	virtual void update( void )
	{
		float val = (float)adc0.get(3) / 0xFFFF * 5000.0f;
		
		lastY = a0 * val + a1 * lastX + b1 * lastY;
		lastY = lastY < 0 ? 0 : (lastY > 5000.0f ? 5000 : lastY);
		//val = lastY;
		
		buffer[sliding++] = val;
		if (sliding >= BUFFER)
		{
			dirty = true;
			sliding = 0;
		}
	}
};

TimerHandler handler;

//*******************************************************************
int main(void)
{
	cDevDisplayGraphic graphics( dispHw );
	GUI::init(&graphics, 320, 240);
	GUI::drawRect(GUI::screenRect, BLACK);
	
	adc0.enable(3);
	
	timer.add(&handler);
	
	double data[BUFFER];
	double out[BUFFER];
	
	int cnter = 0;
	
	while(1)
	{
		if (handler.dirty)
		{
			for (int i = 0; i < BUFFER; i++)
			{
				data[i] = handler.buffer[i];
			}
			
			handler.dirty = false;
			
			DFT(1, M, data, out, scratchX, scratchY);
			
			float max = 0;
			int t = 0;
			for (int i = 0; i < BUFFER; i++)
			{
				if (abs(out[i]) > max) 
				{
					max = abs(out[i]);
					t = i;
				}
			}
			displayValue(max, 0, !((cnter++) % 50), 1);
			
			//displayValue(t * 100.0f, 0, 0);
			
			//displayValue(max / 10.0f, 1, 0);
			//displayValue(max*M_C/(2.0 * FREQ), 1, 0);
			
			//GUI::drawString(Rect(0, 0, 100, 30), BLACK, WHITE, "%d", t);
		}
	}
}

//EOF
