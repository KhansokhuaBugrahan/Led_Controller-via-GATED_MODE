#include <stdint.h>
#include "stm32f4xx.h"
#include <stdbool.h>
#include "gpio.h"
#include "armcortexm4.h"
#include "timer.h"





void TIM2_IRQHandler(void)
{
	static bool flagTrigger = false;
    if (TIMER2->TIMx_SR & (1 << 6))  // Check if trigger occurred
    {
    	 TIMER2->TIMx_SR = ~(1 << 6);  // Clear trigger flag



    	 if(flagTrigger)
    	 {
    		 gpioWritePin(GPIOD, LED_BLUE, GPIO_PIN_RESET );
    		 gpioWritePin(GPIOD, LED_ORANGE, GPIO_PIN_RESET );
    		 gpioWritePin(GPIOD, LED_RED, GPIO_PIN_RESET );
    		 TIMER2->TIMx_CNT = 0; // reset counter

    		 flagTrigger = false;

    		 TIMER2->TIMx_SR = ~(1 << 1);  // Clear CC1 flag
   		 	 TIMER2->TIMx_SR = ~(1 << 2);  // Clear CC2 flag

   		 	 TIMER2->TIMx_DIER |= (0b111 << 0); // UEV enable CC1 enable CC2 enable again
    	 }
    	 else
    	 {
    		 gpioWritePin(GPIOD, LED_ORANGE, GPIO_PIN_SET );
    		 flagTrigger = true;
    	 }


    }
    else if (TIMER2->TIMx_SR & (1 << 1))  //  CC1 interrupt
       {
       	 TIMER2->TIMx_SR = ~(1 << 1);  //    Clear  flag

       	 gpioWritePin(GPIOD, LED_RED, GPIO_PIN_SET );
		 gpioWritePin(GPIOD, LED_BLUE, GPIO_PIN_RESET );
		 gpioWritePin(GPIOD, LED_ORANGE, GPIO_PIN_RESET );
       }
    else if (TIMER2->TIMx_SR & (1 << 2))  //  CC2 interrupt
	   {
		 TIMER2->TIMx_SR = ~(1 << 2);  // Clear  flag


       	 gpioWritePin(GPIOD, LED_RED, GPIO_PIN_RESET );
		 gpioWritePin(GPIOD, LED_BLUE, GPIO_PIN_SET );
		 gpioWritePin(GPIOD, LED_ORANGE, GPIO_PIN_RESET );

		 TIMER2->TIMx_DIER &= ~(0b111 << 0); // UEV disable CC1 disable CC2 disable

	   }
}
int main(void)
{


	 	 	 	 	 GPIO_Handle_t orangeLed = {  .PORTNAME = GPIOD,
												.PINCONF.PIN = LED_ORANGE,
												.PINCONF.MODE = GPIO_MODE_OUTPUT,
												.PINCONF.OTYPE = GPIO_OTYPE_PP,
												.PINCONF.OSPEED = GPIO_OSPEED_HIGH,
												.PINCONF.PUPD = GPIO_PUPD_PU,
												.PINCONF.AF = AFNO
											  };


	 				GPIO_Handle_t redLed = {    .PORTNAME = GPIOD,
	 											.PINCONF.PIN = LED_RED,
	 											.PINCONF.MODE = GPIO_MODE_OUTPUT,
	 											.PINCONF.OTYPE = GPIO_OTYPE_PP,
	 											.PINCONF.OSPEED = GPIO_OSPEED_HIGH,
	 											.PINCONF.PUPD = GPIO_PUPD_PU,
	 											.PINCONF.AF = AFNO
											  };
	 				GPIO_Handle_t blueLed = {   .PORTNAME = GPIOD,
												.PINCONF.PIN = LED_BLUE,
												.PINCONF.MODE = GPIO_MODE_OUTPUT,
												.PINCONF.OTYPE = GPIO_OTYPE_PP,
												.PINCONF.OSPEED = GPIO_OSPEED_HIGH,
												.PINCONF.PUPD = GPIO_PUPD_PU,
												.PINCONF.AF = AFNO
											  };
	 	 	 	 	 gpioInit(&orangeLed);
	 	 	 	 	 gpioInit(&redLed);
	 	 	 	 	 gpioInit(&blueLed);

			   GPIO_Handle_t CCP_TIMER2_CH1_PA0 = {  .PORTNAME = GPIOA,
													.PINCONF.PIN = GPIO_PIN_0,
													.PINCONF.MODE = GPIO_MODE_ALTARNATE,
													.PINCONF.OTYPE = GPIO_OTYPE_PP,
													.PINCONF.OSPEED = GPIO_OSPEED_HIGH,
													.PINCONF.PUPD = GPIO_PUPD_PD ,
													.PINCONF.AF = AF1
												   };



				gpioInit(&CCP_TIMER2_CH1_PA0); // user built-in push button



				timerxConfig(TIMER2, 16000, 15000); // 1 tick 1ms

				TIMER2->TIMx_SMCR |= (0b111 << 4); //  External Trigger input (ETRF)
				TIMER2->TIMx_SMCR |= (0b101 << 0); //gated mode
				TIMER2->TIMx_SMCR |= (1u << 7); // master-slave mode
				TIMER2->TIMx_CCR1 = 5000; // 5 sec   capture at 5th second
				TIMER2->TIMx_CCR2 = 10000; // 10 sec capture at 10th second.

				TIMER2->TIMx_DIER |= (1u << 1); // CC1 enable
				TIMER2->TIMx_DIER |= (1u << 2); // CC2 enable
				TIMER2->TIMx_DIER |= (1u << 6); // trigger enable

				TIMER2->TIMx_SMCR |= (0b1111 << 8); // trigger filter    fSAMPLING = fDTS / 32,  N = 8

				timerxPeripheralEnable(TIMER2);
				timer2InterruptEnable();


				while (1){}


}
				/*
				   Gate mode, as long as PA0 button is held down, the function will keep operating.
				   In other words, it will keep doing the same thing unless PA0 pin state  changes.

				   STM Reference Manual
				   101: Gated mode - The counter clock is enabled when the trigger input (TRGI) is high. The
					counter stops (but is not reset) as soon as the trigger becomes low. Both start and stop of
					the counter are controlled.

					Since as soon as the button is pressed, it will immediately blink the orange LED and starts counting.
					After 5 second passed, only red LED will blink,
					After 5 more second, only blue Led will blink unless PA0 pin state changes.

					If you release the button at any of the stages above( counter stops counting and we reset it),
					the program will reset itself and it will repeat the functions in the next cycle.
				 */


				/*
				   If trigger interrupt and (CC1 or CC2) capture interrupt are set simultenaously,
				   when I release the button.
				  Then red or blue LED may remain high.
				  Thus, I will clear the flag in regard of both in SR register inside the first "if" block.
				  It will prevent facing retained set flags and remained high LED from previous event in the next cycle.
				 */

				/*
				  Also, I am going to configure digital filter to prevent debouncing for the button.
				  I use 16 MHz timer clock and use the most robust choice among the SMCR (more sampling with a longer period).
				  You can increase the period for each sampling by using CKD bits in CR1 register, for me it is default 00.
				  Moreover, you can use an external button and can make a proper hardware setup with RC filter.
				 */

				/*
				   Also,  after each overflow CC1 and CC2 will be triggered again and again.
				   I need to prevent it. I will disable the update event and related CCx interrupts(CC1 and CC2)
				   and I will re-enable them when I release the button.
				 */




