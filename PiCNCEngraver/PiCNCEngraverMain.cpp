/***************************************************************
 * Name:      PiCNCEngraverMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    MBajor ()
 * Created:   2019-12-22
 * Copyright: MBajor ()
 * License:
 **************************************************************/

#include "PiCNCEngraverMain.h"
#include <wx/msgdlg.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/dc.h>

//(*InternalHeaders(PiCNCEngraverFrame)
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

#define px_in_mm 3.8
#define mm_in_inch 25.4
//*F = 6000000000 / (datObj->F * maxSPU);

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

//(*IdInit(PiCNCEngraverFrame)
const long PiCNCEngraverFrame::ID_STATICTEXT13 = wxNewId();
const long PiCNCEngraverFrame::ID_CHECKBOX1 = wxNewId();
const long PiCNCEngraverFrame::ID_STATICTEXT1 = wxNewId();
const long PiCNCEngraverFrame::ID_SLIDER1 = wxNewId();
const long PiCNCEngraverFrame::ID_STATICTEXT2 = wxNewId();
const long PiCNCEngraverFrame::ID_SLIDER2 = wxNewId();
const long PiCNCEngraverFrame::ID_BUTTON1 = wxNewId();
const long PiCNCEngraverFrame::ID_STATICTEXT3 = wxNewId();
const long PiCNCEngraverFrame::ID_STATICTEXT4 = wxNewId();
const long PiCNCEngraverFrame::ID_SPINCTRL1 = wxNewId();
const long PiCNCEngraverFrame::ID_STATICTEXT5 = wxNewId();
const long PiCNCEngraverFrame::ID_SPINCTRL2 = wxNewId();
const long PiCNCEngraverFrame::ID_STATICTEXT6 = wxNewId();
const long PiCNCEngraverFrame::ID_SPINCTRL3 = wxNewId();
const long PiCNCEngraverFrame::ID_STATICTEXT7 = wxNewId();
const long PiCNCEngraverFrame::ID_SPINCTRL4 = wxNewId();
const long PiCNCEngraverFrame::ID_STATICTEXT8 = wxNewId();
const long PiCNCEngraverFrame::ID_SPINCTRL5 = wxNewId();
const long PiCNCEngraverFrame::ID_STATICTEXT9 = wxNewId();
const long PiCNCEngraverFrame::ID_STATICTEXT10 = wxNewId();
const long PiCNCEngraverFrame::ID_SPINCTRL6 = wxNewId();
const long PiCNCEngraverFrame::ID_STATICTEXT11 = wxNewId();
const long PiCNCEngraverFrame::ID_SPINCTRL7 = wxNewId();
const long PiCNCEngraverFrame::ID_BUTTON2 = wxNewId();
const long PiCNCEngraverFrame::ID_STATICTEXT12 = wxNewId();
const long PiCNCEngraverFrame::ID_SPINCTRL8 = wxNewId();
const long PiCNCEngraverFrame::ID_TOGGLEBUTTON1 = wxNewId();
const long PiCNCEngraverFrame::ID_BITMAPBUTTON1 = wxNewId();
const long PiCNCEngraverFrame::ID_BITMAPBUTTON2 = wxNewId();
const long PiCNCEngraverFrame::ID_CHOICE1 = wxNewId();
const long PiCNCEngraverFrame::ID_MENUITEM1 = wxNewId();
const long PiCNCEngraverFrame::idMenuQuit = wxNewId();
const long PiCNCEngraverFrame::idMenuAbout = wxNewId();
const long PiCNCEngraverFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(PiCNCEngraverFrame,wxFrame)
    //(*EventTable(PiCNCEngraverFrame)
    //*)
END_EVENT_TABLE()

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

int sets_fd;
char setsBuf[sizeof(MSets_t)];
MSets_t *datObj = (MSets_t*)setsBuf;
void sigalarm_handler(int sig);
wxFrame *FramePtr;
PiCNCEngraverFrame *PiCNCEngraverFramePtr;
wxBitmap* MPlay, *pauseBmp;
struct sigaction mSig;
void signalFunct(int n, siginfo_t *info, void *unused);

