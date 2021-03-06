/******************************************************************************
Project	:	UNDONE Engine
File	:	Font.cpp
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
#include "Font.h"
#include "ShaderProgram.h"
#include "SystemComponent.h"

#include <freetype.h>
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <cmath>
#include <iostream>

namespace UNDONE_ENGINE { 
	
	Font::Font( ) {
		bLoaded = false;
		m_ppShaderProgram.m_pointer = nullptr;
	}

	Font::~Font( ) { };

	/*--------------------------------------------------------------------------
	Name:	createChar
	Params:	iIndex - character index in Unicode.
	Result:	Creates one single character (its
	texture).
	/*-------------------------------------------------------------------------*/

	inline int next_p2(int n) { int res = 1; while (res < n)res <<= 1; return res; }

	void Font::createChar(int iIndex) {
		
		FT_Load_Glyph(m_ftFace, FT_Get_Char_Index(m_ftFace, iIndex), FT_LOAD_DEFAULT);

		FT_Render_Glyph(m_ftFace->glyph, FT_RENDER_MODE_NORMAL);
		FT_Bitmap* pBitmap = &m_ftFace->glyph->bitmap;

		int iW = pBitmap->width, iH = pBitmap->rows;
		int iTW = next_p2(iW), iTH = next_p2(iH);

		GLbyte* bData = new GLbyte[iTW*iTH];
		// Copy glyph data and add dark pixels elsewhere
		for (int ch = 0; ch<iTH; ++ch) {
			for (int cw = 0; cw<iTW; ++cw) {
				bData[ch*iTW+cw] = (ch>=iH||cw>=iW) ? 0 : pBitmap->buffer[(iH-ch-1)*iW+cw];
			}
		}
		// And create a texture from it

		tCharTextures[iIndex].CreateFromData(bData, iTW, iTH, 8, GL_DEPTH_COMPONENT, false);
		tCharTextures[iIndex].setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);

		tCharTextures[iIndex].setSamplerParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		tCharTextures[iIndex].setSamplerParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Calculate glyph data
		iAdvX[iIndex]		= m_ftFace->glyph->advance.x>>6;
		iBearingX[iIndex]	= m_ftFace->glyph->metrics.horiBearingX>>6;
		iCharWidth[iIndex]	= m_ftFace->glyph->metrics.width>>6;

		iAdvY[iIndex]		= (m_ftFace->glyph->metrics.height-m_ftFace->glyph->metrics.horiBearingY)>>6;
		iBearingY[iIndex]	= m_ftFace->glyph->metrics.horiBearingY>>6;
		iCharHeight[iIndex] = m_ftFace->glyph->metrics.height>>6;

		iNewLine = std::fmax(iNewLine, int(m_ftFace->glyph->metrics.height>>6));

		// Rendering data, texture coordinates are always the same, so now we waste a little memory
		glm::vec2 vQuad[] =
		{
			glm::vec2(0.0f, float(-iAdvY[iIndex]+iTH)),
			glm::vec2(0.0f, float(-iAdvY[iIndex])),
			glm::vec2(float(iTW), float(-iAdvY[iIndex]+iTH)),
			glm::vec2(float(iTW), float(-iAdvY[iIndex]))
		};
		glm::vec2 vTexQuad[] = {glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)};

		// Add this char to VBO
		for (int i = 0; i<4; ++i) {
			vboData.insert(vboData.end( ), (unsigned char*)&vQuad[i], (unsigned char*)&vQuad[i]+sizeof(glm::vec2));
			vboData.insert(vboData.end( ), (unsigned char*)&vTexQuad[i], (unsigned char*)&vTexQuad[i]+sizeof(glm::vec2));
		}
		delete[] bData;
		

	}

	/*-------------------------------------------------------------------------
	Name:	loadFont
	Params:	sFile - path to font file
	iPXSize - desired font pixel size
	Result:	Loads whole font.
	/*-----------------------------------------------------------------------*/

	bool Font::LoadFont(string File, int PixelSize) {
		bool bError = (bool)FT_Init_FreeType(&m_ftLib);

		bError = (bool)FT_New_Face(m_ftLib, File.c_str( ), 0, &m_ftFace);
		if (bError)return false;
		FT_Set_Pixel_Sizes(m_ftFace, PixelSize, PixelSize);
		iLoadedPixelSize = PixelSize;

		glGenVertexArrays(1, &m_uiVAO);
		glBindVertexArray(m_uiVAO);
		glGenBuffers(1, &m_uiVBO);
		
		glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
		
		for (int i = 0; i<128; ++i)createChar(i);
		bLoaded = true;
		
		
		FT_Done_Face(m_ftFace);
		FT_Done_FreeType(m_ftLib);

		glBufferData(GL_ARRAY_BUFFER, vboData.size( )*sizeof(unsigned char), &vboData[0], GL_STATIC_DRAW);
		vboData.clear( );
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2)*2, 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2)*2, (void*)(sizeof(glm::vec2)));
		
		cout<<"FONT LOADED!\n\n";
	
		return true;
	}

	/*--------------------------------------------------------------------------
	Name:	loadSystemFont
	Params:	sName - system font name
	iPXSize - desired font pixel size
	Result:	Loads system font (from system Fonts
	directory).
	/*-------------------------------------------------------------------------*/
	bool Font::LoadSystemFont(string sName, int iPXSize) {
		char buf[512]; 
		SystemComponent::GetInstance()->GetSystemDirectory_(buf, 512); 
		string sPath = buf;
		sPath += "\\Fonts\\";
		sPath += sName;

		return LoadFont(sPath, iPXSize);
	}

	/*---------------------------------------------------------------------------
	Name:	print
	Params:	sText - text to print
	x, y - 2D position
	iPXSize - printed text size
	Result:	Prints text at specified position
	with specified pixel size.
	/*-------------------------------------------------------------------------*/
	void Font::print(string sText, int x, int y, int iPXSize) {

		if (!bLoaded)return;
		if (m_ppShaderProgram.m_pointer){
			GLuint progID = m_ppShaderProgram->GetProgramID( );
			m_ppShaderProgram->UseProgram();
			glBindVertexArray(m_uiVAO);
			glUniform1i(glGetUniformLocation(progID, "gSampler"), 0);
			
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			
			int iCurX = x, iCurY = y;
			if (iPXSize==-1)iPXSize = iLoadedPixelSize;
			float fScale = float(iPXSize)/float(iLoadedPixelSize);
			for (unsigned int i = 0; i<sText.size( ); ++i) {
				if (sText[i]=='\n') {
					iCurX = x;
					iCurY -= iNewLine*iPXSize/iLoadedPixelSize;
					continue;
				}
				int iIndex = int(sText[i]);
				iCurX += 1;// ((iBearingX[iIndex])*iPXSize / iLoadedPixelSize);
				if (sText[i]!=' ') {
					
					tCharTextures[iIndex].BindTexture( );
					glm::mat4 mModelView = glm::translate(glm::mat4(1.0f), glm::vec3(float(iCurX)/32, float(iCurY)/32, 0.0f));
					mModelView = glm::scale(mModelView, glm::vec3(fScale/64));
					glUniformMatrix4fv(glGetUniformLocation(progID,"gMVP"), 1,GL_FALSE, &mModelView[0][0]);
					// Draw character
					glDrawArrays(GL_TRIANGLE_STRIP, iIndex*4, 4);
					
				}

				iCurX += 1;// ((iAdvX[iIndex] - iBearingX[iIndex]))*iPXSize / iLoadedPixelSize;
			}
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
	
			std::cout<<"PRINTING TEXT!\n";
			

		}
	}

	/*--------------------------------------------------------------------------
	Name:		printFormatted
	Params:	x, y - 2D position
	iPXSize - printed text size
	sText - text to print
	Result:	Prints formatted text at specified position
	with specified pixel size.
	/*-------------------------------------------------------------------------*/

	void Font::printFormatted(int x, int y, int iPXSize, char* sText, ...) {
		char buf[512];
		va_list ap;
		va_start(ap, sText);
		vsprintf_s<512>(buf, sText, ap);
		va_end(ap);
		print(buf, x, y, iPXSize);
	}

	/*--------------------------------------------------------------------------

	Name:		releaseFont

	Params:	none

	Result:	Deletes all font textures.

	/*--------------------------------------------------------------------------*/
	void Font::releaseFont( ) {
		for (int i = 0; i<128; ++i)tCharTextures[i].releaseTexture( );
		glDeleteBuffers(1, &m_uiVBO);
		vboData.clear( );
		glDeleteVertexArrays(1, &m_uiVAO);
	}

	/*-----------------------------------------------------------------------------

	Name:	setShaderProgram

	Params:	a_shShaderProgram - shader program

	Result:	Sets shader program that font uses.

	/*-------------------------------------------------------------------------*/

	void Font::SetShaderProgram(Dptr<ShaderProgram> a_shShaderProgram) {
		m_ppShaderProgram = a_shShaderProgram;
	}
}