//*******************************************************************
// STM32L1xx
//*******************************************************************

//-------------------------------------------------------------------
#include "hardware/Common/Font/Font_8x12.h"
#include "hardware/Common/Font/Font_10x20.h"
#include "hardware/Common/Font/Font_16x24.h"

//-------------------------------------------------------------------
cSystem sys;

//-------------------------------------------------------------------
cNetWin  net;
cNetUDP  udp( net );
cNetTCP  tcp( net );

cNetAddr<4> server = {127,0,0,1};

//- Timer------------------------------------------------------------
cHwTimer_MCU   timer( 10000/*us*/ );
cHwTimer      &timerPWM = timer;

//- Digital Port ----------------------------------------------------
cHwPort_Virtual port( server,1000);

//- Analog Output ---------------------------------------------------
cHwDAC_Virtual dac0( server, 1002);
cHwDAC         &dac = dac0;
//- Analog Input ----------------------------------------------------
cHwADC_Virtual adc0( server, 1002, &timer);
cHwADC         &adc = adc0;
//- Display ---------------------------------------------------------
cHwDispChar_Virtual dispHw( server, 1001 );
cDevDisplayChar     disp  ( dispHw );

//- Joystick --------------------------------------------------------
cDevDigital    btnRight( port, 2,cDevDigital::In, 0 );
cDevDigital    btnLeft ( port, 0,cDevDigital::In, 0 );
cDevDigital    btnCtrl ( port, 1,cDevDigital::In, 0 );

cDevControlEncoderJoystick enc( &btnLeft, &btnRight, &btnCtrl, &timer, 120 );

//- LED -------------------------------------------------------------
cDevDigital  led( port, 8, cDevDigital::Out, 0 );

//EOF