PiCNCEngraverFrame::PiCNCEngraverFrame(wxWindow* parent,wxWindowID id)
{
 //   wxInitAllImageHandlers();
    //(*Initialize(PiCNCEngraverFrame)
    wxMenu* Menu1;
    wxMenu* Menu2;
    wxMenuBar* MenuBar1;
    wxMenuItem* MenuItem1;
    wxMenuItem* MenuItem2;

    Create(parent, wxID_ANY, _("PiCNC Engraver"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(400,642));
    StaticText13 = new wxStaticText(this, ID_STATICTEXT13, _("Image quality"), wxPoint(8,176), wxDefaultSize, 0, _T("ID_STATICTEXT13"));
    CheckBox1 = new wxCheckBox(this, ID_CHECKBOX1, _("Monochrome"), wxPoint(8,8), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    CheckBox1->SetValue(false);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Brightness"), wxPoint(8,32), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    StaticText1->Disable();
    Slider1 = new wxSlider(this, ID_SLIDER1, 0, -255, 255, wxPoint(8,56), wxSize(240,28), 0, wxDefaultValidator, _T("ID_SLIDER1"));
    Slider1->Disable();
    StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Contrast"), wxPoint(8,88), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    StaticText2->Disable();
    Slider2 = new wxSlider(this, ID_SLIDER2, 0, 0, 255, wxPoint(8,111), wxSize(240,28), 0, wxDefaultValidator, _T("ID_SLIDER2"));
    Slider2->Disable();
    Button1 = new wxButton(this, ID_BUTTON1, _("Default"), wxPoint(16,144), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    Button1->Disable();
    StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Image dimention (mm)"), wxPoint(8,232), wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("W"), wxPoint(8,264), wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    SpinCtrl1 = new wxSpinCtrl(this, ID_SPINCTRL1, _T("1"), wxPoint(24,256), wxDefaultSize, 0, 1, 100000000, 1, _T("ID_SPINCTRL1"));
    SpinCtrl1->SetValue(_T("1"));
    StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("H"), wxPoint(8,296), wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    SpinCtrl2 = new wxSpinCtrl(this, ID_SPINCTRL2, _T("1"), wxPoint(24,288), wxDefaultSize, 0, 1, 100000000, 1, _T("ID_SPINCTRL2"));
    SpinCtrl2->SetValue(_T("1"));
    StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Step per unit"), wxPoint(8,320), wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    SpinCtrl3 = new wxSpinCtrl(this, ID_SPINCTRL3, _T("0"), wxPoint(24,344), wxDefaultSize, 0, 0, 10000, 0, _T("ID_SPINCTRL3"));
    SpinCtrl3->SetValue(_T("0"));
    StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _("X"), wxPoint(8,352), wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    SpinCtrl4 = new wxSpinCtrl(this, ID_SPINCTRL4, _T("0"), wxPoint(24,376), wxDefaultSize, 0, 0, 10000, 0, _T("ID_SPINCTRL4"));
    SpinCtrl4->SetValue(_T("0"));
    StaticText8 = new wxStaticText(this, ID_STATICTEXT8, _("Lasar width (steps)"), wxPoint(8,472), wxDefaultSize, 0, _T("ID_STATICTEXT8"));
    SpinCtrl5 = new wxSpinCtrl(this, ID_SPINCTRL5, _T("0"), wxPoint(24,408), wxDefaultSize, 0, 0, 10000, 0, _T("ID_SPINCTRL5"));
    SpinCtrl5->SetValue(_T("0"));
    StaticText9 = new wxStaticText(this, ID_STATICTEXT9, _("Y"), wxPoint(8,384), wxDefaultSize, 0, _T("ID_STATICTEXT9"));
    StaticText10 = new wxStaticText(this, ID_STATICTEXT10, _("Z"), wxPoint(8,416), wxDefaultSize, 0, _T("ID_STATICTEXT10"));
    SpinCtrl6 = new wxSpinCtrl(this, ID_SPINCTRL6, _T("0"), wxPoint(24,440), wxDefaultSize, 0, 0, 10000, 0, _T("ID_SPINCTRL6"));
    SpinCtrl6->SetValue(_T("0"));
    StaticText11 = new wxStaticText(this, ID_STATICTEXT11, _("F"), wxPoint(8,448), wxDefaultSize, 0, _T("ID_STATICTEXT11"));
    SpinCtrl7 = new wxSpinCtrl(this, ID_SPINCTRL7, _T("1"), wxPoint(24,496), wxDefaultSize, 0, 1, 10000, 1, _T("ID_SPINCTRL7"));
    SpinCtrl7->SetValue(_T("1"));
    Button2 = new wxButton(this, ID_BUTTON2, _("Save parameter"), wxPoint(24,592), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    StaticText12 = new wxStaticText(this, ID_STATICTEXT12, _("Burning F"), wxPoint(8,528), wxDefaultSize, 0, _T("ID_STATICTEXT12"));
    SpinCtrl8 = new wxSpinCtrl(this, ID_SPINCTRL8, _T("1"), wxPoint(24,552), wxDefaultSize, 0, 1, 10000, 1, _T("ID_SPINCTRL8"));
    SpinCtrl8->SetValue(_T("1"));
    ToggleButton1 = new wxToggleButton(this, ID_TOGGLEBUTTON1, _("&&"), wxPoint(153,278), wxSize(19,23), 0, wxDefaultValidator, _T("ID_TOGGLEBUTTON1"));
    ToggleButton1->SetValue(true);
    wxFont ToggleButton1Font(8,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Sans"),wxFONTENCODING_DEFAULT);
    ToggleButton1->SetFont(ToggleButton1Font);
    BitmapButton1 = new wxBitmapButton(this, ID_BITMAPBUTTON1, wxNullBitmap, wxPoint(136,8), wxSize(33,33), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
    BitmapButton2 = new wxBitmapButton(this, ID_BITMAPBUTTON2, wxBitmap(wxImage(_T("/home/pi/Desktop/netlink/user/PiCNCEngraver/img/cansel.png"))), wxPoint(176,8), wxSize(33,33), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON2"));
    Choice1 = new wxChoice(this, ID_CHOICE1, wxPoint(24,200), wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
    Choice1->Append(_("BICUBIC"));
    Choice1->Append(_("BILINEAR"));
    Choice1->Append(_("BOX AVERAGE"));
    Choice1->SetSelection( Choice1->Append(_("HIGH")) );
    Choice1->Append(_("NEAREST"));
    Choice1->Append(_("NORMAL"));
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu1, ID_MENUITEM1, _("&Open\tctrl+o"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem3);
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, _("Image files |*.png;*.gif;*.bmp|All files |*"), wxFD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    Center();

    Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&PiCNCEngraverFrame::OnCheckBox1Click);
    Connect(ID_SLIDER1,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&PiCNCEngraverFrame::OnSlider1CmdScrollChanged);
    Connect(ID_SLIDER2,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&PiCNCEngraverFrame::OnSlider2CmdScrollThumbTrack);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PiCNCEngraverFrame::OnButton1Click);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PiCNCEngraverFrame::OnButton2Click);
    Connect(ID_BITMAPBUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PiCNCEngraverFrame::OnBitmapButton2Click);
    Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&PiCNCEngraverFrame::OnChoice1Select);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PiCNCEngraverFrame::OnMenuItem3Selected);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PiCNCEngraverFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PiCNCEngraverFrame::OnAbout);
    Connect(wxEVT_PAINT,(wxObjectEventFunction)&PiCNCEngraverFrame::OnPanel1Paint);
    Connect(wxEVT_MOUSEWHEEL,(wxObjectEventFunction)&PiCNCEngraverFrame::OnMouseWheel);
    //*)
    SpinCtrl1->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &PiCNCEngraverFrame::OnSpinCtrl1Change, this);
    SpinCtrl1->Bind(wxEVT_KEY_UP, &PiCNCEngraverFrame::OnSpinCtrl1KeyUp, this);
    SpinCtrl2->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &PiCNCEngraverFrame::OnSpinCtrl2Change, this);
    SpinCtrl2->Bind(wxEVT_KEY_UP, &PiCNCEngraverFrame::OnSpinCtrl2KeyUp, this);
    BitmapButton1->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &PiCNCEngraverFrame::OnBitmapButton1Click, this);
    Maximize(true);
    signal(SIGALRM, &sigalarm_handler);
    FramePtr = this;
    PiCNCEngraverFramePtr = this;

  //  mCanvas1->Bind(wxEVT_PAINT, &PiCNCEngraverFrame::OnPanel1Paint, this);

    if((sets_fd = open("piCNCsetings", O_RDWR)) < 0){
        wxMessageBox("Please enter first mashine parameters in Menu->ManualControl->Control window");
    }else{
        read(sets_fd, setsBuf, sizeof(MSets_t));
        close(sets_fd);
        SpinCtrl3->SetValue(std::to_string(datObj->spuX));
        SpinCtrl4->SetValue(std::to_string(datObj->spuY));
        SpinCtrl5->SetValue(std::to_string(datObj->spuZ));
        SpinCtrl6->SetValue(std::to_string(datObj->F));
        SpinCtrl7->SetValue(std::to_string(datObj->laserWidth));
        SpinCtrl8->SetValue(std::to_string(datObj->burnTime));
    }
    MPlay = new wxBitmap(wxImage(_T("img/play.png")));
    pauseBmp = new wxBitmap(wxImage(_T("img/pause.png")));
    BitmapButton1->SetBitmap(*MPlay);

    mSig.sa_sigaction = signalFunct;
    mSig.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR2, &mSig, NULL);
}
wxImage *mImg;
wxBitmap *playBmp;
int imgW, imgH;
wxImageResizeQuality mQuality = wxIMAGE_QUALITY_HIGH;
long double proportion = 1;
PiCNCEngraverFrame::~PiCNCEngraverFrame()
{
    //(*Destroy(PiCNCEngraverFrame)
    //*)
}

