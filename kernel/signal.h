#include <asm/siginfo.h>    //siginfo
#include <linux/rcupdate.h> //rcu_read_lock
#include <linux/sched.h>    //find_task_by_pid_type

static pid_t pid; // Stores application PID in user space

struct siginfo info;
struct task_struct *t;

void send_sign(int mSig, int mass){
//	memset(&info, 0, sizeof(struct siginfo));
	info.si_signo = mSig;
	// This is bit of a trickery: SI_QUEUE is normally used by sigqueue from user space,    and kernel space should use SI_KERNEL. 
	// But if SI_KERNEL is used the real_time data  is not delivered to the user space signal handler function. */
	info.si_code = SI_QUEUE;

	// real time signals may have 32 bits of data.
	info.si_int = mass; // Any value you want to send
//	rcu_read_lock();
	// find the task with that pid
//	t = pid_task(find_pid_ns(pid, &init_pid_ns), PIDTYPE_PID);
	if (t != NULL) {
		if (send_sig_info(mSig, &info, t) < 0) // send signal
			printk("send_sig_info error\n");
	} else {
		 printk("pid_task error\n");
		//return -ENODEV;
	}
//	rcu_read_unlock();      
}
