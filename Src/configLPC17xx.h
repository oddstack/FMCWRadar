//*******************************************************************
// LPC17xx
//*******************************************************************

//-------------------------------------------------------------------
#include "Hardware/Common/Font/Font_8x12.h"

//-------------------------------------------------------------------
cSystem sys;

//-------------------------------------------------------------------
cHwPinConfig::MAP cHwPinConfig::table[]
  = {
      // ADC
      AD0_0_P0_23,
      AD0_1_P0_24,
      AD0_2_P0_25,
      AD0_3_P0_26,
      AD0_4_P1_30,
      AD0_5_P1_31,
      AD0_6_P0_3,
      AD0_7_P0_2,

      // DAC
      AOUT_P0_26,

      // SPI
      SCK1_P0_7,
      MISO1_P0_8,
      MOSI1_P0_9,

      // Timer PWM
      PWM1_1_P2_0,
      PWM1_2_P2_1,
      PWM1_3_P2_2,
      PWM1_4_P2_3,
      PWM1_5_P2_4,
      PWM1_6_P2_5,

      // UART
      TXD1_P2_0,
      RXD1_P2_1,
			
			// I2C
			SDA2_P0_10,
			SCL2_P0_11,

      END_OF_TABLE
  };

//- Timer------------------------------------------------------------
cHwTimer_N  timer   ( cHwTimer_N::TIM_0,  1000/*us*/ );
cHwTimer_N  timerPWM( cHwTimer_N::TIM_PWM, 100/*us*/ );

//- Digital Port ----------------------------------------------------
cHwPort_N   port0( cHwPort_N::P0 );
cHwPort_N   port1( cHwPort_N::P1 );
cHwPort_N   port4( cHwPort_N::P4 );

//- Analog Output ---------------------------------------------------
cHwDAC_0    dac0;
cHwDAC      &dac = dac0;

//- Analog Input ----------------------------------------------------
cHwADC_0    adc0( &timer );
cHwADC      &adc = adc0;

//- Display ---------------------------------------------------------
#if defined TERMINAL
  cHwUART_N            uart        ( cHwUART_N::UART_1,
                                     cHwUART_N::BR_115200, 100, 100 );
  cHwPort_Terminal     portTerm    ( uart, 0, &timer);
  cHwDisp_Terminal     dispHw      ( uart );
#else
  cHwSPImaster_1       spi1        ( cHwSPImaster::CR_8000kHz,
                                     cHwSPImaster::CPOL_H_CPHA_H );
  cHwSPImaster::Device spiDisplay  ( spi1, port0, 6 );
  cHwPort::Pin         pinBackLight( port4, 28 );
  cHwDisp_SPFD5408Bspi dispHw      ( spiDisplay,
                                     pinBackLight ,
                                     fontFont_8x12, // font
                                     2 );           // actual zoom factor		
	cDevDisplayChar  disp( dispHw );						


	cHwI2Cmaster_N       i2c_N( cHwI2Cmaster_N::I2C_2, 
                            cHwI2Cmaster::CR_100kHz );

	cHwTouch_STMPE811i2c touch( i2c_N, 0, 320, 240);
#endif

//- Joystick --------------------------------------------------------
#if defined TERMINAL
  cDevDigital  btnRight( portTerm, 1,cDevDigital::In, 0 );
  cDevDigital  btnLeft ( portTerm, 0,cDevDigital::In, 0 );
  cDevDigital  btnCtrl ( portTerm, 4,cDevDigital::In, 0 );
#else
  cDevDigital  btnRight( port1, 24, cDevDigital::In, 1 );
  cDevDigital  btnLeft ( port1, 26, cDevDigital::In, 1 );
  cDevDigital  btnCtrl ( port1, 20, cDevDigital::In, 1 );
#endif

cDevControlEncoderJoystick  enc( &btnLeft, &btnRight, &btnCtrl, &timer, 200 );

//- LED -------------------------------------------------------------
cDevDigital  led( port1, 31, cDevDigital::Out, 0 );

//*******************************************************************
