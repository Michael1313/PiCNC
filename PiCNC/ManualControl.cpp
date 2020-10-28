#include "ManualControl.h"
#include <wx/msgdlg.h>
#include <chrono>
#include <thread>

//(*InternalHeaders(ManualControl)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(ManualControl)
const long ManualControl::ID_SPINCTRL1 = wxNewId();
const long ManualControl::ID_SPINCTRL2 = wxNewId();
const long ManualControl::ID_STATICTEXT1 = wxNewId();
const long ManualControl::ID_STATICTEXT2 = wxNewId();
const long ManualControl::ID_STATICTEXT3 = wxNewId();
const long ManualControl::ID_STATICTEXT4 = wxNewId();
const long ManualControl::ID_STATICTEXT5 = wxNewId();
const long ManualControl::ID_STATICTEXT6 = wxNewId();
const long ManualControl::ID_SPINCTRL3 = wxNewId();
const long ManualControl::ID_TEXTCTRL1 = wxNewId();
const long ManualControl::ID_SPINBUTTON1 = wxNewId();
const long ManualControl::ID_TEXTCTRL2 = wxNewId();
const long ManualControl::ID_SPINBUTTON2 = wxNewId();
const long ManualControl::ID_TEXTCTRL3 = wxNewId();
const long ManualControl::ID_SPINBUTTON3 = wxNewId();
const long ManualControl::ID_SPINCTRL4 = wxNewId();
const long ManualControl::ID_BUTTON1 = wxNewId();
const long ManualControl::ID_BUTTON2 = wxNewId();
const long ManualControl::ID_BUTTON3 = wxNewId();
const long ManualControl::ID_BUTTON4 = wxNewId();
const long ManualControl::ID_STATICTEXT7 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ManualControl,wxDialog)
	//(*EventTable(ManualControl)
	//*)
END_EVENT_TABLE()

