#ifndef LIB_ADAFRUIT_AMG88XX_H
#define LIB_ADAFRUIT_AMG88XX_H

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define AMG88xx_ADDRESS                (0x69)
/*=========================================================================*/

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
    enum
    {
        AMG88xx_PCTL = 0x00,
		AMG88xx_RST = 0x01,
		AMG88xx_FPSC = 0x02,
		AMG88xx_INTC = 0x03,
		AMG88xx_STAT = 0x04,
		AMG88xx_SCLR = 0x05,
		//0x06 reserved
		AMG88xx_AVE = 0x07,
		AMG88xx_INTHL = 0x08,
		AMG88xx_INTHH = 0x09,
		AMG88xx_INTLL = 0x0A,
		AMG88xx_INTLH = 0x0B,
		AMG88xx_IHYSL = 0x0C,
		AMG88xx_IHYSH = 0x0D,
		AMG88xx_TTHL = 0x0E,
		AMG88xx_TTHH = 0x0F,
		AMG88xx_INT_OFFSET = 0x010,
		AMG88xx_PIXEL_OFFSET = 0x80
    };
	
	enum power_modes{
		AMG88xx_NORMAL_MODE = 0x00,
		AMG88xx_SLEEP_MODE = 0x01,
		AMG88xx_STAND_BY_60 = 0x20,
		AMG88xx_STAND_BY_10 = 0x21
	};
	
	enum sw_resets {
		AMG88xx_FLAG_RESET = 0x30,
		AMG88xx_INITIAL_RESET = 0x3F
	};
	
	enum frame_rates {
		AMG88xx_FPS_10 = 0x00,
		AMG88xx_FPS_1 = 0x01
	};
	
	enum int_enables{
		AMG88xx_INT_DISABLED = 0x00,
		AMG88xx_INT_ENABLED = 0x01
	};
	
	enum int_modes {
		AMG88xx_DIFFERENCE = 0x00,
		AMG88xx_ABSOLUTE_VALUE = 0x01
	};
	
/*=========================================================================*/

#define AMG88xx_PIXEL_ARRAY_SIZE 64
#define AMG88xx_PIXEL_TEMP_CONVERSION .25
#define AMG88xx_THERMISTOR_CONVERSION .0625

/**************************************************************************/
/*! 
    @brief  Class that stores state and functions for interacting with AMG88xx IR sensor chips
*/
/**************************************************************************/
class Adafruit_AMG88xx {
	public:
		//constructors
		Adafruit_AMG88xx(void) {};
		~Adafruit_AMG88xx(void) {};
		
		bool begin(uint8_t addr = AMG88xx_ADDRESS);
		
		void readPixels(float *buf, uint8_t size = AMG88xx_PIXEL_ARRAY_SIZE);
		float readThermistor();
		
		void setMovingAverageMode(bool mode);
		
		void	  enableInterrupt();
		void	  disableInterrupt();
		void	  setInterruptMode(uint8_t mode);
		void	  getInterrupt(uint8_t *buf, uint8_t size = 8);
		void	  clearInterrupt();
				
		//this will automatically set hysteresis to 95% of the high value
		void	  setInterruptLevels(float high, float low);
				
		//this will manually set hysteresis
		void	  setInterruptLevels(float high, float low, float hysteresis);
		
	private:
		uint8_t _i2caddr;
		
		void      write8(byte reg, byte value);
		void      write16(byte reg, uint16_t value);
        uint8_t   read8(byte reg);
		
		void read(uint8_t reg, uint8_t *buf, uint8_t num);
		void write(uint8_t reg, uint8_t *buf, uint8_t num);
		void _i2c_init();
		
		float signedMag12ToFloat(uint16_t val);
		
		 // The power control register
        struct pctl {
            // 0x00 = Normal Mode
			// 0x01 = Sleep Mode
			// 0x20 = Stand-by mode (60 sec intermittence)
			// 0x21 = Stand-by mode (10 sec intermittence)
           
            uint8_t PCTL : 8;

            uint8_t get() {
                return PCTL;
            }
        };
        pctl _pctl;
		
		//reset register
		struct rst {
			//0x30 = flag reset (all clear status reg 0x04, interrupt flag and interrupt table)
			//0x3F = initial reset (brings flag reset and returns to initial setting)
			
			uint8_t RST : 8;
			
			uint8_t get() {
				return RST;
			}
		};
		rst _rst;
		
		//frame rate register
		struct fpsc {
			
			//0 = 10FPS
			//1 = 1FPS
			uint8_t FPS : 1;
			
