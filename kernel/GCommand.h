#include <linux/slab.h>
//#include <linux/gfp.h>

#define XStep 19
#define XDir  13
#define XMin  6
#define XMax  5

#define YStep 16
#define YDir  12
#define YMin  7
#define YMax  8

#define ZStep 11
#define ZDir  9
#define ZMin  10
#define ZMax  22

#define Spdl  18

#define XBit 524288 //(1<<19)
#define YBit 65536  //(1<<16)
#define ZBit 2048  //(1<<11)
#define XYZBit 591872

u32 int_sqrt64(u64 x)
{
	u64 b, m, y = 0;
	if (x <= ULONG_MAX)
			return int_sqrt((u32) x);
			
	m = 1ULL << (fls64(x) & ~1ULL);
	while (m != 0) {
		b = y + m;
		y >>= 1;
		
		if (x >= b) {
			x -= b;
			y += m;
		}
		m >>= 2;
	}
	
	return y;
}
int xPos = 0, yPos = 0, zPos = 0;
int xInc = 0, yInc = 0, zInc = 0;
unsigned long acseleration, acseleration1;
bool xCtrl, yCtrl, zCtrl;

static struct hrtimer hr_timer;
ktime_t ktime;
unsigned long normF, maxF;

ktime_t startTime;

typedef struct GCommand {
    signed char GM[1];
    unsigned char num;
    unsigned char arr[];
} GCommand_t;
GCommand_t* GCommStruct;
typedef struct engraverSt {
    size_t width;
    size_t height;
    unsigned long fidrate;
    unsigned char lasarWidth;
    unsigned char pixWidth;
    unsigned long burnF;
    unsigned char arr[];
}engraverSt_t;
struct engrPr{
	unsigned int lasarWidth;
	unsigned int lasarHeight;
	unsigned int pixWidth;
	unsigned int pixHeight;
	int imgWidth;
	unsigned int imgHeight;
	unsigned int FIncris;
}engrCurPar;
engraverSt_t* engrSt;
typedef struct C01 {
    long int x;
    long int y;
    long int z;
    unsigned long F;
    unsigned long acseleration;
} C01_t;
typedef struct G01 {
    size_t N;
    long int x;
    long int y;
    long int z;
    unsigned long int dx;
    unsigned long int dy;
    unsigned long int dz;
} G01_t;
G01_t* G01Struct;
G01_t G01Engr;
typedef struct G02 {
    size_t N;
    long int se;
    long int sn;
    long int e;
    long int n;
    char pl;
} G02_t;
G02_t* G02Struct;
typedef struct GCommArr {
	void (*func)(void);
	char *arr;
}GCommArr_t;
size_t mArrSize = 0, currComm = 0;
GCommArr_t *mGCommArr;

void mainLoop(void){
	currComm++;//printk("Stage %d of %d\n", currComm, mArrSize);
	kfree(mGCommArr[currComm-1].arr);
	mGCommArr[currComm].func();
}

void endLoop(void){};
void (*mainLoopPtr)(void);
void engraverLoopNegative(void);
void G01ChackFn(G01_t *C03msg);

