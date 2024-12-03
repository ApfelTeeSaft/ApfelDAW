#include "gui.h"
#include "trackeditor.h"

enum {
    ID_PLAY_PAUSE = 1
};

BEGIN_EVENT_TABLE(GUI, wxFrame)
EVT_BUTTON(ID_PLAY_PAUSE, GUI::OnPlayPause)
EVT_KEY_DOWN(GUI::OnKeyDown)
END_EVENT_TABLE()

GUI::GUI(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(600, 400)) {
    auto* panel = new wxPanel(this, wxID_ANY);
    auto* vbox = new wxBoxSizer(wxVERTICAL);

    trackList = new wxListBox(panel, wxID_ANY);
    vbox->Add(trackList, 1, wxEXPAND | wxALL, 10);

    auto* hbox = new wxBoxSizer(wxHORIZONTAL);
    auto* addBtn = new wxButton(panel, wxID_ANY, wxT("Add Track"));
    auto* removeBtn = new wxButton(panel, wxID_ANY, wxT("Remove Track"));
    auto* playPauseBtn = new wxButton(panel, ID_PLAY_PAUSE, wxT("Play/Pause"));
    auto* trackEditorBtn = new wxButton(panel, wxID_ANY, "Open Track Editor");

    hbox->Add(trackEditorBtn, 1, wxEXPAND | wxALL, 5);
    hbox->Add(addBtn, 1, wxEXPAND | wxALL, 5);
    hbox->Add(removeBtn, 1, wxEXPAND | wxALL, 5);
    hbox->Add(playPauseBtn, 1, wxEXPAND | wxALL, 5);

    vbox->Add(hbox, 0, wxALIGN_CENTER);
    panel->SetSizer(vbox);

    Connect(addBtn->GetId(), wxEVT_BUTTON, wxCommandEventHandler(GUI::OnAddTrack));
    Connect(removeBtn->GetId(), wxEVT_BUTTON, wxCommandEventHandler(GUI::OnRemoveTrack));
    trackEditorBtn->Bind(wxEVT_BUTTON, &GUI::OnOpenTrackEditor, this);
}

void GUI::OnOpenTrackEditor(wxCommandEvent& event) {
    auto* editor = new TrackEditor(&daw, this);
    editor->Show(true);
    this->Hide();
}


void GUI::OnAddTrack(wxCommandEvent& event) {
    wxFileDialog openFileDialog(this, wxT("Select Audio File"), "", "",
        "Audio Files (*.wav;*.mp3;*.flac)|*.wav;*.mp3;*.flac",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL) return;

    daw.addTrack(openFileDialog.GetPath().ToStdString());
    UpdateTrackList();
}

void GUI::OnRemoveTrack(wxCommandEvent& event) {
    int selection = trackList->GetSelection();
    if (selection != wxNOT_FOUND) {
        daw.removeTrack(selection);
        UpdateTrackList();
    }
}

void GUI::OnPlayPause(wxCommandEvent& event) {
    if (daw.isPlaying()) {
        daw.stop();
    }
    else {
        daw.play();
    }
}

void GUI::OnKeyDown(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_SPACE) {
        wxCommandEvent dummyEvent;
        OnPlayPause(dummyEvent);
    }
    event.Skip();
}

void GUI::UpdateTrackList() {
    trackList->Clear();
    for (const auto& track : daw.getTracks()) {
        trackList->Append(wxString::FromUTF8(track.fileName));
    }
}