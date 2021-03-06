/******************************************************************************
Project	:	UNDONE Engine
File	:	DObjectBuffer.h
Author	:	Anurup Dey
******************************************************************************/
///////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef UNDONE_DOBJECTBUFFER_H
#define UNDONE_DOBJECTBUFFER_H


#include "Undone_Engine.h"
#include "UnObjectBuffer.h"
#include <vector>					//We use these for storage.
#include <list>
#include <algorithm>

#include "Header.h"
#include "Component.h"				//The Object buffer can only store components
#include "Camera.h"
#include "Graphic2D.h"

using namespace std;

namespace UNDONE_ENGINE {
	
	typedef  unsigned int OwnerShip;

/*-------------------------------------------------------------------------
THe Object buffer is the place where all the components of the game are 
physically stored. THis Object buffer has the capabiltity to store any 
type of component you throw at it.
-------------------------------------------------------------------------*/
	class DObjectBuffer : public UnObjectBuffer {
		vector<Dptr<Component>> m_Components;
		vector<void*>				m_storage_vectors;
		vector<void*>				m_storage_lists;
		vector<size_t>				m_storage_types;
		vector<OwnerShip>			m_storage_owners;
		bool						m_empty;

		Camera						m_Cam;
		unsigned int				m_init_vec_size;
		unsigned int				m_num_owners;

		unGraphic2D& Getttt(){ return (unGraphic2D&)ttt; }
	public:

		Graphic2D ttt;
		DObjectBuffer( );
		~DObjectBuffer( );

		void SetInitAllocSize(unsigned int size) { m_init_vec_size = size; }
		OwnerShip CreateOwnerShip( ) { ++m_num_owners; return m_num_owners; }

		template<typename T>
		void DeleteAll(OwnerShip ownership = 0 );

		template<typename T>
		Dptr<T> CreateNew(OwnerShip ownership = 0);

		template<typename T>
		vector<T>* GetListOf(OwnerShip ownership = 0);
		template<typename T>
		void SortByPriority(OwnerShip ownership = 0);

		Camera& GetControlCamera( ) { return m_Cam; }
		Dptr<Component> GetComponentByName(const char* name, OwnerShip ownership = 0);
		
		template<typename T>
		Dptr<T> GetComponentByNameOfType(const char* name, OwnerShip ownership = 0);

	#define _GENFUNC_DEC_DOBJECTBUFFER_H_
	#include "GENERATE_FUNCTIONS.h"
	GENFUNCS()

	};

	//////////////////////////Definitions//////////////////////////////////////
	//Prototypes of templates used.
	template<typename storagetype>
	bool Reallocating(vector<storagetype>& vec_to_check);
	template<typename storagetype>
	void Reallocate_vector(vector<storagetype>& storageVector,
						   list<storagetype*>& storageList);
	template<typename storagetype>
	storagetype** MakeNew(vector<storagetype>& storageVector,
						  list<storagetype*>& storageList);

	/*-----------------------------------------------------------------------------
	Template function for checking if a container vector is about to relocate or
	not.
	Parameters:
	[IN] storagetype	-	the type of objects that are stored
	[IN] vec_to_check	-	the vector to check.
	Returns:
	true if about to resize, false otherwise.
	-----------------------------------------------------------------------------*/
	template<typename storagetype>
	bool Reallocating(vector<storagetype>& vec_to_check) {
		if (!vec_to_check.empty( )) {
			if (vec_to_check.size( )==vec_to_check.capacity( )) {
				//things gonna change
				return true;
			}
		}
		return false;
	}

	/*-----------------------------------------------------------------------------
	Called when a vector reallocates to update the corresponding linked list.
	Parameters:
	[IN] storagetype	-	the type of objects that are stored.
	[IN] storageVector	-	the vector (which is reallocating)
	[IN] storageList	-	the linked list which will be updated.
	-----------------------------------------------------------------------------*/
	template<typename storagetype>
	void Reallocate_vector(vector<storagetype>& storageVector,
						   list<storagetype*>& storageList) {
		int i = 0;
		for (auto& ref_in_list:storageList) {
			ref_in_list = &storageVector[i];
			i++;
		}
		//NOTE: this doesn't yet allow for rearrangement of objects in the 
		//vector, so don't try that yet.
	}

