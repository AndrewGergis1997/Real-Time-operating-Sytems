/**
 * @file   irqgen.c
 * @author Nicola Tuveri
 * @date   11 October 2018
 * @version 0.2
 * @target_device Xilinx PYNQ-Z1
 * @brief   A stub module to support the IRQ Generator IP block for the
 *          Real-Time System course.
 */

#include <linux/init.h>             // Macros used to mark up functions e.g., __init __ex
#include <linux/module.h>           // Core header for loading LKMs into the kern
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel

#include <linux/interrupt.h>        // Interrupt handling functions
#include <asm/io.h>                 // IO operations
#include <linux/slab.h>             // Kernel slab allocator


#include "irqgen.h"                 // Shared module specific declarations

/* Linux IRQ number for the first hwirq line */
#define IRQGEN_FIRST_IRQ 45 // DONE: find the right Linux IRQ number for the first hwirq of the device

// Kernel token address to access the IRQ Generator core register
void __iomem *irqgen_reg_base = IRQGEN_REG_PHYS_BASE;

// Module data instance
struct irqgen_data *irqgen_data = NULL;

// Dummy identifier for request_irq()
static int dummy;

const char devname[] = "irq_generator";

// Spin lock Declaration
static spinlock_t irqgen_spinlock;

/* vvvv ---- LKM Parameters vvvv ---- */
static unsigned int generate_irqs = 0;
module_param(generate_irqs, uint, 0444);
MODULE_PARM_DESC(generate_irqs, "Amount of IRQs to generate at load time.");

static unsigned int loadtime_irq_delay = 0;
module_param(loadtime_irq_delay, uint, 0444);
MODULE_PARM_DESC(loadtime_irq_delay, "Set the delay for IRQs generated at load time.");

/* Makes sure that the input values for parameters are sane */
static int parse_parameters(void)
{
    if (generate_irqs > IRQGEN_MAX_AMOUNT) {
        printk(KERN_WARNING KMSG_PFX "generate_irqs parameter exceeded maximum value: capped at %ld.\n",
                IRQGEN_MAX_AMOUNT);
        generate_irqs = IRQGEN_MAX_AMOUNT;
    }

    if (loadtime_irq_delay > IRQGEN_MAX_DELAY) {
        printk(KERN_WARNING KMSG_PFX "loadtime_irq_delay parameter exceeded maximum value: capped at %ld.\n",
                IRQGEN_MAX_DELAY);
        loadtime_irq_delay = IRQGEN_MAX_DELAY;
    }

    return 0;
    // return -EINVAL;
}
/* ^^^^ ---- LKM Parameters ^^^^ ---- */


/* DONE: (1) implement the interrupt handler function */
static irqreturn_t irqgen_irqhandler(int irq, void *data)
{
#ifdef DEBUG
    printk(KERN_INFO KMSG_PFX "IRQ #%d received.\n", irq);
#endif

	// DONE: increment the `count_handled` counter before ACK

	spin_lock(&irqgen_spinlock);
	irqgen_data->count_handled += 1;
	// The irq intger is the linux irq number 
	// HINT: use iowrite32 and the bitfield macroes to modify the register fields
	u32 rawval = 0 | FIELD_PREP(IRQGEN_CTRL_REG_F_ENABLE,1)
		       | FIELD_PREP(IRQGEN_CTRL_REG_F_HANDLED,1)
		       | FIELD_PREP(IRQGEN_CTRL_REG_F_ACK,0);

	iowrite32(rawval, irqgen_reg_base + IRQGEN_CTRL_REG_OFFSET);

	spin_unlock(&irqgen_spinlock);
    return IRQ_HANDLED; // DONE: what should be returned on completion?
}

