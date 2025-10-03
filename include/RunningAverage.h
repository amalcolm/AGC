#pragma once

#include <algorithm> // Required for std::fill
#include <cstddef>
#include <cstdint>   // For uint16_t and uint32_t
#include <vector>
#include <deque>
#include <utility> // pair

class RunningAverage {
public:
    explicit RunningAverage(size_t windowSize = 16) {
        Reset(windowSize); 
    }

    void Reset(int windowSize) {
        m_values.resize(windowSize > 0 ? windowSize : 1);
        std::fill(m_values.begin(), m_values.end(), 0);
        m_sum = 0; m_head = 0; m_count = 0;
        m_seq = 0; m_minq.clear(); m_maxq.clear();
    }

    void Add(uint16_t value) {
        const size_t W = m_values.size();

        // Handle sum overwrite like before
        if (m_count == W) {
            m_sum -= m_values[m_head];
        }
        m_values[m_head] = value;
        m_sum += value;

        // Min deque: pop larger tails, then push
        while (!m_minq.empty() && m_minq.back().first > value) m_minq.pop_back();
        m_minq.emplace_back(value, m_seq);

        // Max deque: pop smaller tails, then push
        while (!m_maxq.empty() && m_maxq.back().first < value) m_maxq.pop_back();
        m_maxq.emplace_back(value, m_seq);

        // Advance circular buffer bookkeeping
        m_head = (m_head + 1) % W;
        if (m_count < W) m_count++;
        m_seq++; 
        
        // Expire anything that fell out of the window
        const size_t expireBefore = (m_seq > W) ? (m_seq - W) : 0;
        while (!m_minq.empty() && m_minq.front().second < expireBefore) m_minq.pop_front();
        while (!m_maxq.empty() && m_maxq.front().second < expireBefore) m_maxq.pop_front();

    }

    float    GetAverage() const { return m_count ? float(m_sum) / m_count : 0.0f; }
    size_t   GetCount()   const { return m_count; }
    bool     IsFull()     const { return m_count == m_values.size(); }

    uint16_t GetMin()     const { return m_count ? m_minq.front().first : 0; }
    uint16_t GetMax()     const { return m_count ? m_maxq.front().first : 0; }

private:
    std::vector<uint16_t> m_values;
    uint32_t m_sum{};
    size_t   m_head{}, m_count{};
    size_t   m_seq{}; // monotonically increasing

    std::deque<std::pair<uint16_t,size_t>> m_minq, m_maxq;
};