			uint8_t get() {
				return FPS & 0x01;
			}
		};
		fpsc _fpsc;
		
		//interrupt control register
		struct intc {
			
			// 0 = INT output reactive (Hi-Z)
			// 1 = INT output active
			uint8_t INTEN : 1;
			
			// 0 = Difference interrupt mode
			// 1 = absolute value interrupt mode
			uint8_t INTMOD : 1;
			
			uint8_t get(){
				return (INTMOD << 1 | INTEN) & 0x03;
			}
		};
		intc _intc;
		
		//status register
		struct stat {
			uint8_t unused : 1;
			//interrupt outbreak (val of interrupt table reg)
			uint8_t INTF : 1;
			
			//temperature output overflow (val of temperature reg)
			uint8_t OVF_IRS : 1;
			
			//thermistor temperature output overflow (value of thermistor)
			uint8_t OVF_THS : 1;
			
			uint8_t get(){
				return ( (OVF_THS << 3) | (OVF_IRS << 2) | (INTF << 1) ) & 0x07;
			}
		};
		stat _stat;
		
		//status clear register
		//write to clear overflow flag and interrupt flag
		//after writing automatically turns to 0x00
		struct sclr {
			uint8_t unused : 1;
			//interrupt flag clear
			uint8_t INTCLR : 1;
			//temp output overflow flag clear
			uint8_t OVS_CLR : 1;
			//thermistor temp output overflow flag clear
			uint8_t OVT_CLR : 1;
			
			uint8_t get(){
				return ((OVT_CLR << 3) | (OVS_CLR << 2) | (INTCLR << 1)) & 0x07;
			}
		};
		sclr _sclr;
		
		//average register
		//for setting moving average output mode
		struct ave {
			uint8_t unused : 5;
			//1 = twice moving average mode
			uint8_t MAMOD : 1;
			
			uint8_t get(){
				return (MAMOD << 5);
			}
		};
		struct ave _ave;
		
		//interrupt level registers
		//for setting upper / lower limit hysteresis on interrupt level
		
		//interrupt level upper limit setting. Interrupt output
		// and interrupt pixel table are set when value exceeds set value
		struct inthl {
			uint8_t INT_LVL_H : 8;
			
			uint8_t get(){
				return INT_LVL_H;
			}
		};
		struct inthl _inthl;
		
		struct inthh {
			uint8_t INT_LVL_H : 4;
			
			uint8_t get(){
				return INT_LVL_H;
			}
		};
		struct inthh _inthh;
		
		//interrupt level lower limit. Interrupt output
		//and interrupt pixel table are set when value is lower than set value
		struct intll {
			uint8_t INT_LVL_L : 8;
			
			uint8_t get(){
				return INT_LVL_L;
			}
		};
		struct intll _intll;
		
		struct intlh {
			uint8_t INT_LVL_L : 4;
			
			uint8_t get(){
				return (INT_LVL_L & 0xF);
			}
		};
		struct intlh _intlh;
		
		//setting of interrupt hysteresis level when interrupt is generated.
		//should not be higher than interrupt level
		struct ihysl {
			uint8_t INT_HYS : 8;
			
			uint8_t get(){
				return INT_HYS;
			}
		};
		struct ihysl _ihysl;
		
		struct ihysh {
			uint8_t INT_HYS : 4;
			
			uint8_t get(){
				return (INT_HYS & 0xF);
			}
		};
		struct ihysh _ihysh;
		
		//thermistor register
		//SIGNED MAGNITUDE FORMAT
		struct tthl {
			uint8_t TEMP : 8;
			
			uint8_t get(){
				return TEMP;
			}
		};
		struct tthl _tthl;
		
		struct tthh {
			uint8_t TEMP : 3;
			uint8_t SIGN : 1;
			
			uint8_t get(){
				return ( (SIGN << 3) | TEMP) & 0xF;
			}
		};
		struct tthh _tthh;
		
		//temperature registers 0x80 - 0xFF
		/*
		//read to indicate temperature data per 1 pixel
		//SIGNED MAGNITUDE FORMAT
		struct t01l {
			uint8_t TEMP : 8;
			
			uint8_t get(){
				return TEMP;
			}
		};
		struct t01l _t01l;
		
		struct t01h {
			uint8_t TEMP : 3;
			uint8_t SIGN : 1;
			
			uint8_t get(){
				return ( (SIGN << 3) | TEMP) & 0xF;
			}
		};
		struct t01h _t01h;
		*/
		
		
};

#endif
