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

#include "Phonolith.h"
#include "Version.h"
#include "FrameGui.h"
#include "SysMidi.h"
#include "SysAudio.h"
#include "BoopsiPeakMeter.h"
#include "BoopsiTextLabel.h"
#include "IOAiffOutput.h"
#include "Texts.h"

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/application.h>
#include <proto/icon.h>

#include <libraries/application.h>
#include <workbench/startup.h>


char *amigaVersionString = "$VER: "APPNAME" "APPVERSION" "APPDATE;
FrameApp *main_app = NULL;

//==================================================================================
// Library Handling
//==================================================================================

Library           *ApplicationBase = NULL;
ApplicationIFace  *IApplication    = NULL;
PrefsObjectsIFace *IPrefsObjects   = NULL;

//----------------------------------------------------------------------------------

void main_openLib()
{
	ApplicationBase = IExec->OpenLibrary("application.library", 50);
	if (ApplicationBase)
	{
		IApplication = (ApplicationIFace *) IExec->GetInterface(ApplicationBase, "application", 1, NULL);
		IPrefsObjects = (PrefsObjectsIFace *) IExec->GetInterface(ApplicationBase, "prefsobjects", 1, NULL);
	}
	if (!ApplicationBase || !IApplication || !IPrefsObjects)
	{
		throw runtime_error("Could not open application.library (version 50 or higher)");
	}
}

//----------------------------------------------------------------------------------

void main_closeLib()
{
	IExec->DropInterface((Interface *) IApplication);
	IApplication = NULL;
	IExec->DropInterface((Interface *) IPrefsObjects);
	IPrefsObjects = NULL;
	IExec->CloseLibrary(ApplicationBase);
	ApplicationBase = NULL;
}

//==================================================================================
// Error Message
//==================================================================================

void showError(exception &e)
{
	char *text = NULL;
	exception *ex = &e;
	if (dynamic_cast<bad_alloc *>(ex))
	{
		text = "Not enough free memory.";
	}
	else
	{
		text = (char *) e.what();
	}

	if (main_app)
	{
		try
		{
			main_app->message(FrameApp::REQ_ERROR, text);
		}
		catch (exception &e)
		{
			cerr << APPNAME" Exception: " << text << endl;
		}
	}
	else
	{
		try
		{
			FrameApp::systemMessage(FrameApp::REQ_ERROR, text);
		}
		catch (exception &e)
		{
			cerr << APPNAME" Exception: " << text << endl;
		}

	}
}

//==================================================================================
// Application Library
//==================================================================================

unsigned long main_appID = 0;
MsgPort *main_appPort = NULL;
unsigned long main_appSigMask = 0;
bool main_hasDockIcon = false;

//----------------------------------------------------------------------------------

void main_registerApplication(int argc, char *argv[])
{
	WBStartup *wbstartup = NULL;

	if (argc == 0)
	{
		wbstartup = (WBStartup *) argv;
		main_hasDockIcon = true;
	}

	main_appID = IApplication->RegisterApplication(APPNAME,
			REGAPP_URLIdentifier, "inutilis.de",
			REGAPP_WBStartup, wbstartup,
			REGAPP_LoadPrefs, TRUE,
			//REGAPP_NoIcon, TRUE,
			TAG_DONE);

	IApplication->GetApplicationAttrs(main_appID, APPATTR_Port, &main_appPort, TAG_DONE);
	main_appSigMask = 1L << main_appPort->mp_SigBit;
}

//----------------------------------------------------------------------------------

void main_unregisterApplication()
{
	IApplication->UnregisterApplication(main_appID, NULL);
}

//==================================================================================
// AmiUpdate
//==================================================================================

