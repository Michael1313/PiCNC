/***************************************************************
 * Name:      PiCNCMain.h
 * Purpose:   Defines Application Frame
 * Author:    MBajor ()
 * Created:   2019-10-10
 * Copyright: MBajor ()
 * License:
 **************************************************************/

#ifndef PICNCMAIN_H
#define PICNCMAIN_H

#include <GL/glew.h>
#include <GL/glut.h>
//(*Headers(PiCNCFrame)
#include <wx/bmpbuttn.h>
#include <wx/filedlg.h>
#include <wx/frame.h>
#include <wx/glcanvas.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/statusbr.h>
//*)

class BasicGLPane : public wxGLCanvas
{
    wxGLContext*	m_context;

public:
	BasicGLPane(wxFrame* parent, int* args);
	virtual ~BasicGLPane();

	void render(wxPaintEvent& evt);

    void prepare3DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y);
	void prepare2DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y);

	// events
	void mouseMoved(wxMouseEvent& event);
	void angleChange(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void rightClick(wxMouseEvent& event);
	void rightUp(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);

	DECLARE_EVENT_TABLE()
};

class PiCNCFrame: public wxFrame
{
    public:

        PiCNCFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~PiCNCFrame();

        void OnPopupClick(wxCommandEvent& evt);
        void leftDClick(wxMouseEvent& evt);
        static const long idMenuEdit;
//        std::string totalGcode;

        BasicGLPane* GLCanvas1;
        wxBitmapButton* BitmapButton1;
        bool replay = false;
    private:

        //(*Handlers(PiCNCFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnMenuItem3Selected(wxCommandEvent& event);
        void OnMenuItem5Selected(wxCommandEvent& event);
        void OnMenuItem4Selected(wxCommandEvent& event);
        void OnGLCanvas1Paint(wxPaintEvent& event);
        void OnBitmapButton1Click(wxCommandEvent& event);
        void OnBitmapButton4Click(wxCommandEvent& event);
        void OnBitmapButton3Click(wxCommandEvent& event);
        void OnBitmapButton2Click(wxCommandEvent& event);
        //*)
        void OnBitmapButton1ClickPause(wxCommandEvent& event);

        //(*Identifiers(PiCNCFrame)
        static const long ID_BITMAPBUTTON1;
        static const long ID_BITMAPBUTTON2;
        static const long ID_BITMAPBUTTON3;
        static const long ID_BITMAPBUTTON4;
        static const long ID_GLCANVAS1;
        static const long idMenuQuit;
        static const long ID_MENUITEM1;
        static const long ID_MENUITEM2;
        static const long ID_MENUITEM3;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(PiCNCFrame)
        wxBitmapButton* BitmapButton2;
        wxBitmapButton* BitmapButton3;
        wxBitmapButton* BitmapButton4;
        wxFileDialog* FileDialog1;
        wxMenu* Menu3;
        wxMenuItem* MenuItem3;
        wxMenuItem* MenuItem4;
        wxMenuItem* MenuItem5;
        wxStatusBar* StatusBar1;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // PICNCMAIN_H
