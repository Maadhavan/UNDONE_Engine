/******************************************************************************
File	:	ShaderProgram.h
Author	:	Anurup Dey

				Copyright (C) 2015  Anurup Dey

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

******************************************************************************/
///////////////////////////////////////////////////////////////////////////////
#include "ShaderProgram.h"
#include "Shader.h"
#include <glew.h>

namespace UNDONE_ENGINE {

	UINT ShaderProgram::CurrentProgramInUse = 0;

	/*-----------------------------------------------------------------------------
	Defualt constructor.
	-----------------------------------------------------------------------------*/
	ShaderProgram::ShaderProgram( ) {
		uiProgram = 0;
		bLinked = false;
	}

	/*-----------------------------------------------------------------------------
	Creates the program.
	-----------------------------------------------------------------------------*/
	void ShaderProgram::CreateProgram( ) {
		uiProgram = glCreateProgram( );
	}

	/*-----------------------------------------------------------------------------
	Deletes the Program.
	-----------------------------------------------------------------------------*/
	void ShaderProgram::DeleteProgram( ) {
		if (!bLinked)return;
		bLinked = false;
		glDeleteProgram(uiProgram);
	}

	/*-----------------------------------------------------------------------------
	Adds a shader to the program.
	Parameters:
	[IN]	shShader	:	the shader to add.
	Returns:
	true on success, false on falure.
	-----------------------------------------------------------------------------*/
	bool ShaderProgram::AddShaderToProgram(unShader* shShader) {
		Shader* pShader = (Shader*)shShader;
		if (!pShader->IsLoaded( ))return false;

		glAttachShader(uiProgram, pShader->GetID( ));

		return true;
	}

	/*-----------------------------------------------------------------------------
	Links the program and makes it ready to be used.
	-----------------------------------------------------------------------------*/
	bool ShaderProgram::LinkProgram( ) {
		glLinkProgram(uiProgram);
		int iLinkStatus;
		glGetProgramiv(uiProgram, GL_LINK_STATUS, &iLinkStatus);
		bLinked = iLinkStatus==GL_TRUE;
		return bLinked;
	}

	/*-----------------------------------------------------------------------------
	Makes the program come under use
	-----------------------------------------------------------------------------*/
	void ShaderProgram::UseProgram( ) {
		if (bLinked){
			if (uiProgram != CurrentProgramInUse) glUseProgram(uiProgram);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////