void sigalarm_handler(int sig){
    FramePtr->Refresh();
}

void signalFunct(int n, siginfo_t *info, void *unused){
    PiCNCEngraverFramePtr->memberFn();
}

void PiCNCEngraverFrame::memberFn(void){
    BitmapButton1->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &PiCNCEngraverFrame::OnBitmapButton1Click, this);
    BitmapButton1->SetBitmap(*MPlay);
}

void PiCNCEngraverFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void PiCNCEngraverFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void PiCNCEngraverFrame::OnMenuItem3Selected(wxCommandEvent& event)
{
    if ( FileDialog1->ShowModal() != wxID_OK ) return;

    wxString FileName = FileDialog1->GetPath();
    if ( FileName.IsEmpty() ) return;

    mImg = new wxImage(FileName);
    imgW = mImg->GetWidth();
    imgH = mImg->GetHeight();
    proportion = double(imgH)/double(imgW);
    SpinCtrl1->SetValue(imgW/px_in_mm);
    SpinCtrl2->SetValue(imgH/px_in_mm);
    ualarm(200000, 0);
}

void PiCNCEngraverFrame::OnPanel1Paint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
 //   playBmp = new wxBitmap(wxImage("/home/pi/PiCNC/img/pause.png"));
//playBmp->LoadFile("/home/pi/PiCNC/img/pause.png", wxBITMAP_TYPE_PNG);
    if(mImg != nullptr){
        wxImage* newImg = new wxImage(mImg->GetWidth(), mImg->GetHeight());
        if(CheckBox1->GetValue()){
            unsigned char monochrome = 0;
            for(int i=0; i<mImg->GetWidth(); i++){
                for(int j=0; j<mImg->GetHeight(); j++){
                    monochrome = (mImg->GetRed(i, j) + mImg->GetGreen(i, j) + mImg->GetBlue(i, j))/3;
                    int liting = monochrome+Slider1->GetValue();
                    if(liting < 126){
                        liting-=Slider2->GetValue();
                    }else{
                        liting+=Slider2->GetValue();
                    }
                    if(liting<0){
                        monochrome = 0;
                    }else if(liting>255){
                        monochrome = 255;
                    }else{
                        monochrome += Slider1->GetValue();
                    }
                    newImg->SetRGB(i, j, monochrome, monochrome, monochrome);
                }
            }
            wxImage scaleImg = newImg->Scale(imgW, imgH, mQuality);
            playBmp = new wxBitmap(scaleImg);
        }else{
//            FramePtr->GetSize(&w, &h);
            wxImage scaleImg = mImg->Scale(imgW, imgH, mQuality);
        //    scaleImg = scaleImg.Blur(20);
            playBmp = new wxBitmap(scaleImg);
        }
        dc.DrawBitmap(*playBmp, 250, 0, false);

    }
