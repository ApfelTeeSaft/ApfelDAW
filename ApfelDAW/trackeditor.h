#ifndef TRACKEDITOR_H
#define TRACKEDITOR_H

#include <wx/wx.h>
#include <wx/dnd.h>
#include "daw.h"

class FileDropTarget : public wxFileDropTarget {
public:
    FileDropTarget(wxListBox* listBox, DAW* daw);
    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& files) override;

private:
    wxListBox* trackList;
    DAW* daw;
};

class TrackEditor : public wxFrame {
public:
    TrackEditor(DAW* daw);

private:
    DAW* daw;
    wxListBox* trackList;

    void OnPlay(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
};

#endif