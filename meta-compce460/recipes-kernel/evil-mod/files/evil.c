#include <linux/kernel.h>           // Low-level kernel macros
#include <linux/module.h>           // Needed for dynamic module loading
#include <linux/interrupt.h>        // Tasklets
#include <linux/slab.h>             // kmalloc
#include <linux/device.h>           // sysfs functions
#include <linux/mutex.h>	    	// Mutexes

#define SYSFS_FILE_ATTR_NAME "evil"

#define STORAGE_SIZE PAGE_SIZE // DON'T CHANGE IN FINAL REVISION
#define INPUT_BUFSIZE 1000     // DON'T CHANGE IN FINAL REVISION

// Dynamic and static allocation for the sake of example
char *data_storage = NULL;
char input_buf[INPUT_BUFSIZE];

/* Mutex that guards access to common variables */
struct mutex drv_mutex;

struct tasklet_struct* tasklet = NULL;
int32_t bytes_stored = 0;
// A standalone kobject for a sysfs entry
static struct kobject* evil_kobj = NULL;

static void do_tasklet(unsigned long data)
{
    int32_t retval = 0;
	printk(KERN_INFO "EVIL: tasklet triggered\n");
	
	/* The tasklet expects to read the data from the data_storage pool. */
    /*if(bytes_stored+strlen((char *)data) >= STORAGE_SIZE-1) {
        printk(KERN_INFO "EVIL: storage full\n");
        return;
    }*/

    mutex_lock(&drv_mutex);
    // Replace 'a's with ' ' in the name of evilness
    strreplace((char *)data, 'a', ' ');
	
    /* Check if the buffer size can handle it */
    /*if(bytes_stored+strlen((char *)data) >= STORAGE_SIZE) {
        printk(KERN_INFO "EVIL: buffer will overflow\n");
        return;
    }*/

    //retval = sprintf(&data_storage[bytes_stored], "%s", (char *)data);
    /*if(retval < 0) {
        printk(KERN_ERR "EVIL: sprintf failed\n");
    } else {
        // Null-character excluded from the sprintf return value so 1 should be added
        bytes_stored += retval+1;
        printk(KERN_INFO "EVIL: tasklet handled %d bytes\n", bytes_stored);
    }*/
    mutex_unlock(&drv_mutex);
}

// The sysfs attribute invoked when writing
static ssize_t store_evil(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
    // What is the size that will be passed to input_buf ?
    // What will happen if the buff data exceeds BUFFER_SIZE ?
    // Understand how does sprintf work ?
    // Read the user parameters

	/* In the write function, consecutive writes should be saved the one after
	the other. The way this code is implemented, each write overwrites the previous
	one. */
	/* Here we  have a possibility for buffer overflow! */
	/* Original code didn't use the bytes_stored variabe! */
	
	/* First let's check if the input can fit in the storage pool: */ 
	if (bytes_stored + strlen(buf) + 1 > STORAGE_SIZE) {
		printk(KERN_ERR "EVIL: Storage overflow, write rejected!\n");
		return 0;
	}
    
	/* If we can place the data in the storage, lock the mutex: */
	mutex_lock(&drv_mutex);

	/* sprintf has 3 return cases:
	 * 	-> Negative if it fails to copy the string
	 * 	-> Zero if the string was empty
	 * 	-> Number of CHARACTERS copied if copy is successful (TERMINATION 
	 * 		CHARACTER NOT INCLUDED!) */
	int retval = 0;
	retval = sprintf(&data_storage[bytes_stored], "%s", buf);
	
	/* If we fail to copy the string we should notify, else hust account
	for the termination character. */
	if (retval < 0) {
		printk(KERN_ERR "EVIL: Failed to copy input to data storage\n");
	}
	else if (retval > 0) {
		retval++;
		bytes_stored = retval;
		printk(KERN_INFO "EVIL: stored %d bytes\n", retval);
	}
	
	/* DANGER: MUTEX SHOULD BE GUARANTEED TO BE RELEASED NO MATTER THE EXIT
	POINT OF THE FUNCTION! */
	/* We are done manipulating the common variables, release the mutex. */
	mutex_unlock(&drv_mutex);

    // Run a tasklet to perform string manipulation and storing the data
    tasklet_schedule(tasklet);

    return retval;
}

