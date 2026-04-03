/**
 * @file        grid.hpp
 * 
 * @brief       A simple grid that stores values.
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-03-30
 */
#pragma once


#include <cescg/utils.hpp>


namespace cescg
{
    
template<typename scalar_t>
class Grid
{
private:
    int m_Width;
    int m_Height;
    std::vector<scalar_t> m_Data;

public:
    // Initializes the grid to whatever 0 means.
    Grid(int Width, int Height)
    {
        m_Width = Width;
        m_Height = Height;
        m_Data.resize(m_Width * m_Height);
    }

    // Initializes the grid to a default value
    Grid(int Width, int Height, const scalar_t& Default)
    {
        m_Width = Width;
        m_Height = Height;
        m_Data.resize(m_Width * m_Height, Default);
    }

    // Copy constructor
    Grid(const cescg::Grid<scalar_t>& G)
    {
        m_Width = G.m_Width;
        m_Height = G.m_Height;
        m_Data = G.m_Data;
    }

    // Move constructor
    Grid(cescg::Grid<scalar_t>&& G)
    {
        m_Width = G.m_Width;
        m_Height = G.m_Height;
        m_Data = std::move(G.m_Data);
    }

    // Copy-assignment operator
    cescg::Grid<scalar_t>& operator=(const cescg::Grid<scalar_t>& G)
    {
        m_Width = G.m_Width;
        m_Height = G.m_Height;
        m_Data = G.m_Data;
        return *this;
    }

    // Move-assignment operator
    cescg::Grid<scalar_t>& operator=(cescg::Grid<scalar_t>&& G)
    {
        m_Width = G.m_Width;
        m_Height = G.m_Height;
        m_Data = std::move(G.m_Data);
        return *this;
    }
    
    // Free the memory
    ~Grid() { }


    // Grid width
    int GetWidth() const { return m_Width; }
    // Grid height
    int GetHeight() const { return m_Height; }

    // Get value by copy. No boundary checks.
    scalar_t GetValue(int i, int j) const { return m_Data[j * m_Width + i]; }

    // Get value by reference. No boundary checks.
    scalar_t& GetValue(int i, int j) { return m_Data[j * m_Width + i]; }

    // Get value by copy. No boundary checks.
    scalar_t operator()(int i, int j) const { return GetValue(i, j); }

    // Get value by reference. No boundary checks.
    scalar_t& operator()(int i, int j) { return GetValue(i, j); }
};

} // namespace cescg