unsigned char* curArr, *engrBuf;
void engraverLoopPositive(void){
//	printk("asm nop%d\n", engrCurPar.imgWidth);
	if(engrCurPar.imgWidth < engrSt->width){
		if(engrCurPar.FIncris < 255){
			if(curArr[engrCurPar.imgWidth] > engrCurPar.FIncris){
				ktime = ktime_set(0, engrSt->burnF * curArr[engrCurPar.imgWidth] / 225);
			}else{
				ktime = ktime_set(0, engrSt->fidrate);
			}
		}else{
			ktime = ktime_set(0, engrSt->fidrate);
		}
		
		if(curArr[engrCurPar.imgWidth]<=engrCurPar.FIncris){
			pwm_ratio(curArr[engrCurPar.imgWidth], engrCurPar.FIncris);
		}else{
			pwm_ratio(engrCurPar.FIncris, engrCurPar.FIncris);
		}
		G01Engr.x = engrCurPar.lasarWidth;
		G01Engr.y = 0;
		G01Engr.z = 0;
		G01Engr.dx = (1<<20);
		G01Engr.dy = 0;
		G01Engr.dz = 0;
		engrCurPar.imgWidth += 1;
		G01ChackFn(&G01Engr);
	}else{
		if(engrCurPar.imgHeight < engrSt->height){
			if(engrCurPar.pixHeight < engrSt->pixWidth){
				G01Engr.x = 0;
				G01Engr.y = engrCurPar.lasarWidth;
				G01Engr.z = 0;
				G01Engr.dx = 0;
				G01Engr.dy = (1<<20);
				G01Engr.dz = 0;
				engrCurPar.pixHeight += 1;
				mainLoopPtr = engraverLoopNegative;
				G01ChackFn(&G01Engr);
			}else{
				engrCurPar.pixHeight = 0;
				engrCurPar.imgHeight++;
				mainLoopPtr = engraverLoopNegative;
				curArr = &engrSt->arr[engrCurPar.imgHeight * engrSt->width];
				if(engrCurPar.FIncris < 255){
					if(curArr[engrCurPar.imgWidth] > engrCurPar.FIncris){
						ktime = ktime_set(0, engrSt->burnF * curArr[engrCurPar.imgWidth] / 225);
					}else{
						ktime = ktime_set(0, engrSt->fidrate);
					}
				}else{
					ktime = ktime_set(0, engrSt->fidrate);
				}
				
				if(curArr[engrCurPar.imgWidth]<=engrCurPar.FIncris){
					pwm_ratio(curArr[engrCurPar.imgWidth], engrCurPar.FIncris);
				}else{
					pwm_ratio(engrCurPar.FIncris, engrCurPar.FIncris);
				}
				G01Engr.x = 0;
				G01Engr.y = engrCurPar.lasarWidth;
				G01Engr.z = 0;
				G01Engr.dx = 0;
				G01Engr.dy = (1<<20);
				G01Engr.dz = 0;
				G01ChackFn(&G01Engr);
			}
		}else{
			vfree(engrBuf);
			pwm_ratio(0, 1024);
			send_sign(SIGUSR2, 0);
		}
	}
}

void engraverLoopNegative(void){
	if(engrCurPar.imgWidth >= 0){
		if(engrCurPar.FIncris < 255){
			if(curArr[engrCurPar.imgWidth] > engrCurPar.FIncris){
				ktime = ktime_set(0, engrSt->burnF * curArr[engrCurPar.imgWidth] / 225);
			}else{
				ktime = ktime_set(0, engrSt->fidrate);
			}
		}else{
			ktime = ktime_set(0, engrSt->fidrate);
		}
		
		if(curArr[engrCurPar.imgWidth]<=engrCurPar.FIncris){
			pwm_ratio(curArr[engrCurPar.imgWidth], engrCurPar.FIncris);
		}else{
			pwm_ratio(engrCurPar.FIncris, engrCurPar.FIncris);
		}
		G01Engr.x = -engrCurPar.lasarWidth;
		G01Engr.y = 0;
		G01Engr.z = 0;
		G01Engr.dx = (1<<20);
		G01Engr.dy = 0;
		G01Engr.dz = 0;
		engrCurPar.imgWidth -= 1;
		G01ChackFn(&G01Engr);
	}else{
		if(engrCurPar.imgHeight < engrSt->height){
			engrCurPar.imgWidth = 0;
			if(engrCurPar.pixHeight < engrSt->pixWidth){
				G01Engr.x = 0;
				G01Engr.y = engrCurPar.lasarWidth;
				G01Engr.z = 0;
				G01Engr.dx = 0;
				G01Engr.dy = (1<<20);
				G01Engr.dz = 0;
				engrCurPar.pixHeight += 1;
				mainLoopPtr = engraverLoopPositive;
				G01ChackFn(&G01Engr);
			}else{
				engrCurPar.pixHeight = 0;
				engrCurPar.imgHeight++;
				mainLoopPtr = engraverLoopPositive;
				curArr = &engrSt->arr[engrCurPar.imgHeight * engrSt->width];
				if(engrCurPar.FIncris < 255){
					if(curArr[engrCurPar.imgWidth] > engrCurPar.FIncris){
						ktime = ktime_set(0, engrSt->burnF * curArr[engrCurPar.imgWidth] / 225);
					}else{
						ktime = ktime_set(0, engrSt->fidrate);
					}
				}else{
					ktime = ktime_set(0, engrSt->fidrate);
				}
				
				if(curArr[engrCurPar.imgWidth]<=engrCurPar.FIncris){
					pwm_ratio(curArr[engrCurPar.imgWidth], engrCurPar.FIncris);
				}else{
					pwm_ratio(engrCurPar.FIncris, engrCurPar.FIncris);
				}
				G01Engr.x = 0;
				G01Engr.y = engrCurPar.lasarWidth;
				G01Engr.z = 0;
				G01Engr.dx = 0;
				G01Engr.dy = (1<<20);
				G01Engr.dz = 0;
				G01ChackFn(&G01Engr);
			}
		}else{
			vfree(engrBuf);
			pwm_ratio(0, 1024);
			send_sign(SIGUSR2, 0);
		}

	}
}

