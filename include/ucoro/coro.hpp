#pragma once

#include <coroutine>
#include <concepts>
#include <utility>
#include <iostream>

namespace ucoro {

template<typename T>
requires (!std::is_reference_v<T> && !std::is_pointer_v<T>)
class [[nodiscard]] Coro final
{
public:
    class FinalAwaitable;
    class Awaitable;

    class promise_type final
    {
    public:
        Coro get_return_object() { return Coro{std::coroutine_handle<promise_type>::from_promise(*this)}; }
        std::suspend_always initial_suspend() noexcept { return {}; }

        void return_value(T&& new_value) {
            std::cerr << "\nPromiseType::return_value";
            m_value = std::move(new_value);
        }

        // an exception escapes the body of the coroutine.
        void unhandled_exception() {
            std::terminate();
        }

        FinalAwaitable final_suspend() noexcept { return FinalAwaitable{}; }

        void continuation(std::coroutine_handle<> continuation) {
            m_continuation = continuation;
        }

        std::coroutine_handle<> continuation() const {
            return m_continuation;
        }

        T&& value() && {
            return std::move(m_value);
        }

    private:
        T m_value{};

        std::coroutine_handle<> m_continuation{nullptr};
    };

    explicit Coro(std::coroutine_handle<promise_type> handle)
    : m_handle{handle} {}

    ~Coro() {
        if(m_handle)
            m_handle.destroy();
    }

    Coro(const Coro&) = delete;
    Coro& operator=(const Coro&) = delete;

    Coro(Coro&& rhs) noexcept
    : m_handle(std::move(rhs.m_handle)) {
        rhs.m_handle = nullptr;
    }

    Coro& operator=(Coro&& rhs) noexcept {
        if(&rhs != this) {
            if(m_handle) {
                m_handle.destroy();
            }
            m_handle = std::exchange(rhs.m_handle, nullptr);
        }
        return *this;
    }
    
    Awaitable operator co_await() && noexcept;

private:
    std::coroutine_handle<promise_type> m_handle;
};

template<typename T>
class Coro<T>::FinalAwaitable final {
public:
    bool await_ready() const noexcept { return false; }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<typename Coro<T>::promise_type> coroutine) noexcept
    {
        std::cerr << "\nFinalAwaitable::await_suspend done? " << coroutine.done()
        << " continuation nullptr? " << (coroutine.promise().continuation() == nullptr);
        // If there is a continuation call it, otherwise this is the end of the line.
        // auto& promise = coroutine.promise();
        // if (promise.continuation() != nullptr)
        // {
        //     return promise.continuation();
        // }
        // else
        // {
        //     return std::noop_coroutine();
        // }
        return coroutine.promise().continuation();
    }

    void await_resume() noexcept {
        std::cerr << "\nFinalAwaitable::await_resume";
    }
};

template<typename T>
class Coro<T>::Awaitable final
{
    using promise_type = typename Coro<T>::promise_type;
public:
    explicit Awaitable(std::coroutine_handle<promise_type> handler) noexcept 
    : m_handler{std::move(handler)} {}

    bool await_ready() const {
        return !m_handler || m_handler.done();
    }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> continuation) {
        std::cerr << "\nAwaitableBase::await_suspend";
        // Store the continuation in the task's promise so that the final_suspend()
        // knows to resume this coroutine when the task completes.
        
        m_handler.promise().continuation(continuation);

        // Then we resume the task's coroutine, which is currently suspended
        // at the initial-suspend-point (ie. at the open curly brace).
        return m_handler;
    }

    T&& await_resume() const {
        std::cerr << "\nResponseAwaitable::await_resume";
        return std::move(m_handler.promise()).value();
    }

protected:
    std::coroutine_handle<promise_type> m_handler;
};


template<typename T>
typename Coro<T>::Awaitable Coro<T>::operator co_await() && noexcept {
    return typename Coro<T>::Awaitable{m_handle};
}


} //< namespace ucoro