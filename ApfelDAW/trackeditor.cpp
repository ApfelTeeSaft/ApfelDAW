#include "trackeditor.h"

FileDropTarget::FileDropTarget(wxListBox* listBox, DAW* daw)
    : trackList(listBox), daw(daw) {}

bool FileDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& files) {
    for (const auto& file : files) {
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

TrackEditor::TrackEditor(DAW* daw)
    : wxFrame(nullptr, wxID_ANY, "Track Editor", wxDefaultPosition, wxSize(600, 400)), daw(daw) {
    auto* panel = new wxPanel(this, wxID_ANY);
    auto* vbox = new wxBoxSizer(wxVERTICAL);

    trackList = new wxListBox(panel, wxID_ANY);
    auto* dropTarget = new FileDropTarget(trackList, daw);
    trackList->SetDropTarget(dropTarget);

    vbox->Add(trackList, 1, wxEXPAND | wxALL, 10);

    auto* hbox = new wxBoxSizer(wxHORIZONTAL);
    auto* playBtn = new wxButton(panel, wxID_ANY, "Play All");
    auto* stopBtn = new wxButton(panel, wxID_ANY, "Stop");

    hbox->Add(playBtn, 1, wxEXPAND | wxALL, 5);
    hbox->Add(stopBtn, 1, wxEXPAND | wxALL, 5);

    vbox->Add(hbox, 0, wxALIGN_CENTER);
    panel->SetSizer(vbox);

    Connect(playBtn->GetId(), wxEVT_BUTTON, wxCommandEventHandler(TrackEditor::OnPlay));
    Connect(stopBtn->GetId(), wxEVT_BUTTON, wxCommandEventHandler(TrackEditor::OnStop));
}

void TrackEditor::OnPlay(wxCommandEvent& event) {
    daw->playAll();
}

void TrackEditor::OnStop(wxCommandEvent& event) {
    daw->stop();
}