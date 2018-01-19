//*******************************************************************
#include "lib.h"
#include "Module/RTOS.h"

//*******************************************************************
#if defined _MCU_TYPE_LPC17XX
  #include "configLPC17xx.h"
#elif defined _MCU_TYPE_STM32L1XX
  #include "configSTM32L1xx.h"
#elif defined _MCU_TYPE_VIRTUAL
  #include "configVirtual.h"
#else
  #error "Device type not defined"
#endif

#include "GUI.h"
#include <math.h>

int currentValueX;
int pixels[320];

typedef struct complex
{
	float Re;
	float Im;
} complex;

#define BUFFER 50
int sliding = 0;
complex window[BUFFER];
complex scratch[BUFFER];

void fft( complex *v, int n, complex *tmp )
{
  if(n>1) {			/* otherwise, do nothing and return */
    int k,m;    complex z, w, *vo, *ve;
    ve = tmp; vo = tmp+n/2;
    for(k=0; k<n/2; k++) {
      ve[k] = v[2*k];
      vo[k] = v[2*k+1];
    }
    fft( ve, n/2, v );		/* FFT on even-indexed elements of v[] */
    fft( vo, n/2, v );		/* FFT on odd-indexed elements of v[] */
    for(m=0; m<n/2; m++) {
      w.Re = cos(2*PI*m/(double)n);
      w.Im = -sin(2*PI*m/(double)n);
      z.Re = w.Re*vo[m].Re - w.Im*vo[m].Im;	/* Re(w*vo[m]) */
      z.Im = w.Re*vo[m].Im + w.Im*vo[m].Re;	/* Im(w*vo[m]) */
      v[  m  ].Re = ve[m].Re + z.Re;
      v[  m  ].Im = ve[m].Im + z.Im;
      v[m+n/2].Re = ve[m].Re - z.Re;
      v[m+n/2].Im = ve[m].Im - z.Im;
    }
  }
  return;
}

void displayValue(float _volts)
{
	currentValueX++;
	if (currentValueX >= 320)
	{
		currentValueX = 0;
	}
	_volts = _volts < 0 ? 0 : (_volts >= 5000 ? 5000 : _volts);
	
	int y = 240 - (int)(_volts / 5000.0f * 240.0f);
	if (y == 240) y = 239;
	if (y == 0) return;
	
	y -= 120.0f;
	y *= 2.0f;
	y -= 120.0f;
	y *= 2.0f;
	
	int lastY = pixels[currentValueX];
	
	GUI::drawRect(Rect(currentValueX, lastY, 1, 1), BLACK);
	
	GUI::drawRect(Rect(currentValueX, y, 1, 1), WHITE);
	pixels[currentValueX] = y;
}

float max = 0;
float min = 100000;

//rc pass
//56k and 47nF
//op amp 
//(in -> 8.2k -> 5.6k, opamp out) (3.9k 47k to positive)

//*******************************************************************
int main(void)
{
	cDevDisplayGraphic graphics( dispHw );
	GUI::init(&graphics, 320, 240);
	GUI::drawRect(GUI::screenRect, BLACK);
	
	adc0.enable(3);
	
	float prevX = 0;
	float lastY = 0;
	float f = 140.0f;
	float b1 = (float)exp(-2.0f * 3.141f * f * 1.0f / 1000.0f);
	float a0 = (1.0f - b1) / 2.0f;
	float a1 = a0;
  while(1)
	{
		float val = (float)adc0.get(3) / 0xFFFF * 5000.0f;
		
		lastY = a0 * val + a1 * prevX + b1 * lastY;
		displayValue(lastY);
		
		for (int i = 0; i < 10000; i++);
		
		/*sliding++;
		if (sliding > BUFFER) sliding = 0;
		
		complex c;
		c.Re = val;
		c.Im = 0;
		window[sliding] = c;
		
		for (int i = 0; i < BUFFER; i++)
			data[i] = window[i];
		
		fft(data, BUFFER, scratch);
		
		displayValue(data[BUFFER/2].Re / 10.0f);
		*/
		//GUI::drawString(Rect(0, 0, 100, 30), BLACK, WHITE, "%f", max);
	}
}

//EOF
