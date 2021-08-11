#pragma once

#include <iostream>
#include <list>
#include <string>
#include <functional>


template<class T>
class Listener {
public:
    Listener(std::function<void(T)> callback) : callback_(callback) {}
    
    void OnNotify(T param) {
        callback_(param);
    }

private:
    std::function<void(T)> callback_;

};

template<class T>
class Delegate {
public:
    
    void Attach(Listener<T>* l) {
        listeners_.push_back(l);
    }
    
    void Detach(Listener<T>* l) {
        listeners_.remove(l);
    }

    void Notify(T param) {
        std::list<Listener<T>*>::iterator iterator = listeners_.begin();
        while (iterator != listeners_.end()) {
            (*iterator)->OnNotify(param);
            ++iterator;
        }
    }

private:
    std::list<Listener<T>*> listeners_;
};