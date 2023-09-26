# Exercise 02 - Q&A

## 1. What is the goal of this exercise? What did you accomplish?
To create a layer, add it to the bitbake layers of the build, make a simple recipe and deploy it in the image as well.

## 2. What is a Yocto layer? What are the essential requirements to correctly define a new one?
Layers are Yocto modules that are used to separate different types of customizations from each other.

## 3. What priority did you assign to your layer? How and why?
Our layer was assigned automatically a priority of 6, because it needs to have a lower priority than base Yocto layers.

## 4. What does LKM stand for (in the context of this exercise)? What does it mean and what are the characteristics of a LKM? What's the alternative?


## 5. What does Out-of-Tree Module mean? What's the alternative and what are the differences?

## 6. How did you define the module metadata? How does it work?

## 7. How is the module built? How does it get installed inside `core-image-minimal`?

## 8. What is the path for the `sysfs` entry for the module? List and explain its contents.

## 9. Feedback (what was difficult? what was easy? how would you improve it?)
