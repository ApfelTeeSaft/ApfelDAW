#ifndef DND_H
#define DND_H

#include <wx/dnd.h>
#include <wx/wx.h>
#include "daw.h"

class TrackDropTarget : public wxFileDropTarget {
public:
    TrackDropTarget(DAW* daw, wxListBox* trackList);
    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) override;

private:
    DAW* daw;
    wxListBox* trackList;
};

#endif