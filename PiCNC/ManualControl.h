#ifndef MANUALCONTROL_H
#define MANUALCONTROL_H

//(*Headers(ManualControl)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/spinbutt.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)
#include <signal.h>

typedef struct {
    double PosX;
    double PosY;
    double PosZ;
    int spuX;
    int spuY;
    int spuZ;
    int F;
    int laserWidth;
    int burnTime;
} MSets_t;

class ManualControl: public wxDialog
{
	public:

		ManualControl(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~ManualControl();

        struct XYZActPos{
            double x;
            double y;
            double z;
        };

        void tKeyDown(wxTextCtrl *TC, wxKeyEvent& evt);
        void GoToPos();

		bool mAccept = false;
		//(*Declarations(ManualControl)
		wxButton* Butt1;
		wxButton* Butt2;
		wxButton* Butt3;
		wxButton* Butt4;
		wxSpinButton* SpinButton1;
		wxSpinButton* SpinButton2;
		wxSpinButton* SpinButton3;
		wxSpinCtrl* SpinCtrl1;
		wxSpinCtrl* SpinCtrl2;
		wxSpinCtrl* SpinCtrl3;
		wxSpinCtrl* SpinCtrl4;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* StaticText3;
		wxStaticText* StaticText4;
		wxStaticText* StaticText5;
		wxStaticText* StaticText6;
		wxStaticText* StaticText7;
		wxTextCtrl* TextCtrl1;
		wxTextCtrl* TextCtrl2;
		wxTextCtrl* TextCtrl3;
		//*)

		XYZActPos ActPos;
		void set_x_pos(int pos){
            ActPos.x += double(pos) / double(SpinCtrl1->GetValue());
            TextCtrl1->SetValue(std::to_string(ActPos.x));
		};
		void set_y_pos(int pos){
            ActPos.y += double(pos) / double(SpinCtrl2->GetValue());
            TextCtrl2->SetValue(std::to_string(ActPos.y));
		};
		void set_z_pos(int pos){
            ActPos.z += double(pos) / double(SpinCtrl3->GetValue());
            TextCtrl3->SetValue(std::to_string(ActPos.z));
		};

		void read_pos();

	protected:

		//(*Identifiers(ManualControl)
		static const long ID_SPINCTRL1;
		static const long ID_SPINCTRL2;
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT4;
		static const long ID_STATICTEXT5;
		static const long ID_STATICTEXT6;
		static const long ID_SPINCTRL3;
		static const long ID_TEXTCTRL1;
		static const long ID_SPINBUTTON1;
		static const long ID_TEXTCTRL2;
		static const long ID_SPINBUTTON2;
		static const long ID_TEXTCTRL3;
		static const long ID_SPINBUTTON3;
		static const long ID_SPINCTRL4;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		static const long ID_BUTTON4;
		static const long ID_STATICTEXT7;
		//*)

	private:

		//(*Handlers(ManualControl)
		void OnButton2Click(wxCommandEvent& event);
		void OnButton3Click(wxCommandEvent& event);
		void OnSpinButton1ChangeUp(wxSpinEvent& event);
		void OnSpinButton1ChangeDown(wxSpinEvent& event);
		void OnSpinButton2ChangeUp(wxSpinEvent& event);
		void OnSpinButton2ChangeDown(wxSpinEvent& event);
		void OnSpinButton3ChangeUp(wxSpinEvent& event);
		void OnSpinButton3ChangeDown(wxSpinEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnKeyUp(wxKeyEvent& event);
		void OnInit(wxInitDialogEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnLeftDown(wxMouseEvent& event);
		void OnButt1Click(wxCommandEvent& event);
		void OnButt4Click(wxCommandEvent& event);
		//*)

        void OnTextCtrl1KeyDown(wxKeyEvent& event);
        void OnTextCtrl2KeyDown(wxKeyEvent& event);
        void OnTextCtrl3KeyDown(wxKeyEvent& event);
        void OnTextCtrlKeyUp(wxKeyEvent& event);
        void OnTextCtrl1KillFocus(wxFocusEvent& event);
        void OnTextCtrl2KillFocus(wxFocusEvent& event);
        void OnTextCtrl3KillFocus(wxFocusEvent& event);
		void OnSpinButton1SetFocus(wxFocusEvent& event);
		void OnSpinButton2SetFocus(wxFocusEvent& event);
		void OnSpinButton3SetFocus(wxFocusEvent& event);
		DECLARE_EVENT_TABLE()
};

#endif