ManualControl::ManualControl(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(ManualControl)
	Create(parent, id, _("ManualControl"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxSize(354,324));
	SpinCtrl1 = new wxSpinCtrl(this, ID_SPINCTRL1, _T("100"), wxPoint(184,32), wxSize(144,33), 0, 1, 10000, 100, _T("ID_SPINCTRL1"));
	SpinCtrl1->SetValue(_T("100"));
	SpinCtrl2 = new wxSpinCtrl(this, ID_SPINCTRL2, _T("100"), wxPoint(184,64), wxSize(144,33), 0, 1, 10000, 100, _T("ID_SPINCTRL2"));
	SpinCtrl2->SetValue(_T("100"));
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Go to"), wxPoint(32,8), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("X"), wxPoint(8,40), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Y"), wxPoint(8,72), wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Z"), wxPoint(8,104), wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("F"), wxPoint(8,136), wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Steps per unit (SPU)"), wxPoint(184,8), wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	SpinCtrl3 = new wxSpinCtrl(this, ID_SPINCTRL3, _T("100"), wxPoint(184,96), wxSize(144,33), 0, 1, 10000, 100, _T("ID_SPINCTRL3"));
	SpinCtrl3->SetValue(_T("100"));
	TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, _("0.000000"), wxPoint(32,32), wxSize(131,33), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	SpinButton1 = new wxSpinButton(this, ID_SPINBUTTON1, wxPoint(152,32), wxDefaultSize, wxSP_VERTICAL|wxSP_ARROW_KEYS|wxSP_WRAP, _T("ID_SPINBUTTON1"));
	SpinButton1->SetRange(0, 100);
	TextCtrl2 = new wxTextCtrl(this, ID_TEXTCTRL2, _("0.000000"), wxPoint(32,64), wxSize(131,33), 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	SpinButton2 = new wxSpinButton(this, ID_SPINBUTTON2, wxPoint(152,64), wxDefaultSize, wxSP_VERTICAL|wxSP_ARROW_KEYS|wxSP_WRAP, _T("ID_SPINBUTTON2"));
	SpinButton2->SetRange(-100, 100);
	TextCtrl3 = new wxTextCtrl(this, ID_TEXTCTRL3, _("0.000000"), wxPoint(32,96), wxSize(131,33), 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	SpinButton3 = new wxSpinButton(this, ID_SPINBUTTON3, wxPoint(152,96), wxDefaultSize, wxSP_VERTICAL|wxSP_ARROW_KEYS|wxSP_WRAP, _T("ID_SPINBUTTON3"));
	SpinButton3->SetRange(-100, 100);
	SpinCtrl4 = new wxSpinCtrl(this, ID_SPINCTRL4, _T("100"), wxPoint(32,128), wxSize(140,33), 0, 1, 10000, 100, _T("ID_SPINCTRL4"));
	SpinCtrl4->SetValue(_T("100"));
	Butt1 = new wxButton(this, ID_BUTTON1, _("Go"), wxPoint(32,168), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	Butt2 = new wxButton(this, ID_BUTTON2, _("SetSPU&&Close"), wxPoint(192,168), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	Butt3 = new wxButton(this, ID_BUTTON3, _("Close"), wxPoint(192,208), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	Butt4 = new wxButton(this, ID_BUTTON4, _("Parking"), wxPoint(32,208), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
	StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _("Click on this caption and press:\nUp, Down, Left, Right, W, S, Space\nTo control the machine."), wxPoint(16,248), wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	Center();

	Connect(ID_SPINBUTTON1,wxEVT_SCROLL_LINEUP,(wxObjectEventFunction)&ManualControl::OnSpinButton1ChangeUp);
	Connect(ID_SPINBUTTON1,wxEVT_SCROLL_LINEDOWN,(wxObjectEventFunction)&ManualControl::OnSpinButton1ChangeDown);
	Connect(ID_SPINBUTTON2,wxEVT_SCROLL_LINEUP,(wxObjectEventFunction)&ManualControl::OnSpinButton2ChangeUp);
	Connect(ID_SPINBUTTON2,wxEVT_SCROLL_LINEDOWN,(wxObjectEventFunction)&ManualControl::OnSpinButton2ChangeDown);
	Connect(ID_SPINBUTTON3,wxEVT_SCROLL_LINEUP,(wxObjectEventFunction)&ManualControl::OnSpinButton3ChangeUp);
	Connect(ID_SPINBUTTON3,wxEVT_SCROLL_LINEDOWN,(wxObjectEventFunction)&ManualControl::OnSpinButton3ChangeDown);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ManualControl::OnButt1Click);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ManualControl::OnButton2Click);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ManualControl::OnButton3Click);
	Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ManualControl::OnButt4Click);
	Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&ManualControl::OnInit);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&ManualControl::OnClose);
	Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&ManualControl::OnKeyDown);
	Connect(wxEVT_KEY_UP,(wxObjectEventFunction)&ManualControl::OnKeyUp);
	Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&ManualControl::OnLeftDown);
	//*)
	Butt1->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&ManualControl::OnKeyDown,0,this);
	Butt2->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&ManualControl::OnKeyDown,0,this);
	Butt3->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&ManualControl::OnKeyDown,0,this);
	Butt4->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&ManualControl::OnKeyDown,0,this);
	Butt1->Bind(wxEVT_KEY_UP, &ManualControl::OnKeyUp, this);
	Butt2->Bind(wxEVT_KEY_UP, &ManualControl::OnKeyUp, this);
	Butt3->Bind(wxEVT_KEY_UP, &ManualControl::OnKeyUp, this);
	Butt4->Bind(wxEVT_KEY_UP, &ManualControl::OnKeyUp, this);
	TextCtrl1->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&ManualControl::OnTextCtrl1KeyDown,0,this);
	TextCtrl2->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&ManualControl::OnTextCtrl2KeyDown,0,this);
	TextCtrl3->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&ManualControl::OnTextCtrl3KeyDown,0,this);
	TextCtrl1->Connect(wxEVT_KEY_UP,(wxObjectEventFunction)&ManualControl::OnTextCtrlKeyUp,0,this);
	TextCtrl2->Connect(wxEVT_KEY_UP,(wxObjectEventFunction)&ManualControl::OnTextCtrlKeyUp,0,this);
	TextCtrl3->Connect(wxEVT_KEY_UP,(wxObjectEventFunction)&ManualControl::OnTextCtrlKeyUp,0,this);
	TextCtrl1->Connect(wxEVT_KILL_FOCUS,(wxObjectEventFunction)&ManualControl::OnTextCtrl1KillFocus,0,this);
	TextCtrl2->Connect(wxEVT_KILL_FOCUS,(wxObjectEventFunction)&ManualControl::OnTextCtrl2KillFocus,0,this);
	TextCtrl3->Connect(wxEVT_KILL_FOCUS,(wxObjectEventFunction)&ManualControl::OnTextCtrl3KillFocus,0,this);
	SpinButton1->Connect(wxEVT_SET_FOCUS,(wxObjectEventFunction)&ManualControl::OnSpinButton1SetFocus,0,this);
	SpinButton2->Connect(wxEVT_SET_FOCUS,(wxObjectEventFunction)&ManualControl::OnSpinButton2SetFocus,0,this);
	SpinButton3->Connect(wxEVT_SET_FOCUS,(wxObjectEventFunction)&ManualControl::OnSpinButton3SetFocus,0,this);
