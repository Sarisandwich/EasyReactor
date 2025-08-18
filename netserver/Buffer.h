#pragma once

#include<string>
#include<iostream>


class Buffer
{
private:
    std::string buf_;   //存放数据。
public:
    Buffer();
    ~Buffer();

    void append(const char* data, size_t size); //把数据追加到buf_。
    size_t size();  //返回buf_大小。
    const char* data(); //返回buf_首地址。
    void erase(size_t pos, size_t len); //删除buf_的部分内容。
    void clear();   //清空buf_。
};