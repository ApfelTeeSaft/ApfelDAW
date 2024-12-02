#ifndef GUI_H
#define GUI_H

#include <wx/wx.h>
#include <wx/dnd.h>
#include "daw.h"

class GUI : public wxFrame {
public:
    GUI(const wxString& title);

private:
    DAW daw;
    wxListBox* trackList;

    void OnAddTrack(wxCommandEvent& event);
    void OnRemoveTrack(wxCommandEvent& event);
    void OnPlayPause(wxCommandEvent& event);
    void OnOpenTrackEditor(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    void UpdateTrackList();

    DECLARE_EVENT_TABLE()
};

#endif