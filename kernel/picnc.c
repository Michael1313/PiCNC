#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/vmalloc.h>

#include <linux/hrtimer.h>
#include <linux/ktime.h>

#define MS_TO_NS(x) (x * 1E6L)

#include "gpio.h"
#include "signal.h"
#include "GCommand.h"

static int my_open(struct inode *i, struct file *f)
{
	if(mGCommArr == NULL) mGCommArr = kmalloc(sizeof(GCommArr_t), GFP_ATOMIC);
	if(mGCommArr == NULL) printk("INIT MODULE ERROR");
	printk(KERN_INFO "Driver: open() %d\n", current->pid);
	pid = current->pid;
	rcu_read_lock();
	t = pid_task(find_pid_ns(pid, &init_pid_ns), PIDTYPE_PID);
	rcu_read_unlock();      
	memset(&info, 0, sizeof(struct siginfo));

	return 0;
}
static int my_close(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: close()\n");
	return 0;
}
typedef struct PXYZ{
    int x;
    int y;
    int z;
}PXYZ_t;
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
//	unsigned long ret;
	PXYZ_t *dat = (PXYZ_t*)buf;
//	ret = copy_to_user(buf, arr, len);
	dat->x = xPos;
	xPos = 0;
	dat->y = yPos;
	yPos = 0;
	dat->z = zPos;
	zPos = 0;
//	printk(KERN_INFO "Driver: read()%lu\n", ret);
//	return ret;
	return 0;
}

