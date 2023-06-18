//
//  ColorMap.cpp
//  
//
//  Created by Andrew Owens on 2015-02-06.
//
//

#include "ColorMap.h"

#include <fstream>
#include <algorithm>
#include <iterator>


ColorMap::ColorMap( std::string const & file )
{
	ColorMap tmp;
	
	if( tmp.loadMap( file ) )
	{
		swap( *this, tmp );
	}
}

ColorMap::ColorMap() :
	m_isLoaded( false )
{}

bool ColorMap::loadMap( std::string const & fileLocation )
{
	
	std::ifstream f( fileLocation, std::fstream::in | std::fstream::binary );

	if( !f.is_open() )
	{
		m_isLoaded = false;
	}
	else
	{
		RGBuc_Vec tmp;
		tmp.reserve( COLORS_NUM );
		
		std::copy(
					std::istream_iterator<RGBuc>(f),
					std::istream_iterator<RGBuc>(),
						std::back_inserter( tmp ) );
		f.close();
		
		
		m_mapF.resize( COLORS_NUM );
	
		typedef std::numeric_limits<unsigned char> UCLimits;
		float inv = 1.f / UCLimits::max();
		
		std::transform(	tmp.begin(),
				tmp.end(),
				m_mapF.begin(),
				[ inv ]( RGBuc const & ucColor )
			    {
				    RGBf fColor = ucColor;
				    fColor *= inv; // normalize
				    
				    return fColor;
			    }
			    );
		
		m_mapUC.swap( tmp );
		
		m_isLoaded = true;
	}
	
	return m_isLoaded;
}

ColorMap::RGBf const & ColorMap::operator[] ( uint8_t idx ) const
{
	return m_mapF[ idx ];
}
ColorMap::RGBf const & ColorMap::rgb_f ( uint8_t idx ) const
{
	return m_mapF[ idx ];
}
ColorMap::RGBuc const & ColorMap::rgb_uc ( uint8_t idx ) const
{
	return m_mapUC[ idx ];
}

void swap( ColorMap & a, ColorMap & b )
{
	std::swap( a.m_isLoaded, b.m_isLoaded );
	std::swap( a.m_mapF, b.m_mapF );
	std::swap( a.m_mapUC, b.m_mapUC );
	std::swap( a.m_fileName, b.m_fileName );
}

