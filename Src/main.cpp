//*******************************************************************
#include "lib.h"
#include "Module/RTOS.h"

//*******************************************************************

#include "configLPC17xx.h"
#include "GUI.h"
#include <math.h>

int currentValueX;
int pixels[320];

#define BUFFER 128
#define M 7

double scratchX[M];
double scratchY[M];

short FFT(short int dir,long m,double *x,double *y)
{
   long n,i,i1,j,k,i2,l,l1,l2;
   double c1,c2,tx,ty,t1,t2,u1,u2,z;

   /* Calculate the number of points */
   n = 1;
   for (i=0;i<m;i++) 
      n *= 2;

   /* Do the bit reversal */
   i2 = n >> 1;
   j = 0;
   for (i=0;i<n-1;i++) {
      if (i < j) {
         tx = x[i];
         ty = y[i];
         x[i] = x[j];
         y[i] = y[j];
         x[j] = tx;
         y[j] = ty;
      }
      k = i2;
      while (k <= j) {
         j -= k;
         k >>= 1;
      }
      j += k;
   }

   /* Compute the FFT */
   c1 = -1.0; 
   c2 = 0.0;
   l2 = 1;
   for (l=0;l<m;l++) {
      l1 = l2;
      l2 <<= 1;
      u1 = 1.0; 
      u2 = 0.0;
      for (j=0;j<l1;j++) {
         for (i=j;i<n;i+=l2) {
            i1 = i + l1;
            t1 = u1 * x[i1] - u2 * y[i1];
            t2 = u1 * y[i1] + u2 * x[i1];
            x[i1] = x[i] - t1; 
            y[i1] = y[i] - t2;
            x[i] += t1;
            y[i] += t2;
         }
         z =  u1 * c1 - u2 * c2;
         u2 = u1 * c2 + u2 * c1;
         u1 = z;
      }
      c2 = sqrt((1.0 - c1) / 2.0);
      if (dir == 1) 
         c2 = -c2;
      c1 = sqrt((1.0 + c1) / 2.0);
   }

   /* Scaling for forward transform */
   if (dir == 1) {
      for (i=0;i<n;i++) {
         x[i] /= n;
         y[i] /= n;
      }
   }
   
   return(1);
}
/*void DFT(int dir,int m,double *x1,double *y1, double *x2, double *y2)
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

   // Copy the data back
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
}*/


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
//NEW						 33k													 

#define M_PI 3.1415
#define M_C 299792568
#define FREQ 10.525e9

#define SAMPLE 1000.0f

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
		f = 140.0f;
		b1 = (float)exp(-2.0f * 3.141f * f * 1.0f / SAMPLE);
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
	int cnter2 = 0;
	
	float finc = 1.0f;
	while(1)
	{
		if (handler.dirty)
		{
			for (int i = 0; i < BUFFER; i++)
			{
				data[i] = handler.buffer[i];
			}
			
			/*for (int i = 0; i < BUFFER; i++)
			{
				displayValue(data[i], 0, 0, 0);
			}*/
			
			handler.dirty = false;
			
			//DFT(1, M, data, out, scratchX, scratchY);
			FFT(1, M, data, out);
			
			float max = 0;
			float min = 10000;
			int mI = 0;
			for (int i = 1; i < M*2; i++)
			{
				float m = sqrt(data[i] * data[i] + out[i] * out[i]);
				if (m > max) 
				{
					max = m; 
					mI = i;
				}
				if (m < min) min = m;
				//for (int j = 0; j < 10; j++) displayValue(m*10.0f, 0, !(cnter2%3), 1);
			}
			
			float f = (mI-0.5f) * (SAMPLE/BUFFER);
			float v = M_C/2.0 * (f / FREQ);
			
			if (abs(min-max) < 10) 
			{
				f = 0;
				v = 0;
			}
			
			displayValue(v * 2000.0f, 0, 0, 1);
			//GUI::drawString(Rect(0, 0, 100, 30), BLACK, WHITE, "%f", f);
			//GUI::drawString(Rect(0, 30, 100, 30), BLACK, WHITE, "%f", v);
			
			finc += 0.05f;
			cnter2++;
			
			//displayValue(t * 100.0f, 0, 0);
			
			//displayValue(max / 10.0f, 1, 0);
			//displayValue(max*M_C/(2.0 * FREQ), 1, 0);
			
			//GUI::drawString(Rect(0, 0, 100, 30), BLACK, WHITE, "%d", t);
		
		}
	}
}

//EOF
