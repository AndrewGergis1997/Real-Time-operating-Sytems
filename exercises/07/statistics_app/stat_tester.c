#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

#define IRQ_LINES 16
#define MAX_LATENCY 150000

volatile sig_atomic_t sigint_received = false;

void sigint_handler(int sig)
{
    sigint_received = true;
}

void main(){
    /**
     * Write buffer
    */
    char buf[100];

    /**
     * Random data variables
    */
    int irq_line = 0;
    int latency = 0;
    int timestamp = 0;
    
    /**
     * Var to capture sprintf return value
    */
    int bytes_written = 0;

    /**
     * Register sigint_handler
    */
    signal(SIGINT, sigint_handler);

    do
    {
        /**
         * Generate random data
        */
        irq_line = rand() % IRQ_LINES;
        latency = rand() % MAX_LATENCY;
        timestamp += latency;

        /**
         * Format data to string
        */
        bytes_written = sprintf(buf, "%d, %d, %d\n", irq_line, latency, timestamp);

        /**
         * Write to stdout and flush
        */
        write(STDOUT_FILENO, buf, bytes_written);
        fflush(stdout);

        /**
         * Sleep for 1 second
        */
        sleep(1);
    } while (!sigint_received);
    
}