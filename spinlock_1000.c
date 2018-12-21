#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
//#include <linux/pthread.h>
MODULE_LICENSE("Dual BSD/GPL");

#define SHAREDMEM_LOCK 0x1103000
#define SHAREDMEM_RW_FLD 0x1104000
#define SHAREDMEM_SNULL_LOCK 0x1105000
#define SHARED_MEM_LOCK 0x1106000


int lock_init_module(void){
	printk("smem_lock: loaded\n");
	printk("lock: %x\n", SHARED_MEM_LOCK);
	spinlock_t *sharedmem_lock = __va(SHARED_MEM_LOCK);
	int i;
	for(i = 0; i < 1000; i++)
	{
		spin_lock(sharedmem_lock);
		printk("smem_lock: smem is locked\n");
		spin_unlock(sharedmem_lock);
		printk("smem_lock: smem is unlocked\n");
	}
	printk("smem_lock: done\n");
	return 0;
}

void lock_exit_module(void){
	printk("smem_lock: unloading\n");
}

module_init(lock_init_module);
module_exit(lock_exit_module);
