#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

#define BUF_SIZE 100
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
    /**
     * Write buffer
    */
    char buf[BUF_SIZE];

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

    do
    {
        /**
		* Read input from stdin
		*/
		bytes_read = read(STDIN_FILENO, buf, BUF_SIZE);
		
        /**
         * Read data from formatted string
        */
        bytes_read = sscanf(
			buf, 
			"%d, %d, %d\n", 
			&irq_line, 
			&latency, 
			&timestamp_current
		);
		
		/**
		 * Compile statistics
		*/
		latency = timestamp_current - timestamp_previous;
		timestamp_previous = timestamp_current;
		
		aggregator.line_occurences[irq_line]++;
		aggregator.latency_average[irq_line] += latency;
		
		if (aggregator.latency_max[irq_line] < latency) 
		{
			aggregator.latency_max[irq_line] = latency;
		}
		
		
    } while (!sigint_received);
	
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
	FILE* out_file = NULL;
	
	out_file = fopen("statistics.csv", "w");
	
	if (out_file == NULL)
	{
		printf("Statistics app: Failed to open output file!");
		exit(-1);
	}
	
	for (int i=0; i<IRQ_LINES; i++)
	{
		fprintf(
			out_file, 
			"%d, %d, %f, %d\n",
			i,
			aggregator.line_occurences[i],
			aggregator.latency_average[i],
			aggregator.latency_max[i]
		);
	}
    
}