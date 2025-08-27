#include<time.h>

#include"TimeStamp.h"

TimeStamp::TimeStamp()
{
    secSinceEpoch_=time(0); //取系统当前时间。
}

TimeStamp::TimeStamp(int64_t secSinceEpoch): secSinceEpoch_(secSinceEpoch)
{}

TimeStamp TimeStamp::now()
{
    return TimeStamp(); //返回当前时间。
}

time_t TimeStamp::toint() const
{
    return secSinceEpoch_;
}

std::string TimeStamp::tostring() const
{
    char buf[72]={0};
    tm* tm_time=localtime(&secSinceEpoch_);
    snprintf(buf, 72, "%4d-%02d-%02d %02d:%02d:%02d",
            tm_time->tm_year+1900,
            tm_time->tm_mon+1,
            tm_time->tm_mday,
            tm_time->tm_hour,
            tm_time->tm_min,
            tm_time->tm_sec);
    
    return buf;
}

// #include<unistd.h>
// #include<iostream>

// int main()
// {
//     TimeStamp ts;
//     std::cout<<ts.toint()<<std::endl;
//     std::cout<<ts.tostring()<<std::endl;

//     sleep(2);
//     std::cout<<TimeStamp::now().toint()<<std::endl;
//     std::cout<<TimeStamp::now().tostring()<<std::endl;
// }