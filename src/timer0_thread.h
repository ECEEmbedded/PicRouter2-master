typedef struct __timer0_thread_struct {
	// "persistent" data for this "lthread" would go here
	int	data;
} timer0_thread_struct;

#define COLOR 1
#define IR 2
#define ENCODERS 3

int timer0_lthread(timer0_thread_struct *,int,int,unsigned char*);