	/*-----------------------------------------------------------------------------
	Template function that adds an object to the buffer by creating an instance fir
	-st and the returns a pointer-to-pointer to that object created.
	Para meters:
	[IN] storagetype	-	the type of objects that are stored.
	[IN] storageVector	-	the vector (which is reallocating)
	[IN] storageList	-	the linked list.
	-----------------------------------------------------------------------------*/
	template<typename storagetype>
	storagetype* MakeNew(vector<storagetype>& storageVector,
						  list<storagetype*>& storageList) {
		bool reallocate_list = Reallocating<storagetype>(storageVector);

		storagetype object;
		storageVector.push_back(object);

		if (reallocate_list) {
			Reallocate_vector<storagetype>(storageVector, storageList);
		}

		storageList.push_back(&storageVector.back( ));
		return &storageList.back( );
	}

	
#include <type_traits>
	template<typename T>
	Dptr<T> DObjectBuffer::CreateNew(OwnerShip ownership) {
		m_empty = false;

		vector<T>*	pvec = nullptr;
		list<T*>*	plist = nullptr;

		//Check if we already store this type of objects.
		size_t this_type = typeid(T).hash_code( );
		for (unsigned x = 0; x<m_storage_owners.size( ); ++x) {
			if (m_storage_owners[x]==ownership) {
				if (m_storage_types[x]==this_type) {
					//This type is stored.
					//we now get the corresponding vector and list.
					pvec = (vector<T>*)m_storage_vectors[x];
					plist = (list<T*>*)m_storage_lists[x];
				}
				
			}
		}
		
		if (pvec==nullptr && plist==nullptr) {
			//the type is not already stored. make arrangements
			//to store it.
			pvec = new vector<T>( );
			plist = new list<T*>( );
			//Make pre-Allocated space.
			pvec->reserve(m_init_vec_size);

			m_storage_vectors.push_back((void*)pvec);
			m_storage_lists.push_back((void*)plist);
			m_storage_types.push_back(this_type);
			m_storage_owners.push_back(ownership);
		}

		T* pointer = MakeNew<T>(*pvec, *plist);
		Dptr<T> returnval;
		returnval.m_pointer = pointer;
		//Components are kept specially, So that they can be
		//searched up by name later
		if (is_base_of<Component,T>::value) {
			//That line above checks if the type is derived from Component
			//or not.

			//So now we proceed...
			Dptr<Component> clrg;
			clrg.m_pointer = (Component**)returnval.m_pointer;
			m_Components.push_back(clrg);
			//give the this DPOinter
			clrg->m_ppMyself = clrg;
		}
		return returnval;
	}

	/*-------------------------------------------------------------------------
	Deletes all objects of the given type.
	Parameters	:	
		T	-	the type of objects to be deleted.
	Since the buffer doesn't have access to the type of objects it stores,
	Each system should be responsible for calling this function for each 
	type of object it uses.
	----------------------------------------------------------------------------*/
	template<typename T>
	void DObjectBuffer::DeleteAll(OwnerShip ownership) {
		vector<T>*	pvec = nullptr;
		list<T*>*	plist = nullptr;

		//Check if we already store this type of objects.
		size_t this_type = typeid(T).hash_code( );
		for (unsigned i = 0; i<m_storage_owners.size( ); ++i) {
			if (m_storage_owners[i]==ownership) {
				if (m_storage_types[i]==this_type) {
					//This type is stored.
					//we now get the corresponding vector and list.
					pvec = (vector<T>*)m_storage_vectors[i];
					plist = (list<T*>*)m_storage_lists[i];
					//Now we empty the list and the vector.
					pvec->clear( ); plist->clear( );

					//delete the vector and list.
					delete pvec;
					delete plist;
					//erase the pos in storage.
					m_storage_vectors.erase(m_storage_vectors.begin( )+i);
					m_storage_lists.erase(m_storage_lists.begin( )+i);
					//We don't store this type anymore.
					m_storage_types.erase(m_storage_types.begin( )+i);
					m_storage_owners.erase(m_storage_owners.begin( )+i);
					return;
				}
			}
		}
		return;
	}

