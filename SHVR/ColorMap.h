//
//  ColorMap.h
//  
//
//  Created by Andrew Owens on 2015-02-06.
//
//

#ifndef COLOR_MAP_H
#define COLOR_MAP_H

#include "RGB.h"

#include <iostream>
#include <vector>
#include <limits>

#define COLORS_NUM 256

class ColorMap
{
public:
	typedef Color::ColorRGB<float> RGBf;
	typedef Color::ColorRGB<unsigned char> RGBuc;
	typedef std::vector< RGBf > RGBf_Vec;
	typedef std::vector< RGBuc > RGBuc_Vec;
	
public:
	ColorMap( std::string const & fileLocation );
	ColorMap();
		
	bool loadMap( std::string const & fileLocation );
	bool isLoaded() const;
	std::string const & fileLocation() const;
	
	RGBf const & operator[]( uint8_t idx ) const;
	
	RGBf const & rgb_f( uint8_t idx ) const;
	RGBuc const & rgb_uc( uint8_t idx ) const;
	
	RGBf const * data_f() const;
	RGBuc const * data_uc() const;

	friend void swap( ColorMap & a, ColorMap & b );
	
private:
	RGBf_Vec m_mapF;
	RGBuc_Vec m_mapUC;
	bool m_isLoaded;
	std::string m_fileName;
};

inline
ColorMap::RGBf const * ColorMap::data_f() const
{
  return m_mapF.data();
}
inline
ColorMap::RGBuc const * ColorMap::data_uc() const
{
  return m_mapUC.data();
}
inline
bool ColorMap::isLoaded() const
{
	return m_isLoaded;
}

inline
std::string const & ColorMap::fileLocation() const
{
	return m_fileName;
}

#endif /* defined(____ColorMap__) */
