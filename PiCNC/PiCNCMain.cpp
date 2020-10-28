/***************************************************************
 * Name:      PiCNCMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    MBajor ()
 * Created:   2019-10-10
 * Copyright: MBajor ()
 * License:
 **************************************************************/

#include "PiCNCMain.h"
#include "ManualControl.h"
#include "GCodeCreator.h"
#include <wx/msgdlg.h>
#include <algorithm>
#include <signal.h>
#include <math.h>

//(*InternalHeaders(PiCNCFrame)
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/vlbox.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <wx/dc.h>
#include <wx/wx.h>
#include <wx/dcmemory.h>

//(*InternalHeaders(PiCNCFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#define M_2PI 2*M_PI
void prepareListBox();
void parseGcode(std::string line);

std::string subA, subB;
uint8_t colorNum = 125;
uint8_t G[6] = {0,0,255,colorNum,colorNum,255}, M[6] = {0xCC,0,0xCC,0xCC,colorNum,0xCC},
XYZ[6] = {0xFF,0,0,0xFF,colorNum,colorNum}, EPSJI[6] = {0xAA,0xAA,0,0xAA,0xAA,0},
F[6] = {0xE9, 0xAB,0x71,0xE9, 0xAB,0x71}, COM[6] = {0xC0,0xC0,0xC0,0xC0,0xC0,0xC0}, BLACK[6] = {0,0,0,colorNum,colorNum,colorNum};
const long PiCNCFrame::idMenuEdit = wxNewId();
class myVListBox:public wxVListBox
{
    public:
        uint16_t noXYZ = 0;
        myVListBox(wxWindow *parent, wxWindowID id=wxID_ANY, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxString &name=wxVListBoxNameStr);
        virtual void OnDrawItem (wxDC &dc, const wxRect &rect, size_t n) const {
//            dc.SetPen(wxPen(wxColour(0x00,0xFF,0x00), 1, wxNORMAL));
            dc.SetBrush(wxBrush(wxColour(0xCC,0xCC,0xCC)));
            wxPoint triangle[3] = {wxPoint(0, 5), wxPoint(BoxHeight-13, BoxHeight/2-1), wxPoint(0, BoxHeight-8)};

            wxFont font = wxFont();
//            font.MakeItalic();
            uint8_t incV=0;
            if (n < curItem)
            {
                font.SetStyle(wxFONTSTYLE_ITALIC);
                incV = 3;
            } else
            {
                font.SetStyle(wxFONTSTYLE_NORMAL);
                incV = 0;
            }
//            font.SetWeight(wxNORMAL);
            dc.SetFont(font);
            dc.SetTextForeground(wxColour(0xCC,0xCC,0xCC));
            uint32_t LStart = rect.GetLeft() + 30;
            dc.DrawText(_(std::to_string(n) + " :   "), LStart, rect.GetTop());
            wxSize textSz = GetTextExtent(std::to_string(n) + " :   ");
            LStart += textSz.GetWidth();
//            (n < curItem)? dc.SetTextForeground(wxColour(0xCC,0xCC,0xCC)) : dc.SetTextForeground(wxColour(0x00,0x00,0x00));
//            dc.DrawText(mStrings[n], LStart, rect.GetTop());
            if(GCodeVector[n].size()==0){
                dc.SetTextForeground(wxColor(BLACK[0+incV],BLACK[1+incV],BLACK[2+incV]));
                dc.DrawText(mStrings[n], LStart, rect.GetTop());
            }else{
                subB = mStrings[n].substr(0, GCodeVector[n][0].first);
                if(subB.size()>0){
                    dc.SetTextForeground(wxColor(BLACK[0+incV],BLACK[1+incV],BLACK[2+incV]));
                    dc.DrawText(subB, LStart, rect.GetTop());
                    LStart += GetTextExtent(subB).GetWidth();
                }
                for(size_t i=0; i<GCodeVector[n].size(); ++i){
                    dc.SetTextForeground(wxColor(*(GCodeColor[n][i]+incV),*(GCodeColor[n][i]+1+incV),*(GCodeColor[n][i]+2+incV)));
                    if(GCodeColor[n][i]!=XYZ)
                    {
                        subA = mStrings[n].substr(GCodeVector[n][i].first, GCodeVector[n][i].second - GCodeVector[n][i].first);
                        subB = mStrings[n].substr(GCodeVector[n][i].second, GCodeVector[n][i+1].first - GCodeVector[n][i].second);
                        dc.DrawText(subA, LStart, rect.GetTop());
                        LStart += GetTextExtent(subA).GetWidth();
                        if(subB.size()>0)
                        {
                            dc.SetTextForeground(wxColor(BLACK[0+incV],BLACK[1+incV],BLACK[2+incV]));
                            dc.DrawText(subB, LStart, rect.GetTop());
                            LStart += GetTextExtent(subB).GetWidth();
                        }
                    }else
                    {
                        dc.DrawText(mStrings[n][GCodeVector[n][i].first], LStart, rect.GetTop());
                        LStart += GetTextExtent(mStrings[n][GCodeVector[n][i].first]).GetWidth();
                        dc.SetTextForeground(wxColor(BLACK[0+incV],BLACK[1+incV],BLACK[2+incV]));
                        subB = mStrings[n].substr(GCodeVector[n][i].first+1, GCodeVector[n][i+1].first-(GCodeVector[n][i].first+1));
                        dc.DrawText(subB, LStart, rect.GetTop());
                        LStart += GetTextExtent(subB).GetWidth();
                    }
                }
            }
            if(n==curItem) dc.DrawPolygon(3, triangle, 13, rect.GetTop());
        }
        void 		OnDrawBackground(wxDC &dc, const wxRect &rect, size_t n) {
/*                wxBrush brush(wxColour(255, 255, 255), wxBRUSHSTYLE_SOLID);
                dc.SetBrush(brush);
                dc.SetPen(wxPen(wxColour(0x00,0x00,0x00), 3, wxSOLID));
                dc.DrawRectangle(0, rect.GetTop(), 9, 6);
*/        }
        virtual wxCoord OnMeasureItem (size_t n) const {
            return BoxHeight;
        }
        std::vector<std::string> mStrings;
        std::vector<std::vector<std::pair<size_t, size_t>>> GCodeVector;
        std::vector<std::vector<uint8_t *>> GCodeColor;
        size_t curItem = 0;
        void onMouseUp(wxMouseEvent& evt)
        {
            wxMenu menu;
            menu.Append(PiCNCFrame::idMenuEdit, wxT("Edit"));
            menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(PiCNCFrame::OnPopupClick), NULL, this);
            PopupMenu(&menu);
        }
        DECLARE_EVENT_TABLE()
    private:
        int BoxHeight;
};

