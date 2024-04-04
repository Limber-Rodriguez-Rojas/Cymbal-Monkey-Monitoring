#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // for sleep function

void get_cpu_usage(FILE *output_file) {
    FILE *fp;
    char buffer[1024];
    
    // Open the /proc/stat file for reading
    fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        perror("Error opening /proc/stat");
        exit(1);
    }

    // Read the first line which contains overall CPU statistics
    fgets(buffer, sizeof(buffer), fp);

    // Parse the CPU metrics from the buffer
    unsigned long long int user, nice, system, idle;
    sscanf(buffer, "cpu %llu %llu %llu %llu", &user, &nice, &system, &idle);

    // Calculate CPU usage
    unsigned long long int total = user + nice + system + idle;
    unsigned long long int usage = user + nice + system;
    double percentage = (double)usage / total * 100;

    // Print the CPU usage percentage
    fprintf(output_file,"CPU Usage: %.2f%%\n", percentage);

    // Close the file
    fclose(fp);
}

void get_memory_usage(FILE *output_file) {
    FILE *fp;
    char buffer[1024];

    // Open the /proc/meminfo file for reading
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        perror("Error opening /proc/meminfo");
        exit(1);
    }

    // Read memory usage information
    unsigned long long int total, free, buffers, cached;
    while (fgets(buffer, sizeof(buffer), fp)) {
        if (sscanf(buffer, "MemTotal: %llu kB", &total) == 1) {}
        else if (sscanf(buffer, "MemFree: %llu kB", &free) == 1) {}
        else if (sscanf(buffer, "Buffers: %llu kB", &buffers) == 1) {}
        else if (sscanf(buffer, "Cached: %llu kB", &cached) == 1) {}
    }

    // Calculate used memory
    unsigned long long int used = total - free - buffers - cached;
    double percentage = (double)used / total * 100;
    // Print memory usage information
    fprintf(output_file,"Memory Usage: %.2f%%\n", percentage);
    // Close the file
    fclose(fp);
}

int main() {
    
    FILE *output_file;
    
    while(1){
    // Open a file for writing (replace "output.txt" with your desired file name)
    output_file = fopen("/root/output.txt", "w"); //path of the output file
    if (output_file == NULL) {
        perror("Error opening output file");
        exit(1);
    }

    // Call functions to retrieve and store usage information
    get_cpu_usage(output_file);
    get_memory_usage(output_file);
    

    // Close the output file
    fclose(output_file);
    system("echo --------- >> /root/new.txt && date >> /root/new.txt && cat /root/output.txt >> /root/new.txt"); // To create a file with the logs
    sleep(5);
    }
    return 0;
}
