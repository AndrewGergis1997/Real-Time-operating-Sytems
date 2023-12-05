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

## 7. Document the RT performance differences and their reasons

## 8. What is the goal of this exercise? What did you accomplish?

## 9. Feedback (what was difficult? what was easy? how would you improve it?)
echo "Adding execute permissions to copied files..."
ssh root@$ip_address "chmod +x $PYNQ_DIR/*"

Added those commands on line 81 due to permission denied error.

