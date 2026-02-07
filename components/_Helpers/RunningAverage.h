#pragma once

#include <cstddef>
#include <cstdint>   // For uint16_t and uint32_t
#include <vector>
#include <deque>
#include <utility> // pair

template<typename T>
class RunningAverage {
public:
    explicit RunningAverage(size_t windowSize = 16) {
        reset(windowSize); 
    }

    void reset(size_t windowSize) {
    m_values.assign(windowSize > 0 ? windowSize : 1, 0);
    m_sum = m_head = m_count = m_seq = 0;
}


    void add(T value) {
        const size_t W = m_values.size();
        if (m_count == W)
          m_sum -= m_values[m_head];
        else
          m_count++;

        m_sum += value;
        m_values[m_head] = value;

        if (++m_head == W) m_head = 0;
        m_seq++;
    }

    float    getAverage() const { return m_count ? float(m_sum) / m_count : 0.0f; }
    size_t   getCount()   const { return m_count; }
    bool     isFull()     const { return m_count == m_values.size(); }

protected:
    std::vector<T> m_values;
    uint32_t m_sum{}, m_count{};
    size_t   m_head{};
    size_t   m_seq{}; // monotonically increasing

};

template<typename T>
class RunningAverageMinMax : public RunningAverage<T> {
public:
    explicit RunningAverageMinMax(size_t windowSize = 16) : RunningAverage<T>(windowSize) {
       m_minq.clear(); m_maxq.clear();
    }

    void reset(size_t windowSize) {
        RunningAverage<T>::reset(windowSize); 
        m_minq.clear(); m_maxq.clear();
    }
    
    void add(T value) {
        RunningAverage<T>::add(value);
        const size_t W = this->m_values.size();
  
        // Min deque: pop larger tails, then push
        while (!m_minq.empty() && m_minq.back().first >= value) m_minq.pop_back();
        m_minq.emplace_back(value, this->m_seq);

        // Max deque: pop smaller tails, then push
        while (!m_maxq.empty() && m_maxq.back().first <= value) m_maxq.pop_back();
        m_maxq.emplace_back(value, this->m_seq);
        // Expire anything that fell out of the window
        const size_t expireBefore = (this->m_seq > W) ? (this->m_seq - W) : 0;
        while (!m_minq.empty() && m_minq.front().second <= expireBefore) m_minq.pop_front();
        while (!m_maxq.empty() && m_maxq.front().second <= expireBefore) m_maxq.pop_front();
    }

    T getMin()     const { return this->m_count ? m_minq.front().first : 0; }
    T getMax()     const { return this->m_count ? m_maxq.front().first : 0; }
private:

    std::deque<std::pair<T,size_t>> m_minq, m_maxq;

};