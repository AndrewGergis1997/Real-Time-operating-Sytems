# Exercise 05 - Q&A

## 1. What is the goal of this exercise? What did you accomplish?
The goal is to add the functionality of counting latency and addind delay to interrupts


## 2. Describe your verilog coding process in 2 paragraphs
skipped bonus


## 3. How much difference there is in latency between the regular driver build and the debug driver build? Why?
IRQGEN_DBG: latency for last handled IRQ: 4130ns.
IRQGEN: latency for last handled IRQ: 1210ns.
the difference is almost 3000ns
Since the debug driver contain more debugging information and run time checks this takes
more time to excute,


## 4. How can you find the base address for the IP block register space in Vivado? Does it match the contents of the devicetree?
skipped bonus


## 5. Feedback (what was difficult? what was easy? how would you improve it?)
The bonus is unfair as it requires prior knowledge in verilog or VHDL which is not included in 
the course description.
