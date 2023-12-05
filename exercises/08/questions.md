# Exercise 08 - Q&A

## 1. What is `uname -a` output on the PYNQ board before starting Ex 8?
Linux pynq 4.9.0-xilinx-v2017.3 #1 SMP PREEMPT Thu Aug 9 16:33:05 UTC 2018 armv7l GNU/Linux

## 2. What is the average latency and worst case latency you measure after Ex 7 using the application you developed?
Average latency: 320.821106ns
Worst case latency: 350470ns

## 3. What is `uname -a` output on the PYNQ board after rebooting with the new kernel image?
Linux pynq 4.9.0-rt1-xilinx-v2017.3 #1 SMP PREEMPT RT Mon Dec 4 13:58:39 UTC 2023 armv7l GNU/Linux

## 4. What is the average latency and worst case latency you measure in the new kernel image (same procedure as question 2)?
Average latency: 948.233948ns
Worst case latency: 207950ns

## 5. Compare the results of questions 2 and 4; do they differ significantly? why?
Yes, the results differ significantly. The average latency has increased dramatically but the worst case latency has decreased by a third. This happened because of the PREEMPT_RT patch we applied to the kernel.

## 6. Compare the plots for the 6 different profiles. Describe each of them and compare them?

latencies-no_rt-no_tort: The average perfomance seems stable at 205 IRQ latency with some fluctuations at 5000 IRQ latency and the worst performence relies on two peaks at IRQ request no 60 and 320 reaching 200000 IRQ latency clk

latencies-no_rt-tort: The performance is fluctuating between 200 and 600 IRQ latency clk with two anolomoly peaks reaching almost 4000 IRQ latency clk which means that the worest case here is less than before

latencies-rt-no_tort: Average performance here is mostly stable at 450 IRQ latency clk and fluctations between 300 and approximately 2000 IRQ latency clk. Again two anomoly cases at IRQ request nom 550 and 450 reaching 200000 IRQ latency clk

latencies-rt-tort: the performance here is almost same as the latencies-rt-no_tort except that the fluctations are reaching slightly near 3000 IRQ latency clk. Only one irregular behaviour at IRQ request nom 700 reaching exactly 200000 IRQ latency clk similar to the previous profile


latencies-opt-rt-no_tort: seems like changing sched_rt_runtime_us to 1000000 did not have that much of an effect on the avergage performance neither in the worest case latency clk value. but the irregular behaviour at IRQ request nom 195 this time 

latencies-opt-rt-tort: In this case there is no average performance line but range from 300 to 1500 IRQ latency clk. And the worest cases peaks only resides between 5000 and 6000 IRQ latency clk.

## 7. Document the RT performance differences and their reasons
The RT (Real-Time) Linux kernel with the RT_PREEMPT patchset generally provides more predictable and consistent performance compared to the non-RT Linux kernel.

The "no-RT Linux, delay=0, torture" scenario shows improved worst-case latency compared to the "no-RT Linux, delay=0, no torture," due to the real-time enhancements handling external stress more efficiently.
The optimization attempt with sched_rt_runtime_us did not yield significant improvements
## 8. What is the goal of this exercise? What did you accomplish?

The goal is to evaluate the performance of a Linux driver with the RT_PREEMPT patchset under different scenarios, including both normal and stressful conditions.

The exercise involves comparing the non-RT and RT Linux environments with and without additional stress, as well as optimizing the RT system using specified tuning parameters.

## 9. Feedback (what was difficult? what was easy? how would you improve it?)
echo "Adding execute permissions to copied files..."
ssh root@$ip_address "chmod +x $PYNQ_DIR/*"

Added those commands on line 81 due to permission denied error.

