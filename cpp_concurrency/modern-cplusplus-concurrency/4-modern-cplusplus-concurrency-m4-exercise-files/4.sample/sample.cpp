#include "precompiled.h"

using namespace std;
using namespace KennyKerr;

template <typename T>
class blocking_queue
{
    deque<T> m_queue;
    slim_lock m_lock;
    event m_event { event_type::manual_reset };

public:

    using value_type = T;

    auto push_back(T const & value) -> void
    {
        auto guard = m_lock.get_exclusive();

        m_queue.push_back(value);

        if (m_queue.size() == 1)
        {
            m_event.set();
        }
    }

    auto pop() -> T
    {
        auto guard = m_lock.get_exclusive();

        while (m_queue.empty())
        {
            m_lock.exit();
            
            VERIFY(m_event.wait());

            m_lock.enter();
        }

        T value = m_queue.front();
        m_queue.pop_front();

        if (m_queue.empty())
        {
            m_event.clear();
        }

        return value;
    }

    auto unsafe_empty() const throw() -> bool
    {
        return m_queue.empty();
    }
};

auto queue = blocking_queue<int> {};

auto consumer() -> void
{
    auto count = unsigned {};

    for (;;)
    {
        auto value = queue.pop();

        if (-1 == value)
        {
            TRACE(L"consumer %10d %10d\n", GetCurrentThreadId(), count);
            break;
        }

        ++count;
    }
}

template <typename Output, typename Count, typename T>
auto iota_n(Output first, Count const count, T value) -> Output
{
    for (auto i = Count {}; i != count; ++i)
    {
        *first++ = value++;
    }

    return first;
}

auto main() -> int
{
    auto c = vector<null_handle>(8);

    for (auto & t : c)
    {
        t = make_thread(consumer);
    }

    auto p = make_thread([]
    {
        iota_n(back_inserter(queue), 100000, 0);
    });

    auto p2 = make_thread([]
    {
        iota_n(back_inserter(queue), 100000, 0);
    });

    wait_all(p, p2);

    fill_n(back_inserter(queue), c.size(), -1);

    ASSERT(c.size() <= MAXIMUM_WAIT_OBJECTS);

    VERIFY_(WAIT_OBJECT_0, WaitForMultipleObjects(static_cast<DWORD>(c.size()),
                                                  reinterpret_cast<HANDLE const *>(&c[0]),
                                                  true,
                                                  INFINITE));

    ASSERT(queue.unsafe_empty());
}
