#pragma once

#include <vector>
#include <stdexcept>      // std::out_of_range

namespace Math {

template < class T >
class Array3D {

public:
    typedef std::vector< T > Array3DData;
    typedef typename Array3D::Array3DData::iterator Array3DDataIterator;
    typedef typename Array3D::Array3DData::const_iterator Array3DDataConstIterator;
    
public:
    Array3D( size_t x = 1, size_t y = 1, size_t z = 1, T init = T() )
            :
            m_width( x ), m_height( y ), m_depth( z ), m_data( x*y*z, init )
    {}

protected:
    size_t m_width, m_height, m_depth;
    Array3DData m_data;

public:

    T & operator()( const size_t x, const size_t y = 0 , const size_t z = 0 )
    {
        if ( x < m_width && y < m_height && z < m_depth )
            return m_data.at(x + y * m_width + z * m_width * m_height);
        else
            throw std::out_of_range("Array3D coordinate out of range");
    }
    
    T const & operator()( const size_t x, const size_t y = 0 , const size_t z = 0 ) const
    {
        if ( x < m_width && y < m_height && z < m_depth )
            return m_data.at(x + y * m_width + z * m_width * m_height);
        else
            throw std::out_of_range("Array3D coordinate out of range");
    }

    //TODO inline maybe?
    T & at( const size_t x, const size_t y = 0 , const size_t z = 0 )
    {
        if ( x < m_width && y < m_height && z < m_depth )
            return m_data.at(x + y * m_width + z * m_width * m_height);
        else
            throw std::out_of_range("Array3D coordinate out of range");
    }
    
    T const & at( const size_t x, const size_t y = 0 , const size_t z = 0 ) const
    {
        if ( x < m_width && y < m_height && z < m_depth )
            return m_data.at(x + y * m_width + z * m_width * m_height);
        else
            throw std::out_of_range("Array3D coordinate out of range");
    }

    inline size_t iterIndex(const size_t x = 0, const size_t y = 0, const size_t z = 0) {
       return x + y * m_width + z * m_width * m_height;
    }

    inline size_t size() const { return m_data.size();}
    inline size_t width() const { return m_width;}
    inline size_t height() const { return m_height;}
    inline size_t depth() const { return m_depth;}

    Array3DDataIterator begin() { return m_data.begin();}
    Array3DDataIterator end() { return m_data.end();}
    
    Array3DDataConstIterator begin() const { return m_data.begin();}
    Array3DDataConstIterator end() const {return m_data.end();}
    T* data() { return m_data.data(); }
    
	void reset()
    {
        m_data.clear();
        m_width = 0;
        m_height = 0;
        m_depth = 0;
    }

    void resize(const size_t w, const size_t h, const size_t d, T initValue = T()) {
        m_width = w;
        m_height = h;
        m_depth = d;
        m_data.resize(w * h * d, initValue);
    }

    //void swap(Array3D<T>& arr) {
    //    m_width = arr.width();
    //    m_height = arr.height();
    //    m_depth = arr.depth();
    //    m_data.swap(arr.data());
    //}
};

}

