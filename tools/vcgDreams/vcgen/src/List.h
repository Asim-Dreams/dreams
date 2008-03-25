/*
 * Copyright (C) 2004-2006 Intel Corporation
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 */


#ifndef __List_h
#define __List_h

#include <stdlib.h>
#include <assert.h>

// If -MD compiler option (GEM cxx) is used assert is not defined
// eventhough we inserted the corresponding include. 
// This can happens when compiling this .h through another
// specific-purpose makefile...

#ifndef assert
#define assert(n)
#endif

/**
 * Implements a simple (dynamic) element list.
 * vcgen framework component
 *
 * @author Santiago Galan, minor changes due to Federico Ardanaz
 * @date 20-05-2001
 * @version 0.1
 */
template<class T> class List 
{
	private:
	
		/** Small helper struct to encapsulate nodes */
		struct Node 
		{
			Node *next;
			T data;
		};

		Node *m_pnFirst;
		unsigned long m_nElements;

		/** 
		 * Bool that says if in destruction time we also destroy 
		 * the contained data
		 */
		bool m_bEraseData;

	public: 

		/**
		 * Default constructor. 
		 */
		List() : m_pnFirst(NULL), m_nElements(0), m_bEraseData(false) {}
		
		/**
		 * Constructor.
		 * @param bEraseData specifies whether the list elements must be deleted or not when List is deleted
		 */
		List(bool bEraseData) : m_pnFirst(NULL), m_nElements(0), m_bEraseData(bEraseData) {}

		/**
		 * Destructor. 
		 */
		~List()
		{
			for(Node *pnCurrent = m_pnFirst; pnCurrent!=NULL;) 
			{
				Node *pnNext = pnCurrent->next;
				if(m_bEraseData) delete pnCurrent->data;
				delete pnCurrent;
				pnCurrent = pnNext;
			}
		}

		/**
		 * Method that adds an element to list (unordered).
		 *
		 * @param data Element to be added
		 */ 
		inline void add(T data) 
		{
			Node *pnNew = new Node;
			pnNew->data = data;
			pnNew->next = m_pnFirst;
			m_pnFirst = pnNew;
			m_nElements++;
		}

		/**
		 * Says if list is empty or no
		 *
		 * @return TRUE is List is empty, FALSE otherwise
		 */
		inline bool isEmpty(void) { return(m_pnFirst==NULL); }

		/**
		 * Method that sets if data contained at list will be destroyed
		 * or no.
		 *
		 * @param bEraseData Boolean that says if data will be destroyed or not
		 */
		inline void setEraseData(bool bEraseData) { m_bEraseData = bEraseData; }

		/**
		 * Returns the number of elements at List
		 *
		 * @return the number of elements at list
		 */
		inline unsigned long getLen(void) { return m_nElements; }

		inline List& operator+=(const List& right) 
		{ 

			for(Node *pnCurrentRight = right.m_pnFirst; pnCurrentRight != NULL; pnCurrentRight = pnCurrentRight->next) 
				add(pnCurrentRight->data);

			return *this; 
		}

		class Iterator;
        	friend class Iterator;

		/**
		 * Inner class that defines an iterator to walk throught the hash
		 */
		class Iterator 
		{
			private:
				List &m_list;
				Node *m_nodeCurrent;
				/** Previous node, necessary to remove method */
				Node *m_nodePrevious;

			public:
				Iterator(List &l) : m_list(l), m_nodePrevious(NULL) 
				{ m_nodeCurrent = m_list.m_pnFirst; }

				/** Constructor to make an end sentinel */
				Iterator(List &l, bool) : m_list(l), m_nodeCurrent(NULL), m_nodePrevious(NULL) {}

				/**
				* Removes current element and step to next one
				*
				* @return Current data
				*/
				inline T remove(void) 
				{
					assert(m_nodeCurrent!=NULL);
					T currentData = m_nodeCurrent->data;

					if(m_nodePrevious!=NULL) 
						m_nodePrevious->next = m_nodeCurrent->next;
					else
						m_list.m_pnFirst = m_nodeCurrent->next;

					Node *nodeTmp = m_nodeCurrent;
					m_nodeCurrent = nodeTmp->next;
					delete nodeTmp;
					m_list.m_nElements--;
					return currentData;
				}

				/**
				 *
				 */
				inline T operator *() const 
				{
					assert(m_nodeCurrent!=NULL);
					return m_nodeCurrent->data;
				}

				/**
				 * Operator, in prefix form, that walks to next element.
				 */
				inline T operator ++() 
				{
					assert(m_nodeCurrent!=NULL);
					m_nodePrevious = m_nodeCurrent; 
					m_nodeCurrent = m_nodeCurrent->next;
				}

				/**
				 * Operator in postfix form that walks to next element
				 */
				inline T operator ++(int) 
				{
					assert(m_nodeCurrent!=NULL);
					T pCurrent = m_nodeCurrent->data;
					m_nodePrevious = m_nodeCurrent; 
					m_nodeCurrent = m_nodeCurrent->next;
					return pCurrent;
				}

				/** Operator to see if you're at the end */
				inline bool operator==(const Iterator& rv) const 
				{return ( m_nodeCurrent == rv.m_nodeCurrent);}

				/** Operator to see if you're at the end */
				inline bool operator!=(const Iterator& rv) const 
				{return ( m_nodeCurrent != rv.m_nodeCurrent);}
		};

		/**
		 * Method to get the iterator 
		 *
		 * @return iterator at the begining of the pattern
		 */
		inline Iterator begin() { return Iterator(*this); }

		/** 
		 * Method to create the "end sentinel" iterator
		 * @return iterator at the end of the pattern
		 */
		inline Iterator end() { return Iterator(*this, true); }
};

#endif 

