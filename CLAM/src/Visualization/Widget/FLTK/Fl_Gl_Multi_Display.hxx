/*
 * Copyright (c) 2004 MUSIC TECHNOLOGY GROUP (MTG)
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

#ifndef __FL_GL_MULTI_DISPLAY__
#define __FL_GL_MULTI_DISPLAY__

#include "Fl_Gl_2DSurface.hxx"
#include <vector>

namespace CLAMVM
{
	class GLRenderingManager;

	class Fl_Gl_Multi_Display : public Fl_Gl_2DSurface
	{
	public:
		Fl_Gl_Multi_Display( int X, int Y, int W, int H, const char* label = 0 );

		~Fl_Gl_Multi_Display();

		void AddRenderer( GLRenderingManager& mgr );
		void ClearRenderers();
	protected:
		void DrawContents();
	private:
		typedef std::vector< GLRenderingManager* > tContainer;

		tContainer    mRenderers;
	};

}

#endif // Fl_Gl_Multi_Display.hxx
