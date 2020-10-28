#ifndef GCODECREATOR_H
#define GCODECREATOR_H

//(*Headers(GCodeCreator)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/stc/stc.h>
//*)

class GCodeCreator: public wxDialog
{
	public:

		GCodeCreator(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize,const wxString& mStr="");
		virtual ~GCodeCreator();

		//(*Declarations(GCodeCreator)
		wxButton* Butt1;
		wxButton* Butt2;
		wxStyledTextCtrl* mSTC;
		//*)
		bool assept = false;

	protected:

		//(*Identifiers(GCodeCreator)
		static const long ID_STYLEDTEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		//*)

	private:

		//(*Handlers(GCodeCreator)
		void OnButton1Click(wxCommandEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		void OnInit(wxInitDialogEvent& event);
		void OnResize(wxSizeEvent& event);
		//*)

        void OnStyleNeeded(wxStyledTextEvent& event);
        void highlightSTCsyntax(size_t fromPos,size_t toPos,wxString &text);
		DECLARE_EVENT_TABLE()
};

#endif