	/*----------------------------------------------------------------------------
	Useed to Get a vector of corresponding things.
	----------------------------------------------------------------------------*/
	template<typename T>
	vector<T>* DObjectBuffer::GetListOf(OwnerShip ownership ) {

		//Check if we already store this type of objects.
		size_t this_type = typeid(T).hash_code( );
		for (unsigned x = 0; x<m_storage_owners.size( ); ++x) {
			if (m_storage_owners[x]==ownership) {
				if (m_storage_types[x]==this_type) {
					//This type is stored.
					//we now get the corresponding vector
					return ((vector<T>*)m_storage_vectors[x]);
				}
			}
		}
		//if execution reaches here, we don't store such an object.
		//In such a case, we create an empty vector and add it under
		//the current ownership. 
		vector<T>*	pvec = new vector<T>();
		list<T*>*	plist = new list<T*>();
		//Make pre-Allocated space.
		pvec->reserve(m_init_vec_size);

		m_storage_vectors.push_back((void*)pvec);
		m_storage_lists.push_back((void*)plist);
		m_storage_types.push_back(this_type);
		m_storage_owners.push_back(ownership);

		return pvec;
	}

	/*----------------------------------------------------------------------------
	Returns a Dpointer to a component of given type and name
	Parameter:
	[IN]	T	:	the type of component.
	[IN]	name:	the name of the componet.
	----------------------------------------------------------------------------*/
	template<typename T>
	Dptr<T> DObjectBuffer::GetComponentByNameOfType(const char* name, OwnerShip ownership) {
		for (Dptr<Component>& component:m_Components) {
			if (component->name==name) {
				//Contruct a Dptr of the given type.
				Dptr<T> returnComp;
				returnComp.m_pointer = (T*)(component.m_pointer);
				return returnComp;
			}
		}
		//If the program got untill here, that means there isn't a component 
		//present with that name, so we give out a fake one.
		Dptr<T> ErrorComponent;
		ErrorComponent.m_pointer = nullptr;
		return ErrorComponent;
	}

	template<class T>
	bool Compare_list(T* ra, T* rb) {
		Component* a = (Component*)ra;
		Component* b = (Component*)rb;
		for (unsigned level = 0;
			 a->GetPriority(level)!=-1||b->GetPriority(level)!=-1;
			 ++level) {
			if (a->GetPriority(level)==b->GetPriority(level)) {
				continue;
			} else {
				return a->GetPriority(level)<=b->GetPriority(level);
			}
		}
		return a->GetNumber( ) < b->GetNumber( );;
	}

	template<class T>
	bool Compare_vec(T& a, T& b) {
		//Component* a = (Component*)&ra;
		//Component* b = (Component*)&rb;
		for (unsigned level = 0;
			 a.GetPriority(level)!=-1||b.GetPriority(level)!=-1;
			 ++level) {
			if (a.GetPriority(level)==b.GetPriority(level)) {
				continue;
			} else {
				return (a.GetPriority(level))<=(b.GetPriority(level));
			}
		}
		return a.GetNumber() < b.GetNumber();
	}

	template<typename T>
	void DObjectBuffer::SortByPriority(OwnerShip ownership) {
		//we proceed to sort, only if T is a type of Component.
		if (!is_base_of<Component, T>::value) {
			return;
		}

		//first, we get the vector and lists to sort,
		vector<T>* pvec = nullptr;
		list<T*>* plist = nullptr;
		//Check if we already store this type of objects.
		size_t this_type = typeid(T).hash_code( );
		for (unsigned x = 0; x<m_storage_owners.size( ); ++x) {
			if (m_storage_owners[x]==ownership) {
				if (m_storage_types[x]==this_type) {
					//This type is stored.
					//we now get the corresponding vector and list.
					pvec = (vector<T>*)m_storage_vectors[x];
					plist = (list<T*>*)m_storage_lists[x];
				}

			}
		}

		if (pvec==nullptr && plist==nullptr) {
			//the type is not already stored.
			return;
		}

		//sorting...
		//STEP 1: sort the linked list first...
		plist->sort(Compare_list<T>);
		//STEP 2: sort the vector..
		sort(pvec->begin( ), pvec->end( ), Compare_vec<T>);
		//STEP 3: relink the list to the vector.
		Reallocate_vector<T>(*pvec, *plist);
		//STEP 4 : enjoy the view!
		return;
	}
}
#endif