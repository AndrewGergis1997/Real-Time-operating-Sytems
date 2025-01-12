#include <linux/kernel.h>           // Low-level kernel macros
#include <linux/module.h>           // Needed for dynamic module loading
#include <linux/interrupt.h>        // Tasklets
#include <linux/slab.h>             // kmalloc
#include <linux/device.h>           // sysfs functions
#include <linux/uaccess.h> 			// Copy from userland

#define SYSFS_FILE_ATTR_NAME "evil"

#define STORAGE_SIZE PAGE_SIZE // DON'T CHANGE IN FINAL REVISION
#define INPUT_BUFSIZE 1000     // DON'T CHANGE IN FINAL REVISION

// Dynamic and static allocation for the sake of example
char *data_storage = NULL;
char input_buf[INPUT_BUFSIZE];

struct tasklet_struct* tasklet = NULL;
int32_t bytes_stored = 0;

// A standalone kobject for a sysfs entry
static struct kobject* evil_kobj = NULL;

static void do_tasklet(unsigned long data)
{
    int32_t retval = 0;
	printk(KERN_INFO "EVIL: tasklet triggered\n");
	
	/* The tasklet expects to read the data from the data_storage pool. */
    if(bytes_stored+strlen((char *)data) >= STORAGE_SIZE-1) {
        printk(KERN_INFO "EVIL: storage full\n");
        return;
    }

    // Replace 'a's with ' ' in the name of evilness
    strreplace((char *)data, 'a', ' ');

    retval = sprintf(&data_storage[bytes_stored], "%s", (char *)data);
	printk(KERN_INFO "EVIL: data written in position: %d\n", bytes_stored);

    if(retval < 0) {
        printk(KERN_ERR "EVIL: sprintf failed\n");
    } 
	else if (retval > 0) {
        // Null-character excluded from the sprintf return value so 1 should be added
        bytes_stored += retval+1;
        printk(KERN_INFO "EVIL: bytes stored: %d\n", bytes_stored);
    }

}

// The sysfs attribute invoked when writing
static ssize_t store_evil(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
	
	/* The main logic for this function is that we take the data from the  
	input buffer of the OS and save them in the intermediary input_buffer  
	from which the tasklet is going to pull the data and manipulate them.*/

	if (strlen(buf) + 1 > INPUT_BUFSIZE) {
		printk(KERN_ERR "EVIL: ERROR: too much data, buffer will overflow\n");
		return -EINVAL;		/* Write failure constant */
	}

	printk(KERN_INFO "EVIL: Preparing to write %d characters into buffer...\n", strlen(buf));
	
	int retval = 0;
	int bytes = 0;
	retval = copy_from_user(input_buf, buf, INPUT_BUFSIZE);

	if (retval != 0) {
		printk(KERN_ERR "EVIL: copy_from_user failed to copy to input_buf\n");
	}
	else {
		
		printk(KERN_INFO "EVIL: %d characters written into buffer...\n", strlen(input_buf));
		bytes = strlen(input_buf) + 1;
		tasklet_schedule(tasklet);
	}
	
	/* When reading from the input buffer, Linux expects the process to consume all the 
	 * products of the buffer and will keep calling store again and again until all the 
	 * data in the buffer has been consumed. For this reason, we must return the count
	 * variable which is the size in bytes of the process input buffer. */
    return count;
}

// The sysfs attribute invoked when reading from the file
static ssize_t show_evil(struct device *dev, struct device_attribute *attr, char *buf) {
    uint32_t bytes = 0;
    int32_t retval = 0;

    // Go through the data storage and write all found strings to the output buffer
    while(retval < bytes_stored) {
        retval += sprintf(&buf[bytes], "%s", &data_storage[bytes]);
		
        if(retval == 0) break;				/* If the return value is zero then exit loop */
		else if (retval > 0) retval++;		/* If it is positive, account for the termination character */
        else {								/* If it is negative, report failure and exit loop */
			printk(KERN_ERR "EVIL: failed to copy to output buffer\n");
			break;
		}
		
		// Null-character excluded from the sprintf return value so 1 should be added
        bytes = retval;
    }

	printk("MUAHAHAHA\n");
	return bytes;
	
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


    /* Initialize the tasklet */
	/* input_buf is our input buffer, we pass is as an argument to the tasklet. */
    tasklet_init(tasklet, do_tasklet, (unsigned long)input_buf);

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