myVListBox::myVListBox(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
:wxVListBox(parent, wxID_ANY, pos, size,  style, name)
{
    wxSize textSz = GetTextExtent("Test");
    BoxHeight = textSz.GetHeight();
 //   SetItemCount(0);
}

myVListBox* lb;

void PiCNCFrame::OnPopupClick(wxCommandEvent& evt)
{
    if(evt.GetId()==idMenuEdit)
    {
        wxString totalGcode;
        for(uint16_t i=0; i<lb->mStrings.size(); i++)
        {
            totalGcode += lb->mStrings[i]+"\n";
        }
        GCodeCreator *Dialog = new GCodeCreator(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, totalGcode);
      //  Dialog->mSTC->AppendText(totalGcode);
        Dialog->ShowModal();
        if(Dialog->assept)
        {
            prepareListBox();
            for(uint16_t i=0; i<Dialog->mSTC->GetLineCount(); i++)
            {
                parseGcode(std::string(Dialog->mSTC->GetLine(i).substr(0,Dialog->mSTC->GetLine(i).size()-1).mbc_str()));
            }
        }
        Dialog->Destroy();
        lb->SetItemCount(lb->mStrings.size());
    }
}

BEGIN_EVENT_TABLE(myVListBox,wxVListBox)
EVT_RIGHT_UP(myVListBox::onMouseUp)
END_EVENT_TABLE()

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(PiCNCFrame)
const long PiCNCFrame::ID_BITMAPBUTTON1 = wxNewId();
const long PiCNCFrame::ID_BITMAPBUTTON2 = wxNewId();
const long PiCNCFrame::ID_BITMAPBUTTON3 = wxNewId();
const long PiCNCFrame::ID_BITMAPBUTTON4 = wxNewId();
const long PiCNCFrame::ID_GLCANVAS1 = wxNewId();
const long PiCNCFrame::idMenuQuit = wxNewId();
const long PiCNCFrame::ID_MENUITEM1 = wxNewId();
const long PiCNCFrame::ID_MENUITEM2 = wxNewId();
const long PiCNCFrame::ID_MENUITEM3 = wxNewId();
const long PiCNCFrame::idMenuAbout = wxNewId();
const long PiCNCFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(PiCNCFrame,wxFrame)
    //(*EventTable(PiCNCFrame)
    //*)
END_EVENT_TABLE()

wxBitmap *playBmp, *pauseBmp, *selectBmp, *select2Bmp;
PiCNCFrame *PiCNCPtr;
void GLCanvasMRefresh();
struct sigaction mSig;
void signalFunct(int n, siginfo_t *info, void *unused){
    if((int)lb->curItem < info->si_int){
    size_t LFrom = lb->curItem;
    lb->curItem = info->si_int;
    if(lb->curItem+2 < lb->GetItemCount()) lb->SetSelection(lb->curItem+2);
    lb->SetSelection(lb->curItem);
    lb->RefreshRows(LFrom, info->si_int);
    }else{
        lb->curItem = info->si_int;
        lb->RefreshAll();
        lb->SetSelection(info->si_int);
        PiCNCPtr->BitmapButton1->SetBitmap(*playBmp);
        PiCNCPtr->replay = false;
    }
    GLCanvasMRefresh();
//    lb->SetScrollPos(wxVERTICAL ,123,true);
//    lb->Refresh();

}

int sets_fd;
char setsBuf[sizeof(MSets_t)];
MSets_t *datObj = (MSets_t*)setsBuf;
PiCNCFrame::PiCNCFrame(wxWindow* parent,wxWindowID id)
{
    playBmp = new wxBitmap(wxImage(_T("img/play.png")));
    pauseBmp = new wxBitmap(wxImage(_T("img/pause.png")));
    selectBmp = new wxBitmap(wxImage(_T("img/select.png")));
    select2Bmp = new wxBitmap(wxImage(_T("img/select2.png")));
/*    wxPoint triangle[3] = {wxPoint(0, 0), wxPoint(24, 12), wxPoint(0, 24)};
    wxBitmap bmp(24,24);
    wxMemoryDC bmpDC;
    bmpDC.SelectObject(bmp);
//    bmpDC.SetBackgroundMode(wxTRANSPARENT);
    bmpDC.SetBackground(wxColour(13, 0, 0, 1));
    bmpDC.Clear();
    bmpDC.SetBrush(wxBrush(wxColour(0x00,0xCC,0x00)));
    bmpDC.SetPen(wxPen(wxColour(0x00,0xCC,0x00), 1, wxNORMAL));
    bmpDC.DrawPolygon(3, triangle, 0, 0);
    wxImage mImg = bmp.ConvertToImage();
    mImg.InitAlpha();
    for(int i=0; i<24; i++){
        for(int j=0; j<24; j++){
            if(mImg.GetRed(j, i)==13) mImg.SetAlpha(j, i, 0);
        }
    }
//    bmpDC.DrawLine(0, 0, 20, 20);
*/
    //(*Initialize(PiCNCFrame)
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer3;
    wxBoxSizer* BoxSizer4;
    wxBoxSizer* BoxSizer5;
    wxBoxSizer* BoxSizer6;
    wxMenu* Menu1;
    wxMenu* Menu2;
    wxMenuBar* MenuBar1;
    wxMenuItem* MenuItem1;
    wxMenuItem* MenuItem2;

    Create(parent, wxID_ANY, _("PiCNC"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    {
    	wxIcon FrameIcon;
    	FrameIcon.CopyFromBitmap(wxBitmap(wxImage(_T("/home/pi/PiCNC/img/icon.ico"))));
    	SetIcon(FrameIcon);
    }
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    BitmapButton1 = new wxBitmapButton(this, ID_BITMAPBUTTON1, *playBmp, wxDefaultPosition, wxSize(33,33), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
    BoxSizer5->Add(BitmapButton1, 0, wxALL|wxALIGN_TOP, 5);
    BitmapButton2 = new wxBitmapButton(this, ID_BITMAPBUTTON2, wxBitmap(wxImage(_T("img/cansel.png"))), wxDefaultPosition, wxSize(33,33), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON2"));
    BoxSizer5->Add(BitmapButton2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
    BoxSizer4->Add(BoxSizer5, 1, wxALL|wxALIGN_TOP, 5);
    BoxSizer6 = new wxBoxSizer(wxVERTICAL);
    BitmapButton3 = new wxBitmapButton(this, ID_BITMAPBUTTON3, *selectBmp, wxDefaultPosition, wxSize(33,33), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON3"));
    BoxSizer6->Add(BitmapButton3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BitmapButton4 = new wxBitmapButton(this, ID_BITMAPBUTTON4, wxBitmap(wxImage(_T("img/s.png"))), wxDefaultPosition, wxSize(33,33), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON4"));
    BoxSizer6->Add(BitmapButton4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4->Add(BoxSizer6, 0, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_TOP, 5);
    BoxSizer2->Add(BoxSizer4, 1, wxALL|wxEXPAND, 1);
    int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
    GLCanvas1 = new BasicGLPane(this,args);
    BoxSizer2->Add(GLCanvas1, 1, wxTOP|wxBOTTOM|wxRIGHT|wxEXPAND, 5);
    BoxSizer1->Add(BoxSizer2, 2, wxALL|wxEXPAND, 5);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer1->Add(BoxSizer3, 1, wxALL|wxEXPAND, 5);
    SetSizer(BoxSizer1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuItem3 = new wxMenuItem(Menu1, ID_MENUITEM1, _("&Open file\tctrl+o"), _("Open file"), wxITEM_NORMAL);
    Menu1->Append(MenuItem3);
    MenuItem4 = new wxMenuItem(Menu1, ID_MENUITEM2, _("&GCodeCreator\tctrl+g"), _("GCodeCreator"), wxITEM_NORMAL);
    Menu1->Append(MenuItem4);
    MenuBar1->Append(Menu1, _("&File"));
    Menu3 = new wxMenu();
    MenuItem5 = new wxMenuItem(Menu3, ID_MENUITEM3, _("&Control\tctrl+m"), _("ManualControl"), wxITEM_NORMAL);
    Menu3->Append(MenuItem5);
    MenuBar1->Append(Menu3, _("&ManualControl"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("&About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("&Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, _("G-code files |*.gcode;*.mpt;*.mpf;*.nc;*.ngc|All files |*"), wxFD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);
    Center();

    Connect(ID_BITMAPBUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PiCNCFrame::OnBitmapButton1Click);
    Connect(ID_BITMAPBUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PiCNCFrame::OnBitmapButton2Click);
    Connect(ID_BITMAPBUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PiCNCFrame::OnBitmapButton3Click);
    Connect(ID_BITMAPBUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PiCNCFrame::OnBitmapButton4Click);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PiCNCFrame::OnQuit);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PiCNCFrame::OnMenuItem3Selected);
    Connect(ID_MENUITEM2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PiCNCFrame::OnMenuItem4Selected);
    Connect(ID_MENUITEM3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PiCNCFrame::OnMenuItem5Selected);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PiCNCFrame::OnAbout);
    //*)
   	Maximize();
   	lb = new myVListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, _("asfd"));
   	BoxSizer3->Add(lb, 1, wxEXPAND|wxALL, 3);
   	lb->SetSelectionBackground(wxColour(0xF0, 0xF0, 0xF0));
   	lb->Bind(wxEVT_LEFT_DCLICK, &PiCNCFrame::leftDClick,this);

    if((sets_fd = open("piCNCsetings", O_RDWR)) < 0){
        wxMessageBox("Please enter first mashine parameters in Menu->ManualControl->Control window");
    }else{
        read(sets_fd, setsBuf, sizeof(MSets_t));
        close(sets_fd);
    }

    mSig.sa_sigaction = signalFunct;
    mSig.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &mSig, NULL);
    PiCNCPtr = this;

}

BasicGLPane::BasicGLPane(wxFrame* parent, int* args) :
    wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
	m_context = new wxGLContext(this);

    // To avoid flashing on MSW
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    int argc = 1;
    char* argv[1] = { wxString((wxTheApp->argv)[0]).char_str() };
    glutInit(&argc, argv);

}

BasicGLPane::~BasicGLPane()
{
	delete m_context;
}

PiCNCFrame::~PiCNCFrame()
{
    //(*Destroy(PiCNCFrame)
    //*)
}

void PiCNCFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void PiCNCFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

std::string str = "Stage: 0 / 0";
GLfloat mScale = 1.0f, tScale;
float maxX, maxY;
void PiCNCFrame::OnMenuItem3Selected(wxCommandEvent& event)
{
    if ( FileDialog1->ShowModal() != wxID_OK ) return;

    wxString FileName = FileDialog1->GetPath();
    if ( FileName.IsEmpty() ) return;

    prepareListBox();
    maxX = maxY = 0;
    std::string line;
    std::ifstream gCode (FileName);
    if (gCode.is_open())
    {
        while ( getline (gCode,line) )
        {
            parseGcode(line);
        }
        gCode.close();
        if(GLCanvas1->GetSize().x > maxX || GLCanvas1->GetSize().y > maxY){
            if(GLCanvas1->GetSize().x / maxX < GLCanvas1->GetSize().y / maxY) {
                mScale = GLCanvas1->GetSize().x / maxX;
            }else{
                mScale = GLCanvas1->GetSize().y / maxY;
            }
        }else{
            if(maxX / GLCanvas1->GetSize().x < maxY / GLCanvas1->GetSize().y) {
                mScale = GLCanvas1->GetSize().y / maxY;
            }else{
                mScale = GLCanvas1->GetSize().x / maxX;
            }
        }
        tScale = mScale;
    } else wxMessageBox(_("Unable to open file " + FileDialog1->GetPath()));
    lb->SetItemCount(lb->mStrings.size());
    str = "Stage: 0 / " + std::to_string(lb->GetItemCount());
    GLCanvas1->Refresh();
    //simple ListBox case
    //ListBox1->Clear();
    //ListBox1->SetSelection(0, true);
    //ListBox1->Append(_("hello World"));
    //ListBox1->Delete(0);
    //ListBox1->Deselect(ListBox1->GetSelection());
    //ListBox1->SetString(ListBox1->GetCount()- 1, "hello world");
    //wxFont font(wxFontInfo(10).Bold().Underlined());
    //ListBox1->SetFont(font);
}

std::vector<GLfloat> oglArr;
float tempArr[3] = {0, 0, 0};
bool isXYZ = false;
std::vector<size_t> relativeIndex;
uint16_t oglCur = 0;
void (*setIndexPtr)(bool ch);
void setIndex(bool ch){
    if(ch){
        relativeIndex.push_back(relativeIndex[relativeIndex.size()-1]+1);
    }else{
        relativeIndex.push_back(relativeIndex[relativeIndex.size()-1]);
    }
}
void setIndexFirst(bool ch){
    relativeIndex.push_back(0);
    if(ch) setIndexPtr = &setIndex;
}
void prepareListBox()
{
    setIndexPtr = &setIndexFirst;
    lb->mStrings.clear();   //erase(from, to);
    lb->GCodeVector.clear();
    lb->GCodeColor.clear();
    lb->Clear();
    lb->Refresh();
    lb->curItem = 0;

    oglArr.clear();
    tempArr[0] = 0;
    tempArr[1] = 0;
    tempArr[2] = 0;
    isXYZ = false;
    relativeIndex.clear();
    oglCur = 0;
}

char GCheck = 0; bool G28XYZ = false; unsigned char XYZCheck;
struct circuitData {
    signed char incPtr1, incPtr2, incPtr3;
    double centerEast, centerNorth;
    double eastFrom, eastTo, northFrom, northTo;
    double vectAngle;
    double I, J, K;
}curCirc;
void parseGcode(std::string line)
{
    char steps=0;
    XYZCheck = 0; curCirc.vectAngle=0;
    lb->mStrings.push_back(line);
    std::transform(line.begin(), line.end(), line.begin(), ::toupper);

    lb->GCodeVector.resize(lb->GCodeVector.size()+1);
    lb->GCodeColor.resize(lb->GCodeColor.size()+1);
    size_t actual_cursorpos = 0;
    size_t startpos = 0;
    size_t end_of_text = line.length();
    bool word_boundary = true; //check for word boundary
    char actualchar;
    while (actual_cursorpos<end_of_text) {
        actualchar = line[actual_cursorpos];
        if(actualchar=='('&&(word_boundary==true)){
            startpos=actual_cursorpos;
            word_boundary=false;
            while(actualchar!=')'&&(actualchar!='\n')&&(actualchar!='\r')&&(actual_cursorpos!=end_of_text)){
                actual_cursorpos++;
                actualchar= line[actual_cursorpos];
            }
            if((actual_cursorpos-startpos)>1&&actualchar==')') {
                actual_cursorpos++;
                actualchar=line[actual_cursorpos];
                lb->GCodeVector[lb->GCodeVector.size()-1].push_back(std::make_pair(startpos, actual_cursorpos));
                lb->GCodeColor[lb->GCodeColor.size()-1].push_back(COM);
            }
            word_boundary=true;
            actual_cursorpos--;
        }else if(actualchar==';'&&(word_boundary==true)){
            startpos=actual_cursorpos;
            word_boundary=false;
            while((actualchar!='\n')&&(actualchar!='\r')&&(actual_cursorpos!=end_of_text)){
                actual_cursorpos++;
                actualchar= line[actual_cursorpos];
            }
            if((actual_cursorpos-startpos)>1) {
                lb->GCodeVector[lb->GCodeVector.size()-1].push_back(std::make_pair(startpos, actual_cursorpos));
                lb->GCodeColor[lb->GCodeColor.size()-1].push_back(COM);
            }
            word_boundary=true;
        }else
        if((actualchar=='G' || actualchar=='M' || actualchar=='X' || actualchar=='Y' || actualchar=='Z' || actualchar=='E'
        || actualchar=='P' || actualchar=='S' || actualchar=='F' || actualchar=='J' || actualchar=='I' || actualchar=='K')&&(word_boundary==true)) {
        //this is a new G-Code, store startposition
        startpos=actual_cursorpos;
        word_boundary=false;
        actual_cursorpos++;
        actualchar= line[actual_cursorpos];
        if(actualchar=='-') {
            actual_cursorpos++;
            actualchar = line[actual_cursorpos];
        }
        //add digits
        while((std::isdigit(actualchar) || actualchar == '.')&&(actual_cursorpos<end_of_text)) {
            actual_cursorpos++;
            actualchar= line[actual_cursorpos];
        }
        //check if word boundary occurs at end of digits
            //success, append this one
            if((actual_cursorpos-startpos)>1 || (GCheck==28 && line[startpos]>=88 && line[startpos]<=100)) {
                //success, append to vector. DO NOT FORGET THE OFFSET HERE! We start from fromPos, so we need to add this
                lb->GCodeVector[lb->GCodeVector.size()-1].push_back(std::make_pair(startpos, actual_cursorpos));
                switch(line[startpos])
                {
                    case 'G' : lb->GCodeColor[lb->GCodeColor.size()-1].push_back(G);
                               GCheck = std::stof(line.substr(startpos+1, actual_cursorpos));
                               if(GCheck == 28)G28XYZ = false;
                        break;
                    case 'M' : lb->GCodeColor[lb->GCodeColor.size()-1].push_back(M);
                        break;
                    case 'X' :
                    case 'Y' :
                    case 'Z' : lb->GCodeColor[lb->GCodeColor.size()-1].push_back(XYZ);
                               isXYZ = true;
                               if(GCheck!=28){
                                   if(line[startpos] == 'X') tempArr[0] = std::stof(line.substr(startpos+1, actual_cursorpos));
                                   if(line[startpos] == 'Y') tempArr[1] = std::stof(line.substr(startpos+1, actual_cursorpos));
                                   if(line[startpos] == 'Z') tempArr[2] = std::stof(line.substr(startpos+1, actual_cursorpos));
                               }else{
                                   if(line[startpos] == 'X') {tempArr[0] = 0;G28XYZ=true;}
                                   if(line[startpos] == 'Y') {tempArr[1] = 0;G28XYZ=true;}
                                   if(line[startpos] == 'Z') {tempArr[2] = 0;G28XYZ=true;}
                               }
                        break;
                    case 'E':
                    case 'P' :
                    case 'S' :
                    case 'I' : XYZCheck |= 0b001;
                               curCirc.I = std::stod(line.substr(startpos+1, actual_cursorpos));
                               lb->GCodeColor[lb->GCodeColor.size()-1].push_back(EPSJI);
                        break;
                    case 'J' : XYZCheck |= 0b010;
                               curCirc.J = std::stod(line.substr(startpos+1, actual_cursorpos));
                               lb->GCodeColor[lb->GCodeColor.size()-1].push_back(EPSJI);
                        break;
                    case 'K' : XYZCheck |= 0b100;
                               curCirc.K = std::stod(line.substr(startpos+1, actual_cursorpos));
                               lb->GCodeColor[lb->GCodeColor.size()-1].push_back(EPSJI);
                        break;
                    case 'F' : lb->GCodeColor[lb->GCodeColor.size()-1].push_back(F);
                        break;
                    default : lb->GCodeColor[lb->GCodeColor.size()-1].push_back(BLACK);
                }
            }
            word_boundary=true;
            actual_cursorpos--;
        }
//        if((actualchar==' ')||(actualchar=='\n')||(actualchar=='\r')||(actualchar=='\t')||(actual_cursorpos==end_of_text)) {
        word_boundary=true;
  //      }
        actual_cursorpos++;
   }
   if(GCheck == 28 && G28XYZ != true){
        tempArr[0] = 0;
        tempArr[1] = 0;
        tempArr[2] = 0;
        oglArr.push_back(0);
        oglArr.push_back(0);
        oglArr.push_back(0);
   }
   if(isXYZ){
        if(!(GCheck == 2 || GCheck ==3)){
            isXYZ = false;
            oglArr.push_back(tempArr[0]);
            oglArr.push_back(tempArr[1]);
            oglArr.push_back(tempArr[2]);
        }else{
            if(XYZCheck == 0b011){
                curCirc.incPtr1 = 0;
                curCirc.incPtr2 = 1;
                curCirc.incPtr3 = 2;
                curCirc.centerEast = oglArr[oglArr.size()-3+curCirc.incPtr1] + curCirc.I;
                curCirc.centerNorth= oglArr[oglArr.size()-3+curCirc.incPtr2] + curCirc.J;
            }else if(XYZCheck == 0b110){
                curCirc.incPtr1 = 1;
                curCirc.incPtr2 = 2;
                curCirc.incPtr3 = 0;
                curCirc.centerEast = oglArr[oglArr.size()-3+curCirc.incPtr1] + curCirc.J;
                curCirc.centerNorth= oglArr[oglArr.size()-3+curCirc.incPtr2] + curCirc.K;
            }else if(XYZCheck == 0b101){
                curCirc.incPtr1 = 0;
                curCirc.incPtr2 = 2;
                curCirc.incPtr3 = 1;
                curCirc.centerEast = oglArr[oglArr.size()-3+curCirc.incPtr1] + curCirc.I;
                curCirc.centerNorth= oglArr[oglArr.size()-3+curCirc.incPtr2] + curCirc.K;
            }
            curCirc.eastFrom = oglArr[oglArr.size()-3+curCirc.incPtr1] - curCirc.centerEast;
            curCirc.northFrom = oglArr[oglArr.size()-3+curCirc.incPtr2] - curCirc.centerNorth;
            curCirc.eastTo = tempArr[curCirc.incPtr1] - curCirc.centerEast;
            curCirc.northTo = tempArr[curCirc.incPtr2] - curCirc.centerNorth;
            double stepAngle = M_PI/8;
            if(GCheck==2){
                curCirc.vectAngle = std::atan2(curCirc.eastFrom*curCirc.northTo - curCirc.northFrom*curCirc.eastTo,
                curCirc.eastFrom*curCirc.eastTo + curCirc.northFrom*curCirc.northTo);
                if(curCirc.vectAngle > 0) curCirc.vectAngle -= M_2PI;
                if(curCirc.vectAngle == 0) curCirc.vectAngle = M_2PI;
                if(std::abs(curCirc.vectAngle) > stepAngle){
                    for(int i=1; i<std::abs(curCirc.vectAngle)/stepAngle; i++){
                        oglArr.push_back(0);
                        oglArr.push_back(0);
                        oglArr.push_back(0);
                        oglArr[oglArr.size()-3+curCirc.incPtr1] = (curCirc.eastFrom * std::sin(M_PI/2 - stepAngle) +
                                                                   curCirc.northFrom * std::cos(M_PI/2 - stepAngle)) + curCirc.centerEast;
                        oglArr[oglArr.size()-3+curCirc.incPtr2] = (curCirc.northFrom * std::sin(M_PI/2 - stepAngle) -
                                                                   curCirc.eastFrom * std::cos(M_PI/2 - stepAngle)) + curCirc.centerNorth;
                        oglArr[oglArr.size()-3+curCirc.incPtr3] =  oglArr[oglArr.size()-6+curCirc.incPtr3];
                        curCirc.eastFrom = oglArr[oglArr.size()-3+curCirc.incPtr1] - curCirc.centerEast;
                        curCirc.northFrom = oglArr[oglArr.size()-3+curCirc.incPtr2] - curCirc.centerNorth;
                        steps++;
                    }
                }
            }else if(GCheck==3){
                curCirc.vectAngle = std::atan2(curCirc.eastFrom*curCirc.northTo - curCirc.northFrom*curCirc.eastTo,
                curCirc.eastFrom*curCirc.eastTo + curCirc.northFrom*curCirc.northTo);
                if(curCirc.vectAngle == 0) curCirc.vectAngle = M_2PI;
                if(std::abs(curCirc.vectAngle) > stepAngle){
                    for(int i=1; i<std::abs(curCirc.vectAngle)/stepAngle; i++){
                        oglArr.push_back(0);
                        oglArr.push_back(0);
                        oglArr.push_back(0);
                        oglArr[oglArr.size()-3+curCirc.incPtr1] = (curCirc.eastFrom * std::sin(M_PI/2 + stepAngle) +
                                                                   curCirc.northFrom * std::cos(M_PI/2 + stepAngle)) + curCirc.centerEast;
                        oglArr[oglArr.size()-3+curCirc.incPtr2] = (curCirc.northFrom * std::sin(M_PI/2 + stepAngle) -
                                                                   curCirc.eastFrom * std::cos(M_PI/2 + stepAngle)) + curCirc.centerNorth;
                        oglArr[oglArr.size()-3+curCirc.incPtr3] =  oglArr[oglArr.size()-6+curCirc.incPtr3];
                        curCirc.eastFrom = oglArr[oglArr.size()-3+curCirc.incPtr1] - curCirc.centerEast;
                        curCirc.northFrom = oglArr[oglArr.size()-3+curCirc.incPtr2] - curCirc.centerNorth;
                        steps++;
                    }
                }
            }
  //          wxMessageBox(std::to_string(std::abs(curCirc.vectAngle)/stepAngle));//curCirc.vectAngle * 180 / M_PI
            oglArr.push_back(tempArr[0]);
            oglArr.push_back(tempArr[1]);
            oglArr.push_back(tempArr[2]);
        }
        if(maxX < tempArr[0]) maxX = tempArr[0];
        if(maxY < tempArr[1]) maxY = tempArr[1];
        setIndexPtr(true);
        relativeIndex[relativeIndex.size()-1] += steps;
        steps = 0;
   } else {
        setIndexPtr(false);
   }
   GCheck = 0;
}

void PiCNCFrame::leftDClick(wxMouseEvent& evt){
    GLCanvasMRefresh();
}

void GLCanvasMRefresh(){
    if(lb->GetItemCount()){
        oglCur = relativeIndex[lb->GetSelection()];
        str = "Stage: " + std::to_string(lb->GetSelection()) + " / " + std::to_string(lb->GetItemCount()-1);
        PiCNCPtr->GLCanvas1->Refresh();
    }
}

void PiCNCFrame::OnMenuItem5Selected(wxCommandEvent& event)
{
    ManualControl Dialog(this);
    Dialog.TextCtrl1->SetValue(std::to_string(datObj->PosX));
    Dialog.ActPos.x = datObj->PosX;
    Dialog.TextCtrl2->SetValue(std::to_string(datObj->PosY));
    Dialog.ActPos.y = datObj->PosY;
    Dialog.TextCtrl3->SetValue(std::to_string(datObj->PosZ));
    Dialog.ActPos.z = datObj->PosZ;
    Dialog.SpinCtrl1->SetValue(std::to_string(datObj->spuX));
    Dialog.SpinCtrl2->SetValue(std::to_string(datObj->spuY));
    Dialog.SpinCtrl3->SetValue(std::to_string(datObj->spuZ));
    Dialog.SpinCtrl4->SetValue(std::to_string(datObj->F));
    Dialog.ShowModal();

    if(Dialog.mAccept){
        datObj->spuX = Dialog.SpinCtrl1->GetValue();
        datObj->spuY = Dialog.SpinCtrl2->GetValue();
        datObj->spuZ = Dialog.SpinCtrl3->GetValue();//wxMessageBox(std::to_string(datObj->PosX));
        datObj->F = Dialog.SpinCtrl4->GetValue();
    }
    datObj->PosX = Dialog.ActPos.x;
    datObj->PosY = Dialog.ActPos.y;
    datObj->PosZ = Dialog.ActPos.z;
    if((sets_fd = open("piCNCsetings", O_RDWR|O_CREAT, S_IRWXU)) < 0){
        wxMessageBox("Can't open file piCNCsetings fd = " + std::to_string(sets_fd), _("PiCNC"));
    }else{
        write(sets_fd, setsBuf, sizeof(MSets_t));
        close(sets_fd);
    }
}

void PiCNCFrame::OnMenuItem4Selected(wxCommandEvent& event)
{
        GCodeCreator *Dialog = new GCodeCreator(this);
        Dialog->ShowModal();
        if(Dialog->assept)
        {
            prepareListBox();
            for(uint16_t i=0; i<Dialog->mSTC->GetLineCount(); i++)
            {
                parseGcode(std::string(Dialog->mSTC->GetLine(i).substr(0,Dialog->mSTC->GetLine(i).size()-1).mbc_str()));
            }
        }
        Dialog->Destroy();
        lb->SetItemCount(lb->mStrings.size());
}

/** Inits the OpenGL viewport for drawing in 3D. */
void BasicGLPane::prepare3DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y)
{
    glViewport(0, 0, bottomrigth_x, bottomrigth_y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0, bottomrigth_x, 0.0, bottomrigth_y, -2147483648.0f, 2147483648.0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/** Inits the OpenGL viewport for drawing in 2D. */
void BasicGLPane::prepare2DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y)
{
    glEnable(GL_TEXTURE_2D);   // textures
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, bottomrigth_x, bottomrigth_y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(0.0, bottomrigth_x, 0.0, bottomrigth_y);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

BEGIN_EVENT_TABLE(BasicGLPane, wxGLCanvas)
//EVT_MOTION(BasicGLPane::mouseMoved)
EVT_LEFT_DOWN(BasicGLPane::mouseDown)
EVT_LEFT_UP(BasicGLPane::mouseReleased)
EVT_RIGHT_DOWN(BasicGLPane::rightClick)
EVT_RIGHT_UP(BasicGLPane::rightUp)
EVT_LEAVE_WINDOW(BasicGLPane::mouseLeftWindow)
EVT_KEY_DOWN(BasicGLPane::keyPressed)
EVT_KEY_UP(BasicGLPane::keyReleased)
EVT_MOUSEWHEEL(BasicGLPane::mouseWheelMoved)
EVT_PAINT(BasicGLPane::render)
END_EVENT_TABLE()

// some useful events to use
GLfloat angleX = 0.0f, oldAngleX = 0.0f, angleY = 0.0f, oldAngleY = 0.0f;
GLfloat DX = 0.0f, oldX=0.0f, tempX=0.0f, DY = 0.0f, oldY=0.0f, tempY=0.0f;
bool FullPart = true;
void BasicGLPane::mouseMoved(wxMouseEvent& event) {
    DX = event.GetX() - tempX;
    DY = tempY - event.GetY();
    Refresh();
}
void BasicGLPane::mouseDown(wxMouseEvent& event) {
    Connect(wxEVT_MOTION,(wxObjectEventFunction)&BasicGLPane::mouseMoved,0,this);
    tempX = event.GetX();
    tempY = event.GetY();
}
void BasicGLPane::mouseReleased(wxMouseEvent& event) {
    Disconnect(wxEVT_MOTION,(wxObjectEventFunction)NULL,0,this);
    oldX += DX;
    oldY += DY;
    DX = 0.0f;
    DY = 0.0f;
}
void BasicGLPane::angleChange(wxMouseEvent& event) {
    angleX = (event.GetX() - tempX) * 0.5f;
    angleY = (event.GetY() - tempY) * 0.5f;
    Refresh();
}
void BasicGLPane::rightClick(wxMouseEvent& event) {
    Connect(wxEVT_MOTION,(wxObjectEventFunction)&BasicGLPane::angleChange,0,this);
    tempX = event.GetX();
    tempY = event.GetY();
}
void BasicGLPane::rightUp(wxMouseEvent& event) {
    Disconnect(wxEVT_MOTION,(wxObjectEventFunction)NULL,0,this);
    oldAngleX += angleX;
    oldAngleY += angleY;
    angleX = 0.0f;
    angleY = 0.0f;
}
void BasicGLPane::mouseWheelMoved(wxMouseEvent& event) {
    mScale += event.GetWheelRotation() > 0 ? 1.0f : -1.0f;
    Refresh();
}
void BasicGLPane::mouseLeftWindow(wxMouseEvent& event) {}
void BasicGLPane::keyPressed(wxKeyEvent& event) {}
void BasicGLPane::keyReleased(wxKeyEvent& event) {}

void BasicGLPane::render( wxPaintEvent& evt )
{
    if(!IsShown()) return;

    wxGLCanvas::SetCurrent(*m_context);
    wxPaintDC(this);

    glClear(GL_COLOR_BUFFER_BIT);
    prepare3DViewport(0,0,GetSize().x, GetSize().y);

    glTranslatef(oldX+DX, oldY+DY, 0.0f);
    glRotatef(oldAngleX + angleX, 0.0f, 1.0f, 0.0f);
    glRotatef(oldAngleY + angleY, 1.0f, 0.0f, 0.0f);
    glScalef(mScale, mScale, mScale);

    glLineWidth(1);
/*    glBegin(GL_LINE_STRIP);
        for(size_t i=0; i<oglArr.size(); i++){
            glVertex3f(oglArr[i][0], oglArr[i][1], oglArr[i][2]);
        }
    glEnd();
*/
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &oglArr[0]);

    glColor3f(1.0f, 0.3f, 0.3f);
    glDrawArrays(GL_LINE_STRIP, 0, oglCur+1);
    if(FullPart){
        glColor3f(1.0f, 1.0f, 1.0f);
        glDrawArrays(GL_LINE_STRIP, oglCur, oglArr.size()/3-oglCur);
    }

    glDisableClientState(GL_VERTEX_ARRAY);

    glLoadIdentity();
    glTranslatef(20.0f, 20.0f, 0.0f);
    glRotatef(oldAngleX + angleX, 0.0f, 1.0f, 0.0f);
    glRotatef(oldAngleY + angleY, 1.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
        glColor3f(1,0,0);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(30.0f, 0.0f, 0.0f);
        glVertex3f(25.0f, -3.0f, 0.0f);
        glVertex3f(25.0f, 3.0f, 0.0f);
        glVertex3f(30.0f, 0.0f, 0.0f);
        glVertex3f(25.0f, 0.0f, -3.0f);
        glVertex3f(25.0f, 0.0f, 3.0f);
        glVertex3f(30.0f, 0.0f, 0.0f);
    glEnd();
        glRasterPos3f(32.0f, 0.0f, 0.0f);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'x');

    glBegin(GL_LINE_STRIP);
        glColor3f(0,1,0);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 30.0f, 0.0f);
        glVertex3f(-3.0f, 25.0f, 0.0f);
        glVertex3f(3.0f, 25.0f, 0.0f);
        glVertex3f(0.0f, 30.0f, 0.0f);
        glVertex3f(0.0f, 25.0f, -3.0f);
        glVertex3f(0.0f, 25.0f, 3.0f);
        glVertex3f(0.0f, 30.0f, 0.0f);
    glEnd();
        glRasterPos3f(0.0f, 34.0f, 0.0f);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'y');

    glBegin(GL_LINE_STRIP);
        glColor3f(0.3f, 0.3f, 1.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 30.0f);
        glVertex3f(-3.0f, 0.0f, 25.0f);
        glVertex3f(3.0f, 0.0f, 25.0f);
        glVertex3f(0.0f, 0.0f, 30.0f);
        glVertex3f(0.0f, -3.0f, 25.0f);
        glVertex3f(0.0f, 3.0f, 25.0f);
        glVertex3f(0.0f, 0.0f, 30.0f);
    glEnd();
        glRasterPos3f(0.0f, 0.0f, 32.0f);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'z');

    prepare2DViewport(0, 0, GetSize().x, GetSize().y);
    glColor3f(1,0,0);
    glRasterPos2f(10, GetSize().y-10);
    for(uint8_t i = 0; i < str.size(); ++i){
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, str[i]);
    }
    glDisable(GL_TEXTURE_2D);

    glFlush();
    SwapBuffers();
}

char GStruct[100];
typedef struct GCommand {
    signed char GM[1];
    unsigned char num;
    unsigned char arr[];
} GCommand_t;
GCommand_t *GComm = (GCommand_t*)GStruct;
char FStruct[sizeof(GCommand_t)+4];
GCommand_t *FComm = (GCommand_t*)FStruct;
typedef struct G01 {
    size_t N;
    long int x;
    long int y;
    long int z;
    unsigned long int dx;
    unsigned long int dy;
    unsigned long int dz;
} G01_t;
G01_t *mG01 = (G01_t*)GComm->arr;
typedef struct G02 {
    size_t N;
    long int se;
    long int sn;
    long int e;
    long int n;
    char pl;
} G02_t;
G02_t *mG02 = (G02_t*)GComm->arr;
long int se, sn, e, n, gip;

void PiCNCFrame::OnBitmapButton1Click(wxCommandEvent& event)
{
    if(replay){
        int fd;
        if((fd = open("/dev/piCNC", O_RDWR)) < 0){
            wxMessageBox("Can't open file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
            return;
        }
        FComm->GM[0] = 'C';
        FComm->num = 10;
        write(fd, FStruct, sizeof(GCommand_t));
        BitmapButton1->SetBitmap(*pauseBmp);
        Connect(ID_BITMAPBUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PiCNCFrame::OnBitmapButton1ClickPause);
        if (close(fd) < 0) wxMessageBox("Can't close file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
    }else{
        replay = true;
        if(lb->GetItemCount()==0) return;
        double GF;
        uint16_t maxSPU = std::max(std::max(datObj->spuX, datObj->spuY), datObj->spuZ);
        unsigned long *F = (unsigned long*)(FComm->arr);
        FComm->GM[0] = 'C';
        FComm->num = 0;
        *F = 30000000000 / (datObj->F * maxSPU);
        char ch;
        double zn=0, Xzn=0, Yzn=0, Zzn=0, OldXzn=0, OldYzn=0, OldZzn=0;
        double I=0, J=0, K=0;
        unsigned char ParkingAx, polarity;
        int fd;
        if((fd = open("/dev/piCNC", O_RDWR)) < 0){
            wxMessageBox("Can't open file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
            return;
        }
        write (fd, FStruct, sizeof(GCommand_t)+4);
    /*    pid_t *pid = (pid_t*)(FComm->arr);
        FComm->num = 6;
        *pid = getpid();
        write (fd, FStruct, sizeof(GCommand_t)+sizeof(pid_t));
    */
        for(unsigned int i=0; i<lb->GetItemCount(); i++){
    //        Xzn = Yzn = Zzn = 0;
            ParkingAx = 0;
            polarity = 0;
            GComm->GM[0] = '0';
            for(size_t j=0; j<lb->GCodeVector[i].size(); j++){
                ch = lb->mStrings[i].at(lb->GCodeVector[i][j].first);
                if(ch == 'G'){
                    GComm->GM[0] = 'G';
                    zn = std::stod(lb->mStrings[i].substr(lb->GCodeVector[i][j].first+1, lb->GCodeVector[i][j].second-1));
                    if(zn == 0 || zn == 1 || zn == 2 || zn == 3 || zn == 4 || zn == 28){
                        GComm->num = zn;
                        //wxMessageBox(std::to_string(sizeof(GCommand_t)+sizeof(G01_t)));
                    }
                }else if(ch == 'X'){
                    if(lb->GCodeVector[i][j].second-lb->GCodeVector[i][j].first>1){
                        Xzn = std::stod(lb->mStrings[i].substr(lb->GCodeVector[i][j].first+1, lb->GCodeVector[i][j].second-1));
                    }else{
                        Xzn = 0;
                        ParkingAx |= (1<<0);
                    }
                }else if(ch == 'Y'){
                    if(lb->GCodeVector[i][j].second-lb->GCodeVector[i][j].first>1){
                        Yzn = std::stod(lb->mStrings[i].substr(lb->GCodeVector[i][j].first+1, lb->GCodeVector[i][j].second-1));
                    }else{
                        Yzn = 0;
                        ParkingAx |= (1<<1);
                    }
                }else if(ch == 'Z'){
                    if(lb->GCodeVector[i][j].second-lb->GCodeVector[i][j].first>1){
                        Zzn = std::stod(lb->mStrings[i].substr(lb->GCodeVector[i][j].first+1, lb->GCodeVector[i][j].second-1));
                    }else{
                        Zzn = 0;
                        ParkingAx |= (1<<2);
                    }
                }else if(ch == 'I'){
                    I = std::stod(lb->mStrings[i].substr(lb->GCodeVector[i][j].first+1, lb->GCodeVector[i][j].second-1));
                    polarity |= 0b001;
                }else if(ch == 'J'){
                    J = std::stod(lb->mStrings[i].substr(lb->GCodeVector[i][j].first+1, lb->GCodeVector[i][j].second-1));
                    polarity |= 0b010;
                }else if(ch == 'K'){
                    K = std::stod(lb->mStrings[i].substr(lb->GCodeVector[i][j].first+1, lb->GCodeVector[i][j].second-1));
                    polarity |= 0b100;
                }else if(ch == 'F'){
                    GF = std::stod(lb->mStrings[i].substr(lb->GCodeVector[i][j].first+1, lb->GCodeVector[i][j].second-1));
                    FComm->num = 4;
                    if(zn!=4){
                        FComm->GM[0] = 'C';
                        if(GF <= datObj->F){
                            *F = 30000000000 / (GF * maxSPU);
                        }else{
                            *F = 30000000000 / (datObj->F * maxSPU);
                        }
                    }else{
                        FComm->GM[0] = 'G';
                        *F = GF * 1000000000;
                    }
                    write (fd, FStruct, sizeof(GCommand_t)+4);
                    continue;
                }else if(ch == 'P' && zn==4){
                    FComm->GM[0] = 'G';
                    FComm->num = 4;
                    if(lb->mStrings[i][lb->GCodeVector[i][j].second-1]=='.'){
                        *F = std::stod(lb->mStrings[i].substr(lb->GCodeVector[i][j].first+1, lb->GCodeVector[i][j].second-1)) * 1000000000;
                    }else{
                        *F = std::stod(lb->mStrings[i].substr(lb->GCodeVector[i][j].first+1, lb->GCodeVector[i][j].second-1)) * 1000000;
                    }
                    write (fd, FStruct, sizeof(GCommand_t)+4);
                    continue;
                }else if(ch == 'M'){
                    zn = std::stod(lb->mStrings[i].substr(lb->GCodeVector[i][j].first+1, lb->GCodeVector[i][j].second-1));
                    if(zn == 3 || zn ==5){
                        FComm->GM[0] = 'M';
                        FComm->num = zn;
                        *F = i;
                        write (fd, FStruct, sizeof(GCommand_t)+4);
                    }
                }
            }
            if(GComm->GM[0]=='G' && (GComm->num == 0 || GComm->num == 1)){
                mG01->x = (Xzn - OldXzn) * datObj->spuX;
                mG01->y = (Yzn - OldYzn) * datObj->spuY;
                mG01->z = (Zzn - OldZzn) * datObj->spuZ;
                double MaxDist = std::max(std::max(std::abs(mG01->x), std::abs(mG01->y)), std::abs(mG01->z));
                if(MaxDist != 0){
                    mG01->N = i;
                    mG01->dx = std::abs((double(mG01->x) / double(MaxDist)) * 1048576);
                    mG01->dy = std::abs((double(mG01->y) / double(MaxDist)) * 1048576);
                    mG01->dz = std::abs((double(mG01->z) / double(MaxDist)) * 1048576);//(1<<20) 1048576
                    write (fd, GStruct, sizeof(GCommand_t)+sizeof(G01_t));
                    OldXzn = Xzn;
                    OldYzn = Yzn;
                    OldZzn = Zzn;
                }
            }else if(GComm->GM[0]=='G' && (GComm->num == 28)){
                if(ParkingAx==0){
                    GComm->arr[0] = 0b0111;
                    Xzn = Yzn = Zzn = 0;
                }else{
                    GComm->arr[0] = ParkingAx;
                }
                write(fd, GStruct, sizeof(GCommand_t)+1);
            }else if(GComm->GM[0]=='G' && GComm->num == 2){
                mG02->N = i;
                I += OldXzn;
                J += OldYzn;
                K += OldZzn;//wxMessageBox("rad="+std::to_string(std::sqrt((OldXzn-I)*(OldXzn-I)+(OldYzn-I)*(OldYzn-I)))+"rad2="+std::to_string(std::sqrt((Xzn-I)*(Xzn-I)+(Yzn-I)*(Yzn-I))));
                if(polarity & 0b001){
                    se = (OldXzn - I) * datObj->spuX;
                    e = (Xzn - I) * datObj->spuX;
                }
                if(polarity & 0b010){
                    if(polarity & 0b001){
                        sn = (OldYzn - J) * datObj->spuY;
                        n = (Yzn - J) * datObj->spuY;
                    }else if(polarity & 0b100){
                        se = (OldYzn - J) * datObj->spuY;
                        e = (Yzn - J) * datObj->spuY;
                    }
                }
                if(polarity & 0b100){
                    sn = (OldZzn - K) * datObj->spuZ;
                    n = (Zzn - K) * datObj->spuZ;
                }
                mG02->pl = polarity;
                gip = std::sqrt(e*e + n*n);

                if(sn == 0 && n == 0){
                    if(se < 0 && e < 0){
                        mG02->pl |= 0b10000;
                        mG02->pl |= 0b01000;
                        mG02->se = -gip;
                        mG02->sn = 0;
                        mG02->e = gip;
                        mG02->n = 0;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                        mG02->pl &=~ 0b10000;
                        mG02->pl &=~ 0b01000;
                        mG02->se = gip;
                        mG02->e = -gip;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                    }else if(se < 0 && e > 0){
                        mG02->pl |= 0b10000;
                        mG02->pl |= 0b01000;
                        mG02->se = -gip;
                        mG02->sn = 0;
                        mG02->e = gip;
                        mG02->n = 0;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                    }else if(se > 0 && e > 0){
                        mG02->pl &=~ 0b10000;
                        mG02->pl &=~ 0b01000;
                        mG02->se = gip;
                        mG02->sn = 0;
                        mG02->e = -gip;
                        mG02->n = 0;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                        mG02->pl |= 0b10000;
                        mG02->pl |= 0b01000;
                        mG02->se = -gip;
                        mG02->e = gip;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                    }else if(se > 0 && e < 0){
                        mG02->pl &=~ 0b10000;
                        mG02->pl &=~ 0b01000;
                        mG02->se = gip;
                        mG02->sn = 0;
                        mG02->e = -gip;
                        mG02->n = 0;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                    }
                }else if(sn >= 0 && n >= 0){
                    if(se < e){//wxMessageBox("se="+std::to_string(se)+"e="+std::to_string(e)+"sn="+std::to_string(sn)+"n="+std::to_string(n));
                        mG02->pl |= 0b10000;
                        mG02->pl |= 0b01000;
                        mG02->se = se;
                        mG02->sn = sn;
                        mG02->e = e;
                        mG02->n = n;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                    }else{
                        if(sn != 0){
                            mG02->pl |= 0b10000;
                            mG02->pl |= 0b01000;
                            mG02->se = se;
                            mG02->sn = sn;
                            mG02->e = gip;
                            mG02->n = 0;
                            write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                        }
                        mG02->pl &=~ 0b10000;
                        mG02->pl &=~ 0b01000;
                        mG02->se = gip;
                        mG02->sn = 0;
                        mG02->e = -gip;
                        mG02->n = 0;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                        if(n != 0){
                            mG02->pl |= 0b10000;
                            mG02->pl |= 0b01000;
                            mG02->se = -gip;
                            mG02->sn = 0;
                            mG02->e = se;
                            mG02->n = sn;
                            write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                        }
                    }
                }else if(sn <= 0 && n <= 0){
                    if(se > e){
                        mG02->pl &=~ 0b10000;
                        mG02->pl &=~ 0b01000;
                        mG02->se = se;
                        mG02->sn = sn;
                        mG02->e = e;
                        mG02->n = n;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                    }else{
                        if(sn != 0){
                            mG02->pl &=~ 0b10000;
                            mG02->pl &=~ 0b01000;
                            mG02->se = se;
                            mG02->sn = sn;
                            mG02->e = -gip;
                            mG02->n = 0;
                            write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                        }
                        mG02->pl |= 0b10000;
                        mG02->pl |= 0b01000;
                        mG02->se = -gip;
                        mG02->sn = 0;
                        mG02->e = gip;
                        mG02->n = 0;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                        if(n != 0){
                            mG02->pl &=~ 0b10000;
                            mG02->pl &=~ 0b01000;
                            mG02->se = gip;
                            mG02->sn = 0;
                            mG02->e = se;
                            mG02->n = sn;
                            write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                        }
                    }
                }else if(sn > 0 && n < 0){
                    mG02->pl |= 0b10000;
                    mG02->pl |= 0b01000;
                    mG02->se = se;
                    mG02->sn = sn;
                    mG02->e = gip;
                    mG02->n = 0;
                    write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                    mG02->pl &=~ 0b10000;
                    mG02->pl &=~ 0b01000;
                    mG02->se = gip;
                    mG02->sn = 0;
                    mG02->e = e;
                    mG02->n = n;
                    write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                }else if(sn < 0 && n > 0){
                    mG02->pl &=~ 0b10000;
                    mG02->pl &=~ 0b01000;
                    mG02->se = se;
                    mG02->sn = sn;
                    mG02->e = -gip;
                    mG02->n = 0;
                    write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                    mG02->pl |= 0b10000;
                    mG02->pl |= 0b01000;
                    mG02->se = -gip;
                    mG02->sn = 0;
                    mG02->e = e;
                    mG02->n = n;
                    write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                }

                I=J=K=0;
                OldXzn = Xzn;
                OldYzn = Yzn;
                OldZzn = Zzn;
            }else if(GComm->GM[0]=='G' && GComm->num == 3){
                mG02->N = i;
                I += OldXzn;
                J += OldYzn;
                K += OldZzn;//wxMessageBox("rad="+std::to_string(std::sqrt((OldXzn-I)*(OldXzn-I)+(OldYzn-I)*(OldYzn-I)))+"rad2="+std::to_string(std::sqrt((Xzn-I)*(Xzn-I)+(Yzn-I)*(Yzn-I))));
                if(polarity & 0b001){
                    se = (OldXzn - I) * datObj->spuX;
                    e = (Xzn - I) * datObj->spuX;
                }
                if(polarity & 0b010){
                    if(polarity & 0b001){
                        sn = (OldYzn - J) * datObj->spuY;
                        n = (Yzn - J) * datObj->spuY;
                    }else if(polarity & 0b100){
                        se = (OldYzn - J) * datObj->spuY;
                        e = (Yzn - J) * datObj->spuY;
                    }
                }
                if(polarity & 0b100){
                    sn = (OldZzn - K) * datObj->spuZ;
                    n = (Zzn - K) * datObj->spuZ;
                }
                mG02->pl = polarity;
                gip = std::sqrt(e*e + n*n);

                if(sn == 0 && n == 0){
                    if(se < 0 && e < 0){
                        mG02->pl &=~ 0b10000;
                        mG02->pl |= 0b01000;
                        mG02->se = -gip;
                        mG02->sn = 0;
                        mG02->e = gip;
                        mG02->n = 0;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                        mG02->pl |= 0b10000;
                        mG02->pl &=~ 0b01000;
                        mG02->se = gip;
                        mG02->e = -gip;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                    }else if(se < 0 && e > 0){
                        mG02->pl &=~ 0b10000;
                        mG02->pl |= 0b01000;
                        mG02->se = -gip;
                        mG02->sn = 0;
                        mG02->e = gip;
                        mG02->n = 0;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                    }else if(se > 0 && e > 0){
                        mG02->pl |= 0b10000;
                        mG02->pl &=~ 0b01000;
                        mG02->se = gip;
                        mG02->sn = 0;
                        mG02->e = -gip;
                        mG02->n = 0;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                        mG02->pl &=~ 0b10000;
                        mG02->pl |= 0b01000;
                        mG02->se = -gip;
                        mG02->e = gip;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                    }else if(se > 0 && e < 0){
                        mG02->pl |= 0b10000;
                        mG02->pl &=~ 0b01000;
                        mG02->se = gip;
                        mG02->sn = 0;
                        mG02->e = -gip;
                        mG02->n = 0;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                    }
                }else if(sn >= 0 && n >= 0){
                    if(se > e){//wxMessageBox("se="+std::to_string(se)+"e="+std::to_string(e)+"sn="+std::to_string(sn)+"n="+std::to_string(n));
                        mG02->pl |= 0b10000;
                        mG02->pl &=~ 0b01000;
                        mG02->se = se;
                        mG02->sn = sn;
                        mG02->e = e;
                        mG02->n = n;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                    }else{
                        if(sn != 0){
                            mG02->pl |= 0b10000;
                            mG02->pl &=~ 0b01000;
                            mG02->se = se;
                            mG02->sn = sn;
                            mG02->e = -gip;
                            mG02->n = 0;
                            write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                        }
                        mG02->pl &=~ 0b10000;
                        mG02->pl |= 0b01000;
                        mG02->se = -gip;
                        mG02->sn = 0;
                        mG02->e = gip;
                        mG02->n = 0;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                        if(n != 0){
                            mG02->pl |= 0b10000;
                            mG02->pl &=~ 0b01000;
                            mG02->se = gip;
                            mG02->sn = 0;
                            mG02->e = se;
                            mG02->n = sn;
                            write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                        }
                    }
                }else if(sn <= 0 && n <= 0){
                    if(se < e){
                        mG02->pl &=~ 0b10000;
                        mG02->pl |= 0b01000;
                        mG02->se = se;
                        mG02->sn = sn;
                        mG02->e = e;
                        mG02->n = n;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                    }else{
                        if(sn != 0){
                            mG02->pl &=~ 0b10000;
                            mG02->pl |= 0b01000;
                            mG02->se = se;
                            mG02->sn = sn;
                            mG02->e = gip;
                            mG02->n = 0;
                            write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                        }
                        mG02->pl |= 0b10000;
                        mG02->pl &=~ 0b01000;
                        mG02->se = gip;
                        mG02->sn = 0;
                        mG02->e = -gip;
                        mG02->n = 0;
                        write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                        if(n != 0){
                            mG02->pl &=~ 0b10000;
                            mG02->pl |= 0b01000;
                            mG02->se = -gip;
                            mG02->sn = 0;
                            mG02->e = se;
                            mG02->n = sn;
                            write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                        }
                    }
                }else if(sn > 0 && n < 0){
                    mG02->pl |= 0b10000;
                    mG02->pl &=~ 0b01000;
                    mG02->se = se;
                    mG02->sn = sn;
                    mG02->e = -gip;
                    mG02->n = 0;
                    write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                    mG02->pl &=~ 0b10000;
                    mG02->pl |= 0b01000;
                    mG02->se = -gip;
                    mG02->sn = 0;
                    mG02->e = e;
                    mG02->n = n;
                    write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                }else if(sn < 0 && n > 0){
                    mG02->pl &=~ 0b10000;
                    mG02->pl |= 0b01000;
                    mG02->se = se;
                    mG02->sn = sn;
                    mG02->e = gip;
                    mG02->n = 0;
                    write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                    mG02->pl |= 0b10000;
                    mG02->pl &=~ 0b01000;
                    mG02->se = gip;
                    mG02->sn = 0;
                    mG02->e = e;
                    mG02->n = n;
                    write(fd, GStruct, sizeof(GCommand_t)+sizeof(G02_t));
                }

                I=J=K=0;
                OldXzn = Xzn;
                OldYzn = Yzn;
                OldZzn = Zzn;
            }
        }

        FComm->GM[0] = 'C';
        FComm->num = 5;
        write(fd, FStruct, sizeof(GCommand_t)+4);

        BitmapButton1->SetBitmap(*pauseBmp);
        Connect(ID_BITMAPBUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PiCNCFrame::OnBitmapButton1ClickPause);

        if (close(fd) < 0) wxMessageBox("Can't close file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
    }
}

void PiCNCFrame::OnBitmapButton4Click(wxCommandEvent& event)
{
    mScale = tScale;
    oldAngleX = 0.0f;
    oldAngleY = 0.0f;
    oldX = 0.0f;
    oldY = 0.0f;
    GLCanvas1->Refresh();
}

void PiCNCFrame::OnBitmapButton3Click(wxCommandEvent& event)
{
    if(FullPart){
        BitmapButton3->SetBitmap(*select2Bmp);
        FullPart = false;
    }else{
        BitmapButton3->SetBitmap(*selectBmp);
        FullPart = true;
    }
    GLCanvas1->Refresh();
}

void PiCNCFrame::OnBitmapButton2Click(wxCommandEvent& event)
{
    int fd;
    if((fd = open("/dev/piCNC", O_RDWR)) < 0){
        wxMessageBox("Can't open file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
        return;
    }
    FComm->GM[0] = 'C';
    FComm->num = 9;
    write(fd, FStruct, sizeof(GCommand_t));
    BitmapButton1->SetBitmap(*playBmp);
    Connect(ID_BITMAPBUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PiCNCFrame::OnBitmapButton1Click);
    if (close(fd) < 0) wxMessageBox("Can't close file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
}

void PiCNCFrame::OnBitmapButton1ClickPause(wxCommandEvent& event){
    int fd;
    if(!replay){
        OnBitmapButton1Click(event);
        return;
    }
    if((fd = open("/dev/piCNC", O_RDWR)) < 0){
        wxMessageBox("Can't open file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
        return;
    }
    FComm->GM[0] = 'C';
    FComm->num = 8;
    write(fd, FStruct, sizeof(GCommand_t));
    BitmapButton1->SetBitmap(*playBmp);
    Connect(ID_BITMAPBUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PiCNCFrame::OnBitmapButton1Click);
    if (close(fd) < 0) wxMessageBox("Can't close file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
}