//	SpinButton1->Bind(wxEVT_IDLE, &ManualControl::OnSpinButton1SetFocus, this);
	StaticText7->Bind(wxEVT_LEFT_DOWN, &ManualControl::OnLeftDown, this);
}

ManualControl::~ManualControl()
{
	//(*Destroy(ManualControl)
	//*)
}

void incVal(wxTextCtrl *mCtrl){
    double xVal;
    mCtrl->GetValue().ToDouble(&xVal);
    mCtrl->SetValue(std::to_string(++xVal));
}

void decVal(wxTextCtrl *mCtrl){
    double xVal;
    mCtrl->GetValue().ToDouble(&xVal);
    mCtrl->SetValue(std::to_string(--xVal));
}

bool keyCtrl = false;
void ManualControl::tKeyDown(wxTextCtrl *TC, wxKeyEvent& evt)
{
    if (evt.GetKeyCode() == 315){
        incVal(TC);
        evt.Skip(false);
    }else if (evt.GetKeyCode() == 317){
        decVal(TC);
        evt.Skip(false);
    }else if (evt.GetKeyCode() == 308) {
        keyCtrl = true;
        evt.Skip(true);
    }else if (evt.GetUnicodeKey() == 65){
        keyCtrl? evt.Skip(true) : evt.Skip(false);
    }else if (evt.GetUnicodeKey() == 0 || evt.GetUnicodeKey() == 8 || evt.GetUnicodeKey() == 9 || evt.GetUnicodeKey() == 127) {
        evt.Skip(true);
    }else if(evt.GetUnicodeKey() == 46){
        long int mStart, mEnd;
        TC->GetSelection(&mStart, &mEnd);
        (TC->GetValue().find(".")==4294967295 && (mStart!=0 || mEnd!=0))? evt.Skip(true) : evt.Skip(false);
    }else if(evt.GetUnicodeKey() == 45){
        long int mStart, mEnd;
        TC->GetSelection(&mStart, &mEnd);
        (TC->GetValue().find("-")==4294967295 && (mStart==0 || mEnd==0))? evt.Skip(true) : evt.Skip(false);
    }else if(evt.GetKeyCode() == 13){
        GoToPos();
    }else if(evt.GetUnicodeKey() < 48 || evt.GetUnicodeKey() > 57){
        evt.Skip(false);
    }else{
        evt.Skip(true);
    }
}

void ManualControl::OnButton2Click(wxCommandEvent& event)
{
    mAccept = true;
    Close();
}

void ManualControl::OnButton3Click(wxCommandEvent& event)
{
    Close();
}

void ManualControl::OnSpinButton1ChangeUp(wxSpinEvent& event)
{
    incVal(TextCtrl1);
    TextCtrl1->SetFocus();
/** GetObject
    wxTextCtrl *object = (wxTextCtrl*)event.GetEventObject();
**/
}

void ManualControl::OnSpinButton1ChangeDown(wxSpinEvent& event)
{
    decVal(TextCtrl1);
    TextCtrl1->SetFocus();
}

void ManualControl::OnSpinButton2ChangeUp(wxSpinEvent& event)
{
    incVal(TextCtrl2);
    TextCtrl2->SetFocus();
}

void ManualControl::OnSpinButton2ChangeDown(wxSpinEvent& event)
{
    decVal(TextCtrl2);
    TextCtrl2->SetFocus();
}

