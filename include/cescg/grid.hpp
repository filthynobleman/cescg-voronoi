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
    scalar_t* m_Data;

public:
    // Initializes the grid to whatever 0 means.
    Grid(int Width, int Height)
    {
        m_Width = Width;
        m_Height = Height;
        m_Data = (scalar_t*)std::calloc(m_Width * m_Height, sizeof(scalar_t));
        if (m_Data == nullptr)
            throw std::runtime_error("Cannot initialize the grid.");
    }

    // Initializes the grid to a default value
    Grid(int Width, int Height, const scalar_t& Default)
    {
        m_Width = Width;
        m_Height = Height;
        m_Data = (scalar_t*)std::malloc(m_Width * m_Height * sizeof(scalar_t));
        if (m_Data == nullptr)
            throw std::runtime_error("Cannot initialize the grid.");
        for (int j = 0; j < m_Height; ++j)
        {
            for (int i = 0; i < m_Width; ++i)
                m_Data[j * m_Width + i] = Default;
        }
    }

    // Copy constructor
    Grid(const cescg::Grid<scalar_t>& G)
    {
        m_Width = G.m_Width;
        m_Height = G.m_Height;
        m_Data = (scalar_t*)std::malloc(m_Width * m_Height * sizeof(scalar_t));
        if (m_Data == nullptr)
            throw std::runtime_error("Cannot initialize the grid.");
        m_Data = (scalar_t*)std::memcpy(m_Data, G.m_Data, m_Width * m_Height * sizeof(scalar_t));
        if (m_Data == nullptr)
            throw std::runtime_error("Cannot copy the grid.");
    }

    // Move constructor
    Grid(cescg::Grid<scalar_t>&& G)
    {
        m_Width = G.m_Width;
        m_Height = G.m_Height;
        m_Data = G.m_Data;
        G.m_Data = nullptr;
    }

    // Copy-assignment operator
    cescg::Grid<scalar_t>& operator=(const cescg::Grid<scalar_t>& G)
    {
        m_Width = G.m_Width;
        m_Height = G.m_Height;
        m_Data = (scalar_t*)std::malloc(m_Width * m_Height * sizeof(scalar_t));
        if (m_Data == nullptr)
            throw std::runtime_error("Cannot initialize the grid.");
        m_Data = (scalar_t*)std::memcpy(m_Data, G.m_Data, m_Width * m_Height * sizeof(scalar_t));
        if (m_Data == nullptr)
            throw std::runtime_error("Cannot copy the grid.");
        return *this;
    }

    // Move-assignment operator
    cescg::Grid<scalar_t>& operator=(cescg::Grid<scalar_t>&& G)
    {
        m_Width = G.m_Width;
        m_Height = G.m_Height;
        m_Data = G.m_Data;
        G.m_Data = nullptr;
        return *this;
    }
    
    // Free the memory
    ~Grid()
    {
        delete m_Data;
    }


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
