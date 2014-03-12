#include "editor.h"
#include "globals.h"
#include "jspfile.h"
#include "fontawesome.h"
#include "gui.h"
#include <iostream>

using namespace std;

namespace editor {

/****************************************************************/
/* File chooser utility functions */

namespace dialog {

typedef std::function<void(std::string)> callback;

void go(callback func, const char* title, const char* patterns, int flags) {
    NativeFileDialog dlg { title, nullptr, patterns, flags };
    if (dlg.show(display) && dlg.getSelectedFileCount() == 1) {
        func(al_get_native_file_dialog_path(dlg.get(), 0));
    }
}

void open(const char* title, const char* patterns, callback func) {
    go(func, title, patterns, ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
}

void save(const char* title, const char* patterns, callback func) {
    go(func, title, patterns, ALLEGRO_FILECHOOSER_SAVE);
}

void showMessage(const char* head, const char* body) {
    al_show_native_message_box(display.get(), "JspEdit", head, body, nullptr, 0);
}

bool showOkCancel(const char* head, const char* body, bool def) {
    int r = al_show_native_message_box(display.get(), "JspEdit", head, body, nullptr, ALLEGRO_MESSAGEBOX_OK_CANCEL);
    return r == 0 ? def : r == 1;
}

} // namespace dialog

/****************************************************************/
/* Editor class */

struct FileInfo {
    string fname, shortname;
    JspFile jsp;
    int curSprite;
};

struct Editor : public GameScreen {
    FileInfo file;
    Gui gui;

    Editor();

    void load(std::string fname);
    void save();
    void saveAs(std::string fname);

    void render();
    void handleEvent(Event event);
};

static GuiRect rect(int x, int y, int w, int h) {
    return GuiRect(x, y, x+w, y+h);
}

Editor::Editor()
    : GameScreen(display, 30)
{
    int w = 100, g = 110, h = 22;
    int x = 6, y = 4;
    // top bar
    gui.addButton(rect(x, y, w, h), "Open", std::bind(dialog::open, "Open JSP", "*.jsp", [this](std::string str) { load(str); }));
    gui.addButton(rect(x += g, y, w, h), "Save", [this]() { save(); });
    gui.addButton(rect(x += g, y, w, h), "Save As", std::bind(dialog::save, "Save JSP", "*.jsp", [this](std::string str) { saveAs(str); }));
    gui.addButton(rect(x += g, y, w, h), "Import");
    gui.addButton(rect(x += g, y, w, h), "Export");

    gui.addIconButton(20, 200, FAChar::arrow_up);
}

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

void Editor::save() {
    cout << "Editor::save " << file.fname << endl;

    // show please wait dialog
    gfx::fillRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, Color(255, 255, 255, 128));
    gFont.draw(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 - 50, ALLEGRO_ALIGN_CENTER, Color(0, 0, 0), "Saving, please wait...");
    al_flip_display();

    // perform save
    if (file.jsp.save(file.fname)) {
        cout << "Success" << endl;
    } else {
        cout << "Failure: " << file.jsp.error << endl;
    }
}

void Editor::saveAs(string fname) {
    file.fname = fname;
    file.shortname = fname; // todo: trim
    save();
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
    gfx::line(CENTER_X - CH_SIZE, CENTER_Y, CENTER_X + CH_SIZE - 1, CENTER_Y, black, 0);
    gfx::line(CENTER_X, CENTER_Y - CH_SIZE, CENTER_X, CENTER_Y + CH_SIZE - 1, black, 0);

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

    // buttons
    gui.render();


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

void Editor::handleEvent(Event event) {
    if (event.getType() == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        // todo: confirm
        running = false;
    }

    if (gui.handleEvent(event)) {
        return;
    }


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
