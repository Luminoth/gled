#if !defined __CANVAS_H__
#define __CANVAS_H__

#include <wx/glcanvas.h>

class wxEraseEvent;
class wxGLContext;
class wxKeyEvent;
class wxMouseEvent;
class wxSizeEvent;
class wxWindow;

class Canvas : public wxGLCanvas
{
public:
    // NOTE: these need to be updated whenever
    // attributes are added to/removed from the array
    enum Attribs
    {
        AttribColorPalette = 0,
        AttribDoubleBuffer = 1,
        AttribColorDepth = 3,
        /*AttribSampleBuffers = 5,
        AttribSamples = 7,*/
        AttribDepthSize = 5,
        AttribStencilSize = 7,
    };
    static int ATTRIB_LIST[];

private:
    static energonsoftware::Logger& logger;

public:
    virtual ~Canvas() throw();

private:
    void OnEraseBackground(wxEraseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);

    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent& event);
    void OnMouseLeftDoubleClick(wxMouseEvent& event);

    void OnMouseMiddleDown(wxMouseEvent& event);
    void OnMouseMiddleUp(wxMouseEvent& event);
    void OnMouseMiddleDoubleClick(wxMouseEvent& event);

    void OnMouseRightDown(wxMouseEvent& event);
    void OnMouseRightUp(wxMouseEvent& event);
    void OnMouseRightDoubleClick(wxMouseEvent& event);

    void OnMouseMotion(wxMouseEvent& event);
    void OnMouseEnterWindow(wxMouseEvent& event);
    void OnMouseLeaveWindow(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);

    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);

private:
    void handle_key_event(const wxKeyEvent& event, bool pressed);

private:
    friend class MainFrame;
    DECLARE_EVENT_TABLE()

private:
    wxGLContext* _context;
    bool _record_mouse;

private:
    Canvas();
    explicit Canvas(wxWindow* parent, int attribs[]);
    DISALLOW_COPY_AND_ASSIGN(Canvas);
};

#endif