void ManualControl::OnSpinButton3ChangeUp(wxSpinEvent& event)
{
    incVal(TextCtrl3);
    TextCtrl3->SetFocus();
}

void ManualControl::OnSpinButton3ChangeDown(wxSpinEvent& event)
{
    decVal(TextCtrl3);
    TextCtrl3->SetFocus();
}

void ManualControl::OnTextCtrl1KeyDown(wxKeyEvent& event)
{
    tKeyDown(TextCtrl1, event);
}

void ManualControl::OnTextCtrl2KeyDown(wxKeyEvent& event)
{
    tKeyDown(TextCtrl2, event);
}

void ManualControl::OnTextCtrl3KeyDown(wxKeyEvent& event)
{
    tKeyDown(TextCtrl3, event);
}

void ManualControl::OnTextCtrlKeyUp(wxKeyEvent& event){
    if(event.GetKeyCode() == 308) keyCtrl = false;
}

void ManualControl::OnTextCtrl1KillFocus(wxFocusEvent& event){
    double tempVal;
    TextCtrl1->GetValue().ToDouble(&tempVal);
    TextCtrl1->SetValue(std::to_string(tempVal));
}

void ManualControl::OnTextCtrl2KillFocus(wxFocusEvent& event){
    double tempVal;
    TextCtrl2->GetValue().ToDouble(&tempVal);
    TextCtrl2->SetValue(std::to_string(tempVal));
}

void ManualControl::OnTextCtrl3KillFocus(wxFocusEvent& event){
    double tempVal;
    TextCtrl3->GetValue().ToDouble(&tempVal);
    TextCtrl3->SetValue(std::to_string(tempVal));
}

void ManualControl::OnSpinButton1SetFocus(wxFocusEvent& event)
{
    TextCtrl2->SetFocus();
    TextCtrl2->SetSelection(-1, -1);
}

void ManualControl::OnSpinButton2SetFocus(wxFocusEvent& event)
{
    TextCtrl3->SetFocus();
    TextCtrl3->SetSelection(-1, -1);
}

void ManualControl::OnSpinButton3SetFocus(wxFocusEvent& event)
{
    SpinCtrl4->SetFocus();
    SpinCtrl4->SetSelection(-1, -1);
}

typedef struct GCommand {
    signed char GM[1];
    unsigned char num;
    unsigned char arr[];
} GCommand_t;

typedef struct G01 {
    long int x;
    long int y;
    long int z;
    unsigned long F;
    unsigned long acseleration;
} G01_t;

typedef struct C03 {
    size_t N;
    long int x;
    long int y;
    long int z;
    unsigned long int dx;
    unsigned long int dy;
    unsigned long int dz;
} C03_t;

typedef struct PXYZ{
    int x;
    int y;
    int z;
}PXYZ_t;
char posBuf[sizeof(PXYZ_t)];
PXYZ_t *dat = (PXYZ_t*)posBuf;

int fd;
int32_t keyComand[3] = {0, 0, 0};
void ManualControl::read_pos(){
    read(fd, posBuf, sizeof(PXYZ_t));
    set_x_pos(dat->x);
    set_y_pos(dat->y);
    set_z_pos(dat->z);
}

