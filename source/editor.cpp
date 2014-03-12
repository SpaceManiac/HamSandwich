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

const int MAX_CROSSHAIRS = 3;

struct Editor : public GameScreen {
    FileInfo file;
    Gui gui;

    int crosshairs;

    bool dragging;

    Editor();

    string trimFilename(string fname);
    void load(string fname);
    void save();
    void saveAs(string fname);

    bool import_frame(string fname, bool batch = false);
    bool export_frame(string fname, bool batch = false);

    string batch_filename(string fname, int num);
    void import_batch(string fname);
    void export_batch(string fname);

    void move(int dir);
    void shift(int dir);

    void render();
    void handleEvent(Event event);
};

static GuiRect rect(int x, int y, int w, int h) {
    return GuiRect(x, y, x+w, y+h);
}

Editor::Editor()
    : GameScreen(display)
{
    file = { "", "", JspFile(), 0, false };
    crosshairs = 1;

    // create empty blank frame
    {
        JspFrame newFrame = { Bitmap(32, 24), 0, 0 };
        gfx::SetTarget target(newFrame.bmp);
        al_clear_to_color(palette::getColor(240));
        file.jsp.frames.push_back(newFrame);
    }

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
    gui.addButton(rect(x += g, y, w, h), "Import All", "Import from folder",
        { ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_E },
        std::bind(dialog::open, "Select first frame (0.png)", "*.png;*.*", [this](std::string str) { import_batch(str); }));
    gui.addButton(rect(x += g, y, w, h), "Export All", "Export to folder",
        { ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_E },
        std::bind(dialog::save, "Export all frames", "*.png;*.*", [this](std::string str) { export_batch(str); }));

    x = DISPLAY_WIDTH - g;
    gui.addButton(rect(x, y, w, h), "Crosshairs", "Cycle crosshairs mode",
        { ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_H },
        [this]() { crosshairs = (crosshairs + 1) % MAX_CROSSHAIRS; });

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

    x = 20; g = 30; y = 150;
    gui.addIconButton(x, y, FAChar::fast_backward, "First frame",
        { 0, ALLEGRO_KEY_HOME },
        [this]() { file.curSprite = 0; });
    x += g/2;
    gui.addIconButton(x += g, y, FAChar::plus, "Add frame",
        { ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_INSERT },
        [this]() {
            JspFrame newFrame = { Bitmap(32, 24), 0, 0 };
            gfx::SetTarget target(newFrame.bmp);
            al_clear_to_color(palette::getColor(240));
            file.jsp.frames.insert(file.jsp.frames.begin() + file.curSprite, newFrame);
        });
    gui.addIconButton(x += g, y, FAChar::minus, "Delete frame",
        { ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_DELETE },
        [this]() {
            if (file.jsp.frames.size() <= 1) return;

            file.jsp.frames.erase(file.jsp.frames.begin() + file.curSprite);
            if (file.curSprite >= file.jsp.frames.size()) {
                file.curSprite = file.jsp.frames.size() - 1;
            }
        });
    x += g/2;
    gui.addIconButton(x += g, y, FAChar::fast_forward, "Last frame",
        { 0, ALLEGRO_KEY_END },
        [this]() { file.curSprite = file.jsp.frames.size() - 1; });
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
    FileInfo newFile;
    newFile.fname = fname;
    newFile.shortname = trimFilename(fname);
    newFile.curSprite = 0;
    newFile.unsaved = false;

    cout << "Editor::load " << fname << endl;
    if (newFile.jsp.load(fname)) {
        cout << "Success" << endl;
        file = newFile;
    } else {
        cout << "Failure: " << newFile.jsp.error << endl;
        dialog::error("Failed to load:", newFile.jsp.error.c_str());
    }
}

void Editor::save() {
    if (file.fname == "") {
        // call save as which calls this instead
        dialog::save("Save JSP", "*.jsp;*.*", [this](std::string str) { saveAs(str); });
        return;
    }

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
    if (fname.find('.') == string::npos) {
        fname += ".jsp";
    }

    file.fname = fname;
    file.shortname = trimFilename(fname);
    save();
}

