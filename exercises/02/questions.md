# Exercise 02 - Q&A

## 1. What is the goal of this exercise? What did you accomplish?
To create a layer, add it to the bitbake layers of the build, make a simple recipe with a Kernel Loadable Module and deploy it in the image as well.

## 2. What is a Yocto layer? What are the essential requirements to correctly define a new one?
Layers are Yocto abstractions that are used to separate different types of customizations from each other.

## 3. What priority did you assign to your layer? How and why?
Our layer was assigned automatically a priority of 6, because it needs to have a lower priority than base Yocto layers.

## 4. What does LKM stand for (in the context of this exercise)? What does it mean and what are the characteristics of a LKM? What's the alternative?
Loadable Kernel Module. It is used in the process of adding or removing sections of code to/from 
linux kernel at run time. It supports multiple drivers, it is more flexible as it can be loaded and unloaded during run time. The other alternative would be to compile the driver modules directly with the kernel which means thant each time that we wanted to add or remove a module we would have to rebuild the whole kernel from scratch. 

## 5. What does Out-of-Tree Module mean? What's the alternative and what are the differences?
An Out-Of-Tree module is a module that is not included in the main Linux kernel tree that is monitored by Linus.

## 6. How did you define the module metadata? How does it work?
The module metadata are pieces of information like the license, the author names, the module description and the module version. These pieces of information are defined using their respective macros.

## 7. How is the module built? How does it get installed inside `core-image-minimal`?
The module is built using a standard makefile. This makefile is used during cross-compilation from BitBake. It gets installed inside the core-image-minimal because we have added the core-image-minimal.bbappend file on our layer, in the recipes-core subfolder. This file contains the LKMs that should be added to the image and is appended to the image recipe.

## 8. What is the path for the `sysfs` entry for the module? List and explain its contents.
root@pynq:/sys/module/hello# ls -al
drwxr-xr-x    6 root     root             0 Sep 27 16:53 .
drwxr-xr-x   59 root     root             0 Sep 27 16:16 ..
-r--r--r--    1 root     root          4096 Sep 27 16:54 coresize
drwxr-xr-x    2 root     root             0 Sep 27 16:54 holders
-r--r--r--    1 root     root          4096 Sep 27 16:54 initsize
-r--r--r--    1 root     root          4096 Sep 27 16:54 initstate
drwxr-xr-x    2 root     root             0 Sep 27 16:54 notes
drwxr-xr-x    2 root     root             0 Sep 27 16:54 parameters
-r--r--r--    1 root     root          4096 Sep 27 16:54 refcnt
drwxr-xr-x    2 root     root             0 Sep 27 16:54 sections
-r--r--r--    1 root     root          4096 Sep 27 16:54 srcversion
-r--r--r--    1 root     root          4096 Sep 27 16:54 taint
--w-------    1 root     root          4096 Sep 27 16:53 uevent
-r--r--r--    1 root     root          4096 Sep 27 16:54 version

## 9. Feedback (what was difficult? what was easy? how would you improve it?)
Finding the correct hello world template for the module was time consuming.
Initializing PuTTy for the first time, nothing seemed to appear on the screen until any button was pressed was a bit misleading.
