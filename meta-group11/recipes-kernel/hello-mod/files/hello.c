/**
 * @file    hello.c
 * @author  Andrew Gergis
 * @date    27 Sep 2023
 * @version 0.1
 * @brief  A simple Loadable Kernel Module that greets you when you load it.
 * 	   Exercise 02 of real timwsystems course
*/

#include <linux/init.h>             // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andreas Stergiopoulos");
MODULE_AUTHOR("Andrew Gergis");
MODULE_DESCRIPTION("A simple Loadable Kernel Module that greets you when you load it.");
MODULE_VERSION("0.1");

static  char* param = "Andrews";
module_param(param, charp, S_IRUGO);
MODULE_PARM_DESC(param, " The name to be displayed");

/** @brief The LKM initialization function.
 *  @return 0 if module is loaded successfully.
 */
static int __init greeting_hello(void)
{
	printk(KERN_INFO "Hello  %s!\n", param);
	return 0;
}

/** @bried The LKM cleanup function.
 * 
 */
static void __exit greeting_goodbye(void)
{
	printk(KERN_INFO "Goodbye Cruel World! Goodbye %s\n",param);
}


module_init(greeting_hello);
module_exit(greeting_goodbye);