bool Editor::import_frame(string fname, bool batch) {
    if (file.jsp.frames.size() == 0) return false;
    Bitmap bmp = file.jsp.frames[file.curSprite].bmp;
    if (!bmp.load(fname.c_str())) {
        if (!batch) dialog::error("Failed to import");
        return false;
    }
    if (palette::reduceImage(bmp) && !batch) {
        dialog::showMessage("The image's colors were adjusted", nullptr);
    }
    file.unsaved = true;
    file.jsp.frames[file.curSprite].bmp = bmp;
    return true;
}

bool Editor::export_frame(string fname, bool batch) {
    if (file.jsp.frames.size() == 0) return false;

    if (fname.find('.') == string::npos) {
        fname += ".png";
    }

    if (!file.jsp.frames[file.curSprite].bmp.save(fname.c_str())) {
        if (!batch) dialog::error("Failed to export");
        return false;
    }
    return true;
}

/****************************************************************/
/* Batch import and export */

void Editor::import_batch(string fname) {
    if (fname.substr(fname.length() - 5) == "0.png") {
        fname = fname.substr(0, fname.length() - 5);
    } else {
        dialog::error("Cannot batch import", "Must select file ending in 0.png");
        return;
    }

    int sprite = file.curSprite;
    for (file.curSprite = 0; file.curSprite < file.jsp.frames.size(); ++file.curSprite) {
        string name = toString << fname << file.curSprite << ".png";
        if (!import_frame(name, true)) {
            dialog::error("Batch import failed on file:", name.c_str());
            break;
        }
    }
    file.curSprite = sprite;
}

void Editor::export_batch(string fname) {
    if (fname.substr(fname.length() - 4) == ".png") {
        fname = fname.substr(0, fname.length() - 4);
    }

    int sprite = file.curSprite;
    for (file.curSprite = 0; file.curSprite < file.jsp.frames.size(); ++file.curSprite) {
        string name = toString << fname << file.curSprite << ".png";
        if (!export_frame(name, true)) {
            dialog::error("Batch export failed on file:", name.c_str());
            break;
        }
    }
    file.curSprite = sprite;
}

/****************************************************************/
/* View manipulation */

