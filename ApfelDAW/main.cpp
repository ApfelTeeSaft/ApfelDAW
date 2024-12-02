#include <wx/wx.h>
#include "gui.h"

class ApfelDAWApp : public wxApp {
public:
    virtual bool OnInit() {
        GUI* gui = new GUI("Apfel DAW");
        gui->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(ApfelDAWApp);