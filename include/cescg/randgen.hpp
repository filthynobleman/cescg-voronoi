/**
 * @file        randgen.hpp
 * 
 * @brief       A wrapper for random generation of content.
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-03-31
 */
#include <cescg/utils.hpp>

#include <random>



namespace cescg
{
    
class RandGen
{
private:
    std::mt19937 m_Eng;
    std::uniform_real_distribution<float> m_FloatDist;

    static cescg::RandGen* m_Instance;
    RandGen();
    ~RandGen();

public:
    static cescg::RandGen& GetInstance();

    // Generate an integer in range [Min, Max)
    int RandomInteger(int MinInclusive, int MaxExclusive);
    // Generate a float in range [Min, Max]
    float RandomReal(float MinInclusive, float MaxInclusive);
    // Generate a float in range [0, 1]
    float RandomReal();
};

} // namespace cescg
