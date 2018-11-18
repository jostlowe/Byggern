//#define F_CPU 16000000UL
#include "node_setup.h"
#include "uart_driver.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "can.h"
#include "MCP2515.h"
#include "spi.h"
#include "pwm.h"
#include "TWI_Master.h"
#include "dac.h"
#include "motor_driver.h"
#include "encoder.h"
#include "adc.h"
#include "timer.h"
#include "score_counter.h"
#include "analog_controller.h"

#define JOYSTICK 1
#define TOUCH 2
#define RESET_NODE_2 3
uint8_t wolol = 0;
uint8_t run_motor_init = 1;

uint8_t RUN_SPEED_CONTROL;

void main(){
    write_bit(1, DDRB, PB4);
    write_bit(1, PORTB, PB4);
    adc_init();
    uart_init();
    PWM_init();

    sei();
    DAC_init();

    motor_init();
    SPI_init();
    MCP_init();
    CAN_init();
    timer_init();


    solenoid_init();
    score_counter_init();
    analog_controller_init();
    while(1){
      //cli();
      //printf("Score: \r\n",score_counter_get());
      //printf("Score: %d\r\n", score_counter_get());

      //printf("Button: %d Up: %d Right: %d Down: %d Left: %d\r\n", analog_controller_get_button(), analog_controller_get_up(), analog_controller_get_right(), analog_controller_get_down(), analog_controller_get_left());
      //analog_speed_control();
      //score_counter_update();
      _delay_ms(2);
  }
}

uint8_t position_reference;

ISR(INT2_vect){
    Can_block received_can_block = CAN_recieve(JOYSTICK);
    switch (received_can_block.data[0]) {
        case JOYSTICK:{

            int x_pos = received_can_block.data[1];
            int y_pos = received_can_block.data[2];
            int joystick_button = received_can_block.data[3];
            //printf("X-position: \t%d | Y-position: %d\r\n",x_pos, y_pos);
            PWM_set_angle(x_pos);
            if (joystick_button == 1){
                score_counter_reset();
            }

            //motor_set_speed_from_joystick(x_pos);
            //solenoid_punch(received_can_block.data[3]);
            break;
        }
        case TOUCH :{
            int l_slider = received_can_block.data[1];
            int r_slider = received_can_block.data[2];
            position_reference = l_slider;
            //PWM_set_angle(r_slider);
            solenoid_punch(received_can_block.data[3]);
            Can_block score = {1, 2, {score_counter_get()}};
            CAN_send(&score);
            break;
        }
        case RESET_NODE_2 :{
            write_bit(0, PORTB, PB4);
            break;
        }
        case 4:{
          RUN_SPEED_CONTROL = 1;
          printf("Test\r\n");
          break;
        }
        case 5:{
          RUN_SPEED_CONTROL = 0;
          printf("Test2\r\n");
          break;
        }
        default:
            break;
    }

}


ISR(TIMER3_COMPA_vect){
    if(!RUN_SPEED_CONTROL){
      position_regulator(position_reference);

    }
    else{
      analog_speed_control();
      printf("testtest\r\n" );
    }
    TCNT3 = 0x0000;

}
