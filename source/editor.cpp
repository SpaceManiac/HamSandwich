#include "editor.h"
#include "globals.h"
#include "jspfile.h"
#include "fontawesome.h"
#include "gui.h"
#include "palette.h"
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

void error(const char* head, const char* body = nullptr) {
    al_show_native_message_box(display.get(), "JspEdit", head, body, nullptr, ALLEGRO_MESSAGEBOX_ERROR);
}

bool showOkCancel(const char* head, bool def) {
    int r = al_show_native_message_box(display.get(), "JspEdit", head, nullptr, nullptr, ALLEGRO_MESSAGEBOX_OK_CANCEL | ALLEGRO_MESSAGEBOX_QUESTION);
    return r == 0 ? def : r == 1;
}

} // namespace dialog

/****************************************************************/
/* Editor class */

struct FileInfo {
    string fname, shortname;
    JspFile jsp;
    size_t curSprite;
    bool unsaved;
};

struct Editor : public GameScreen {
    FileInfo file;
    Gui gui;

    Editor();

    string trimFilename(string fname);
    void load(string fname);
    void save();
    void saveAs(string fname);

    void import_frame(string fname);
    void export_frame(string fname);

    void move(int dir);
    void shift(int dir);

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
    gui.addButton(rect(x, y, w, h), "Open", "Open JSP",
        { ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_O },
        [this]() {
            if (file.unsaved && !dialog::showOkCancel("You have unsaved changes, do you want to open anyways?", false)) return;
            dialog::open("Open JSP", "*.jsp;*.*", [this](std::string str) { load(str); });
        });
    gui.addButton(rect(x += g, y, w, h), "Save", "Save JSP",
        { ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_S },
        [this]() { save(); });
    gui.addButton(rect(x += g, y, w, h), "Save As", "Save JSP As",
        { ALLEGRO_KEYMOD_CTRL | ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_S },
        std::bind(dialog::save, "Save JSP", "*.jsp;*.*", [this](std::string str) { saveAs(str); }));
    gui.addButton(rect(x += g, y, w, h), "Import", "Import from image",
        { ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_I },
        std::bind(dialog::open, "Import Image", "*.png;*.bmp;*.tga;*.pcx;*.*", [this](std::string str) { import_frame(str); }));
    gui.addButton(rect(x += g, y, w, h), "Export", "Export to image",
        { ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_E },
        std::bind(dialog::save, "Export Image", "*.png;*.bmp;*.tga;*.pcx;*.*", [this](std::string str) { export_frame(str); }));

    x = 20; g = 30; y = 80;
    gui.addIconButton(x, y, FAChar::arrow_left, "Previous frame",
        { 0, ALLEGRO_KEY_LEFT },
        [this]() { move(-1); });
    gui.addIconButton(x += g, y, FAChar::backward, "Shift frame left",
        { ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_LEFT },
        [this]() { shift(-1); });
    x += g; //gui.addIconButton(x += g, y, FAChar::minus);
    gui.addIconButton(x += g, y, FAChar::forward, "Shift frame right",
        { ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_RIGHT },
        [this]() { shift(1); });
    gui.addIconButton(x += g, y, FAChar::arrow_right, "Next frame",
        { 0, ALLEGRO_KEY_RIGHT },
        [this]() { move(1); });
}

/****************************************************************/
/* File manipulation */

string Editor::trimFilename(string fname) {
    size_t slash = fname.find_last_of("/\\:");
    if (slash == string::npos) {
        return fname;
    } else {
        return fname.substr(slash + 1);
    }
}

void Editor::load(string fname) {
    file.fname = fname;
    file.shortname = trimFilename(fname);
    file.curSprite = 0;
    file.unsaved = false;

    cout << "Editor::load " << fname << endl;
    if (file.jsp.load(fname)) {
        cout << "Success" << endl;
    } else {
        cout << "Failure: " << file.jsp.error << endl;
        dialog::error("Failed to load:", file.jsp.error.c_str());
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
        file.unsaved = false;
        cout << "Success" << endl;
    } else {
        cout << "Failure: " << file.jsp.error << endl;
        dialog::error("Failed to save:", file.jsp.error.c_str());
    }
}