void G01ChackFn(G01_t *C03msg);
void G28ChackFn(GCommand_t *C28msg);
void G00func(void){
	GCommStruct = (GCommand_t*)mGCommArr[currComm].arr;
	if(GCommStruct->num==0){
		ktime = ktime_set(0, maxF);
	}else if(GCommStruct->num == 1){
		ktime = ktime_set(0, normF);
	}
	G01Struct = (G01_t*)GCommStruct->arr;
	mainLoopPtr = mainLoop;
	if(ktime_to_ms(ktime_sub(ktime_get(), startTime)) > 200){
		send_sign(SIGUSR1, G01Struct->N);
		startTime = ktime_get();
	}
	G01ChackFn(G01Struct);
}
void G02func(void){
	ktime = ktime_set(0, normF);
	GCommStruct = (GCommand_t*)mGCommArr[currComm].arr;
	G02Struct = (G02_t*)GCommStruct->arr;
	if(G02Struct->pl & 0b001) {
		EastStep = 1 << XStep;
		EastDir = 1 << XDir;
		EastMax = 1 << XMax;
		EastMin = 1 << XMin;
	}
	if((G02Struct->pl & 0b010) && (G02Struct->pl & 0b001)) {
		NorthStep = 1 << YStep;
		NorthDir = 1 << YDir;
		NorthMax = 1 << YMax;
		NorthMin = 1 << YMin;
	}else if((G02Struct->pl & 0b010) && (G02Struct->pl & 0b100)){
		EastStep = 1 << YStep;
		EastDir = 1 << YDir;
		EastMax = 1 << YMax;
		EastMin = 1 << YMin;
	}
	if(G02Struct->pl & 0b100) {
		NorthStep = 1 << ZStep;
		NorthDir = 1 << ZDir;
		NorthMax = 1 << ZMax;
		NorthMin = 1 << ZMin;
	}
	if(G02Struct->pl & 0b01000){
		s_pGpioRegisters->GPSET[0] = EastDir;
		eInc = 1;
	}else{
		s_pGpioRegisters->GPCLR[0] = EastDir;
		eInc = -1;
	}
	if(G02Struct->pl & 0b10000){
		nInc = 1;
	}else{
		nInc = -1;
	}
	se = G02Struct->se;
	e = G02Struct->e;
	sn = G02Struct->sn;
	n = G02Struct->n;
	gip = (u64)abs(se)*(u64)abs(se) + (u64)abs(sn)*(u64)abs(sn);
	if(ktime_to_ms(ktime_sub(ktime_get(), startTime)) > 200){
		send_sign(SIGUSR1, G02Struct->N);
		startTime = ktime_get();
	}
	hr_timer.function = &G02_f;
	if(!hrtimer_is_queued( &hr_timer )) hrtimer_start( &hr_timer, ktime, HRTIMER_MODE_ABS );//HRTIMER_MODE_ABS, HRTIMER_ABS, HRTIMER_MODE_REL
}
void C04func(void){
	unsigned long *Fmsg;
	GCommStruct = (GCommand_t*)mGCommArr[currComm].arr;
	Fmsg = (unsigned long*)(GCommStruct->arr);
	normF = *Fmsg;
	currComm++;
	kfree(mGCommArr[currComm-1].arr);
	mGCommArr[currComm].func();//printk(KERN_INFO "FF%lu %lu\n", normF, maxF);
}
void G04func(void){
	unsigned long *deley;
	GCommStruct = (GCommand_t*)mGCommArr[currComm].arr;
	deley = (unsigned long*)(GCommStruct->arr);
	ktime = ktime_set(0, *deley);
	hr_timer.function = &G04_f;
	if(!hrtimer_is_queued( &hr_timer )) hrtimer_start( &hr_timer, ktime, HRTIMER_MODE_ABS );//HRTIMER_MODE_ABS, HRTIMER_ABS, HRTIMER_MODE_REL
}
void G28func(void){
	ktime = ktime_set(0, maxF);
	GCommStruct = (GCommand_t*)mGCommArr[currComm].arr;
	mainLoopPtr = mainLoop;
	G28ChackFn(GCommStruct);
}
void M3func(void){
	pwm_ratio(1024, 1024);
	currComm++;
	kfree(mGCommArr[currComm-1].arr);
	mGCommArr[currComm].func();
}
void M5func(void){
	pwm_ratio(0, 1024);
	currComm++;
	kfree(mGCommArr[currComm-1].arr);
	mGCommArr[currComm].func();
}
void ENDfunc(void){
	kfree(mGCommArr);
	mGCommArr = NULL;
	mArrSize = 0;
	currComm = 0;
	send_sign(SIGUSR1, 0);
}
static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
//	unsigned long ret;
//	ret = copy_from_user(arr, buf, len);

	size_t i=0;
	int ret;

	if(buf[0] == '%'){
		printk(KERN_INFO "my massage incris\n");
	} else {
		GCommand_t *msg = (GCommand_t*)(buf);
		if(msg->GM[0] == 'C'){
			if(msg->num == 0){  /** sets max fidrate **/
				unsigned long *F = (unsigned long *)msg->arr;
				maxF = *F;
				ktime = ktime_set( 0, *F );
				normF = *F;
			}else if(msg->num == 1){
				C01_t *C01msg = (C01_t*)(msg->arr);
				
				if(C01msg->x < 0 && !(s_pGpioRegisters->GPLEV[0] & (1<<XMin))){
					SetGPIOOutputValue(XDir, false);
					xInc = -1;
					xCtrl = true;
					asm("nop");
				} else if(C01msg->x > 0 && !(s_pGpioRegisters->GPLEV[0] & (1<<XMax))){
					SetGPIOOutputValue(XDir, true);
					xInc = 1;
					xCtrl = true;
				} else {
					xInc = 0;
					xCtrl = false;
				};
				
				if(C01msg->y < 0 && !(s_pGpioRegisters->GPLEV[0] & (1<<YMin))){
					SetGPIOOutputValue(YDir, false);
					yInc = -1;
					yCtrl = true;
					asm("nop");
				} else if(C01msg->y > 0 && !(s_pGpioRegisters->GPLEV[0] & (1<<YMax))){
					SetGPIOOutputValue(YDir, true);
					yInc = 1;
					yCtrl = true;
				} else {
					yInc = 0;
					yCtrl = false;
				};
				
				if(C01msg->z < 0 && !(s_pGpioRegisters->GPLEV[0] & (1<<ZMin))){
					SetGPIOOutputValue(ZDir, false);
					zInc = -1;
					zCtrl = true;
					asm("nop");
				} else if(C01msg->z > 0 && !(s_pGpioRegisters->GPLEV[0] & (1<<ZMax))){
					SetGPIOOutputValue(ZDir, true);
					zInc = 1;
					zCtrl = true;
				} else {
					zInc = 0;
					zCtrl = false;
				};
				
				acseleration = C01msg->acseleration * C01msg->acseleration;
				acseleration1= C01msg->acseleration;
				
				if(xCtrl || yCtrl || zCtrl){
					asm("nop");
					ktime = ktime_set( 0, C01msg->F );//MS_TO_NS(250)
					hr_timer.function = &manual_control;
					if(!hrtimer_is_queued( &hr_timer )) hrtimer_start( &hr_timer, ktime, HRTIMER_MODE_ABS );//HRTIMER_MODE_ABS, HRTIMER_ABS, HRTIMER_MODE_REL
				}else{
					hr_timer.function = &empty_func;
				}

	//			printk(KERN_INFO "my massage C01 is %ld - %ld - %ld\n", C01msg->x, C01msg->y, C01msg->z);
	//			printk(KERN_INFO "my massage C01 is %zx\n", len);
			}else if(msg->num == 2){  /** sets fidrate online **/
				unsigned long *Fmsg = (unsigned long*)(msg->arr);
				ktime = ktime_set(0, *Fmsg);
			}else if(msg->num == 3){

				G01_t *C03msg = (G01_t*)(msg->arr);
				mainLoopPtr = &endLoop;
				G01ChackFn(C03msg);

			}else if(msg->num == 4){  /** sets fidrate function **/
				mArrSize++;
				mGCommArr = krealloc(mGCommArr, mArrSize * sizeof(GCommArr_t), GFP_ATOMIC);
				if(!mGCommArr){
					printk(KERN_INFO "Coud not \"realloc\" memory\n");
				}else{
					mGCommArr[mArrSize-1].func = &C04func;
					mGCommArr[mArrSize-1].arr = kmalloc(sizeof(GCommand_t)+4, GFP_ATOMIC);
					if(mGCommArr[mArrSize-1].arr == NULL){
						printk(KERN_INFO "Coud not \"realloc\" memory\n");
					}else{
						if(copy_from_user(mGCommArr[mArrSize-1].arr, buf, sizeof(GCommand_t)+4)){
							printk(KERN_INFO "Coud not \"copy_from_user\" memory\n");
						}
					}
				}
			}else if(msg->num == 5){
				mArrSize++;
				mGCommArr = krealloc(mGCommArr, mArrSize * sizeof(GCommArr_t), GFP_ATOMIC);
				if(!mGCommArr){
					printk(KERN_INFO "Coud not \"realloc\" memory\n");
				}else{
					mGCommArr[mArrSize-1].func = &ENDfunc;
				}
				mGCommArr[0].func();
			}else if(msg->num == 6){
//				pid_t *mPID = (pid_t*)(msg->arr);
//				pid = *mPID;
			}else if(msg->num == 7){//printk("size %u, %lu, %d", len, PAGE_SIZE, MAX_ORDER);return len;
				engrBuf = vmalloc(len);
				if(copy_from_user(engrBuf, buf, len)){
					printk(KERN_INFO "Coud not \"copy_from_user\" memory\n");
				}
				GCommStruct = (GCommand_t*)engrBuf;
				engrSt = (engraverSt_t*)GCommStruct->arr;
				mainLoopPtr = engraverLoopPositive;
				curArr = &engrSt->arr[0];
				engrCurPar.lasarWidth = engrSt->lasarWidth * engrSt->pixWidth;
				engrCurPar.lasarHeight = engrSt->lasarWidth;
				engrCurPar.pixHeight = 0;
				engrCurPar.imgWidth = 1;
				engrCurPar.imgHeight= 0;
				engrCurPar.FIncris = engrSt->fidrate * 255 / engrSt->burnF;
				if(engrCurPar.FIncris < 255){
					if(engrSt->arr[0] > engrCurPar.FIncris){
						ktime = ktime_set(0, engrSt->burnF * engrSt->arr[0] / 225);
					}else{
						ktime = ktime_set(0, engrSt->fidrate);
					}
				}else{
					ktime = ktime_set(0, engrSt->fidrate);
				}
				
				if(engrSt->arr[0]<=engrCurPar.FIncris){
					pwm_ratio(engrSt->arr[0], engrCurPar.FIncris);
				}else{
					pwm_ratio(engrCurPar.FIncris, engrCurPar.FIncris);
				}
				G01Engr.x = engrCurPar.lasarWidth;
				G01Engr.y = 0;
				G01Engr.z = 0;
				G01Engr.dx = (1<<20);
				G01Engr.dy = 0;
				G01Engr.dz = 0;//printk("asm nop%d\n", engrSt->width);
				G01ChackFn(&G01Engr);
			}else if(msg->num == 8){//pause
				ret = hrtimer_cancel(&hr_timer);
				udelay(100);
				if(ret && hrtimer_is_queued( &hr_timer )) return len;
				pwm_ctl->PWEN1 = 0;
			}else if(msg->num == 9){//cancel
				ret = hrtimer_cancel(&hr_timer);
				udelay(100);
				if(ret && hrtimer_is_queued( &hr_timer )) return len;
				for(i=currComm; i<mArrSize-1; i++){
					if(mGCommArr[currComm].arr != NULL){
						kfree(mGCommArr[currComm].arr);
					}
				}
				if(mGCommArr != NULL){
					kfree(mGCommArr);
					mGCommArr = NULL;
				}
				mArrSize = 0;
				currComm = 0;
				send_sign(SIGUSR1, 0);
				pwm_ctl->PWEN1 = 0;
//				printk("cancel");
			}else if(msg->num == 10){//replay
				if(!hrtimer_is_queued( &hr_timer )) hrtimer_restart( &hr_timer );//HRTIMER_MODE_ABS, HRTIMER_ABS, HRTIMER_MODE_REL
				pwm_ctl->PWEN1 = 1;
			}else if(msg->num == 11){//cancel engraver
				ret = hrtimer_cancel(&hr_timer);
				udelay(100);
				if(ret && hrtimer_is_queued( &hr_timer )) return len;
				vfree(GCommStruct);
				GCommStruct = NULL;
				//send_sign(SIGUSR2, 0);
				pwm_ctl->PWEN1 = 0;
			}else if(msg->num == 12){
				if(s_pPwmRegisters->DAT1 > 0) {
					pwm_ratio(0, 1024);
				}else{
					pwm_ratio(1024, 1024);
				}
			}else if(msg->num == 28){
				mainLoopPtr = &endLoop;
				G28ChackFn(msg);
			}
		} else if(msg->GM[0] == 'G'){
			if(msg->num == 0 || msg->num == 1){
				mArrSize++;
				mGCommArr = krealloc(mGCommArr, mArrSize * sizeof(GCommArr_t), GFP_ATOMIC);
				if(!mGCommArr){
					printk(KERN_INFO "Coud not \"realloc\" memory\n");
				}else{
					mGCommArr[mArrSize-1].func = &G00func;
					mGCommArr[mArrSize-1].arr = kmalloc(sizeof(GCommand_t) + sizeof(G01_t), GFP_ATOMIC);
					if(mGCommArr[mArrSize-1].arr == NULL){
						printk(KERN_INFO "Coud not \"realloc\" memory\n");
					}else{
						if(copy_from_user(mGCommArr[mArrSize-1].arr, buf, sizeof(GCommand_t) + sizeof(G01_t))){
							printk(KERN_INFO "Coud not \"copy_from_user\" memory\n");
						}
					}
				}
			}else if(msg->num == 2 || msg->num == 3){
				mArrSize++;
				mGCommArr = krealloc(mGCommArr, mArrSize * sizeof(GCommArr_t), GFP_ATOMIC);
				if(!mGCommArr){
					printk(KERN_INFO "Coud not \"realloc\" memory\n");
				}else{
					mGCommArr[mArrSize-1].func = &G02func;
					mGCommArr[mArrSize-1].arr = kmalloc(sizeof(GCommand_t) + sizeof(G02_t), GFP_ATOMIC);
					if(mGCommArr[mArrSize-1].arr == NULL){
						printk(KERN_INFO "Coud not \"realloc\" memory\n");
					}else{
						if(copy_from_user(mGCommArr[mArrSize-1].arr, buf, sizeof(GCommand_t) + sizeof(G02_t))){
							printk(KERN_INFO "Coud not \"copy_from_user\" memory\n");
						}
					}
				}
			}else if(msg->num == 4){
				mArrSize++;
				mGCommArr = krealloc(mGCommArr, mArrSize * sizeof(GCommArr_t), GFP_ATOMIC);
				if(!mGCommArr){
					printk(KERN_INFO "Coud not \"realloc\" memory\n");
				}else{
					mGCommArr[mArrSize-1].func = &G04func;
					mGCommArr[mArrSize-1].arr = kmalloc(sizeof(GCommand_t) + sizeof(G01_t), GFP_ATOMIC);
					if(mGCommArr[mArrSize-1].arr == NULL){
						printk(KERN_INFO "Coud not \"realloc\" memory\n");
					}else{
						if(copy_from_user(mGCommArr[mArrSize-1].arr, buf, sizeof(GCommand_t) + sizeof(G01_t))){
							printk(KERN_INFO "Coud not \"copy_from_user\" memory\n");
						}
					}
				}
			}else if(msg->num == 28){
				mArrSize++;
				mGCommArr = krealloc(mGCommArr, mArrSize * sizeof(GCommArr_t), GFP_ATOMIC);
				if(!mGCommArr){
					printk(KERN_INFO "Coud not \"realloc\" memory\n");
				}else{
					mGCommArr[mArrSize-1].func = &G28func;
					mGCommArr[mArrSize-1].arr = kmalloc(sizeof(GCommand_t) + 1, GFP_ATOMIC);
					if(mGCommArr[mArrSize-1].arr == NULL){
						printk(KERN_INFO "Coud not \"realloc\" memory\n");
					}else{
						if(copy_from_user(mGCommArr[mArrSize-1].arr, buf, sizeof(GCommand_t) + 1)){
							printk(KERN_INFO "Coud not \"copy_from_user\" memory\n");
						}
					}
				}
			}
		}else if(msg->GM[0] == 'M'){
			if(msg->num == 3){
				mArrSize++;
				mGCommArr = krealloc(mGCommArr, mArrSize * sizeof(GCommArr_t), GFP_ATOMIC);
				if(!mGCommArr){
					printk(KERN_INFO "Coud not \"realloc\" memory\n");
				}else{
					mGCommArr[mArrSize-1].func = &M3func;
					mGCommArr[mArrSize-1].arr = kmalloc(1, GFP_ATOMIC);
					if(mGCommArr[mArrSize-1].arr == NULL){
						printk(KERN_INFO "Coud not \"realloc\" memory\n");
					}
				}
			}else if(msg->num == 5){
				mArrSize++;
				mGCommArr = krealloc(mGCommArr, mArrSize * sizeof(GCommArr_t), GFP_ATOMIC);
				if(!mGCommArr){
					printk(KERN_INFO "Coud not \"realloc\" memory\n");
				}else{
					mGCommArr[mArrSize-1].func = &M5func;
					mGCommArr[mArrSize-1].arr = kmalloc(1, GFP_ATOMIC);
					if(mGCommArr[mArrSize-1].arr == NULL){
						printk(KERN_INFO "Coud not \"realloc\" memory\n");
					}
				}
			}
		} else {
			printk(KERN_INFO "Unnown Command\n");
		}
	}
	return len;
}

