# Exercise 07 - Q&A

## 1. What is the goal of this exercise? What did you accomplish?

	The goal is to add a character device and experiment with interaction between regular userland applications and the driver. Then testing with a simple app the functionallity of the driver.

## 2. What is a Character Device? How is it different from a Block Device?

	In character device communication is done through sending and receiving single character at a time. In character device no buffering is performed. 
	In block device communication is established through sending and receiving entire blocks of data. In block device are accessed through a cache. 

## 3. What design limits of the Ex6 version of `irqgen-mod` do we try to address adding a character device?

	Adding cahracer device exposes the properties and functionalities od device. This can be used in the exchange of data between the device and the user application, 
	this is giving us the opportunity to control the real physical device. 

## 4. What system calls are we implementing for the filesystem representation of our character device?

    .open = irqgen_cdev_open,
    .release = irqgen_cdev_release,
    .read = irqgen_cdev_read,

## 5. How does a device node differ from a regular file? What things are similar/identical?

	In regular file read and write operations usually involve data to be stored in a file. On the other hand, in device node the communication is established through physical device.
Both supports file system operations such as read write excute

## 6. Which parts of our LKM code are potentially running in parallel? Which parts could interact in a conflicting way? Make a few concrete examples of conditions in which conflicts in concurrency may arise.

## 7. In the proposed structure, we opted for a single lock for the whole `irqgen_data` structure. Could you describe advantages and disadvantages of a single lock vs several locks? What about the effects on the IRQ handling latency?

Using single lock presents more simple code and reduced probabilty of dead locks. However in some situtions this could increase the latency
Using multible locks has higher possibilities of decreasing the latency. Neverthless it adds more complixity to the code and increases the overhead.

## 8. To avoid concurrency issues among the different parts of `irqgen-mod` you had to choose a synchronization mechanisms. What kind did you choose? Why? Where? What alternatives did you consider? Why did you reject them?

We used the spin lock mechanism. to protected every read and write operations in the files which might have concurrent access. 
We could also use the nutexs we have not rejected them but the spin lock offered more ways to handle the locking such as 
spin_lock
spin_lock_irq

## 9. To optimize even more the IRQ handling latency we should abandon the monolithic handler function for a more efficient pattern: what's its name? Describe how the code currently handling the requests would be organized and describe a few reasons to choose the alternative over the monolithic style.

Interrupt Coalescing.

The monolithic IRQ handler method in the current code is called irqgen_irqhandler. The two components of the IRQ handler would be divided in the more efficient pattern fast IRQ handler and Since the work runs in a kernel thread, it does not affect the responsiveness of the system to other interrupts.

## 10. What is an SDK?

## 11. In the context of the Yocto Project, what are the differences between the Standard and the Extensible SDK?

## 12. Describe your workflow in developing and testing the `irqgen_statistics` app

## 13. Feedback (what was difficult? what was easy? how would you improve it?)
	Impleminting the statistics app and testing it with the character device driver was challenging.
