#pragma once
#include <iostream>
#include <memory>

namespace tori{
namespace net{

using namespace std;

template< typename T >
class Singleton
{
    public:
        static T* getInstance()
        {
            if( _instance==NULL )
                _instance = new T();
            return _instance;
        }
        Singleton(const Singleton&) = delete ;
        Singleton& operator=(const Singleton&) = delete ;
    private:
        Singleton(){};
        static T* _instance;
};

template<typename T> 
T* Singleton<T>::_instance = NULL;

}
}
