#include <unistd.h>
#include <sys/resource.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <time.h>
#include <sys/mman.h>
#include <string.h>
#include <iostream>
#include <sstream> 

#include <chrono>
#include <thread>
using namespace std::chrono_literals;


void setThreadHighPriority()
{
    pid_t pid = getpid();
    int priority_status = setpriority(PRIO_PROCESS, pid, -19);
    if (priority_status){
        //printWarning("setThreadHighPriority. Failed to set priority.");
        return;
    }
}

void setThreadRealTime()
{  
    /* Declare ourself as a real time task, priority 49.
       PRREMPT_RT uses priority 50
       for kernel tasklets and interrupt handler by default */
    struct sched_param param;
    param.sched_priority = 49;
    //pthread_t this_thread = pthread_self();
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        perror("sched_setscheduler failed");
        exit(-1);
    }

    /* Lock memory */
    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
        perror("mlockall failed");
        exit(-2);
    }

    /* Pre-fault our stack
       8*1024 is the maximum stack size
       which is guaranteed safe to access without faulting */
    int MAX_SAFE_STACK = 8*1024;
    unsigned char dummy[MAX_SAFE_STACK];
    memset(dummy, 0, MAX_SAFE_STACK);
}

int main(int, char**) {
    //setThreadRealTime();
    setThreadHighPriority();

    auto startTime = std::chrono::system_clock::now();

    while(true){
        auto start = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(2ms);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end-start;
        std::cout << "Waited " << elapsed.count() << " ms\n";
    }
}
