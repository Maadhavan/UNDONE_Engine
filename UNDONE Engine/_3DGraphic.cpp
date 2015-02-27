/******************************************************************************
File	:	_3DGraphic.cpp
Author	:	Anurup Dey
******************************************************************************/
///////////////////////////////////////////////////////////////////////////////
#include "_3DGraphic.h"
#include "GameObject.h"
#include<glm.hpp>
#include<gtx\projection.hpp>
#include<gtx\transform.hpp>
#include "UNDONE_DEBUG.h"
using namespace std;


/*-----------------------------------------------------------------------------
Default Constructor
-----------------------------------------------------------------------------*/
_3DGraphic::_3DGraphic(){
	m_ppMaterial.m_pointer			= nullptr;
	m_ppworldTransform.m_pointer	= nullptr;
	m_ppMesh.m_pointer				= nullptr;
	
}

/*------------------------------------------------------------------------------
Default destructor
-----------------------------------------------------------------------------*/
void _3DGraphic::Release(){

}

/*-----------------------------------------------------------------------------
This callback function is called when the Context is created. Initialize all
the graphical resources here.
-----------------------------------------------------------------------------*/
void _3DGraphic::OnInit(){

}

/*-----------------------------------------------------------------------------
This callback function is called when the context is deleted. Release all 
Graphical resources here.
-----------------------------------------------------------------------------*/
void _3DGraphic::OnDestroy(){

}

/*-----------------------------------------------------------------------------
Renders the _3Dgraphic on the screen.
-----------------------------------------------------------------------------*/
void _3DGraphic::Render(RenderParams& refRenderParams){
	if (m_ppMaterial.m_pointer && 
		m_ppworldTransform.m_pointer &&
		m_ppMesh.m_pointer) {
		
				
		glm::mat4 mMVP = refRenderParams.View_x_Projection*(m_ppworldTransform.ptr( )->GetTransform( ));
		
		UniformDataInterface &MatData = m_ppMaterial.Obj( ).GetUniformDataInterface( );
		MatData.pairs[0].data.Data_fp = &mMVP[0][0];
		MatData.pairs[1].data.Data_fp = &(m_ppworldTransform.Obj( ).GetTransform( )[0][0]);

		m_ppMaterial.Obj( ).ApplyMaterial( );
		
		m_ppMesh.ptr( )->Render( );
	
	}

}

/*-----------------------------------------------------------------------------
Sets the parent of this 3dGraphic Component.
-----------------------------------------------------------------------------*/
void _3DGraphic::SetParent(DPointer<GameObject> ppParent) {
	Component::SetParent(ppParent);
	if (m_ppParent.m_pointer) {
		m_ppworldTransform = (m_ppParent.ptr( ))->worldTransform;
		m_ppMesh = (m_ppParent.ptr( )->mesh);
		m_ppMaterial = m_ppParent.ptr( )->graphicMaterial;
		coutput(name+" aquired transformation "+m_ppworldTransform.Obj( ).name+"\n");
		coutput(name+" aquired mesh "+m_ppMesh.Obj( ).name+"\n");
		coutput(name+" aquired material "+m_ppMaterial.Obj( ).name+"\n");
	}
}

unsigned int SimpleObject::num_objects = 0;
/*-----------------------------------------------------------------------------
Default Constructor
-----------------------------------------------------------------------------*/
SimpleObject::SimpleObject(){
	
}




string operator +(string lhs, int rhs) {
	char numstr[20];
	sprintf_s(numstr, "%i", rhs);
	return lhs+numstr;
}

string operator +(string lhs, float rhs) {
	char numstr[20];
	sprintf_s(numstr, "%f", rhs);
	return lhs+numstr;
}