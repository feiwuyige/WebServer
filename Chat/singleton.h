#ifndef SINGLETON_H
#define SINGLETON_H
#include "global.h"
/**
 * @file  singleton.h
 * @brief 以cpp11的方式实现单例模式
 *
 * @author zyl
 * @date 2025/01/19
 *
 * 详细描述
 * 其主要目的是确保某个类在系统中只有一个实例，并且提供一个全局的访问点来获取这个实例。
 * 作为基类使用
 */

//声明单例类
template <typename T>
class Singleton{
protected: //让子类继承时可以构造
    Singleton() = default;
    //不允许拷贝构造和拷贝赋值
    Singleton(const Singleton<T>& s) = delete;
    Singleton& operator=(const Singleton<T>& s) = delete;
    static std::shared_ptr<T> _instance;
public:
    static std::shared_ptr<T> GetInstance(){
        static std::once_flag s_flag;
        std::call_once(s_flag, [&](){
            _instance = std::shared_ptr<T>(new T);
        });
        return _instance;
    }
    void PrintAddress(){
        std::cout << _instance.get() << std::endl;
    }
    ~Singleton(){
        std::cout << "this is single deconstruct" << std::endl;
    }
};
template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;
#endif // SINGLETON_H
