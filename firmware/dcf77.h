extern volatile unsigned char timecount;
extern volatile unsigned short breakcount;
extern volatile unsigned char dcf_active;
extern volatile unsigned char array_count;
extern volatile unsigned char dcf_array[60];
extern volatile unsigned char dcf_min_l_old;
extern volatile unsigned char dcf_min_h_old;
extern volatile unsigned char dcf_std_l_old;
extern volatile unsigned char dcf_std_h_old;

#define TIMER_PRESET (65536 - 15625)


void dcf77_exec();

unsigned char plausibilitaetscheck ( unsigned char std_new, unsigned char std_old,
									 unsigned char min_new, unsigned char min_old);

void check_time_flip();
void check_gong();

ISR(INT0_vect);
ISR(TIMER0_OVF_vect);
ISR(TIMER1_OVF_vect);