void ManualControl::OnKeyDown(wxKeyEvent& event)
{
    bool RP = false;
    uint16_t maxSPU = std::max(std::max(SpinCtrl1->GetValue(), SpinCtrl2->GetValue()), SpinCtrl3->GetValue());
    char dataBuf[sizeof(GCommand_t)+sizeof(G01_t)];
    GCommand_t *msg = (GCommand_t*)dataBuf;
    G01_t *G01msg = (G01_t*)(msg->arr);
    msg->GM[0] = 'C';
    msg->num = 1;
    G01msg->x = keyComand[0];
    G01msg->y = keyComand[1];
    G01msg->z = keyComand[2];
    G01msg->F = 30000000000 / (SpinCtrl4->GetValue() * maxSPU);
    G01msg->acseleration = SpinCtrl4->GetValue() * maxSPU / 100;
   // StaticText1->SetLabel(std::to_string(G01msg->F));
    if(event.GetKeyCode() == 314){
        if(keyComand[0] != -2147483648){
            keyComand[0] = -2147483648;
            G01msg->x = keyComand[0];
            write (fd, dataBuf, sizeof(GCommand_t)+sizeof(G01_t));
        }else{
            RP = true;
        }
        event.Skip(false);
    } else if(event.GetKeyCode() == 315) {
            if(keyComand[1] != 2147483647){
            keyComand[1] = 2147483647;
            G01msg->y = keyComand[1];
            write (fd, dataBuf, sizeof(GCommand_t)+sizeof(G01_t));
        }else{
            RP = true;
        }
        event.Skip(false);
    } else if(event.GetKeyCode() == 316) {
        if(keyComand[0] != 2147483647){
            keyComand[0] = 2147483647;
            G01msg->x = keyComand[0];
            write (fd, dataBuf, sizeof(GCommand_t)+sizeof(G01_t));
        }else{
            RP = true;
        }
        event.Skip(false);
    } else if(event.GetKeyCode() == 317) {
        if(keyComand[1] != -2147483648){
            keyComand[1] = -2147483648;
            G01msg->y = keyComand[1];
            write (fd, dataBuf, sizeof(GCommand_t)+sizeof(G01_t));
        }else{
            RP = true;
        }
        event.Skip(false);
    } else if(event.GetKeyCode() == 87) {
        if(keyComand[2] != -2147483648){
            keyComand[2] = -2147483648;
            G01msg->z = keyComand[2];
            write (fd, dataBuf, sizeof(GCommand_t)+sizeof(G01_t));
        }else{
            RP = true;
        }
        event.Skip(false);
    } else if(event.GetKeyCode() == 83) {
        if(keyComand[2] != 2147483647){
            keyComand[2] = 2147483647;
            G01msg->z = keyComand[2];
            write (fd, dataBuf, sizeof(GCommand_t)+sizeof(G01_t));
        }else{
            RP = true;
        }
        event.Skip(false);
    } else if(event.GetKeyCode() == 32){
        event.Skip(false);
    } else {
        event.Skip(true);
    }
    if(RP) read_pos();
/*    uint8_t dataBuf[sizeof(GCommand_t)*2];
    GCommand_t *msg = (GCommand_t*)dataBuf;
    msg->GM[0] = 'G';
    (msg+1)->GM[0] = '%';
*/
}

void ManualControl::OnKeyUp(wxKeyEvent& event)
{
    char stBuf[1] = {'%'};
    if(event.GetKeyCode() == 314 || event.GetKeyCode() == 316){
        keyComand[0] = 0;
    } else if(event.GetKeyCode() == 315 || event.GetKeyCode() == 317) {
        keyComand[1] = 0;
    } else if(event.GetKeyCode() == 87 || event.GetKeyCode() == 83) {
        keyComand[2] = 0;
    }

    if(event.GetKeyCode() == 314 || event.GetKeyCode() == 315 || event.GetKeyCode() == 316 || event.GetKeyCode() == 317 || event.GetKeyCode() == 87 || event.GetKeyCode() == 83){
        uint16_t maxSPU = std::max(std::max(SpinCtrl1->GetValue(), SpinCtrl2->GetValue()), SpinCtrl3->GetValue());
        char dataBuf[sizeof(GCommand_t)+sizeof(G01_t)];
        GCommand_t *msg = (GCommand_t*)dataBuf;
        G01_t *G01msg = (G01_t*)(msg->arr);
        msg->GM[0] = 'C';
        msg->num = 1;
        G01msg->x = keyComand[0];
        G01msg->y = keyComand[1];
        G01msg->z = keyComand[2];
        G01msg->F = 30000000000 / (SpinCtrl4->GetValue() * maxSPU);
        write (fd, dataBuf, sizeof(GCommand_t)+sizeof(G01_t));
        std::this_thread::sleep_for(std::chrono::nanoseconds(150000000));
        read_pos();
        event.Skip(false);
    } else if(event.GetKeyCode() == 32){
        char dataBuf[sizeof(GCommand_t)];
        GCommand_t *msg = (GCommand_t*)dataBuf;
        msg->GM[0] = 'C';
        msg->num = 12;
        write (fd, stBuf, 2);
        write (fd, dataBuf, sizeof(GCommand_t));
        event.Skip(false);
    } else {
        event.Skip(true);
    }
}

