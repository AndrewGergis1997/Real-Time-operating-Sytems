# Exercise 03 - Q&A

## 1. What is the goal of this exercise? What did you accomplish?
The goal of this exercise is to get familiar with the workflow of Linux kernel debugging and how a Loadable Kernel
Module should generally behave.

## 2. Where is the `evil-tests` script installed inside the image? Where is the accompanying data installed? Describe how this is set up in Yocto.
Test file: /opt/evil-tests/sbin/evil-tests.sh
Data file: /opt/evil-tests/share/Data.txt
The installation at those paths is done with the do_install function inside the evil-tests.bb recipe.

## 3. How are the LKM and `evil-tests` added to `core-image-minimal`? Briefly describe what sets apart the `evil-tests` recipe compared with most recipes included in `meta-groupXX`, `meta-pynq`, `meta-xilinx` and `poky`.
The LKM and the tests are added to the core-image-minimal using the meta-compce460/recipes-core/images/core-image-minimal.bbappend file. This file when parsed, it is appended on the recipe core-image-minimal. However, this can be done only if we add the meta-compce460 layer on the build configuration.

## 4. How many buffers does the `evil` LKM use? List their names, their size and their purpose. How is their memory allocated?
The evil LKM uses 3 buffers and one storage pool. The first buffer is the buffer givn as an input to the store attribute. This buffer has a size of 4096 byte and its allocation is handled by the operating system. The second one is the input_buf. This is a buffer that is statically allocated at compile time and its handled by the LKM store trait and tasklet. This buffer is created by us the programmers. Finally, the last buffer is the output buffer that is passed as a parameter in the show attribute of the module. As in the show attribute, this buffer is 4096 bytes long and its allocation is handled by the operating system. 4096 bytes is the size of one page.

## 5. How are user-input strings stored inside the module? How does the module iterate over this list? What's the loop end condition?
They are stored in the data\_storage array which is 4096 bytes long. This array is allocated from the kernel address space on the initialization of the LKM. The module iterate over the list using the sprintf function and the bytes variable. The sprintf identifies strings at the memory location pointed by bytes. The bytes variable is initialized at 0 and after each consecutive iteration it gets assigned the return value of sprintf plus one (accounting for the string termination character). This way, the bytes variable always points to the location in the data\_storage pool where the next string (if one exists) may be found. This variable is bounded by the bytes\_stored variable, because at any point in time we know the number of bytes stored in the data\_storage array. The loop's condition is the bytes variable to be smaller than the bytes\_stored variable. This way, when we have read our last string in the storage pool, the bytes and bytes\_stored variables will be equal and so we will break out of the loop.

## 6. What is the path for the `evil` LKM's sysfs entry? How is it determined? How is it mapped to the module functionality?
Path: /sys/kernel/evil_module/evil
This path is determined by the programmer and it is created at the time the LKM is loaded into the kernel. It is mapped to the functionality using the attributes show and store. The sysfs entry is something that basically looks like a file, that when we write data into it the OS is calling the .store attribute and when we read data from it the OS calls the .show attribute.

## 7. What bugs did you find/fix? Categorize them and briefly describe each matching *category* (what's the *impact* of each *category*? How common do you *think* it is?)
I cannot remember all the bugs to be honest, but the most striking were the following:
Unbounded memory accesses
 * The show attribute had a loop which in certain conditions would not end and thus start poking in illegal addresses
 * The store attribute didn't check if the data it was about to store in the input\_buf could actually fit in the inpu\_buf
The tasklet struct was not initialized properly
There was a null pointer derefrence at some point
There were local variables that were not initialized properlys
The bytes\_stored variable was not properly maintained throughout the program
The most important ones were the unbounded memory accesses/buffer overflows. Those errors are unfortunately very common, can be proven dangerous both for the safety and the security of a system and they account for the biggest part of software vulnerabilities out there.

## 8. What are out-of-bounds accesses and stack overflows? Are they relevant for this exercise? What could be the consequences of such defects in a LKM?
Out-of-bounds accesses are accesses in memory which happen outside the memory section on which a program has a rightful access. Stack overflows are situations in which the calling stack of a program surpasses its predefined boundaries and may overflow to the memory locations of other programs. Both of those are relevant for the exercise because, especially the first one as it was one of the most prevalent bugs inside the evil module. Those bugs can have a lot of consequences, with the lightest being a kernel crash and the most severe a compromise of the system or confidential information about it to a malicious agent.

## 9. Feedback (what was difficult? what was easy? how would you improve it?)
The most difficult part was trying to understand the evil-tests.sh file. One thing that could be improved would be to make this file a little less cryptic by adding some comments about what it is supposed to do and what is supposed to expect at each test.
