#ifndef TRACKEDITOR_H
#define TRACKEDITOR_H

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/scrolwin.h>
#include <wx/timer.h>
#include "daw.h"

class TrackTab : public wxScrolledWindow {
public:
    TrackTab(wxNotebook* parent, const Track& track);

    void UpdateProgress(size_t playbackPosition);
    void ResetProgress();

private:
    void DrawWaveform(wxDC& dc);
    void OnPaint(wxPaintEvent& event);

    Track track;
    size_t playbackPosition = 0;
    wxColour progressColor = wxColour(0, 255, 0);

    DECLARE_EVENT_TABLE()
};

class TrackEditor : public wxFrame {
public:
    TrackEditor(DAW* daw, wxFrame* mainWindow);

private:
    DAW* daw;
    wxNotebook* notebook;
    wxTimer* progressTimer;

    wxFrame* mainWindow;
    size_t playbackPosition = 0;

    void AddTab(const Track& track);
    void OnPlay(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnProgress(wxTimerEvent& event);
    void OnOpenMainWindow(wxCommandEvent& event);

    void ResetProgress();

    DECLARE_EVENT_TABLE()
};

#endif