//    dc.DrawText(wxT("asf"), 10, 15);
//    event.Skip(true);
}

void PiCNCEngraverFrame::OnCheckBox1Click(wxCommandEvent& event)
{
    FramePtr->Refresh();
    if(CheckBox1->GetValue()){
        StaticText1->Enable();
        Slider1->Enable();
        StaticText2->Enable();
        Slider2->Enable();
        Button1->Enable();
    }else{
        StaticText1->Disable();
        Slider1->Disable();
        StaticText2->Disable();
        Slider2->Disable();
        Button1->Disable();
    }
}

void PiCNCEngraverFrame::OnSlider1CmdScrollChanged(wxScrollEvent& event)
{
    StaticText1->SetLabel("Brightness " + std::to_string(Slider1->GetValue()));
    FramePtr->Refresh();
}

void PiCNCEngraverFrame::OnSlider2CmdScrollThumbTrack(wxScrollEvent& event)
{
    StaticText2->SetLabel("Contrast " + std::to_string(Slider2->GetValue()));
    FramePtr->Refresh();
}

void PiCNCEngraverFrame::OnButton1Click(wxCommandEvent& event)
{
    StaticText1->SetLabel("Brightness 0");
    Slider1->SetValue(0);
    StaticText2->SetLabel("Contrast 0");
    Slider2->SetValue(0);
    FramePtr->Refresh();
}

