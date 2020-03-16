
#define OS_STACK_SIZE 256
#define OS_RESERVED_MEMORY 512
#define OS_THREADS_MAXCOUNT (2048-OS_RESERVED_MEMORY)/OS_STACK_SIZE
#define OS_REINITTIMER() TCNT2=0x06
#define OS_PUSH_ALL() asm volatile(\
  "push r31\n\t"\
  "push r30\n\t"\
  "push r29\n\t"\
  "push r28\n\t"\
  "push r27\n\t"\
  "push r26\n\t"\
  "push r25\n\t"\
  "push r24\n\t"\
  "push r23\n\t"\
  "push r22\n\t"\
  "push r21\n\t"\
  "push r20\n\t"\
  "push r19\n\t"\
  "push r18\n\t"\
  "push r17\n\t"\
  "push r16\n\t"\
  "push r15\n\t"\
  "push r14\n\t"\
  "push r13\n\t"\
  "push r12\n\t"\
  "push r11\n\t"\
  "push r10\n\t"\
  "push r9\n\t"\
  "push r8\n\t"\
  "push r7\n\t"\
  "push r6\n\t"\
  "push r5\n\t"\
  "push r4\n\t"\
  "push r3\n\t"\
  "push r2\n\t"\
  "push r1\n\t"\
  "push r0\n\t"\
  "in r16, __SREG__\n\t"\
  "push r16\n\t"\
  )

#define OS_POP_ALL() asm volatile(\
  "pop r16\n\t"\
  "out __SREG__, r16\n\t"\
  "pop r0\n\t"\
  "pop r1\n\t"\
  "pop r2\n\t"\
  "pop r3\n\t"\
  "pop r4\n\t"\
  "pop r5\n\t"\
  "pop r6\n\t"\
  "pop r7\n\t"\
  "pop r8\n\t"\
  "pop r9\n\t"\
  "pop r10\n\t"\
  "pop r11\n\t"\
  "pop r12\n\t"\
  "pop r13\n\t"\
  "pop r14\n\t"\
  "pop r15\n\t"\
  "pop r16\n\t"\
  "pop r17\n\t"\
  "pop r18\n\t"\
  "pop r19\n\t"\
  "pop r20\n\t"\
  "pop r21\n\t"\
  "pop r22\n\t"\
  "pop r23\n\t"\
  "pop r24\n\t"\
  "pop r25\n\t"\
  "pop r26\n\t"\
  "pop r27\n\t"\
  "pop r28\n\t"\
  "pop r29\n\t"\
  "pop r30\n\t"\
  "pop r31\n\t")

struct Tcontext{
	byte spl,sph;
	void (*thr)(void);
};

byte OS_THREADS_COUNT = 0; 
volatile struct Tcontext OS_thr[OS_THREADS_MAXCOUNT];

byte OS_index_th = 0;

void OS_ADD_THREAD(void (*callback)(void));
void OS_DEL_THREAD(void (*callback)(void));
void OS_START(void);
void OS_STOP(void);

ISR(TIMER2_OVF_vect)__attribute__((naked));

inline void OS_INITTIMER2(void); 

void OS_START(void){
  cli();	
  OS_THREADS_COUNT = 0;
  OS_INITTIMER2();
  sei();
}

void OS_STOP(void){
  OS_THREADS_COUNT = 0;
  if(OS_index_th==0) TIMSK2=(0<<OCIE2B) | (0<<OCIE2A) | (0<<TOIE2);
}

void OS_ADD_THREAD(void (*callback)(void)){
  word w =0;
  byte tspl, tsph;
  byte *stackByte;
  byte tempcount = OS_THREADS_COUNT+1;
  if(tempcount<OS_THREADS_MAXCOUNT){
    OS_thr[tempcount].thr = callback;
    cli();
		w =RAMEND - tempcount*OS_STACK_SIZE; 
		stackByte = w;
		*stackByte =((word)OS_thr[tempcount].thr)&0xff;
		w--;
		stackByte = w;
		*stackByte =(((word)OS_thr[tempcount].thr)>>8)&0xff;
		w--;
		OS_thr[tempcount].spl = w&0xff;
		OS_thr[tempcount].sph = (w>>8)&0xff;
		tspl = SPL;
		tsph = SPH;
		SPL = OS_thr[tempcount].spl;
		SPH = OS_thr[tempcount].sph;
		OS_PUSH_ALL();
		OS_thr[tempcount].spl = SPL;
		OS_thr[tempcount].sph = SPH;
		SPL = tspl;
		SPH = tsph;
    OS_THREADS_COUNT++;
    sei();
  }
}

void OS_DEL_THREAD(void (*callback)(void)){
 cli();
  for(byte i = 1; i<=OS_THREADS_COUNT; i++){
    if(OS_thr[i].thr==callback){
      if(OS_index_th==i){
        
      }else{
        if(OS_index_th==OS_THREADS_COUNT){
          
        }else{
          
        }
      }
      break;
    }
  }
 sei();
}


void OS_INITTIMER2(void){
// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: 250,000 kHz
// Mode: Normal top=0xFF
// OC2A output: Disconnected
// OC2B output: Disconnected
// Timer Period: 1 ms
ASSR=(0<<EXCLK) | (0<<AS2);
TCCR2A=(0<<COM2A1) | (0<<COM2A0) | (0<<COM2B1) | (0<<COM2B0) | (0<<WGM21) | (0<<WGM20);
TCCR2B=(0<<WGM22) | (1<<CS22) | (0<<CS21) | (0<<CS20);
OS_REINITTIMER();
OCR2A=0x00;
OCR2B=0x00;
// Timer/Counter 2 Interrupt(s) initialization
TIMSK2=(0<<OCIE2B) | (0<<OCIE2A) | (1<<TOIE2);
}



ISR(TIMER2_OVF_vect){
  OS_PUSH_ALL();
  OS_REINITTIMER();
      OS_thr[OS_index_th].spl = SPL;
      OS_thr[OS_index_th].sph = SPH;
      OS_index_th++;
      if(OS_index_th>OS_THREADS_COUNT) OS_index_th = 0;
      SPL = OS_thr[OS_index_th].spl;
      SPH = OS_thr[OS_index_th].sph;
  OS_POP_ALL();
  asm volatile("reti\n\t");
}
