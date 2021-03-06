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
#include "CLAM/FFT.hxx"
#include "CLAM/FFT_base.hxx"
#include "CLAM/FFT_ooura.hxx"
#include "CLAM/FFT_fftw3.hxx"
#include "CLAM/FFTConfig.hxx"
#include "src/Definitions.hxx"
#include "FFTConfig.pypp.hpp"

namespace bp = boost::python;

void register_FFTConfig_class(){

    { //::CLAM::FFTConfig
        typedef bp::class_< CLAM::FFTConfig > FFTConfig_exposer_t;
        FFTConfig_exposer_t FFTConfig_exposer = FFTConfig_exposer_t( "FFTConfig" );
        bp::scope FFTConfig_scope( FFTConfig_exposer );
        { //::CLAM::FFTConfig::GetAudioSize
        
            typedef int const & ( ::CLAM::FFTConfig::*GetAudioSize_function_type )(  ) const;
            
            FFTConfig_exposer.def( 
                "GetAudioSize"
                , GetAudioSize_function_type( &::CLAM::FFTConfig::GetAudioSize )
                , bp::return_value_policy< bp::copy_const_reference >() );
        
        }
        { //::CLAM::FFTConfig::SetAudioSize
        
            typedef void ( ::CLAM::FFTConfig::*SetAudioSize_function_type )( int const & ) ;
            
            FFTConfig_exposer.def( 
                "SetAudioSize"
                , SetAudioSize_function_type( &::CLAM::FFTConfig::SetAudioSize )
                , ( bp::arg("arg") ) );
        
        }
    }

}