void G01ChackFn(G01_t *C03msg){
	if(C03msg->x < 0 && !(s_pGpioRegisters->GPLEV[0] & (1<<XMin))){
		SetGPIOOutputValue(XDir, false);
		targetX = abs(C03msg->x);
		xCtrl = true;
		dx = xInc = C03msg->dx;
	} else if(C03msg->x > 0 && !(s_pGpioRegisters->GPLEV[0] & (1<<XMax))){
		SetGPIOOutputValue(XDir, true);
		targetX = C03msg->x;
		xCtrl = true;
		dx = xInc = C03msg->dx;
	} else {
		targetX = 0;
		dx = xInc = 0;
		xCtrl = false;
	};
	
	if(C03msg->y < 0 && !(s_pGpioRegisters->GPLEV[0] & (1<<YMin))){
		SetGPIOOutputValue(YDir, false);
		targetY = abs(C03msg->y);
		yCtrl = true;
		dy = yInc = C03msg->dy;
	} else if(C03msg->y > 0 && !(s_pGpioRegisters->GPLEV[0] & (1<<YMax))){
		SetGPIOOutputValue(YDir, true);
		targetY = C03msg->y;
		yCtrl = true;
		dy = yInc = C03msg->dy;
	} else {
		targetY = 0;
		yInc = 0;
		dy = yCtrl = false;
	};
	
	if(C03msg->z < 0 && !(s_pGpioRegisters->GPLEV[0] & (1<<ZMin))){
		SetGPIOOutputValue(ZDir, false);
		targetZ = abs(C03msg->z);
		zCtrl = true;
		dz = zInc = C03msg->dz;
	} else if(C03msg->z > 0 && !(s_pGpioRegisters->GPLEV[0] & (1<<ZMax))){
		SetGPIOOutputValue(ZDir, true);
		targetZ = C03msg->z;
		zCtrl = true;
		dz = zInc = C03msg->dz;
	} else {
		targetZ = 0;
		zCtrl = false;
		dz = zInc = 0;
	};
	if(xCtrl || yCtrl || zCtrl){
		hr_timer.function = &G01_f;
		if(!hrtimer_is_queued( &hr_timer )) hrtimer_start( &hr_timer, ktime, HRTIMER_MODE_ABS );//HRTIMER_MODE_ABS, HRTIMER_ABS, HRTIMER_MODE_REL
	}else{
		hr_timer.function = &empty_func;
	}
}

