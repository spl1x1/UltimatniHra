//
// Created by Lukáš Kaplánek on 19.10.2025.
//

#ifndef EVENT_H
#define EVENT_H

#pragma once

#include <functional>
#include <map>
#include <mutex>
#include <atomic>

template<typename Sig>
class Event;

template<typename R, typename... Args>
class Event<R(Args...)> {
public:
    using Callback = std::function<R(Args...)>;
    using Id = std::uint64_t;

    Event() : nextId(1) {}

    // Register listener
    Id add(Callback cb) {
        std::lock_guard<std::mutex> lk(mutex_);
        Id id = nextId++;
        listeners.emplace(id, std::move(cb));
        return id;
    }

    // Unregister listener
    void remove(Id id) {
        std::lock_guard lk(mutex_);
        listeners.erase(id);
    }

    // Call all listeners
    void invoke(Args... args) {
        std::map<Id, Callback> copy;
        {
            std::lock_guard lk(mutex_);
            copy = listeners;
        }
        for (auto &p : copy) {
            if (p.second) p.second(args...);
        }
    }

    // Operator pro volani eventu
    void operator()(Args... args) { invoke(std::forward<Args>(args)...); }

private:
    std::mutex mutex_;
    std::map<Id, Callback> listeners;
    std::atomic<Id> nextId;
};


#endif //EVENT_H
