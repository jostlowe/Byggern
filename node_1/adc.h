#define SLIDER_L 1
#define SLIDER_R 2
#define JOYSTICK_Y 3
#define JOYSTICK_X 4

void adc_init(void);
int adc_read_channel(unsigned int channel);