void G28ChackFn(GCommand_t *C28msg){
	if((C28msg->arr[0] & 0b0001) && !(s_pGpioRegisters->GPLEV[0] & (1<<XMin))) {
		SetGPIOOutputValue(XDir, false);
		xCtrl = true;
	}
	if((C28msg->arr[0] & 0b0010) && !(s_pGpioRegisters->GPLEV[0] & (1<<YMin))) {
		SetGPIOOutputValue(YDir, false);
		yCtrl = true;
	}
	if((C28msg->arr[0] & 0b0100) && !(s_pGpioRegisters->GPLEV[0] & (1<<XMin))) {
		SetGPIOOutputValue(YDir, false);
		zCtrl = true;
	}
	if(xCtrl || yCtrl || zCtrl) {
		hr_timer.function = &G28_f;
		if(!hrtimer_is_queued( &hr_timer )) hrtimer_start( &hr_timer, ktime, HRTIMER_MODE_ABS );
	}
}

  static struct file_operations sample_fops =
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
	.write = my_write
};

struct miscdevice sample_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "piCNC",
	.fops = &sample_fops,
	.mode = 0666,
};

/** GPIO IRQ **/

static irqreturn_t XMax_irq(int irq, void *dev_id)
{
	if(s_pGpioRegisters->GPLEV[0] & (1<<XDir)) xCtrl = false;
	return IRQ_HANDLED;
}
 
