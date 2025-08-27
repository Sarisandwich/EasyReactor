#pragma once

#include<iostream>
#include<string>

//时间戳类。
class TimeStamp
{
private:
    time_t secSinceEpoch_;  //整数表示的时间。

public:
    TimeStamp();    //用当前时间初始化对象。
    TimeStamp(int64_t secSinceEpoch);   //用一个整数表示的时间初始化对象。

    static TimeStamp now(); //返回当前时间的TimeStamp对象。
    time_t toint() const;   //返回整数表示的时间。
    std::string tostring() const;   //返回字符串表示的时间。yyyy-mm-dd。
};