void SetAmiUpdateENVVariable( char *varname )
{
  /* AmiUpdate support code */

  BPTR lock;
  APTR oldwin;

  /* obtain the lock to the home directory */
  if(( lock = IDOS->GetProgramDir() ))
  {
    TEXT progpath[2048];
    TEXT varpath[1024] = "AppPaths";

    /*
    get a unique name for the lock,
    this call uses device names,
    as there can be multiple volumes
    with the same name on the system
    */

    if( IDOS->DevNameFromLock( lock, progpath, sizeof(progpath), DN_FULLPATH ))
    {
	  /* stop any "Insert volume..." type requesters */
      oldwin = IDOS->SetProcWindow((APTR)-1);

      /*
      finally set the variable to the
      path the executable was run from
      don't forget to supply the variable
      name to suit your application
      */

      IDOS->AddPart( varpath, varname, 1024);
      IDOS->SetVar( varpath, progpath, -1, GVF_GLOBAL_ONLY|GVF_SAVE_VAR );

      /* turn requesters back on */
      IDOS->SetProcWindow( oldwin );
    }
  }
}

//==================================================================================
// Icon
//==================================================================================

DiskObject *main_applicationIcon = NULL;

//----------------------------------------------------------------------------------

void main_loadIcon()
{
	main_applicationIcon = IIcon->GetDiskObjectNew("PROGDIR:"APPNAME);
	if (!main_applicationIcon)
	{
		throw runtime_error("Could not get application icon.");
	}
}

//----------------------------------------------------------------------------------

void main_freeIcon()
{
	IIcon->FreeDiskObject(main_applicationIcon);
	main_applicationIcon = NULL;
}

//==================================================================================
// Main
//==================================================================================

bool main_quit = false;
int main_meterUpdateSignal = -1;
int main_statusUpdateSignal = -1;


int main(int argc, char *argv[])
{
	try
	{
		main_openLib();
		Texts::init();
		main_loadIcon();
		FrameGui::openLib();
		PEAKMETER_MakeBoopsiClass();
		TEXTLABEL_MakeBoopsiClass();
		SysMidi::openLib();
		SysAudio::openLib();
		Phonolith::loadImages();

		main_registerApplication(argc, argv);

		main_app = new Phonolith();

		main_meterUpdateSignal = IExec->AllocSignal(-1);
		unsigned long meterRefreshSigMask = (main_meterUpdateSignal != -1) ? (1L << main_meterUpdateSignal) : 0;

		main_statusUpdateSignal = IExec->AllocSignal(-1);
		unsigned long statusRefreshSigMask = (main_statusUpdateSignal != -1) ? (1L << main_statusUpdateSignal) : 0;

		SetAmiUpdateENVVariable(APPNAME);

		do
		{
			unsigned long signals = IExec->Wait(main_app->mSigMask | meterRefreshSigMask | statusRefreshSigMask | main_appSigMask);
			if (signals & meterRefreshSigMask)
			{
				((Phonolith *) main_app)->updateMixer();
			}

			if (signals & statusRefreshSigMask)
			{
				((Phonolith *) main_app)->updateStatus();
			}

			try
			{
				if (signals & main_appSigMask)
				{
					ApplicationMsg *msg;
					while (msg = (ApplicationMsg *) IExec->GetMsg(main_appPort))
					{
					    switch (msg->type)
					    {
					        case APPLIBMT_Quit:
								main_app->quitRequested();
							break;

							case APPLIBMT_ForceQuit:
								main_quit = true;
							break;

							case APPLIBMT_ToFront:
								main_app->toFront();
							break;

							case APPLIBMT_OpenDoc: {
								ApplicationOpenPrintDocMsg *openMsg = (ApplicationOpenPrintDocMsg *) msg;
								main_app->openProject(openMsg->fileName);
							} break;
					    }

						IExec->ReplyMsg((Message *)msg);
					}
				}

				main_app->checkWindows(signals);
			}
			catch (exception &e)
			{
				showError(e);
			}

		} while (!main_quit);

	}
	catch (exception &e)
	{
		showError(e);
	}

	if (main_app)
	{
		delete main_app;
	}

	main_unregisterApplication();

	Phonolith::freeImages();
	FrameGui::closeLib();
	SysMidi::closeLib();
	SysAudio::closeLib();
	PEAKMETER_FreeBoopsiClass();
	TEXTLABEL_FreeBoopsiClass();
    main_freeIcon();
	Texts::dispose();
	main_closeLib();

	IExec->FreeSignal(main_meterUpdateSignal);
	IExec->FreeSignal(main_statusUpdateSignal);

	return 0;
}

