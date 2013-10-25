#ifndef __MODUL2_H__
#define __MODUL2_H__

#define COUNT 1


int mymod_read(struct file *f, char __user *buffer, size_t s, loff_t *off);
int mymod_ioctl(struct inode *i, struct file *f, unsigned int arg1, unsigned long arg2);
int mymod_release (struct inode *i, struct file *f);
int mymod_open (struct inode *i, struct file *f);

struct file_operations my_fops = {
	owner: THIS_MODULE,
	read: mymod_read,
	ioctl: mymod_ioctl,
	open: mymod_open,
	release: mymod_release,
};

struct t_info
{
	int num_entrades;
	int num_sortides_ok;
	int num_sortides_error;
	unsigned long long durada_total;
};


#endif