ManualControl *mObj;
void signalXFunct(int n, siginfo_t *info, void *unused){
    mObj->set_x_pos(info->si_int);
}

void signalYFunct(int n, siginfo_t *info, void *unused){
    mObj->set_y_pos(info->si_int);
}

void signalZFunct(int n, siginfo_t *info, void *unused){
    mObj->set_z_pos(info->si_int);
}

void ManualControl::OnInit(wxInitDialogEvent& event)
{
    if((fd = open("/dev/piCNC", O_RDWR)) < 0){
        wxMessageBox("Can't open file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
    }
//    args.pid = getpid();
    mObj = this;
/*    struct sigaction sigX, sigY, sigZ;
    sigX.sa_sigaction = signalXFunct;
    sigX.sa_flags = SA_SIGINFO;
    sigaction(SIGALRM, &sigX, NULL);*/
}

void ManualControl::OnClose(wxCloseEvent& event)
{
    if (close(fd) < 0) wxMessageBox("Can't close file /dev/piCNC fd = " + std::to_string(fd), _("PiCNC"));
    event.Skip(true);
}

void ManualControl::OnLeftDown(wxMouseEvent& event)
{
    Butt1->SetFocus();
}

void ManualControl::GoToPos(){
    uint16_t maxSPU = std::max(std::max(SpinCtrl1->GetValue(), SpinCtrl2->GetValue()), SpinCtrl3->GetValue());
    char dataBuf[sizeof(GCommand_t)+sizeof(C03_t)];
    GCommand_t *msg = (GCommand_t*)dataBuf;
    msg->GM[0] = 'C';

    msg->num = 2;
    unsigned long *F = (unsigned long*)(msg->arr);
    *F = 30000000000 / (SpinCtrl4->GetValue() * maxSPU);
    write (fd, dataBuf, sizeof(GCommand_t)+4);

    C03_t *C03msg = (C03_t*)(msg->arr);
    msg->num = 3;
    double GX;
    TextCtrl1->GetValue().ToDouble(&GX);
    C03msg->x = (GX - ActPos.x) * SpinCtrl1->GetValue();
    double GY;
    TextCtrl2->GetValue().ToDouble(&GY);
    C03msg->y = (GY - ActPos.y) * SpinCtrl2->GetValue();
    double GZ;
    TextCtrl3->GetValue().ToDouble(&GZ);
    C03msg->z = (GZ - ActPos.z) * SpinCtrl3->GetValue();

    double MaxDist = std::max(std::max(std::abs(C03msg->x), std::abs(C03msg->y)), std::abs(C03msg->z));
    if(MaxDist != 0){
        C03msg->dx = std::abs((double(C03msg->x) / double(MaxDist)) * 1048576);
        C03msg->dy = std::abs((double(C03msg->y) / double(MaxDist)) * 1048576);
        C03msg->dz = std::abs((double(C03msg->z) / double(MaxDist)) * 1048576);//(1<<20) 1048576
        write (fd, dataBuf, sizeof(GCommand_t)+sizeof(C03_t));
    }

    ActPos.x = GX;
    ActPos.y = GY;
    ActPos.z = GZ;
}

void ManualControl::OnButt1Click(wxCommandEvent& event)
{
    GoToPos();
}

void ManualControl::OnButt4Click(wxCommandEvent& event)
{
    uint16_t maxSPU = std::max(std::max(SpinCtrl1->GetValue(), SpinCtrl2->GetValue()), SpinCtrl3->GetValue());
    char dataBuf[sizeof(GCommand_t)+sizeof(unsigned long)];
    GCommand_t *msg = (GCommand_t*)dataBuf;
    msg->GM[0] = 'C';

    msg->num = 2;
    unsigned long *F = (unsigned long*)(msg->arr);
    *F = 30000000000 / (SpinCtrl4->GetValue() * maxSPU);
    write (fd, dataBuf, sizeof(GCommand_t)+4);

    msg->num = 28;
    msg->arr[0] = 0b0111;
    write (fd, dataBuf, sizeof(GCommand_t)+1);

    TextCtrl1->SetValue("0.000000");
    TextCtrl2->SetValue("0.000000");
    TextCtrl3->SetValue("0.000000");
    ActPos.x = 0;
    ActPos.y = 0;
    ActPos.z = 0;
}
