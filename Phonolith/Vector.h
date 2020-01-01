#ifndef _TIMOVECTOR_
#define _TIMOVECTOR_


class Vector
{
	public:
		Vector();
		~Vector();
		void       *getElement(int n);
		int        addElement(void *element);
		void       insertElement(int n, void *element);
		void       deleteElement(int n);
		void       deleteElement(void *element);
		void       deleteAllElements();
		void       moveElement(int fromN, int toN);
		int        getSize();

	private:
		void       **array;
		long int   size;
};

#endif

