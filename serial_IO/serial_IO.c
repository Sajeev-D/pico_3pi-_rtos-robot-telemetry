#include <stdio.h>
#include "pico/stdlib.h"

int counter = 0;

int dummy(void) {
    counter++;  
    for (int i = 0; i < 1000000; i++) {
        // Just a busy loop to consume some time
        counter += 1;
    }
    
    int a = 8;
    int b = 10;
    return a + b;
}

int main()
{
    stdio_init_all();

    while (true) {

        absolute_time_t t = get_absolute_time();
        dummy();
        absolute_time_t t2 = get_absolute_time();
        int64_t elapsed = absolute_time_diff_us(t, t2);
        printf("Elapsed time: %lld microseconds\n", elapsed);


        // printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