void Editor::saveAs(string fname) {
    file.fname = fname;
    file.shortname = trimFilename(fname);
    save();
}

void Editor::import_frame(string fname) {
    if (file.jsp.frames.size() == 0) return;
    Bitmap bmp = file.jsp.frames[file.curSprite].bmp;
    if (!bmp.load(fname.c_str())) {
        dialog::error("Failed to import");
    }
    if (palette::reduceImage(bmp)) {
        dialog::showMessage("The image's colors were adjusted", nullptr);
    }
    file.unsaved = true;
    file.jsp.frames[file.curSprite].bmp = bmp;
}

void Editor::export_frame(string fname) {
    if (file.jsp.frames.size() == 0) return;

    if (fname.find('.') == string::npos) {
        fname += ".png";
    }

    if (!file.jsp.frames[file.curSprite].bmp.save(fname.c_str())) {
        dialog::error("Failed to export");
    }
}

/****************************************************************/
/* View manipulation */

void Editor::move(int dir) {
    if (dir == -1) {
        if (file.curSprite > 0) {
            file.curSprite--;
        }
    } else if (dir == 1) {
        if (file.curSprite < file.jsp.frames.size() - 1) {
            file.curSprite++;
        }
    }
}

void Editor::shift(int dir) {
    if (dir == -1) {
        if (file.curSprite > 0) {
            file.curSprite--;
            swap(file.jsp.frames[file.curSprite], file.jsp.frames[file.curSprite + 1]);
            file.unsaved = true;
        }
    } else if (dir == 1) {
        if (file.curSprite < file.jsp.frames.size() - 1) {
            file.curSprite++;
            swap(file.jsp.frames[file.curSprite], file.jsp.frames[file.curSprite - 1]);
            file.unsaved = true;
        }
    }
}

/****************************************************************/
/* Game loop overrides */

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
        gFont.drawf(90, 80, ALLEGRO_ALIGN_CENTER, black, "%d", file.curSprite);
        gFont.drawf(5, 105, black, "Size: (%d, %d)", current.bmp.getWidth(), current.bmp.getHeight());
        gFont.drawf(5, 125, black, "Origin: (%d, %d)", current.ofsX, current.ofsY);

        //gIconFont.drawf(5, 145, black, "%s %s", FA::str(FAChar::check).cstr(), FA::str(FAChar::adjust).cstr());
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
    if (gui.handleEvent(event)) {
        return;
    }

    switch (event.getType()) {
    case ALLEGRO_EVENT_DISPLAY_CLOSE:
        if (file.unsaved) {
            running = !dialog::showOkCancel("You have unsaved changes, do you want to quit anyways?", false);
        } else {
            running = false;
        }
        break;

    case ALLEGRO_EVENT_KEY_CHAR:
        if (file.jsp.frames.size() == 0) break;

        int mods = event.getKeyboardModifiers();
        bool ctrl = mods & ALLEGRO_KEYMOD_CTRL;
        bool shift = mods & ALLEGRO_KEYMOD_SHIFT;

        int move = 1;
        if (shift) move = 10;

        switch (event.getKeyboardKeycode()) {
        case ALLEGRO_KEY_LEFT:
            if (ctrl) {
                file.jsp.frames[file.curSprite].ofsX += move;
                file.unsaved = true;
            }
            break;
        case ALLEGRO_KEY_RIGHT:
            if (ctrl) {
                file.jsp.frames[file.curSprite].ofsX -= move;
                file.unsaved = true;
            }
            break;
        case ALLEGRO_KEY_UP:
            if (ctrl) {
                file.jsp.frames[file.curSprite].ofsY += move;
                file.unsaved = true;
            }
            break;
        case ALLEGRO_KEY_DOWN:
            if (ctrl) {
                file.jsp.frames[file.curSprite].ofsY -= move;
                file.unsaved = true;
            }
            break;
        }
        break;
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