enum hrtimer_restart timer_fall( struct hrtimer *timer );
enum hrtimer_restart line_interp_fall( struct hrtimer *timer );
enum hrtimer_restart line_interp_end( struct hrtimer *timer );
enum hrtimer_restart G28_fall( struct hrtimer *timer );
enum hrtimer_restart manual_control( struct hrtimer *timer )
{
	if(acseleration1 > 0){
		acseleration1--;
		ktime = ktime_set( 0, 300000000 / int_sqrt64(acseleration - acseleration1 * acseleration1) );
	}
	hrtimer_forward_now(& hr_timer, ktime);//hrtimer_forward()
	if(xCtrl) {
		s_pGpioRegisters->GPSET[0] = XBit;
		xPos += xInc;
	}
	if(yCtrl) {
		s_pGpioRegisters->GPSET[0] = YBit;
		yPos += yInc;
	}
	if(zCtrl) {
		s_pGpioRegisters->GPSET[0] = ZBit;
		zPos += zInc;
	}
	hr_timer.function = &timer_fall;
	return HRTIMER_RESTART;//HRTIMER_NORESTART
}
 
enum hrtimer_restart empty_func( struct hrtimer *timer )
{
	s_pGpioRegisters->GPCLR[0] = XYZBit;
//	send_sign(SIGALRM, xPos);
//	xPos = 0;
//	send_sign(33, yPos);
//	yPos = 0;
//	send_sign(34, zPos);
//	zPos = 0;
	return HRTIMER_NORESTART;
}

enum hrtimer_restart timer_fall( struct hrtimer *timer ){
	hrtimer_forward_now(& hr_timer, ktime);//hrtimer_forward()
	s_pGpioRegisters->GPCLR[0] = XYZBit;
//	s_pGpioRegisters->GPCLR[0] = XBit;
//	s_pGpioRegisters->GPCLR[0] = YBit;
//	s_pGpioRegisters->GPCLR[0] = ZBit;
	hr_timer.function = &manual_control;
	return HRTIMER_RESTART;
}

unsigned int targetX, targetY, targetZ, dx, dy, dz;
enum hrtimer_restart G01_f( struct hrtimer *timer ){
	hrtimer_forward_now(& hr_timer, ktime);
		if(targetX && xCtrl){
			if(dx & (1<<20)){
				dx &= ~(1<<20);
				s_pGpioRegisters->GPSET[0] = XBit;
				--targetX;
			}
			dx += xInc;
		}
		if(targetY && yCtrl){
			if(dy & (1<<20)){
				dy &= ~(1<<20);
				s_pGpioRegisters->GPSET[0] = YBit;
				--targetY;
			}
			dy += yInc;
		}
		if(targetZ && zCtrl){
			if(dz & (1<<20)){
				dz &= ~(1<<20);
				s_pGpioRegisters->GPSET[0] = ZBit;
				--targetZ;
			}
			dz += zInc;
		}
		hr_timer.function = &line_interp_fall;
	return HRTIMER_RESTART;
}
enum hrtimer_restart line_interp_fall( struct hrtimer *timer ){
	ktime_t now = hrtimer_cb_get_time(&hr_timer);
	s_pGpioRegisters->GPCLR[0] = XYZBit;
	if(targetX || targetY || targetZ){
		hr_timer.function = &G01_f;
	}else{
		hr_timer.function = &line_interp_end;
	}
	hrtimer_forward(&hr_timer, now, ktime);
	return HRTIMER_RESTART;
}

