#include <iostream>
#include <iomanip>
#include "new_timer.h"
//#include "timer.h"
int main() {

    Timer timer;
    //立即执行然后每隔多少秒执行
    timer.schedule(new TimerTask([] {
        std::cout << "执行成功了1" << std::endl;
    }), 0,10000);
    //延迟多少毫秒执行并且后面每隔多少秒执行
    timer.schedule(new TimerTask([] {
        std::cout << "执行成功了2" << std::endl;
    }), 3000,5000);
    //延迟多少毫秒执行
    timer.schedule(new TimerTask([] {
        std::cout << "执行成功了3" << std::endl;
    }), 2000);
    //指定时间
    timer.schedule(new TimerTask([] {
        std::cout << "执行成功了4" << std::endl;
    }), Timer::getTime(2019,9,20,16,19));

}