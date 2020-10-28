#include "GCodeCreator.h"
#include <vector>
#include <signal.h>

//(*InternalHeaders(GCodeCreator)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(GCodeCreator)
const long GCodeCreator::ID_STYLEDTEXTCTRL1 = wxNewId();
const long GCodeCreator::ID_BUTTON1 = wxNewId();
const long GCodeCreator::ID_BUTTON2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(GCodeCreator,wxDialog)
	//(*EventTable(GCodeCreator)
	//*)
END_EVENT_TABLE()
wxString totalStr;
GCodeCreator *GCodeCreatorPtr;
const uint8_t G=4, M=5, XYZ=6, EPSJI=7, F=8, COM=9, BLACK=10;
GCodeCreator::GCodeCreator(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size,const wxString& mStr)
{
	//(*Initialize(GCodeCreator)
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* BoxSizer2;

	Create(parent, id, _("GCodeCreator"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	mSTC = new wxStyledTextCtrl(this,ID_STYLEDTEXTCTRL1,wxDefaultPosition,wxSize(100,25),0,_T("ID_STYLEDTEXTCTRL1"));
	BoxSizer1->Add(mSTC, 1, wxALL|wxEXPAND, 5);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	Butt1 = new wxButton(this, ID_BUTTON1, _("Go"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer2->Add(Butt1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Butt2 = new wxButton(this, ID_BUTTON2, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	BoxSizer2->Add(Butt2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 0, wxALL|wxEXPAND, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GCodeCreator::OnButton1Click);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GCodeCreator::OnButton2Click);
	//*)
    mSTC->SetWrapMode (wxSTC_WRAP_WORD);
    mSTC->SetLexer(wxSTC_LEX_CONTAINER);
    mSTC->SetMarginWidth (0, 50);
    mSTC->StyleSetForeground (wxSTC_STYLE_LINENUMBER, wxColour (75, 75, 75) );
    mSTC->StyleSetBackground (wxSTC_STYLE_LINENUMBER, wxColour (220, 220, 220));
    mSTC->SetMarginType (0, wxSTC_MARGIN_NUMBER);
    mSTC->Bind(wxEVT_STC_STYLENEEDED, &GCodeCreator::OnStyleNeeded, this);
    mSTC->StyleSetForeground(G,wxColor(0,0,255));
    mSTC->StyleSetForeground(M,wxColor(0xCC,0,0xCC));
    mSTC->StyleSetForeground(XYZ,wxColor(0xFF,0,0));
    mSTC->StyleSetForeground(EPSJI,wxColor(0xAA,0xAA,0));
    mSTC->StyleSetForeground(F,wxColor(0xE9, 0xAB,0x71));
    mSTC->StyleSetForeground(COM, wxColor(0xC0,0xC0,0xC0));
    mSTC->StyleSetForeground(BLACK, wxColor(0,0,0));
    //mSTC->AppendText(std::to_string(parent->GetBestWidth()));
    totalStr = mStr;
    GCodeCreatorPtr = this;
    mSTC->AppendText(mStr);
    wxString text=mSTC->GetText().Upper();
    this->highlightSTCsyntax(0,mSTC->GetTextLength(),text);
	Maximize();
}

GCodeCreator::~GCodeCreator()
{
	//(*Destroy(GCodeCreator)
	//*)
}


void GCodeCreator::OnButton1Click(wxCommandEvent& event)
{
    assept = true;
    Close();
}

void GCodeCreator::OnButton2Click(wxCommandEvent& event)
{
    Close();
}

 void GCodeCreator::OnStyleNeeded(wxStyledTextEvent& event) {
    /*this is called every time the styler detects a line that needs style, so we style that range.
    This will save a lot of performance since we only style text when needed instead of parsing the whole file every time.*/
    size_t line_end=mSTC->LineFromPosition(mSTC->GetCurrentPos());
    size_t line_start=mSTC->LineFromPosition(mSTC->GetEndStyled());
    /*fold level: May need to include the two lines in front because of the fold level these lines have- the line above
    may be affected*/
    if(line_start>1) {
        line_start-=2;
    } else {
        line_start=0;
    }
    //if it is so small that all lines are visible, style the whole document
    if(mSTC->GetLineCount()==mSTC->LinesOnScreen()){
        line_start=0;
        line_end=mSTC->GetLineCount()-1;
    }
    if(line_end<line_start) {
        //that happens when you select parts that are in front of the styled area
        size_t temp=line_end;
        line_end=line_start;
        line_start=temp;
    }
    //style the line following the style area too (if present) in case fold level decreases in that one
    if(line_end<mSTC->GetLineCount()-1){
        line_end++;
    }
    //get exact start positions
    size_t startpos=mSTC->PositionFromLine(line_start);
    size_t endpos=(mSTC->GetLineEndPosition(line_end));
    int startfoldlevel=mSTC->GetFoldLevel(line_start);
    startfoldlevel &= wxSTC_FOLDFLAG_LEVELNUMBERS; //mask out the flags and only use the fold level
    wxString text=mSTC->GetTextRange(startpos,endpos).Upper();
    //call highlighting function
    this->highlightSTCsyntax(startpos,endpos,text);
 }
int G28M= 0;
 void GCodeCreator::highlightSTCsyntax(size_t fromPos,size_t toPos, wxString &text) {
    //this vector will hold the start and end position of each word to highlight
    //if you want to highlight more than one, you should pass a whole class or struct containing the offsets
    std::vector<std::pair<size_t,size_t>>GcodeVector;
    std::vector<char>GcodeChar;
    //the following example is a quick and dirty parser for G-Codes.
    //it just iterates through the Text Range and finds "Gxx" where xx is a digit.
    //you could also use regex, but one can build a pretty fast routine based on single char evaluation
    size_t actual_cursorpos = 0;
    size_t startpos = 0;
    size_t end_of_text = text.length();
    bool word_boundary = true; //check for word boundary
    char actualchar;
    while (actual_cursorpos<end_of_text) {
        actualchar= text[actual_cursorpos];
        //check if syntax matches "G" followed by a couple of numbers
        if(actualchar=='('&&(word_boundary==true)){
            startpos=actual_cursorpos;
            word_boundary=false;
            actual_cursorpos++;
            if(actual_cursorpos<end_of_text) {
                //refresh actual character
                actualchar= text[actual_cursorpos];
            }
            while(actualchar!=')'&&(actualchar!='\n')&&(actualchar!='\r')&&(actual_cursorpos!=end_of_text)){
                actual_cursorpos++;
                actualchar= text[actual_cursorpos];
            }
                if((actual_cursorpos-startpos)>1&&actualchar==')') {
                    actual_cursorpos++;
                    actualchar=text[actual_cursorpos];
                    GcodeVector.push_back(std::make_pair(startpos+fromPos, actual_cursorpos+fromPos));
                    GcodeChar.push_back(text[startpos]);
                }
                word_boundary=true;
        }else if(actualchar==';'&&(word_boundary==true)){
            startpos=actual_cursorpos;
            word_boundary=false;
            actual_cursorpos++;
            if(actual_cursorpos<end_of_text) {
                //refresh actual character
                actualchar= text[actual_cursorpos];
            }
            while((actualchar!='\n')&&(actualchar!='\r')&&(actual_cursorpos!=end_of_text)){
                actual_cursorpos++;
                actualchar= text[actual_cursorpos];
            }
                if((actual_cursorpos-startpos)>1) {
                    GcodeVector.push_back(std::make_pair(startpos+fromPos, actual_cursorpos+fromPos));
                    GcodeChar.push_back(text[startpos]);
                }
                word_boundary=true;
        }else
        if((actualchar=='G' || actualchar=='M' || actualchar=='X' || actualchar=='Y' || actualchar=='Z' || actualchar=='E'
            || actualchar=='P' || actualchar=='S' || actualchar=='F' || actualchar=='J' || actualchar=='I' || actualchar=='K')&&(word_boundary==true)) {
            //this is a new G-Code, store startposition
            startpos=actual_cursorpos;
            word_boundary=false;
            actual_cursorpos++;
            if(actual_cursorpos<end_of_text) {
                //refresh actual character
                actualchar= text[actual_cursorpos];
            }
            if(actualchar=='-') {
                actual_cursorpos++;
                actualchar = text[actual_cursorpos];
            }
            //add digits
            while((std::isdigit(actualchar) || actualchar == '.')&&(actual_cursorpos<end_of_text)) {
                actual_cursorpos++;
                actualchar= text[actual_cursorpos];
            }
            //check if word boundary occurs at end of digits
            if((actualchar==' ')||(actualchar=='\n')||(actualchar=='\r')||(actualchar=='\t')||(actual_cursorpos==end_of_text)) {
                //success, append this one
                if((actual_cursorpos-startpos)>1 || (G28M==28 && text[startpos]>=88 && text[startpos]<=100)) {
                if(text[startpos]=='G') {G28M = wxAtoi(text.SubString(startpos+1, startpos+3));}
                    //success, append to vector. DO NOT FORGET THE OFFSET HERE! We start from fromPos, so we need to add this
                    GcodeVector.push_back(std::make_pair(startpos+fromPos, actual_cursorpos+fromPos));
                    GcodeChar.push_back(text[startpos]);
                }
                word_boundary=true;
            }
        }
        if((actualchar==' ')||(actualchar=='\n')||(actualchar=='\r')||(actualchar=='\t')||(actual_cursorpos==end_of_text)) {
            word_boundary=true;
        }
        actual_cursorpos++;
    }
    //remove old styling
    mSTC->StartStyling(fromPos, 255); //from here
    mSTC->SetStyling(toPos-fromPos,0); //with that length and style -> cleared
    //now style the G-Codes

    for (int i=0; i<GcodeVector.size(); i++) {//TextCtrl1->SetValue(TextCtrl1->GetValue()+GcodeChar[i]);
        size_t startpos=GcodeVector[i].first;
        size_t endpos=GcodeVector[i].second;
        size_t length=(endpos-startpos);
        mSTC->StartStyling(startpos, 255);
        if(GcodeChar[i] == 'G'){
            mSTC->SetStyling(length,G);
        }else if(GcodeChar[i] == 'M'){
            mSTC->SetStyling(length,M);
        }else if(GcodeChar[i] == 'X' || GcodeChar[i] == 'Y' || GcodeChar[i] == 'Z'){
            mSTC->SetStyling(1,XYZ);
        }else if(GcodeChar[i] == 'E' || GcodeChar[i] == 'P' || GcodeChar[i] == 'S' || GcodeChar[i] == 'J' || GcodeChar[i] == 'I' || GcodeChar[i] == 'K'){
            mSTC->SetStyling(length,EPSJI);
        }else if(GcodeChar[i] == 'F'){
            mSTC->SetStyling(length,F);
        }else if(GcodeChar[i] == '(' || GcodeChar[i] == ';'){
            mSTC->SetStyling(length,COM);
        }else {
            mSTC->SetStyling(length,BLACK);
        }
    }
 }

void sigalarm_handler(int sig){
    int w, h;
    GCodeCreatorPtr->mSTC->AppendText(totalStr);
    GCodeCreatorPtr->mSTC->GetSize(&w, &h);
    if(GCodeCreatorPtr->mSTC->TextHeight(0) * (GCodeCreatorPtr->mSTC->GetLineCount()-1) < h){
        GCodeCreatorPtr->mSTC->GotoLine(GCodeCreatorPtr->mSTC->GetLineCount()-1);
    }
}

void GCodeCreator::OnInit(wxInitDialogEvent& event)
{
    signal(SIGALRM, &sigalarm_handler);
    ualarm(200000, 0);
}

void GCodeCreator::OnResize(wxSizeEvent& event)
{
}
