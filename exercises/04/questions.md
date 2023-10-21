# Exercise 04 - Q&A

## 1. What is the goal of this exercise? What did you accomplish?
The goal of this exercise was to learn how to communicate with memory mapped IO devices and how to manipulate registers in a low level in order to make devices do stuff for you.

## 2. Summarize the functionality provided by the IRQ Generator IP block, then describe what is the purpose of the various registers exposed by the FPGA device. Finally, ignore the fact that you were instructed to look for the base address of the IRQ Generator register space in the provided `devicetree.dts` and instead imagine you were also the developer of the FPGA core design, where would you have found the base address of the register space?
The functionality of the IP IRQ genetor block is that of a programmable IRQ generator. It contains 4 32-bit registers that control IRQ generator and provide metadata to the programmer concerning the IRQ generator. The first two registers control the generation and acknowledgment of IRQs, whereas the two last ones provide metadata such as the amount of IRQs generated and their handling delay (not yet supported). As far as the base address is concerned, if we were the developers of the IP block we would probably have to find the base address from the AMBA port that we were about to connect it.

## 3. Describe the relationship between interrupt lines in the IRQ Generator, HW IRQ and Linux IRQ numbers, and how did you proceed to discover the IRQ number to use in the driver.
The IP block is connected to a Shared Peripheral Interrupts port named IRQF2P which maps its interrupts into HQIRQ line 61. HWIRQ line 61 maps into Linux IRQ 45. We found the correct Linux IRQ number by following the instructions provided. The HQIRQ line can be found on the documentation nad can be also deduced from the IRQF2P port base IRQ line.

## 4. How many interrupt lines are available in the documented FPGA core (refer to the spec, even if we only used one in the *beta* bitstream we were given)?
16 but only 1 (IRQ 0x00) is available in the current implementation.

## 5. What value is reported in the devicetree for the first IRQ line? How is it determined? (check the spec document, it has all the required information to map the line number to the HW IRQ on the processing system)
The first IRQ line is mapped to port 0x1b. It is determined by reading the first thruple of the interrupts array. 

## 6. Using the information detailed in the previous answer, what should be written in the `devicetree.dts` line describing the IRQ Generator interrupts if it were to describe all the 16 HW IRQs? (write the exact line as it would appear in the `devicetree.dts`, notice that the HW IRQs are not contiguous!). Each interrupt line is specified by three cells in the device tree file; what information does each of these three cells represent?
The cells of the thruples on the devicetree file specify the following; The first is the interrupt type (SPI vs PPI), the second one is the interrupt number and the third one is the sensitivity (rising edge, falling edge, low-level or high-level) which in our case is high-level sensitivity.

## 7. Why do we need to use `ioremap`, `ioread32` and `iowrite32`? Why do we want to use the Linux kernel bitfields macroes rather than using bitwise arithmetic directly?
To write or read respectively to 32 bit memory mapped IO registers. In order to use the proper coding style and to avoid making mistakes.

## 8. (BONUS, optional) Did you find any bug in the bitstream implementation while testing the sysfs interface?
We didn't find any bugs =(

## 9. Feedback (what was difficult? what was easy? how would you improve it?)
Finding the HWIRQ number was the most difficult part. Also we have no clue how to answer the first part of question 6.