// The sysfs attribute invoked when reading from the file
static ssize_t show_evil(struct device *dev, struct device_attribute *attr, char *buf) {
    uint32_t bytes = 0;
    int32_t retval = 0;
    
    /* I FIGURED THIS OUT: IT IS AN UNBOUNDED READ/buffer overflow! */
	/* The first time the sprintf is called, it copies the data from the storage 
	to the output, copying every byte until \n is read. For each call after that, 
	it is not guaranteed that an escape character will be found, so it reads until it 
	overflows. When it overflows, it is stopped by the memory protection unit 
	of the CPU. */
    
    mutex_lock(&drv_mutex);

	int i = 0;		/* Counter */
	int j = 0;		/* Holds the position of the previous terminating character */
	while (i < bytes_stored) {
		/* If you find a termination character... */
		if (data_storage[i] == '\0') {
			retval = sprintf(buf, "%s",&data_storage[j]);
			j += retval + 1;
		}
		i++;
	}
	//retval = 

	/* If we copy any bytes, we must account for the terminator that sprintf doesn't count. */
	//if (retval > 0) retval++;

	mutex_unlock(&drv_mutex);

    printk("MUAHAHAHA\n");

    return j;
}

//
//                     Kobject attributes declaration
//
static struct device_attribute dev_attr_evil = {
    .attr = {
        .name = SYSFS_FILE_ATTR_NAME,
        .mode = S_IRUGO|S_IWUSR,
    },
    .show = show_evil,
    .store = store_evil,
};


// The kernel module init function
static int32_t __init evil_init(void)
{
    int32_t retval;

    printk(KERN_INFO "EVIL: THIS IS THE EVIL MODULE\n");

    data_storage = (char*)kmalloc(STORAGE_SIZE, GFP_KERNEL);
    if(data_storage == NULL) {
        printk(KERN_ERR "EVIL: data storage memory allocation failed\n");
        retval = -ENOMEM;
        goto error_alloc_data_storage;
    }

    // Create a sysfs directory entry under /sys/kernel/
    evil_kobj = kobject_create_and_add("evil_module", kernel_kobj);
    if(evil_kobj == NULL) {
        printk(KERN_ERR "EVIL: kobject_create_and_add failed\n");
        retval = -EINVAL;
        goto error_kobject_create;
    }

    // Add attributes to the kobject
    // The attributes are presented as a file in the created directory
    retval = sysfs_create_file(evil_kobj, &dev_attr_evil.attr);
    if(retval) {
        printk(KERN_ERR "EVIL: sysfs_create_file failed\n");
        goto error_sysfs_create;
    }

    /* Allocate space for the tasklet_struct */
    tasklet=kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
    if (tasklet == NULL) {
        printk(KERN_ERR "EVIL: Failed to allocate space for the tasklet_struct\n");
        goto error_tasklet_failure;
    }

    mutex_init(&drv_mutex);

    /* Initialize the tasklet */
	/* data_storage is our main data pool, we pass is as an argument to the tasklet. */
    tasklet_init(tasklet, do_tasklet, (unsigned long)data_storage);

    return 0;

 error_tasklet_failure:
 error_sysfs_create:
	sysfs_remove_file(evil_kobj, &dev_attr_evil.attr);
    kobject_del(evil_kobj);
 error_kobject_create:
    kfree(data_storage);
 error_alloc_data_storage:
    printk(KERN_ERR "EVIL: error occurred while evil init!\n");
    return retval;
}

// The kernel module exit function
static void __exit evil_exit(void)
{
    tasklet_kill(tasklet);
    kfree(tasklet);
    sysfs_remove_file(evil_kobj, &dev_attr_evil.attr);
    kobject_del(evil_kobj);
    kfree(data_storage);

    printk(KERN_INFO "EVIL: MUAHAHAHA\n");
}

module_init(evil_init);
module_exit(evil_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("The evil kernel module for the Real-time systems course");
MODULE_AUTHOR("Jan Lipponen <jan.lipponen@wapice.com>");
MODULE_AUTHOR("Andreas Stergiopoulos <andreas.stergiopoulos@tuni.fi");
MODULE_AUTHOR("Andrew Gergis <andrew.gergis@tuni.fi");
MODULE_VERSION("1.0");
