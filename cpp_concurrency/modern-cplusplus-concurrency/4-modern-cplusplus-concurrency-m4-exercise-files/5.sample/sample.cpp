#include "precompiled.h"

using namespace std;
using namespace KennyKerr;

class condition_variable
{
    CONDITION_VARIABLE m_cv;

public:

    condition_variable(condition_variable const &) = delete;
    condition_variable(condition_variable &&) = delete;
    auto operator=(condition_variable const &) -> condition_variable & = delete;
    auto operator=(condition_variable &&) -> condition_variable & = delete;

    condition_variable() throw() :
        m_cv {}
    {}

    template <typename T>
    auto wait_while(slim_lock & lock,
                    T predicate) throw() -> void
    {
        while (predicate())
        {
            VERIFY(SleepConditionVariableSRW(&m_cv,
                                             lock.native_handle(),
                                             INFINITE,
                                             0));
        }
    }

    auto wake_one() throw() -> void
    {
        WakeConditionVariable(&m_cv);
    }

    auto wake_all() throw() -> void
    {
        WakeAllConditionVariable(&m_cv);
    }
};

template <typename T>
class blocking_queue
{
    deque<T> m_queue;
    slim_lock m_lock;
    condition_variable m_cv;

public:

    using value_type = T;

    auto push_back(T const & value) -> void
    {
        auto guard = m_lock.get_exclusive();

        m_queue.push_back(value);

        m_cv.wake_one();
    }

    auto pop() -> T
    {
        auto guard = m_lock.get_exclusive();

        m_cv.wait_while(m_lock, [&]
        {
            return m_queue.empty();
        });

        T value = m_queue.front();
        m_queue.pop_front();
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