void PiCNCEngraverFrame::OnMouseWheel(wxMouseEvent& event)
{
    int w, h;
    FramePtr->GetSize(&w, &h);
    if(event.GetWheelRotation() > 0 && imgW <= (w-250)){
        imgW += 10;
    }else if(event.GetWheelRotation() < 0 && imgW > 10){
        imgW -= 10;
    }
    imgH = imgW * proportion;
    FramePtr->Refresh();
}

void PiCNCEngraverFrame::OnButton2Click(wxCommandEvent& event)
{
    if((sets_fd = open("piCNCsetings", O_RDWR)) < 0){
        wxMessageBox("Please enter first mashine parameters in Menu->ManualControl->Control window");
    }else{
        datObj->spuX = SpinCtrl3->GetValue();
        datObj->spuY = SpinCtrl4->GetValue();
        datObj->spuZ = SpinCtrl5->GetValue();
        datObj->F = SpinCtrl6->GetValue();
        datObj->laserWidth = SpinCtrl7->GetValue();
        datObj->burnTime = SpinCtrl8->GetValue();
        write(sets_fd, setsBuf, sizeof(MSets_t));
        close(sets_fd);
        wxMessageBox("Parameter saved", "PiCNC Engraver");
    }
}

void PiCNCEngraverFrame::OnSpinCtrl1Change(wxSpinEvent& event)
{
    if(ToggleButton1->GetValue()){
        //SpinCtrl2->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &PiCNCEngraverFrame::OnSpinCtrl2Change, this);
        SpinCtrl2->SetValue(SpinCtrl1->GetValue() * proportion);
    } else {
        proportion = (double)SpinCtrl2->GetValue()/(double)SpinCtrl1->GetValue();
        FramePtr->Refresh();
    }
}

