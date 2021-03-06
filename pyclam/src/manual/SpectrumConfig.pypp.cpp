/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
 
#include "boost/python.hpp"
#include "CLAM/SpectrumConfig.hxx"
#include "src/Definitions.hxx"
#include "SpectrumConfig.pypp.hpp"

namespace bp = boost::python;

void register_SpectrumConfig_class(){

    { //::CLAM::SpectrumConfig
        typedef bp::class_< CLAM::SpectrumConfig > SpectrumConfig_exposer_t;
        SpectrumConfig_exposer_t SpectrumConfig_exposer = SpectrumConfig_exposer_t( "SpectrumConfig" );
        bp::scope SpectrumConfig_scope( SpectrumConfig_exposer );
        { //::CLAM::SpectrumConfig::GetSize
        
            typedef int const & ( ::CLAM::SpectrumConfig::*GetSize_function_type )(  ) const;
            
            SpectrumConfig_exposer.def( 
                "GetSize"
                , GetSize_function_type( &::CLAM::SpectrumConfig::GetSize )
                , bp::return_value_policy< bp::copy_const_reference >() );
        
        }
        { //::CLAM::SpectrumConfig::SetSize
        
            typedef void ( ::CLAM::SpectrumConfig::*SetSize_function_type )( int const & ) ;
            
            SpectrumConfig_exposer.def( 
                "SetSize"
                , SetSize_function_type( &::CLAM::SpectrumConfig::SetSize )
                , ( bp::arg("arg") ) );
        
        }
    }

}
