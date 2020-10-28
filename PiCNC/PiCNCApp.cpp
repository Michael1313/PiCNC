/***************************************************************
 * Name:      PiCNCApp.cpp
 * Purpose:   Code for Application Class
 * Author:    MBajor ()
 * Created:   2019-10-10
 * Copyright: MBajor ()
 * License:
 **************************************************************/

#include "PiCNCApp.h"

//(*AppHeaders
#include "PiCNCMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(PiCNCApp);

bool PiCNCApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	PiCNCFrame* Frame = new PiCNCFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
