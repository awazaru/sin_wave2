/*H27 7/31
 *Sin波生成
 *Atmega328p使用
 */
#include <avr/io.h>
#include <math.h>
#include <stdlib.h>
#include <avr/interrupt.h>
//#include <util/delay.h>

#define PI 3.14

/*関数宣言*/
void sin_gen(void);

/*変数宣言*/
uint8_t timer1_count = 0;/*16bitタイマ用カウント変数*/
uint16_t max_timer1_count = 1000;//1KHz
uint16_t  half_timer1_count = 0;
//uint8_t OCR0B_up  = 1;/*OCR0Aを増加させたいときは1を減少させたいときは0を入れる*/
double   rad = 0;//deg * PI / 180.0;
float sinwave = 0;
//float abs_sin =0;

/*関数宣言*/
/*void usart_tx(unsigned char data){//送信用関数
 while( !(UCSR0A & (1<<UDRE0)) );        //送信ﾊﾞｯﾌｧ空き待機
 UDR0 = data;
 }
 unsigned char usart_rx(void){//受信用関数
 while( !(UCSR0A & (1<<RXC0)) );                //受信完了待機
 return UDR0;                                //受信ﾃﾞｰﾀ返す
 }
 void puts_tx(char *str){//文字列送信用関数
 while( *str != 0x00 ){ //nullになるまで実行
 usart_tx(*str);
 str++;                                    //ｱﾄﾞﾚｽ+1
 }
 }
 void serial_ini(){// シリアル通信設定
 UBRR0 = MYUBRR;
 UCSR0A=0b00000000;//受信すると10000000 送信有効になると00100000
 UCSR0B|=0b00011000;//送受信有効
 UCSR0C=0b00000110;//データ8bit、非同期、バリティなし、stop1bit
 }
 */

/*高速PWMではTOP値がOCR0A、比較値がOCR0Bとなるので注意*/

void timer_ini(){//タイマー設定
    /*単位タイマ　 100us*/
    TCCR1A = 0;
    TCCR1B|=_BV(WGM13)|_BV(WGM12)|_BV(CS00);
    /*WGM13 WGM12 WGM11 WGM10: 1100 比較一致タイマ/CTC動作 ICR1
     *CS12 CS11 CS10 : 001 分周なし*/
    ICR1 = 99;/*0.0001s*/
    TIMSK1|=_BV(ICIE1);/*タイマ/カウンタ1比較A割り込み許可*/
    
    /*8bitタイマ1 PWM用*/
    TCCR0A|=_BV(COM0B1)|_BV(WGM01)|_BV(WGM00);
    /*COM0B1 COM0B0 : 10 比較一致でLOW,OC0Bピンに出力
     *(WGM02) WGM01 WGM00 : 111 高速PWM動作,TOPはOCR0A
     */
    TCCR0B|=_BV(WGM02)|_BV(CS00);
    /*CS02 CS01 CS00:001 クロック源=CLKi/o 前置分周なし
     */
    //TIMSK0|=_BV(TOIE0);
    /* TOIE0 :1     タイマ/カウンタ0溢れ割り込み許可
     */
    OCR0A = 99;//TOP
    /*10000HzのPWM周波数, 0.0001s
     */
    OCR0B = 50;//比較
}

/*単位タイマ
*10usで1回カウント
 */

ISR (TIMER1_CAPT_vect){
    if(timer1_count>max_timer1_count)
        timer1_count = 0;
    timer1_count++;
    sin_gen();
}

void pin_ini(){//ピン設定
    DDRD = 0b01100000;//PD6(OCR0A)を出力設定
    PORTD =0b00000000;
}

void sin_gen(void){
    //half_timer1_count = max_timer1_count*0.5;
    if(timer1_count<=half_timer1_count){
        rad =  (timer1_count*180.0/half_timer1_count) * PI / 180.0;
        sinwave = sin(rad);
        //abs_sin = abs(sinwave);
        OCR0B = 50 + 40 * sinwave;
    }else if (timer1_count>half_timer1_count){
        rad =  ((timer1_count-half_timer1_count)*180.0/half_timer1_count) * PI / 180.0;
        sinwave = sin(rad);
        //abs_sin = abs(sinwave);
        OCR0B = 50 - 40 * sinwave;
    }
}


/*メイン関数*/
int main(void){
    
    timer_ini();
    pin_ini();
    
   half_timer1_count = max_timer1_count/2.0;
    sei();//割り込み許可
    
    for(;;){
     //   sin_gen();
    }
    return 0;
}
