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

    uint16_t frame = 0;

    for(;;)
    {
        _delay_ms(1);

        frame = (frame + 1) % (6*256);

        uint8_t inner_frame = frame % 256;

        switch(frame / 256) {
            case GREEN_RISING:
                // green goes up
                showColor(255, inner_frame, 0);
                break;
            case RED_FALLING:
                // red goes down
                showColor(255 - inner_frame, 255, 0);		
                break;
            case BLUE_RISING:
                // blue goes up
                showColor(0, 255, inner_frame);
                break;
            case GREEN_FALLING:
                // green goes down
                showColor(0, 255 - inner_frame, 255);
                break;
            case RED_RISING:
                // red goes up
                showColor(inner_frame, 0, 255);
                break;
            case BLUE_FALLING:
                // blue goes down
                showColor(255, 0, 255 - inner_frame);
                break;
            default:
                frame = 0;
                break;
        }
    }
}