enum hrtimer_restart line_interp_end( struct hrtimer *timer ){
	(*mainLoopPtr)();
	return HRTIMER_NORESTART;
}

enum hrtimer_restart G28_f( struct hrtimer *timer ){
	ktime_t now = hrtimer_cb_get_time(&hr_timer);
	if(xCtrl){
		s_pGpioRegisters->GPSET[0] = XBit;
	}
	if(yCtrl){
		s_pGpioRegisters->GPSET[0] = YBit;
	}
	if(zCtrl){
		s_pGpioRegisters->GPSET[0] = ZBit;
	}
	if(xCtrl || yCtrl || zCtrl){
		hr_timer.function = &G28_fall;
		hrtimer_forward(&hr_timer, now, ktime);
		return HRTIMER_RESTART;
	}else{
		(*mainLoopPtr)();
		return HRTIMER_NORESTART;
	}
}
enum hrtimer_restart G28_fall( struct hrtimer *timer ){
	hrtimer_forward_now(& hr_timer, ktime);
	s_pGpioRegisters->GPCLR[0] = XYZBit;
	hr_timer.function = &G28_f;
	return HRTIMER_RESTART;
}

enum hrtimer_restart G04_f( struct hrtimer *timer )
{
	mainLoop();
	return HRTIMER_NORESTART;
}

enum hrtimer_restart G02_fall( struct hrtimer *timer );
int EastStep, EastDir, EastMax, EastMin, NorthStep, NorthDir, NorthMax, NorthMin;
long int se, sn, e, n;
u64 gip;
signed char eInc, nInc;
enum hrtimer_restart G02_f( struct hrtimer *timer )
{
	hrtimer_forward_now(& hr_timer, ktime);
	hr_timer.function = &G02_fall;
	if(G02Struct->se != G02Struct->e){
		if(G02Struct->sn != sn){
			if((sn - G02Struct->sn) > 0){//G02Struct->sn from sn to
				s_pGpioRegisters->GPSET[0] = NorthDir;
				++G02Struct->sn;
			}else{
				s_pGpioRegisters->GPCLR[0] = NorthDir;
				--G02Struct->sn;
			}
			s_pGpioRegisters->GPSET[0] = NorthStep;
		}else{
			s_pGpioRegisters->GPSET[0] = EastStep;
			G02Struct->se += eInc;
			se = G02Struct->se;
			sn = int_sqrt64(gip - (u64)abs(se)*(u64)abs(se)) * nInc;
//			printk(KERN_INFO "G02Struct->sn==%ld G02Struct->n==%ld se==%ld e==%ld\n", G02Struct->sn, G02Struct->n, se, e);
			if(sn != G02Struct->sn){
				if((sn - G02Struct->sn) > 0){//G02Struct->sn from sn to
					s_pGpioRegisters->GPSET[0] = NorthDir;
					++G02Struct->sn;
				}else{
					s_pGpioRegisters->GPCLR[0] = NorthDir;
					--G02Struct->sn;
				}
				s_pGpioRegisters->GPSET[0] = NorthStep;
			}
		}
	}else{
		if(G02Struct->sn != G02Struct->n){
			if((G02Struct->n - G02Struct->sn) > 0){//G02Struct->sn from sn to
				s_pGpioRegisters->GPSET[0] = NorthDir;
				++G02Struct->sn;
			}else{
				s_pGpioRegisters->GPCLR[0] = NorthDir;
				--G02Struct->sn;
			}
			s_pGpioRegisters->GPSET[0] = NorthStep;
		}
	}//printk("se%ld e%ld sn%ld n%ld\n", G02Struct->se, e, G02Struct->sn, n);
	return HRTIMER_RESTART;
}

enum hrtimer_restart G02_fall( struct hrtimer *timer )
{
	ktime_t now = hrtimer_cb_get_time(&hr_timer);
	s_pGpioRegisters->GPCLR[0] = XYZBit;
	if((xCtrl || yCtrl || zCtrl) && (G02Struct->se != G02Struct->e || G02Struct->sn != G02Struct->n)){
		hr_timer.function = &G02_f;
		hrtimer_forward(&hr_timer, now, ktime);
		return HRTIMER_RESTART;
	}else{
		mainLoop();
		return HRTIMER_NORESTART;
	}
}
