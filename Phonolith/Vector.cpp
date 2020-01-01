/*
Phonolith Software Sampler
Copyright (C) 2006-2008 Timo Kloss

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <iostream>
#include <stdexcept>
#include <new>

using namespace std;

#include "Vector.h"


//==================================================================================
// Constructor/Destructor
//==================================================================================

Vector::Vector()
{
	size = 0;
	array = 0;
}

//----------------------------------------------------------------------------------

Vector::~Vector()
{
	if (array)
	{
		delete []array;
	}
}

//==================================================================================
// Elements
//==================================================================================

void *Vector::getElement(int n)
{
	if (n >= 0 && n < size)
	{
		return array[n];
	}
	throw out_of_range("Vector index out of range (getElement).");
}

//----------------------------------------------------------------------------------

int Vector::addElement(void *element)
{
	void **newArray = new void*[size + 1];

	if (array)
	{
		for (int i = 0; i < size; i++)
		{
			newArray[i] = array[i];
		}

		delete []array;
	}
	newArray[size] = element;
	array = newArray;
	size++;
	return size - 1; //index of added element
}

//----------------------------------------------------------------------------------

void Vector::insertElement(int n, void *element)
{
	if (n < 0 || n > size)
	{
		throw out_of_range("Vector index out of range (insertElement)");
	}
	
	void **newArray = new void*[size + 1];

	//make space...
	for (int i = 0; i < n; i++)
	{
		newArray[i] = array[i];
	}

	for (int i = n; i < size ; i++)
	{
		newArray[i + 1] = array[i];
	}
			
	delete []array;

	//new element
	newArray[n] = element;
	array = newArray;
	size++;
}

//----------------------------------------------------------------------------------

void Vector::deleteElement(int n)
{
	if (n < 0 || n >= size)
	{
		throw out_of_range("Vector index out of range (deleteElement)");
	}
	
	if (size == 1)
	{
		delete []array;
		array = NULL;
		size = 0;
	}
	else
	{
		size--;
		void **newArray = new void*[size];

		//copy elements to new array...
		for (int i = 0; i < n; i++)
		{
			newArray[i] = array[i];
		}

		for (int i = n; i < size; i++)
		{
			newArray[i] = array[i + 1];
		}

		delete []array;
		array = newArray;
	}
}

//----------------------------------------------------------------------------------

void Vector::deleteElement(void *element)
{
	for (int i = 0; i < size; i++)
	{
		if (array[i] == element)
		{
			deleteElement(i);
			break;
		}
	}
}

//----------------------------------------------------------------------------------

void Vector::deleteAllElements()
{
	if (array)
	{
		delete []array;
	}
		
	array = NULL;
	size = 0;
}

//----------------------------------------------------------------------------------

void Vector::moveElement(int fromN, int toN)
{
	if (fromN < 0 || fromN >= size || toN < 0 || toN >= size)
	{
		throw out_of_range("Vector index out of range (moveElement)");;
	}
	
	void *movElem = array[fromN];
	
	if (toN > fromN)
	{
		for (int i = fromN; i < toN; i++)
		{
			array[i] = array[i + 1];
		}
	}
	else if (toN < fromN)
	{
		for (int i = fromN; i > toN; i--)
		{
			array[i] = array[i - 1];
		}
	}

	array[toN] = movElem;
}

//----------------------------------------------------------------------------------

int Vector::getSize()
{
	return size;
}

