#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(void)
{

    //initialise the struct
    //r is able to get out the struct, that's all
    
    //timer return time_t, which is the number of seconds since the epoch
    ////so local_time_r convert the number of second since the epoch to another time
    //http://sourceware.org/git/?p=glibc.git;a=blob;f=time/tzset.c;h=834cc3ccec63b67f9e0ec422f203e6ef2817b70b;hb=72b8692d7e640eb85ea0fb7de6d5e797512691c1
    time_t timer;
    timer = time(NULL);
    struct tm result;
    localtime_r(&timer, &result);
    //printf("%s secs since the Epoch\n",
        //asctime(localtime_r(&timer, &result)));

  printf("is_dst is %d \n", result.tm_isdst);
  printf("\n");
  printf("tm_zone is %s", result.tm_zone);
  printf("\n");
  printf("tm_sec is %d", result.tm_sec);
  printf("\n");
  printf("tm_min is %d", result.tm_min);
  printf("\n");
  printf("tm_hour is %d", result.tm_hour);
  printf("\n");
  printf("result tm_mday is %d", result.tm_mday);
  printf("\n");
  printf("result tm_mon is %d", result.tm_mon);
  printf("\n");
  printf("result tm_year is %d", result.tm_year);
  printf("\n");
  printf("result tm_wday is %d", result.tm_wday);
  printf("\n");
  printf("result tm_yday is %d", result.tm_yday);
  printf("\n");
    return(0);
}