void PiCNCEngraverFrame::OnSpinCtrl1KeyUp(wxKeyEvent& event){
    if((event.GetUnicodeKey()>=48 && event.GetUnicodeKey() <= 57) || event.GetKeyCode() == 8 || event.GetKeyCode() == 127){
        if(ToggleButton1->GetValue()){
            SpinCtrl2->SetValue(SpinCtrl1->GetValue() * proportion);
        } else {
            proportion = (double)SpinCtrl2->GetValue()/(double)SpinCtrl1->GetValue();
        }
    }
}

void PiCNCEngraverFrame::OnSpinCtrl2Change(wxSpinEvent& event)
{
    if(ToggleButton1->GetValue()){
        SpinCtrl1->SetValue(SpinCtrl2->GetValue() / proportion);
    } else {
        proportion = (double)SpinCtrl2->GetValue()/(double)SpinCtrl1->GetValue();
        FramePtr->Refresh();
    }
}

void PiCNCEngraverFrame::OnSpinCtrl2KeyUp(wxKeyEvent& event){
    if((event.GetUnicodeKey()>=48 && event.GetUnicodeKey() <= 57) || event.GetKeyCode() == 8 || event.GetKeyCode() == 127){
        if(ToggleButton1->GetValue()){
            SpinCtrl1->SetValue(SpinCtrl2->GetValue() / proportion);
        } else {
            proportion = (double)SpinCtrl2->GetValue()/(double)SpinCtrl1->GetValue();
        }
    }
}

typedef struct GCommand {
    signed char GM[1];
    unsigned char num;
    unsigned char arr[];
} GCommand_t;
typedef struct engraverSt {
    size_t width;
    size_t height;
    unsigned long fidrate;
    unsigned char lasarWidth;
    unsigned char pixWidth;
    unsigned long burnF;
    unsigned char arr[];
}engraverSt_t;

