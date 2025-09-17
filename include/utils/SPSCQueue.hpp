#pragma once
#include <atomic>
#include <optional>
#include <vector>

template <typename T>
class SPSCQueue
{
public:
    explicit SPSCQueue(const size_t& capacity);

    bool push(const T& item);
    std::optional<T> pop();

    bool empty() const;
    bool full() const;
private:
    size_t GetNextIndex(size_t currIndex) const;

    std::vector< std::optional<T> > buffer_; //using std::optional to initially fill buffer will nullptr values instead of possible expensive initialisation of T type objects
    size_t capacity_;
    std::atomic<size_t> head_;
    std::atomic<size_t> tail_;

};

template <typename T>
SPSCQueue<T>::SPSCQueue(const size_t& capacity):
    buffer_(capacity),
    capacity_(capacity),
    head_{0},
    tail_{0}
{}

template <typename T>
bool SPSCQueue<T>::push(const T& item)
{
    auto currHead = head_.load(std::memory_order_relaxed);

    const auto nextHead = GetNextIndex(currHead);
    if (nextHead == tail_.load(std::memory_order_acquire)) return false;

    head_.compare_exchange_weak(currHead, nextHead,
        std::memory_order_release, std::memory_order_relaxed);

    buffer_[currHead] = item;
    head_.store(nextHead, std::memory_order_release);
    return true;
}

template <typename T>
std::optional<T> SPSCQueue<T>::pop()
{
    auto currTail = tail_.load(std::memory_order_relaxed);

    if (currTail == head_.load(std::memory_order_acquire))
    {
        return std::nullopt;
    }

    auto poppedVal = std::move(buffer_[currTail]);
    buffer_[currTail] = std::nullopt;
    tail_.store(GetNextIndex(currTail), std::memory_order_release);
    return poppedVal;
}

template <typename T>
bool SPSCQueue<T>::empty() const
{
    return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
}

template <typename T>
bool SPSCQueue<T>::full() const
{
    return GetNextIndex(head_.load(std::memory_order_acquire)) == tail_.load(std::memory_order_acquire);
}

template <typename T>
size_t SPSCQueue<T>::GetNextIndex(size_t currIndex) const
{
    return (currIndex + 1) % capacity_;
}
