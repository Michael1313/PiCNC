/***************************************************************
 * Name:      PiCNCEngraverMain.h
 * Purpose:   Defines Application Frame
 * Author:    MBajor ()
 * Created:   2019-12-22
 * Copyright: MBajor ()
 * License:
 **************************************************************/

#ifndef PICNCENGRAVERMAIN_H
#define PICNCENGRAVERMAIN_H

#include <signal.h>

//(*Headers(PiCNCEngraverFrame)
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/filedlg.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/statusbr.h>
#include <wx/tglbtn.h>
//*)

class PiCNCEngraverFrame: public wxFrame
{
    public:

        PiCNCEngraverFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~PiCNCEngraverFrame();

        wxBitmapButton* BitmapButton1;
        void memberFn(void);

        void thread_handler(wxCommandEvent& event);
        void send_thread_event();

    private:

        //(*Handlers(PiCNCEngraverFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnMenuItem3Selected(wxCommandEvent& event);
        void OnPanel1Paint(wxPaintEvent& event);
        void OnCheckBox1Click(wxCommandEvent& event);
        void OnSlider1CmdScrollChanged(wxScrollEvent& event);
        void OnSlider2CmdScrollThumbTrack(wxScrollEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        void OnSlider3CmdScrollThumbTrack(wxScrollEvent& event);
        void OnMouseWheel(wxMouseEvent& event);
        void OnButton2Click(wxCommandEvent& event);
        void OnSpinCtrl1Change(wxSpinEvent& event);
        void OnBitmapButton1Click(wxCommandEvent& event);
        void OnChoice1Select(wxCommandEvent& event);
        void OnBitmapButton1ClickPause(wxCommandEvent& event);
        void OnBitmapButton1ClickReplay(wxCommandEvent& event);
        void OnBitmapButton2Click(wxCommandEvent& event);
        //*)
        void OnSpinCtrl1KeyUp(wxKeyEvent& event);
        void OnSpinCtrl2Change(wxSpinEvent& event);
        void OnSpinCtrl2KeyUp(wxKeyEvent& event);

        //(*Identifiers(PiCNCEngraverFrame)
        static const long ID_STATICTEXT13;
        static const long ID_CHECKBOX1;
        static const long ID_STATICTEXT1;
        static const long ID_SLIDER1;
        static const long ID_STATICTEXT2;
        static const long ID_SLIDER2;
        static const long ID_BUTTON1;
        static const long ID_STATICTEXT3;
        static const long ID_STATICTEXT4;
        static const long ID_SPINCTRL1;
        static const long ID_STATICTEXT5;
        static const long ID_SPINCTRL2;
        static const long ID_STATICTEXT6;
        static const long ID_SPINCTRL3;
        static const long ID_STATICTEXT7;
        static const long ID_SPINCTRL4;
        static const long ID_STATICTEXT8;
        static const long ID_SPINCTRL5;
        static const long ID_STATICTEXT9;
        static const long ID_STATICTEXT10;
        static const long ID_SPINCTRL6;
        static const long ID_STATICTEXT11;
        static const long ID_SPINCTRL7;
        static const long ID_BUTTON2;
        static const long ID_STATICTEXT12;
        static const long ID_SPINCTRL8;
        static const long ID_TOGGLEBUTTON1;
        static const long ID_BITMAPBUTTON1;
        static const long ID_BITMAPBUTTON2;
        static const long ID_CHOICE1;
        static const long ID_MENUITEM1;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(PiCNCEngraverFrame)
        wxBitmapButton* BitmapButton2;
        wxButton* Button1;
        wxButton* Button2;
        wxCheckBox* CheckBox1;
        wxChoice* Choice1;
        wxFileDialog* FileDialog1;
        wxMenuItem* MenuItem3;
        wxSlider* Slider1;
        wxSlider* Slider2;
        wxSpinCtrl* SpinCtrl1;
        wxSpinCtrl* SpinCtrl2;
        wxSpinCtrl* SpinCtrl3;
        wxSpinCtrl* SpinCtrl4;
        wxSpinCtrl* SpinCtrl5;
        wxSpinCtrl* SpinCtrl6;
        wxSpinCtrl* SpinCtrl7;
        wxSpinCtrl* SpinCtrl8;
        wxStaticText* StaticText10;
        wxStaticText* StaticText11;
        wxStaticText* StaticText12;
        wxStaticText* StaticText13;
        wxStaticText* StaticText1;
        wxStaticText* StaticText2;
        wxStaticText* StaticText3;
        wxStaticText* StaticText4;
        wxStaticText* StaticText5;
        wxStaticText* StaticText6;
        wxStaticText* StaticText7;
        wxStaticText* StaticText8;
        wxStaticText* StaticText9;
        wxStatusBar* StatusBar1;
        wxToggleButton* ToggleButton1;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // PICNCENGRAVERMAIN_H
