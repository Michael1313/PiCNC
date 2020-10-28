/***************************************************************
 * Name:      PiCNCEngraverApp.cpp
 * Purpose:   Code for Application Class
 * Author:    MBajor ()
 * Created:   2019-12-22
 * Copyright: MBajor ()
 * License:
 **************************************************************/

#include "PiCNCEngraverApp.h"

//(*AppHeaders
#include "PiCNCEngraverMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(PiCNCEngraverApp);

bool PiCNCEngraverApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	PiCNCEngraverFrame* Frame = new PiCNCEngraverFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
