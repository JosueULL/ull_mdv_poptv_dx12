#pragma once

#include "Common.h"
#include <iostream>
#include <list>
#include <string>
#include <functional>


#define OBSERVER(name, t) \
	Observer<t>* name; \
	void name ## Callback(t data); 

#define BIND_OBSERVER(name, t) \
    name = new Observer<t>([=](t data) { this-> ## name ## Callback(data); });

#define FREE_OBSERVER(name) SAFE_FREE(name);

template<typename>
class Subject;

template<typename T>
class Observer {
public:
    Observer(std::function<void(T)> callback) : 
        callback_(callback), 
        subjects_(), 
        detaching_(false) 
    {
    }

    ~Observer() {
        DetachFromAll();
    }
    
    void OnAttached(Subject<T>* d) {
        subjects_.push_back(d);
    }

    void OnDetached(Subject<T>* d) {
        if (!detaching_)
            subjects_.remove(d);
    }

    void DetachFromAll() {
        detaching_ = true;
        for (Subject<T>* s : subjects_) {
            s->Detach(this);
        }
        detaching_ = false;
        subjects_.clear();
    }

    void OnNotify(T param) {
        callback_(param);
    }

private:
    std::function<void(T)> callback_;
    std::list<Subject<T>*> subjects_;
    bool detaching_;
};

template<typename T>
class Subject {
public:
    
    void Attach(Observer<T>* o) {
        observers_.push_back(o);
        o->OnAttached(this);
    }
    
    void Detach(Observer<T>* o) {
        observers_.remove(o);
        o->OnDetached(this);
    }

    void Notify(T param) {
        std::list<Observer<T>*>::iterator iterator = observers_.begin();
        while (iterator != observers_.end()) {
            (*iterator)->OnNotify(param);
            ++iterator;
        }
    }

private:
    std::list<Observer<T>*> observers_;
};