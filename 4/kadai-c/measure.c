#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#define NANO 1000000000
// struct timespec {
//   time_t tv_sec; /* Seconds.  */
//   long tv_nsec;  /* Nanoseconds.  */
// };

int main(){
    clockid_t clk_id = CLOCK_REALTIME;
    struct timespec t1a, t1b, t1000a, t1000b;
    char *buf = "";


    //1回
    clock_gettime( clk_id, &t1a);
    write(1, buf, 0);
    clock_gettime( clk_id, &t1b);

    printf("1回: %ld ns\n", (t1b.tv_sec - t1a.tv_sec) * NANO + t1b.tv_nsec - t1a.tv_nsec);

    //1000回
    clock_gettime( clk_id, &t1000a);
    for(int i = 0; i < 1000; i++){
        write(1, buf, 0);
    }
    clock_gettime( clk_id, &t1000b);

    printf("1000回: %ld ns\n", (t1000b.tv_sec - t1000a.tv_sec) * NANO +  t1000b.tv_nsec - t1000a.tv_nsec);

    //1000回
    clock_gettime( clk_id, &t1000a);
    for(int i = 0; i < 1000; i++){
    }
    clock_gettime( clk_id, &t1000b);

    printf("1000回(forのみ): %ld ns\n", (t1000b.tv_sec - t1000a.tv_sec) * NANO + t1000b.tv_nsec - t1000a.tv_nsec);

}