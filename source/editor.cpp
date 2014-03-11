#include "editor.h"
#include "globals.h"
#include "jspfile.h"
#include "fontawesome.h"
#include <iostream>

using namespace std;

namespace editor {

/****************************************************************/
/* Editor class */

struct FileInfo {
    string fname, shortname;
    JspFile jsp;
    int curSprite;
};

struct Editor : public GameScreen {

    FileInfo file;

    Editor() : GameScreen(display) {}

    void load(std::string fname);


    void render();
};


void Editor::load(string fname) {
    file.fname = fname;
    file.shortname = fname; // todo: trim
    file.curSprite = 0;

    cout << "Editor::load " << fname << endl;
    if (file.jsp.load(fname)) {
        cout << "Success" << endl;
    } else {
        cout << "Failure: " << file.jsp.error << endl;
    }
}

void Editor::render() {
    Color lightbg(200, 200, 200), black(0, 0, 0);

    al_clear_to_color(Color(0xffffffff));

    // draw sprite
    const int CENTER_X = 180 + (DISPLAY_WIDTH - 180)/2, CENTER_Y = 30 + (DISPLAY_HEIGHT - 30)/2, CH_SIZE = 5;
    if (file.jsp.frames.size() > 0) {
        JspFrame current = file.jsp.frames[file.curSprite];
        current.bmp.draw(CENTER_X - current.ofsX, CENTER_Y - current.ofsY);
    }
    // crosshairs
    gfx::line(CENTER_X - CH_SIZE, CENTER_Y, CENTER_X + CH_SIZE - 1, CENTER_Y, black, 1);
    gfx::line(CENTER_X, CENTER_Y - CH_SIZE, CENTER_X, CENTER_Y + CH_SIZE - 1, black, 1);

    // left bar
    gfx::fillRect(0, 0, 180, DISPLAY_HEIGHT, lightbg);
    gfx::line(180, 0, 180, DISPLAY_HEIGHT, black, 0);

    if (file.jsp.frames.size() == 0) {
        gFont.draw(5, 35, black, "No file open");
    } else {
        JspFrame current = file.jsp.frames[file.curSprite];
        gFont.draw(5, 35, black, file.shortname);
        gFont.drawf(5, 55, black, "Sprite count: %d", file.jsp.frames.size());
        gFont.drawf(5, 75, black, "Sprite #%d", file.curSprite);
        gFont.drawf(5, 95, black, "Origin: (%d, %d)", current.ofsX, current.ofsY);

        gIconFont.drawf(5, 125, black, "%s %s", FA::str(FAChar::check).cstr(), FA::str(FAChar::adjust).cstr());
    }

    // top bar
    gfx::fillRect(0, 0, DISPLAY_WIDTH, 30, lightbg);
    gfx::line(0, 30, DISPLAY_WIDTH, 30, black, 0);
    gFont.draw(DISPLAY_WIDTH - 5, 5, ALLEGRO_ALIGN_RIGHT, black, "JspEdit 2 by SpaceManiac");


    /* int x = 200, y = 50, h = 0;
    for (size_t i = 0; i < file.jsp.frames.size(); ++i) {
        JspFrame frame = file.jsp.frames[i];
        if (x + frame.bmp.getWidth() > DISPLAY_WIDTH) {
            y += h;
            x = 200;
            h = 0;
        }
        if (y >= DISPLAY_HEIGHT) {
            break;
        }
        frame.bmp.draw(x, y);
        x += frame.bmp.getWidth();
        if (frame.bmp.getHeight() > h) {
            h = frame.bmp.getHeight();
        }
    } */

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
