/*
 * Copyright (c) 2001-2004 MUSIC TECHNOLOGY GROUP (MTG)
 *                         UNIVERSITAT POMPEU FABRA
 *
 *
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

/** @file UserTutorial1.cxx
 * Tutorial example ilustrating the creation and configuration
 * of processing objects
 */

// We include the FFT class
#include <FFT.hxx>

// And C++ input-output library to write the output message.
#include <iostream> 

int main(void)
{
	// This creates an FFT configuration object
	CLAM::FFTConfig my_fft_config;

	// This sets some configuration parameters.
	my_fft_config.SetAudioSize(1024);

	// And this finally creates the FFT object.
	CLAM::FFT my_fft(my_fft_config);
}
