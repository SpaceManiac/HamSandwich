#include "editor.h"
#include "globals.h"
#include "jspfile.h"
#include <iostream>

using namespace std;

namespace editor {

/****************************************************************/
/* Editor class */

struct Editor : public GameScreen {

    JspFile jsp;

    Editor() : GameScreen(display) {}

    void load(std::string fname);


    void render();
};


void Editor::load(string fname) {
    cout << "Editor::load " << fname << endl;
    if (jsp.load(fname)) {
        cout << "Success" << endl;
    } else {
        cout << "Failure: " << jsp.error << endl;
    }
}

void Editor::render() {
    al_clear_to_color(Color(0, 0, 0));

    int x = 0, y = 0, h = 0;
    for (size_t i = 0; i < jsp.frames.size(); ++i) {
        JspFrame frame = jsp.frames[i];
        if (x + frame.bmp.getWidth() > DISPLAY_WIDTH) {
            y += h;
            x = h = 0;
        }
        if (y >= DISPLAY_HEIGHT) {
            break;
        }
        frame.bmp.draw(x, y);
        x += frame.bmp.getWidth();
        if (frame.bmp.getHeight() > h) {
            h = frame.bmp.getHeight();
        }
    }

    al_flip_display();
}


/****************************************************************/
/* Startup code */

const char* start_fname = nullptr;

void loadOnStartup(const char* fname) {
    start_fname = fname;
    cout << "loadOnStartup " << fname << endl;
}

void main() {
    Editor ed = Editor();
    if (start_fname) {
        ed.load(start_fname);
    }
    ed.run();
}

} // namespace editor
