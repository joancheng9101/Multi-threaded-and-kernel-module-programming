#include<linux/module.h>
#include<linux/init.h>
#include<linux/proc_fs.h>
#include<linux/sched.h>
#include<linux/uaccess.h>
#include<linux/fs.h>
#include<linux/seq_file.h>
#include<linux/slab.h>

static char str[200];

static int my_proc_show(struct seq_file *m,void *v){
	//seq_printf(m,"hello from proc file\n");
	seq_printf(m,"%s",str);
	return 0;
}

static ssize_t my_proc_write(struct file* file,const char __user *buffer,size_t count,loff_t *f_pos){
	//char *tmp = kzalloc((count+1),GFP_KERNEL);
	char tmp[100];
	int tid;
	if(!tmp)return -ENOMEM;
	if(copy_from_user(tmp,buffer,count)){
		kfree(tmp);
		return EFAULT;
	}
	kstrtoint(tmp,10,&tid);
	struct task_struct *task = get_pid_task(find_get_pid(tid),PIDTYPE_PID);
	unsigned long int utime = task-> utime;
	utime/=1000000;
	unsigned long int cts = task -> nvcsw + task -> nivcsw;
	sprintf(str,"\tThreadID:%d Time:%lu(ms) context switch time:%lu",tid,utime,cts);
	//kfree(str);
	return count;
}

static int my_proc_open(struct inode *inode,struct file *file){
	return single_open(file,my_proc_show,NULL);
}
static struct proc_ops my_fops={
	//.proc_owner = THIS_MODULE,
	.proc_open = my_proc_open,
	.proc_release = single_release,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_write = my_proc_write
};
sssss
static int __init my_init(void){
	struct proc_dir_entry *entry;
	entry = proc_create("thread_info",0666,NULL,&my_fops);
	if(!entry){
		return -1;	
	}else{
		printk(KERN_INFO "create proc file successfully\n");
	}
	return 0;
}

static void __exit my_exit(void){
	remove_proc_entry("thread_info",NULL);
	printk(KERN_INFO "Goodbye world!\n");
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");

