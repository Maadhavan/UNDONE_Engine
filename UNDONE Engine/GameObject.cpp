/******************************************************************************
Project	:	UNDONE Engine
File	:	GameObject.cpp
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
#include "GameObject.h"
namespace UNDONE_ENGINE {
	/*-----------------------------------------------------------------------------
	Default Constructor.
	-----------------------------------------------------------------------------*/
	GameObject::GameObject( ) {
		//Need to set things to default values.
		//But there is nothing to do here.
	}

	/*----------------------------------------------------------------------------
	The Default Destructor.
	----------------------------------------------------------------------------*/
	void GameObject::Release( ) {
		//TODO: See what has to be handled here.
	}
	
	void GameObject::Load( ) {
		//Load all children
		for (auto& childComponent:m_Components) {
			childComponent->Load( );
		}
	}

	void GameObject::Unload( ) {
		//UnLoad all children
		for (auto& childComponent:m_Components) {
			childComponent->Unload( );
		}
	}

	void GameObject::OnParentSet( ) {
		//Tell all children
		for (auto& childComponent:m_Components) {
			childComponent->SetPriority(GetPriority(0)+1,0);
			childComponent->OnParentBeingChilded();
		}
	}

	void GameObject::OnParentBeingChilded( ) {
		//We do nothing...
	}

	void GameObject::Set_Parent(Component* component) {
		component->SetParent(dcast<GameObject, Component>(m_ppMyself));
	}

	#define _GENFUNC_DEF_GAMEOBJECT_CPP_
	#include "GENERATE_FUNCTIONS.h"
	GENFUNCS_ONLY_FOR_COMPONENTS()
}

