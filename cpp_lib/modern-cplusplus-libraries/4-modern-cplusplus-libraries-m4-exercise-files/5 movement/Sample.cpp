#include "Precompiled.h"

using namespace std;

struct Hen
{
    char const * m_name;

    Hen() : m_name { "" }
    {
        TRACE(L"default constructor (%S)\n", m_name);
    }

    explicit Hen(char const * name) : m_name { name }
    {
        TRACE(L"explicit constructor (%S)\n", m_name);
    }

    Hen(Hen const & other) : m_name { other.m_name }
    {
        TRACE(L"copy constructor (%S)\n", m_name);
    }

    Hen(Hen && other) : m_name { other.m_name }
    {
        other.m_name = "null";
        TRACE(L"move constructor (%S)\n", m_name);
    }

    ~Hen() 
    {
        TRACE(L"destructor (%S)\n", m_name);
    }

    auto operator = (Hen const & other) -> Hen &
    {
        m_name = other.m_name;
        TRACE(L"copy assignment (%S)\n", m_name);
        return *this;
    }

    auto operator = (Hen && other) -> Hen &
    {
        m_name = other.m_name;
        other.m_name = "null";
        TRACE(L"move assignment (%S)\n", m_name);
        return *this;
    }

    void swap(Hen & other)
    {
        std::swap(m_name, other.m_name);
    }
};

void swap(Hen & left, Hen & right)
{
    TRACE(L"swap(%S, %S)\n", left.m_name, right.m_name);

    left.swap(right);
}

auto main() -> int
{
    auto c = list<Hen> {};
    
    c.emplace_back("Henrietta");
    c.emplace_back("Rowena");
    c.emplace_back("Charlotte");
    c.emplace_back("Margaret");
    c.emplace_back("Jane");
    c.emplace_back("Constance");

    c.reverse();
}