/* Enable the IRQ Generator */
void enable_irq_generator(void)
{
#ifdef DEBUG
    printk(KERN_INFO KMSG_PFX "Enabling IRQ Generator.\n");
#endif
    // DONE: use iowrite32 and the bitfield macroes to modify the register fields
	
	u32 regvalue = 0 | FIELD_PREP(IRQGEN_CTRL_REG_F_ENABLE, 1);
	iowrite32(regvalue, IRQGEN_CTRL_REG);
	printk(KERN_INFO KMSG_PFX "IRQ generator enabled.\n");
}

/* Disable the IRQ Generator */
void disable_irq_generator(void)
{
#ifdef DEBUG
    printk(KERN_INFO KMSG_PFX "Disabling IRQ Generator.\n");
#endif
    // DONE: set to zero the `amount` field, then disable the controller
	
	u32 regvalue = 0 | FIELD_PREP(IRQGEN_GENIRQ_REG_F_AMOUNT,  0);
	iowrite32(regvalue, IRQGEN_GENIRQ_REG);

	printk(KERN_INFO KMSG_PFX "IRQ amount set to 0.\n");
	
    // DONE: use iowrite32 and the bitfield macroes to modify the register fields
	regvalue = 0 | FIELD_PREP(IRQGEN_CTRL_REG_F_ENABLE,  0);
	iowrite32(regvalue, IRQGEN_CTRL_REG);

	printk(KERN_INFO KMSG_PFX "IRQ Generator disabled.\n");
}

/* Generate specified amount of interrupts on specified IRQ_F2P line [IRQLINES_AMNT-1:0] */
void do_generate_irqs(uint16_t amount, uint8_t line, uint16_t delay)
{
    u32 regvalue = 0
                   | FIELD_PREP(IRQGEN_GENIRQ_REG_F_AMOUNT,  amount)
                   | FIELD_PREP(IRQGEN_GENIRQ_REG_F_DELAY,    delay)
                   | FIELD_PREP(IRQGEN_GENIRQ_REG_F_LINE,      line);

    printk(KERN_INFO KMSG_PFX "Generating %u interrupts with IRQ delay %u on line %d.\n",
           amount, delay, line);

    iowrite32(regvalue, IRQGEN_GENIRQ_REG);
}

// Returns the latency of last successfully served IRQ, in ns
u64 irqgen_read_latency(void)
{	
    // not supported by current IP block implementation
	u32 cycles = ioread32(IRQGEN_LATENCY_REG);
	u64 latency = 10 * cycles;
    return latency;
}

// Returns the total generated IRQ count from IRQ_GEN_IRQ_COUNT_REG
u32 irqgen_read_count(void)
{
    // DONE: use ioread32 to read the proper register
	u32 rawval = ioread32(IRQGEN_IRQ_COUNT_REG);
	return rawval;
}

// Debugging wrapper for request_irq()
#ifdef DEBUG
static
int _request_irq(unsigned int _irq, irq_handler_t _handler, unsigned long _flags, const char *_name, void *_dev)
{
    printk(KERN_DEBUG KMSG_PFX "request_irq(%u, %p, %lu, %s, %p)\n",
            _irq, _handler, _flags, _name, _dev);
    return request_irq(_irq, _handler, _flags, _name, _dev);
}
#else
# define _request_irq request_irq
#endif


