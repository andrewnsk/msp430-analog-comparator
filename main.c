#include <msp430g2553.h> 

#define RED_LED    BIT0
#define AIN1    BIT1


char flash_led = 0;                        



void main(void) {
        
        WDTCTL = WDTPW + WDTHOLD;       // отключаем WatchDog

        P1OUT = 0;
        P1DIR = RED_LED;                   //  P1.0
        
CACTL1 = CAREF_1 + CARSEL + CAIE; 	/* опорное напряжение 0.25 Vcc,
                                	инвертирующий вход,
                                 	разрешаем прерывание
					*/

CACTL2 = P2CA4 + CAF; /* неинвертирующий  вход CA1, выход фильтруется

			отключаем цифровой блок на P1.1 */
CAPD = AIN1;         

        
        TACCR0 = 6000;   // Задержка таймера для мигания светодиодом, 6000 циклов
        TACCTL0 = CCIE;   // разрешаем прерывание таймер по достижении CCR0
        TACTL = TASSEL_2 + ID_3 + MC_1 + TACLR; // SMCLK, делитель 8,
                                                // прямой счёт, обнуление таймера
        CACTL1 |= CAON;           // включаем компаратор        
        _BIS_SR(LPM0_bits + GIE); // включаем режим экономии  LPM0 и разрешаем прерывания

} // main

/*  Обработчики прерываний  */

#pragma vector = TIMER0_A0_VECTOR
__interrupt void CCR0_ISR(void) {
        P1OUT ^= flash_led;                 // если flash_led == 0, светодиод не горит
                                        // если flash_led == RED_LED, мигаем светодиодом
} // CCR0_ISR

#pragma vector = COMPARATORA_VECTOR
__interrupt void COMPA_ISR(void) {
  if ((CACTL2 & CAOUT)==0x01) {
    CACTL1 |= CAIES;    // значение высокое, ждем спадания фронта
    flash_led = RED_LED;       // разрешаем светодиоду мигать
  }
  else {
    CACTL1 &= ~CAIES;   // значение низкое, ждем возрастания фронта 
    flash_led = 0;          // отключаем светодиод
    P1OUT = 0;          // обнуляем выход порта P1
  }  
} // COMPA_ISR
