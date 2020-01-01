#ifndef _FRAMEAPP_
#define _FRAMEAPP_


#include "FrameWindow.h"
#include "Vector.h"

#include <libraries/application.h>
#include <intuition/classusr.h>
#include <intuition/intuition.h>

class FrameWindow;

struct WindowBorders
{
	string idName;
	int left;
	int top;
	int width;
	int height;
};

class FrameApp
{
	public:
		FrameApp(char *name);
		virtual ~FrameApp();

		virtual void menuPicked(int menuID) = 0;
		virtual void quitRequested() = 0;
		virtual void openProject(const char *fileName) = 0;

		void addWindow(FrameWindow *inWindow);
		void removeWindow(FrameWindow *inWindow);

		void checkWindows(unsigned long signals);
		void hideApplication();
		void showApplication();
		void toFront();

		void setWindowBorders(const char *idName, int left, int top, int width, int height);
		WindowBorders *getWindowBorders(const char *idName);
		void rememberAllWindowBorders();
		void addPrefsWindowBorders(PrefsObject *dict);
		void getPrefsWindowBorders(PrefsObject *dict);

		const static int REQ_QUESTION    = 1;
		const static int REQ_INFORMATION = 2;
		const static int REQ_WARNING     = 3;
		const static int REQ_ERROR       = 4;

		static void systemMessage(int type, const char *text);
		void message(int type, const char *text);
		int question(int type, const char *text, const char *buttons);

		void setBusyMouse(bool active);

		char *mName;
		unsigned long mSigMask;

		static Hook mIDCMPHook;

	private:
		static uint32 IDCMPFunc(Hook *hook, Object *window, IntuiMessage *msg);
		void refreshSigMask();

		Vector mFWindows;
		Vector mWindowBorders;

};

#endif

