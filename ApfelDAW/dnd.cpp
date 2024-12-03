#include "dnd.h"

TrackDropTarget::TrackDropTarget(DAW* daw, wxListBox* trackList)
    : daw(daw), trackList(trackList) {}

bool TrackDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) {
    for (const auto& file : filenames) {
        try {
            daw->addTrack(file.ToStdString());
            trackList->Append(file);
        }
        catch (const std::exception& e) {
            wxMessageBox(e.what(), "Error", wxICON_ERROR);
        }
    }
    return true;
}