void Editor::move(int dir) {
    if (file.jsp.frames.size() == 0) return;

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
    if (file.jsp.frames.size() == 0) return;

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

    const float CENTER_X = 180.5 + (DISPLAY_WIDTH - 180)/2, CENTER_Y = 30.5 + (DISPLAY_HEIGHT - 30)/2;

    // crosshairs in behind
    if (crosshairs == 1) {
        gfx::line(180, CENTER_Y, DISPLAY_WIDTH, CENTER_Y, Color(0, 128, 0), 1);
        gfx::line(CENTER_X, 30, CENTER_X, DISPLAY_HEIGHT, Color(0, 128, 0), 1);
        gfx::rect(CENTER_X - 16, CENTER_Y - 12, CENTER_X + 15, CENTER_Y + 11, Color(0, 196, 0), 1);
    }

    // draw sprite
    if (file.jsp.frames.size() > 0) {
        JspFrame current = file.jsp.frames[file.curSprite];
        current.bmp.draw((int) CENTER_X - current.ofsX, (int) CENTER_Y - current.ofsY);
    }

    // crosshairs in front
    if (crosshairs == 2) {
        gfx::line(180, CENTER_Y, DISPLAY_WIDTH, CENTER_Y, Color(0, 128, 0), 1);
        gfx::line(CENTER_X, 30, CENTER_X, DISPLAY_HEIGHT, Color(0, 128, 0), 1);
        gfx::rect(CENTER_X - 16, CENTER_Y - 12, CENTER_X + 15, CENTER_Y + 11, Color(0, 196, 0), 1);
    }

    // left bar
    gfx::fillRect(0, 0, 180, DISPLAY_HEIGHT, lightbg);
    gfx::line(180, 0, 180, DISPLAY_HEIGHT, black, 0);
    gfx::line(0, 200, 180, 200, black, 0);
    gfx::line(0, DISPLAY_HEIGHT - 24, 180, DISPLAY_HEIGHT - 24, black, 0);

    // file stats
    vector<JspFrame> frames = file.jsp.frames;
    if (frames.size() == 0) {
        gFont.draw(5, 35, black, "No file open");
    } else {
        JspFrame current = frames[file.curSprite];
        gFont.draw(5, 35, black, file.shortname);
        gFont.drawf(5, 55, black, "Sprite count: %d", frames.size());
        gFont.drawf(90, 80, ALLEGRO_ALIGN_CENTER, black, "%d", file.curSprite);
        gFont.drawf(5, 105, black, "Size: (%d, %d)", current.bmp.getWidth(), current.bmp.getHeight());
        gFont.drawf(5, 125, black, "Origin: (%d, %d)", current.ofsX, current.ofsY);

        //gIconFont.drawf(5, 145, black, "%s %s", FA::str(FAChar::check).cstr(), FA::str(FAChar::adjust).cstr());
    }

    // sprites
    if (frames.size() > 0) {
        Rect<int> prevClipping = Rect<int>::getClipping();
        Rect<int> clipRect(0, 200, 178, DISPLAY_HEIGHT - 26);
        clipRect.setClipping();

        // setup
        Bitmap curBmp = frames[file.curSprite].bmp;
        int spr_ = file.curSprite, y_ = 200 + (DISPLAY_HEIGHT - 200 - 24 - curBmp.getHeight()) / 2;
        curBmp.draw(90 - curBmp.getWidth() / 2, y_);

        // frames before
        size_t spr = spr_;
        int y = y_;
        while (y > 200 && spr > 0) {
            Bitmap bmp = frames[--spr].bmp;
            y -= bmp.getHeight();
            bmp.draw(90 - bmp.getWidth() / 2, y);
        }

        // frames after
        spr = spr_, y = y_;
        while (y < DISPLAY_HEIGHT && spr < frames.size() - 1) {
            y += frames[spr++].bmp.getHeight();
            Bitmap bmp = frames[spr].bmp;
            bmp.draw(90 - bmp.getWidth() / 2, y);
        }

        // red box
        gfx::rect(90 - curBmp.getWidth() / 2, y_, 91 + curBmp.getWidth() / 2, 1 + y_ + curBmp.getHeight(), Color(255, 0, 0), 1);

        prevClipping.setClipping();
    }

    // top bar
    gfx::fillRect(0, 0, DISPLAY_WIDTH, 30, lightbg);
    gfx::line(0, 30, DISPLAY_WIDTH, 30, black, 0);
    //gFont.draw(DISPLAY_WIDTH - 5, 5, ALLEGRO_ALIGN_RIGHT, black, "JspEdit 2 by SpaceManiac");

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

    GuiRect dragArea = GuiRect(181, 31, DISPLAY_WIDTH, DISPLAY_HEIGHT);

    switch (event.getType()) {
    case ALLEGRO_EVENT_DISPLAY_CLOSE:
        if (file.unsaved) {
            running = !dialog::showOkCancel("You have unsaved changes, do you want to quit anyways?", false);
        } else {
            running = false;
        }
        break;

    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
        if (dragArea.intersects(event.getMouseX(), event.getMouseY()) && event.get().mouse.button == 1) {
            dragging = true;
        }
        break;

    case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
        if (event.get().mouse.button == 1) {
            dragging = false;
        }
        break;

    case ALLEGRO_EVENT_MOUSE_AXES:
    case ALLEGRO_EVENT_MOUSE_WARPED:
        if (file.jsp.frames.size() == 0 || !dragging) break;
        file.jsp.frames[file.curSprite].ofsX -= event.getMouseDX();
        file.jsp.frames[file.curSprite].ofsY -= event.getMouseDY();
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
