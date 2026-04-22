/**
 * @file        randgen.cpp
 * 
 * @brief       
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-03-31
 */
#include <cescg/randgen.hpp>


cescg::RandGen* cescg::RandGen::m_Instance = nullptr;

cescg::RandGen::RandGen()
{
    m_Eng = std::mt19937(0);
    m_FloatDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
}

cescg::RandGen::~RandGen() { } 


cescg::RandGen &cescg::RandGen::GetInstance()
{
    if (m_Instance == nullptr)
        m_Instance = new cescg::RandGen();
    return *m_Instance;
}

float cescg::RandGen::RandomReal() { return m_FloatDist(m_Eng); }

float cescg::RandGen::RandomReal(float MinInclusive, float MaxInclusive)
{
    return RandomReal() * (MaxInclusive - MinInclusive) + MinInclusive;
}

int cescg::RandGen::RandomInteger(int MinInclusive, int MaxExclusive)
{
    return std::roundf(RandomReal(MinInclusive - 0.4f, MaxExclusive - 0.6f));
}

/* ***** Jenkins Lookup3 Hash Functions ***** */

/* Source: http://burtleburtle.net/bob/c/lookup3.c */

#define rot(x, k) (((x) << (k)) | ((x) >> (32 - (k))))

#define mix(a, b, c) \
  { \
    a -= c; \
    a ^= rot(c, 4); \
    c += b; \
    b -= a; \
    b ^= rot(a, 6); \
    a += c; \
    c -= b; \
    c ^= rot(b, 8); \
    b += a; \
    a -= c; \
    a ^= rot(c, 16); \
    c += b; \
    b -= a; \
    b ^= rot(a, 19); \
    a += c; \
    c -= b; \
    c ^= rot(b, 4); \
    b += a; \
  } \
  ((void)0)

#define final(a, b, c) \
  { \
    c ^= b; \
    c -= rot(b, 14); \
    a ^= c; \
    a -= rot(c, 11); \
    b ^= a; \
    b -= rot(a, 25); \
    c ^= b; \
    c -= rot(b, 16); \
    a ^= c; \
    a -= rot(c, 4); \
    b ^= a; \
    b -= rot(a, 14); \
    c ^= b; \
    c -= rot(b, 24); \
  } \
  ((void)0)

unsigned int cescg::RandGen::HashInt(unsigned int Value) const
{
    unsigned int a;
    unsigned int b;
    unsigned int c;
    a = b = c = 0xdeadbeef + (1 << 2) + 13;

    a += Value;
    final(a, b, c);

    return c;
}

float cescg::RandGen::HashFloat(unsigned int Value) const
{
    return HashInt(Value) / (float)std::numeric_limits<unsigned int>::max();
}

glm::vec3 cescg::RandGen::HashColor(unsigned int Value)
{
    glm::vec3 Result;
    Result.x = HashFloat(glm::ivec2(Value, 0));
    Result.y = HashFloat(glm::ivec2(Value, 1));
    Result.z = HashFloat(glm::ivec2(Value, 2));
    return Result;
}

unsigned int cescg::RandGen::HashInt(const glm::ivec2 &Value) const
{
    unsigned int a;
    unsigned int b;
    unsigned int c;
    a = b = c = 0xdeadbeef + (1 << 2) + 13;

    a += Value.x;
    b += Value.y;
    final(a, b, c);

    return c;
}

float cescg::RandGen::HashFloat(const glm::ivec2 &Value) const
{
    return HashInt(Value) / (float)std::numeric_limits<unsigned int>::max();
}