// The kernel module init function
static int32_t __init irqgen_init(void)
{
    int retval = 0;

    printk(KERN_INFO KMSG_PFX DRIVER_LNAME " initializing.\n");

    retval = parse_parameters();
    if (0 != retval) {
        printk(KERN_ERR KMSG_PFX "fatal failure parsing parameters.\n");
        goto err_parse_parameters;
    }
	printk(KERN_INFO KMSG_PFX DRIVER_LNAME " parameters parsed.\n");

    irqgen_data = kzalloc(sizeof(*irqgen_data), GFP_KERNEL);
    if (NULL == irqgen_data) {
        printk(KERN_ERR KMSG_PFX "Allocation of irqgen_data failed.\n");
        retval = -ENOMEM;
        goto err_alloc_irqgen_data;
    }
	printk(KERN_INFO KMSG_PFX DRIVER_LNAME " irq_gen data allocated.\n");

    /* DONE: Map the IRQ Generator core register with ioremap */
    irqgen_reg_base = ioremap_nocache(IRQGEN_REG_PHYS_BASE, IRQGEN_REG_PHYS_SIZE);
    if (NULL == irqgen_reg_base) {
        printk(KERN_ERR KMSG_PFX "ioremap() failed.\n");
        retval = -EFAULT;
        goto err_ioremap;
    }
	printk(KERN_INFO KMSG_PFX DRIVER_LNAME " ioremap_cache successful.\n");

    /* DONE: Register the handle to the relevant IRQ number */
    retval = _request_irq(IRQGEN_FIRST_IRQ, (irq_handler_t) irqgen_irqhandler, IRQF_TRIGGER_HIGH, devname, NULL);
    if (retval != 0) {
        printk(KERN_ERR KMSG_PFX "request_irq() failed with return value %d while requesting IRQ id %u.\n",
                retval, IRQGEN_FIRST_IRQ);
        goto err_request_irq;
    }
	printk(KERN_INFO KMSG_PFX DRIVER_LNAME " request_irq() done.\n");

    retval = irqgen_sysfs_setup();
    if (0 != retval) {
        printk(KERN_ERR KMSG_PFX "Sysfs setup failed.\n");
        goto err_sysfs_setup;
    }
	printk(KERN_INFO KMSG_PFX DRIVER_LNAME " sysfs setup done.\n");

    /* Enable the IRQ Generator */
    enable_irq_generator();

    if (generate_irqs > 0) {
        /* Generate IRQs (amount, line, delay) */
        do_generate_irqs(generate_irqs, 0, loadtime_irq_delay);
	printk(KERN_INFO KMSG_PFX DRIVER_LNAME " irq param received.\n");
    }

    return 0;

 err_sysfs_setup:
    // DONE: free the appropriate resource when handling this error step
	irqgen_sysfs_cleanup();
 err_request_irq:
    // DONE: free the appropriate resource when handling this error step
	free_irq(IRQGEN_FIRST_IRQ, NULL);
 err_ioremap:
    // DONE: free the appropriate resource when handling this error step
	iounmap(irqgen_reg_base);
	kfree(irqgen_data);
 err_alloc_irqgen_data:
 err_parse_parameters:
    printk(KERN_ERR KMSG_PFX "module initialization failed\n");
    return retval;
}

// The kernel module exit function
static void __exit irqgen_exit(void)
{
    // Read interrupt latency from the IRQ Generator on exit
    printk(KERN_INFO KMSG_PFX "IRQ count: generated since reboot %u, handled since load %u.\n",
	irqgen_read_count(), irqgen_data->count_handled);
    // Read interrupt latency from the IRQ Generator on exit
    printk(KERN_INFO KMSG_PFX "latency for last handled IRQ: %lluns.\n",
	irqgen_read_latency());


    // DONE: step through `init` in reverse order and disable/free/unmap allocated resources
	disable_irq_generator();
    irqgen_sysfs_cleanup(); 	// DONE: place this line in the right order
	free_irq(IRQGEN_FIRST_IRQ, NULL);
	iounmap(irqgen_reg_base);
	kfree(irqgen_data);
    printk(KERN_INFO KMSG_PFX DRIVER_LNAME " exiting.\n");
}

module_init(irqgen_init);
module_exit(irqgen_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module for the IRQ Generator IP block for the realtime systems course");
MODULE_AUTHOR("Jan Lipponen <jan.lipponen@wapice.com>");
MODULE_AUTHOR("Nicola Tuveri <nicola.tuveri@tut.fi>");
MODULE_AUTHOR("Andreas Stergiopoulos <andreas.stergiopoulos@tuni.fi>");
MODULE_AUTHOR("Andrew Gergis <andrew.gergis@tuni.fi>");
MODULE_VERSION("0.2");