static irqreturn_t XMin_irq(int irq, void *dev_id)
{
	if(!(s_pGpioRegisters->GPLEV[0] & (1<<XDir))) xCtrl = false;
	return IRQ_HANDLED;
}
 
static irqreturn_t YMax_irq(int irq, void *dev_id)
{
	if(s_pGpioRegisters->GPLEV[0] & (1<<YDir)) yCtrl = false;
	return IRQ_HANDLED;
}
 
static irqreturn_t YMin_irq(int irq, void *dev_id)
{
	if(!(s_pGpioRegisters->GPLEV[0] & (1<<YDir))) yCtrl = false;
	return IRQ_HANDLED;
}
 
static irqreturn_t ZMax_irq(int irq, void *dev_id)
{
	if(s_pGpioRegisters->GPLEV[0] & (1<<ZDir)) zCtrl = false;
	return IRQ_HANDLED;
}
 
static irqreturn_t ZMin_irq(int irq, void *dev_id)
{
	if(!(s_pGpioRegisters->GPLEV[0] & (1<<ZDir))) zCtrl = false;
	return IRQ_HANDLED;
}
 
/** GPIO IRQ **/

static int __init ofcd_init(void) /* Constructor */
{
	int error, mret;

	error = misc_register(&sample_device);
	if (error) {
		pr_err("can't misc_register :(\n");
		return error;
	}

	remap_gpio();
	remap_pwm();

	SetGPIOFunction(XStep, GPIO_OUTPUT);
	SetGPIOFunction(XDir, GPIO_OUTPUT);

	SetGPIOFunction(YStep, GPIO_OUTPUT);
	SetGPIOFunction(YDir, GPIO_OUTPUT);

	SetGPIOFunction(ZStep, GPIO_OUTPUT);
	SetGPIOFunction(ZDir, GPIO_OUTPUT);
	
	SetGPIOFunction(Spdl, GPIO_OUTPUT);

	SetGPIOFunction(XMax, GPIO_INPUT);
	set_pull_resistor(XMax, PIN_PULL_DOWN);
	SetGPIOFunction(XMin, GPIO_INPUT);
	set_pull_resistor(XMin, PIN_PULL_DOWN);
	SetGPIOFunction(YMax, GPIO_INPUT);
	set_pull_resistor(YMax, PIN_PULL_DOWN);
	SetGPIOFunction(YMin, GPIO_INPUT);
	set_pull_resistor(YMin, PIN_PULL_DOWN);
	SetGPIOFunction(ZMax, GPIO_INPUT);
	set_pull_resistor(ZMax, PIN_PULL_DOWN);
	SetGPIOFunction(ZMin, GPIO_INPUT);
	set_pull_resistor(ZMin, PIN_PULL_DOWN);

	hrtimer_init( &hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_ABS );//CLOCK_MONOTONIC, CLOCK_REALTIME

	s_pGpioRegisters->GPREN[ XMax / 32 ] |= (1 << (XMax % 32));
//	s_pGpioRegisters->GPFEN[ XMax / 32 ] |= (1 << (XMax % 32));
	mret = request_irq(gpio_to_irq(XMax), XMax_irq, 0, "sample", NULL);
	if (mret < 0) printk(KERN_ALERT "%s: request_irg failed with %d\n", __func__, mret);

	s_pGpioRegisters->GPREN[ XMin / 32 ] |= (1 << (XMin % 32));
	mret = request_irq(gpio_to_irq(XMin), XMin_irq, 0, "sample", NULL);
	if (mret < 0) printk(KERN_ALERT "%s: request_irg failed with %d\n", __func__, mret);

	s_pGpioRegisters->GPREN[ YMax / 32 ] |= (1 << (YMax % 32));
	mret = request_irq(gpio_to_irq(YMax), YMax_irq, 0, "sample", NULL);
	if (mret < 0) printk(KERN_ALERT "%s: request_irg failed with %d\n", __func__, mret);

	s_pGpioRegisters->GPREN[ YMin / 32 ] |= (1 << (YMin % 32));
	mret = request_irq(gpio_to_irq(YMin), YMin_irq, 0, "sample", NULL);
	if (mret < 0) printk(KERN_ALERT "%s: request_irg failed with %d\n", __func__, mret);

	s_pGpioRegisters->GPREN[ ZMax / 32 ] |= (1 << (ZMax % 32));
	mret = request_irq(gpio_to_irq(ZMax), ZMax_irq, 0, "sample", NULL);
	if (mret < 0) printk(KERN_ALERT "%s: request_irg failed with %d\n", __func__, mret);

	s_pGpioRegisters->GPREN[ ZMin / 32 ] |= (1 << (ZMin % 32));
	mret = request_irq(gpio_to_irq(ZMin), ZMin_irq, 0, "sample", NULL);
	if (mret < 0) printk(KERN_ALERT "%s: request_irg failed with %d\n", __func__, mret);

	// frequence must be 19200000.0 < f < 586.0
	//dev 0x1 > d > 0xFFF
	pwm_frequency(64);

	return 0;
}
 
static void __exit ofcd_exit(void) /* Destructor */
{
	int ret;
	pin_release(XStep);
	pin_release(XDir);
	pin_release(XMax);
	pin_release(XMin);
	pin_release(YStep);
	pin_release(YDir);
	pin_release(YMax);
	pin_release(YMin);
	pin_release(ZStep);
	pin_release(ZDir);
	pin_release(ZMax);
	pin_release(ZMin);
	pin_release(Spdl);
	SetGPIOFunction(18, GPIO_INPUT); /* Set ALT = 0 */
	unmap_gpio();
	unmap_pwm();

	ret = hrtimer_cancel( &hr_timer );//hrtimer_restart()
	if (ret) printk("The timer was still in use...\n");
	misc_deregister(&sample_device);

	printk(KERN_INFO "Module unregistered");
}

module_init(ofcd_init);
module_exit(ofcd_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("MBajor>");
MODULE_DESCRIPTION("PiCNC driver");
MODULE_VERSION("0.1");
