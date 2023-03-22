#include <stdio.h>
#include <time.h>
int main() {    time_t current_time;    struct tm* time_info;    char time_string[20];    time(&current_time);    time_info = localtime(&current_time);    strftime(time_string, sizeof(time_string), "%y/%m/%d %H:%M", time_info);          printf("Current Time: %s\n", time_string);    return 0; }               
