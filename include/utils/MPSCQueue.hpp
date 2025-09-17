#pragma once
#include <atomic>
#include <optional>
#include <thread>
#include <vector>

template <typename T>
class MPSCQueue {
private:
    struct Slot {
        std::atomic<size_t> sequence{0};
        T data;
    };

    std::vector<Slot> buffer_;
    size_t capacity_;
    std::atomic<size_t> enqueue_pos_{0};
    std::atomic<size_t> dequeue_pos_{0};

public:
    MPSCQueue(size_t capacity) : capacity_(capacity), buffer_(capacity)
    {
        for (size_t i = 0; i < capacity; ++i) {
            buffer_[i].sequence.store(i);
        }
    }

    bool push(const T& item) {
        size_t pos = enqueue_pos_.fetch_add(1);
        size_t index = pos % capacity_;
        size_t expected_seq = pos + 1;

        size_t current_dequeue = dequeue_pos_.load();
        if (pos >= current_dequeue + capacity_) {
            return false;  // Queue is full
        }

        // Wait for slot to be available (for wraparound cases)
        while (buffer_[index].sequence.load() != pos) {
            std::this_thread::yield();
        }

        buffer_[index].data = item;
        buffer_[index].sequence.store(expected_seq, std::memory_order_release);
        return true;
    }

    std::optional<T> pop() {
        size_t pos = dequeue_pos_.load();
        size_t index = pos % capacity_;
        size_t expected_seq = pos + 1;

        if (buffer_[index].sequence.load(std::memory_order_acquire) != expected_seq) {
            return std::nullopt;
        }

        T data = std::move(buffer_[index].data);
        buffer_[index].sequence.store(pos + capacity_, std::memory_order_release);
        dequeue_pos_.store(pos + 1);
        return data;
    }
};