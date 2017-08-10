#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "neopixel.h"

#define GET_BIT(TGT, PIN)    ((TGT) & (1 << (PIN)))
#define SET_BIT(TGT, PIN)    do { TGT |=  (1 << (PIN)); } while(0)
#define CLEAR_BIT(TGT, PIN)  do { TGT &= ~(1 << (PIN)); } while(0)
#define TOGGLE_BIT(TGT, PIN) do { TGT ^= (1 << (PIN)); } while(0)

#define NUM_LEDS    16

#define PIN_LED     PB2
#define PIN_SWITCH  PB3

typedef enum IdleStates {
    GREEN_RISING, RED_FALLING,
    BLUE_RISING, GREEN_FALLING,
    RED_RISING, BLUE_FALLING
} IdleStates;

int main(void)
{
    SET_BIT(DDRB, PIN_LED);         // Set LED pin as output
    CLEAR_BIT(DDRB, PIN_SWITCH);    // Set switch pin as input
    SET_BIT(PORTB, PIN_SWITCH);     // Enable switch pin's internal pullup

    uint16_t frame[NUM_LEDS] = {
        000, 052, 104, 156, 208,
      260, 208, 156, 104, 052, 000,     // flip middle row
        000, 52, 104, 156, 208
    };

    for(uint8_t spread=0; spread < NUM_LEDS; spread++){
        frame[spread] = frame[spread]*3;
    }
    uint8_t led_buffer[NUM_LEDS * 3];

    for(;;){
    
        _delay_us(200);


        for(uint8_t cur_led = 0; cur_led < NUM_LEDS; cur_led++) {

            frame[cur_led] = (frame[cur_led] + 1) % (6*256);

            uint8_t inner_frame = frame[cur_led] % 256;

            switch(frame[cur_led] / 256) {
                case GREEN_RISING:
                    // green goes up
                    led_buffer[cur_led * 3 + 0] = 255;
                    led_buffer[cur_led * 3 + 1] = inner_frame;
                    led_buffer[cur_led * 3 + 2] = 0;
                    break;
                case RED_FALLING:
                    // red goes down
                    led_buffer[cur_led * 3 + 0] = 255 - inner_frame;
                    led_buffer[cur_led * 3 + 1] = 255;
                    led_buffer[cur_led * 3 + 2] = 0;
                    break;
                case BLUE_RISING:
                    // blue goes up
                    led_buffer[cur_led * 3 + 0] = 0;
                    led_buffer[cur_led * 3 + 1] = 255;
                    led_buffer[cur_led * 3 + 2] = inner_frame;
                    break;
                case GREEN_FALLING:
                    // green goes down
                    led_buffer[cur_led * 3 + 0] = 0;
                    led_buffer[cur_led * 3 + 1] = 255 - inner_frame;
                    led_buffer[cur_led * 3 + 2] = 255;
                    break;
                case RED_RISING:
                    // red goes up
                    led_buffer[cur_led * 3 + 0] = inner_frame;
                    led_buffer[cur_led * 3 + 1] = 0;
                    led_buffer[cur_led * 3 + 2] = 255;
                    break;
                case BLUE_FALLING:
                    // blue goes down
                    led_buffer[cur_led * 3 + 0] = 255;
                    led_buffer[cur_led * 3 + 1] = 0;
                    led_buffer[cur_led * 3 + 2] = 255 - inner_frame;
                    break;
                default:
                    frame[cur_led] = 0;
                    break;
            }
        }

        // Apply logarithmic gamma correction
        for(uint8_t i = 0; i < NUM_LEDS * 3; i++) {
            //led_buffer[i] = lookup(led_buffer[i]);
        }

        // Dump the color buffer to the strip
        for(uint8_t i = 0; i < NUM_LEDS; i++) {
            sendPixel(led_buffer[i*3 + 0], led_buffer[i*3 + 1], led_buffer[i*3 + 2]);
        }
        show();
        
    }
}