void PiCNCEngraverFrame::OnBitmapButton1Click(wxCommandEvent& event)
{
    if(mImg == nullptr) return;
   // FramePtr->SetCursor(wxCursor(wxCURSOR_WAIT));
    int sw, sh;
    if((double)SpinCtrl1->GetValue()*px_in_mm*(double)SpinCtrl3->GetValue()/SpinCtrl7->GetValue() > 3656){
        sw = 3656;
        sh = sw * proportion;
        if(sh > 2664){
            sh = 2664;
            sw = sh / proportion;
        }
    }else{
        sw = (double)SpinCtrl1->GetValue()*px_in_mm*(double)SpinCtrl3->GetValue()/SpinCtrl7->GetValue();
        sh = sw * proportion;
        if(sh > 2664){
            sh = 2664;
            sw = sh / proportion;
        }
    }
    wxImage dataImg = mImg->Scale(sw, sh, mQuality);
    char dataBuf[sizeof(GCommand_t) + sizeof(engraverSt_t) + dataImg.GetWidth() * dataImg.GetHeight()];
    GCommand_t *msg = (GCommand_t*)dataBuf;
    msg->GM[0] = 'C';
    msg->num = 7;
    engraverSt_t* engrSt = (engraverSt_t*)msg->arr;
    engrSt->width = dataImg.GetWidth();//wxMessageBox(std::to_string(engrSt->width));return;
    engrSt->height = dataImg.GetHeight();
    int maxSPU = std::max(std::max(SpinCtrl3->GetValue(), SpinCtrl4->GetValue()), SpinCtrl5->GetValue());
    engrSt->fidrate = 30000000000 / (SpinCtrl6->GetValue() * maxSPU);
    //wxMessageBox(std::to_string(engrSt->fidrate)+" "+std::to_string(SpinCtrl8->GetValue()*500/SpinCtrl7->GetValue()));
    engrSt->lasarWidth = SpinCtrl7->GetValue();
    engrSt->pixWidth = ((double)SpinCtrl1->GetValue()*px_in_mm*(double)SpinCtrl3->GetValue()/SpinCtrl7->GetValue()) / sw;
    engrSt->burnF = 30000000000 / (SpinCtrl8->GetValue() * maxSPU);
    size_t i0 = 0;
    unsigned char monochrome = 0;
    for(int i=0; i<dataImg.GetWidth(); i++){
        for(int j=0; j<dataImg.GetHeight(); j++){
            monochrome = (dataImg.GetRed(i, j) + dataImg.GetGreen(i, j) + dataImg.GetBlue(i, j))/3;
            int liting = monochrome+Slider1->GetValue();
            if(liting < 126){
                liting-=Slider2->GetValue();
            }else{
                liting+=Slider2->GetValue();
            }
            if(liting<0){
                monochrome = 0;
            }else if(liting>255){
                monochrome = 255;
            }else{
                monochrome += Slider1->GetValue();
            }
            engrSt->arr[i0] = 255 - monochrome;
            ++i0;
        }
    }
    int fd;
    if((fd = open("/dev/piCNC", O_RDWR)) < 0){
        wxMessageBox("Can't open file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
        return;
    }
    write (fd, msg, sizeof(GCommand_t) + dataImg.GetWidth() * dataImg.GetHeight());
    if (close(fd) < 0) wxMessageBox("Can't close file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
    BitmapButton1->SetBitmap(*pauseBmp);
    BitmapButton1->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &PiCNCEngraverFrame::OnBitmapButton1ClickPause, this);
}

void PiCNCEngraverFrame::OnChoice1Select(wxCommandEvent& event)
{
    switch(Choice1->GetSelection()){
        case 0: mQuality = wxIMAGE_QUALITY_BICUBIC;
                break;
        case 1: mQuality = wxIMAGE_QUALITY_BILINEAR;
                break;
        case 2: mQuality = wxIMAGE_QUALITY_BOX_AVERAGE;
                break;
        case 3: mQuality = wxIMAGE_QUALITY_HIGH;
                break;
        case 4: mQuality = wxIMAGE_QUALITY_NEAREST;
                break;
        case 5: mQuality = wxIMAGE_QUALITY_NORMAL;
                break;
        default:mQuality = wxIMAGE_QUALITY_HIGH;
    }
    this->Refresh();
}

void PiCNCEngraverFrame::OnBitmapButton1ClickPause(wxCommandEvent& event)
{
    char dataBuf[sizeof(GCommand_t)];
    GCommand_t *msg = (GCommand_t*)dataBuf;
    msg->GM[0] = 'C';
    msg->num = 8;
    int fd;
    if((fd = open("/dev/piCNC", O_RDWR)) < 0){
        wxMessageBox("Can't open file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
        return;
    }
    write (fd, msg, sizeof(GCommand_t));
    if (close(fd) < 0) wxMessageBox("Can't close file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
    BitmapButton1->SetBitmap(*MPlay);
    BitmapButton1->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &PiCNCEngraverFrame::OnBitmapButton1ClickReplay, this);
}

void PiCNCEngraverFrame::OnBitmapButton1ClickReplay(wxCommandEvent& event)
{
    char dataBuf[sizeof(GCommand_t)];
    GCommand_t *msg = (GCommand_t*)dataBuf;
    msg->GM[0] = 'C';
    msg->num = 10;
    int fd;
    if((fd = open("/dev/piCNC", O_RDWR)) < 0){
        wxMessageBox("Can't open file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
        return;
    }
    write (fd, msg, sizeof(GCommand_t));
    if (close(fd) < 0) wxMessageBox("Can't close file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
    BitmapButton1->SetBitmap(*pauseBmp);
    BitmapButton1->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &PiCNCEngraverFrame::OnBitmapButton1ClickPause, this);
}

void PiCNCEngraverFrame::OnBitmapButton2Click(wxCommandEvent& event)
{
    char dataBuf[sizeof(GCommand_t)];
    GCommand_t *msg = (GCommand_t*)dataBuf;
    msg->GM[0] = 'C';
    msg->num = 11;
    int fd;
    if((fd = open("/dev/piCNC", O_RDWR)) < 0){
        wxMessageBox("Can't open file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
        return;
    }
    write (fd, msg, sizeof(GCommand_t));
    if (close(fd) < 0) wxMessageBox("Can't close file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
    BitmapButton1->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &PiCNCEngraverFrame::OnBitmapButton1Click, this);
    BitmapButton1->SetBitmap(*MPlay);
}
