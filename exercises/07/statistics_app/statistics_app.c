#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

#define BUF_SIZE 500
#define IRQ_LINES 16

volatile sig_atomic_t sigint_received = false;

void sigint_handler(int sig)
{
    sigint_received = true;
}

struct stats 
{
	int line_occurences[IRQ_LINES];
	float latency_average[IRQ_LINES];
	int latency_max[IRQ_LINES];
};

void main(){
	printf("Statistics app\n");

    /**
     * Write buffer
    */
    char buf[BUF_SIZE];
	char out_buf[BUF_SIZE];

    /**
     * Data variables
    */
    int irq_line = 0;
    int latency = 0;
    int timestamp_current = 0;
    int timestamp_previous = 0;
	struct stats aggregator;
	
	
	/**
	 * Initialize values of aggregator
	*/
	for (int i=0; i<IRQ_LINES; i++)
	{
		aggregator.line_occurences[i] = 0;
		aggregator.latency_average[i] = 0.0f;
		aggregator.latency_max[i] = 0;
	}
	
    /**
     * Var to capture sprintf return value
    */
    int bytes_written = 0, bytes_read = 0;

    /**
     * Register sigint_handler
    */
    signal(SIGINT, sigint_handler);

    
    while(!sigint_received){
        /**
		* Read input from stdin
		*/
		if (fgets(buf, BUF_SIZE, stdin) != NULL)
		{
			
		    /**
		     * Read data from formatted string
		    */
		    sscanf(
				buf, 
				"%d, %d, %d\n", 
				&irq_line, 
				&latency, 
				&timestamp_current
			);
			
			/**
			 * Compile statistics
			*/
			aggregator.line_occurences[irq_line]++;
			aggregator.latency_average[irq_line] += latency;
			
			if (aggregator.latency_max[irq_line] < latency) 
			{
				aggregator.latency_max[irq_line] = latency;
			}
		}
		
    }

	printf("Sigint received\n");
	
	/**
	* Calculate the mean leatency
	*/
	for (int i=0; i<IRQ_LINES; i++)
	{
		if (aggregator.line_occurences[i] != 0) aggregator.latency_average[i] /= aggregator.line_occurences[i];
		else continue;
		
	}
	
	/**
	 * Print the statistics for each IRQ line
	*/
	int total_occurences = 0;
	int max_latency = 0;
	float average_latency = 0.0f;
	
	for (int i=0; i<IRQ_LINES; i++)
	{
		bytes_written = sprintf(
			out_buf, 
			"%d, %d, %f, %d\n",
			i,
			aggregator.line_occurences[i],
			aggregator.latency_average[i],
			aggregator.latency_max[i]
		);

		total_occurences += aggregator.line_occurences[i];
		average_latency += aggregator.latency_average[i];
		if (aggregator.latency_max[i] > max_latency)
		{
			max_latency = aggregator.latency_max[i];
		}

		write(STDOUT_FILENO, out_buf, bytes_written);
        fflush(stdout);
	}
	
	int non_zero_lines = 0;
	for (int i=0; i<IRQ_LINES; i++)
	{
		if (aggregator.line_occurences[i] != 0)
		{
			non_zero_lines++;
		}
	}
	
	if(non_zero_lines != 0)
	{
		bytes_written = sprintf(
			out_buf, 
			"%d, %d, %f, %d\n",
			-1,
			total_occurences,
			average_latency / non_zero_lines,
			max_latency
		);
	}
	else
	{
		bytes_written = sprintf(
			out_buf, 
			"%d, %d, %f, %d\n",
			-1,
			total_occurences,
			average_latency,
			max_latency
		);
	}

	write(STDOUT_FILENO, out_buf, bytes_written);
    fflush(stdout);
    
}
