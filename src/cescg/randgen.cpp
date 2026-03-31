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