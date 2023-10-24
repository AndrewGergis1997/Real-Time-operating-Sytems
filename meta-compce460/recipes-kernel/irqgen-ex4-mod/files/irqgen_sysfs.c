/**
 * @file   irqgen_sysfs.c
 * @author Nicola Tuveri
 * @date   11 October 2018
 * @version 0.2
 * @target_device Xilinx PYNQ-Z1
 * @brief   A stub module to support the IRQ Generator IP block for the
 *          Real-Time System course (Bonus task: sysfs support).
 */

#define BONUS_SYSFS_IS_IMPLEMENTED // DONE: enable for the bonus exercise
#ifndef BONUS_SYSFS_IS_IMPLEMENTED

int irqgen_sysfs_setup(void) { return 0; }
void irqgen_sysfs_cleanup(void) { return; }

#else

# include <linux/kernel.h>           // Contains types, macros, functions for the kernel
# include <asm/io.h>                 // IO operations

# include <linux/sysfs.h>
# include <linux/device.h>
# include <linux/string.h>

# include "irqgen.h"                 // Shared module specific declarations

# define IRQGEN_ATTR_GET_NAME(_name) \
    irqgen_attr_##_name
# define IRQGEN_ATTR_RO(_name) \
    static struct kobj_attribute IRQGEN_ATTR_GET_NAME(_name) = __ATTR_RO(_name)
# define IRQGEN_ATTR_RW(_name) \
    static struct kobj_attribute IRQGEN_ATTR_GET_NAME(_name) = __ATTR_RW(_name)
# define IRQGEN_ATTR_WO(_name) \
    static struct kobj_attribute IRQGEN_ATTR_GET_NAME(_name) = __ATTR_WO(_name)

//static u32 count_handled_buf =0;
static ssize_t count_handled_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    // DONE: write to buf (as a string) the value stored inside the module data structure
	int32_t value = sprintf(buf, "Interrupts count handled: %d\n", irqgen_data->count_handled);
	if (value < 0){
		printk(KERN_ERR KMSG_PFX "sprintf failed\n");
		return 0;
	}
	return value+1;
}
IRQGEN_ATTR_RO(count_handled);

static ssize_t enabled_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    // DONE: read this value from the field in the CTRL register, print 1 or 0 a string to buf
	// HINT: check linux/bitfield.h to see how to use the bitfield macroes
	u32 retval = ioread32(IRQGEN_CTRL_REG);
	u32 temp = FIELD_GET(IRQGEN_CTRL_REG_F_ENABLE, retval);
	int32_t value = sprintf(buf, "Interrupts count handled: %d\n", temp);
	if (value < 0){
		printk(KERN_ERR KMSG_PFX "sprintf failed\n");
		return 0;
	}
	return retval+1;
}

static ssize_t enabled_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    bool var;
    if (strtobool(buf, &var) < 0)
        return -EINVAL;

    if (var)
        enable_irq_generator();
    else
        disable_irq_generator();

    return count;
}
IRQGEN_ATTR_RW(enabled);

static u32 delay_store_buf = 0;
static ssize_t delay_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    // DONE: check boundaries, then store the value in delay_store_buf
	// HINT: use kstrtoul()
	u32 value = kstrtoul(buf, 0, (unsigned long *)&delay_store_buf);
	if (value == 0){						/* If the buffer contains a valid integer... */
		if (delay_store_buf <= 16383){		/* If the value is in bounds... */
			u32 regval = 0 | FIELD_PREP(IRQGEN_GENIRQ_REG_F_DELAY, delay_store_buf);
			iowrite32(regval, IRQGEN_GENIRQ_REG);
			return count;
		}
		else{								/* If the value is NOT in bounds... */
			printk(KERN_INFO KMSG_PFX "The input is larger than expected, which is 16383");	
			return count;			
		}	
	}
	return value;
}
IRQGEN_ATTR_WO(delay);

static ssize_t amount_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    unsigned long val;
    // DONE: save in val, then check boundaries
    // HINT: use kstrtoul()
	u32 temp = kstrtoul(buf, 0, &val);
	if (temp == 0){						/* If the buffer contains a valid integer... */ 
		if (val >= 1 && val <= 4095){	/* If the value is in bounds... */
			do_generate_irqs(val, 0, delay_store_buf);
    		return count;
		}
		else {							/* If the value is NOT in bounds... */
			printk(KERN_INFO KMSG_PFX "The input is out of range, range = [1, 4095]");
			return count;
		}			
	}
	return temp;	    
}
IRQGEN_ATTR_WO(amount);


/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *attrs[] = {
    // DONE: add entries for `enabled`,`delay`,`amount`
    &IRQGEN_ATTR_GET_NAME(count_handled).attr,
	&IRQGEN_ATTR_GET_NAME(enabled).attr,
	&IRQGEN_ATTR_GET_NAME(delay).attr,
	&IRQGEN_ATTR_GET_NAME(amount).attr,
    NULL,   /* need to NULL terminate the list of attributes */
};

/*
 * An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.
 */
static struct attribute_group attr_group = {
    .attrs = attrs,
};

static struct kobject *irqgen_kobj = NULL;

int irqgen_sysfs_setup(void)
{
    int retval = 0;

    /*
     * Create a simple kobject with the name of DRIVER_NAME,
     * located under /sys/kernel/
     *
     * As this is a simple directory, no uevent will be sent to
     * userspace.  That is why this function should not be used for
     * any type of dynamic kobjects, where the name and number are
     * not known ahead of time.
     */
    irqgen_kobj = kobject_create_and_add(DRIVER_NAME, kernel_kobj);
    if (IS_ERR(irqgen_kobj)) {
        printk(KERN_ERR KMSG_PFX "kobject_create_and_add() failed.\n");
        return PTR_ERR(irqgen_kobj);
    }

    /* Create the files associated with this kobject */
    retval = sysfs_create_group(irqgen_kobj, &attr_group);
    if (0 != retval) {
        printk(KERN_ERR KMSG_PFX "sysfs_create_group() failed.\n");
        // DONE: decrease ref count for irqgen_kobj
		sysfs_remove_group(irqgen_kobj, &attr_group);
		kobject_put(irqgen_kobj);
    }

    return retval;
}

void irqgen_sysfs_cleanup(void)
{
    if (irqgen_kobj){
        // DONE: decrease ref count for irqgen_kobj
		sysfs_remove_group(irqgen_kobj, &attr_group);
		kobject_put(irqgen_kobj);
	}
}

#endif /* !defined(BONUS_SYSFS_IS_IMPLEMENTED) */
