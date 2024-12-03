#include "trackeditor.h"
#include <wx/dcbuffer.h>

enum {
    ID_PLAY = 1,
    ID_PAUSE,
    ID_STOP,
    ID_TIMER,
    ID_OPEN_MAIN
};

BEGIN_EVENT_TABLE(TrackTab, wxScrolledWindow)
EVT_PAINT(TrackTab::OnPaint)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TrackEditor, wxFrame)
EVT_BUTTON(ID_PLAY, TrackEditor::OnPlay)
EVT_BUTTON(ID_PAUSE, TrackEditor::OnPause)
EVT_BUTTON(ID_STOP, TrackEditor::OnStop)
EVT_TIMER(ID_TIMER, TrackEditor::OnProgress)
EVT_BUTTON(ID_OPEN_MAIN, TrackEditor::OnOpenMainWindow)
END_EVENT_TABLE()

TrackTab::TrackTab(wxNotebook* parent, const Track& track)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL), track(track) {
    SetScrollbars(1, 1, 1000, 100);
}

void TrackTab::OnPaint(wxPaintEvent& event) {
    wxBufferedPaintDC dc(this);
    DoPrepareDC(dc);
    DrawWaveform(dc);
}

void TrackTab::DrawWaveform(wxDC& dc) {
    int width, height;
    GetClientSize(&width, &height);

    dc.SetPen(wxPen(wxColour(255, 255, 255)));

    size_t samplesPerPixel = std::max(1, static_cast<int>(track.audioData.size() / width));
    int centerY = height / 2;

    for (int x = 0; x < width; ++x) {
        float sample = 0.0f;
        for (size_t i = 0; i < samplesPerPixel; ++i) {
            size_t sampleIndex = x * samplesPerPixel + i;
            if (sampleIndex < track.audioData.size()) {
                sample += track.audioData[sampleIndex];
            }
        }
        sample /= samplesPerPixel;

        int y = static_cast<int>(sample * centerY + centerY);
        dc.DrawLine(x, centerY, x, y);
    }

    int progressX = static_cast<int>(playbackPosition * width / track.audioData.size());
    dc.SetPen(wxPen(progressColor));
    dc.DrawLine(progressX, 0, progressX, height);
}

void TrackTab::UpdateProgress(size_t playbackPosition) {
    this->playbackPosition = playbackPosition;
    Refresh();
    Update();
}

void TrackTab::ResetProgress() {
    playbackPosition = 0;
    Refresh();
    Update();
}

TrackEditor::TrackEditor(DAW* daw, wxFrame* mainWindow)
    : wxFrame(nullptr, wxID_ANY, "Track Editor", wxDefaultPosition, wxSize(800, 600)), daw(daw), mainWindow(mainWindow) {
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    notebook = new wxNotebook(panel, wxID_ANY);
    mainSizer->Add(notebook, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* controlsSizer = new wxBoxSizer(wxHORIZONTAL);

    auto* playButton = new wxButton(panel, ID_PLAY, "Play");
    auto* pauseButton = new wxButton(panel, ID_PAUSE, "Pause");
    auto* stopButton = new wxButton(panel, ID_STOP, "Stop");

    controlsSizer->Add(playButton, 1, wxEXPAND | wxALL, 5);
    controlsSizer->Add(pauseButton, 1, wxEXPAND | wxALL, 5);
    controlsSizer->Add(stopButton, 1, wxEXPAND | wxALL, 5);

    auto* openMainButton = new wxButton(panel, ID_OPEN_MAIN, "Open Main Window");
    controlsSizer->Add(openMainButton, 1, wxEXPAND | wxALL, 5);

    mainSizer->Add(controlsSizer, 0, wxEXPAND | wxALL, 5);

    panel->SetSizer(mainSizer);

    progressTimer = new wxTimer(this, ID_TIMER);

    for (const auto& track : daw->getTracks()) {
        AddTab(track);
    }
}

void TrackEditor::AddTab(const Track& track) {
    TrackTab* tab = new TrackTab(notebook, track);
    notebook->AddPage(tab, wxString::FromUTF8(track.fileName));
}

void TrackEditor::OnPlay(wxCommandEvent& event) {
    daw->play();
    progressTimer->Start(100);
}

void TrackEditor::OnPause(wxCommandEvent& event) {
    daw->stop();
    progressTimer->Stop();
}

void TrackEditor::OnStop(wxCommandEvent& event) {
    daw->stop();
    progressTimer->Stop();
    ResetProgress();
}

void TrackEditor::OnProgress(wxTimerEvent& event) {
    playbackPosition += 100;
    for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
        auto* tab = dynamic_cast<TrackTab*>(notebook->GetPage(i));
        if (tab) {
            tab->UpdateProgress(playbackPosition);
        }
    }

    if (playbackPosition >= daw->getTracks()[0].audioData.size() / (daw->getTracks()[0].sampleRate * daw->getTracks()[0].channels)) {
        progressTimer->Stop();
        ResetProgress();
    }
}

void TrackEditor::OnOpenMainWindow(wxCommandEvent& event) {
    mainWindow->Show(true);
    this->Hide();
}

void TrackEditor::ResetProgress() {
    playbackPosition = 0;

    for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
        auto* tab = dynamic_cast<TrackTab*>(notebook->GetPage(i));
        if (tab) {
            tab->ResetProgress();
        }
    }

    // OLD
    //if (positionSlider) {
    //    positionSlider->SetValue(0);
    //}
}
