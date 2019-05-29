#include "editor.h"
#include "globals.h"
#include "jspfile.h"
#include "fontawesome.h"
#include "gui.h"
#include "palette.h"
#include <iostream>
#include <sstream>
#include <SDL2/SDL_image.h>
#include <nfd.h>

using namespace std;

namespace editor {

/****************************************************************/
/* File chooser utility functions */

namespace dialog {

typedef std::function<void(std::string)> callback;

void open(const char* title, const char* patterns, callback func) {
    char *chosen = nullptr;
    nfdresult_t result = NFD_OpenDialog(patterns, nullptr, &chosen);
    if (result == NFD_OKAY) {
        func(chosen);
    }
    free(chosen);
}

void save(const char* title, const char* patterns, callback func) {
    char *chosen = nullptr;
    nfdresult_t result = NFD_SaveDialog(patterns, nullptr, &chosen);
    if (result == NFD_OKAY) {
        func(chosen);
    }
    free(chosen);
}

void showMessage(const char* head, const char* body) {
    SDL_ShowSimpleMessageBox(0, head, body, window);
}

void error(const char* head, const char* body = nullptr) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, head, body, window);
}

bool showOkCancel(const char* head, bool def) {
    const SDL_MessageBoxButtonData buttons[] = {
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "OK" },
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Cancel" },
    };
    const SDL_MessageBoxData data = {
        SDL_MESSAGEBOX_INFORMATION,
        window,
        "JspEdit",
        head,
        SDL_arraysize(buttons),
        buttons,
        nullptr,
    };
    int buttonid;
    if (SDL_ShowMessageBox(&data, &buttonid) < 0 || buttonid == -1) {
        return def;
    }
    return buttonid == 1;
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

struct Editor {
    FileInfo file;
    Gui gui;

    int crosshairs;

    bool dragging;
    bool running;

    Editor();

    string trimFilename(string fname);
    void load(string fname);
    void save();
    void saveAs(string fname);

    bool import_frame(string fname, bool batch = false);
    bool export_frame(string fname, bool batch = false);

    void import_batch(string fname);
    void export_batch(string fname);

    void convertAlpha();

    void move(int dir);
    void shift(int dir);

    void render();
    void handleEvent(const SDL_Event &event);
};

static GuiRect rect(int x, int y, int w, int h) {
    return { x, y, x+w, y+h };
}

Editor::Editor()
{
    file = { "", "", JspFile(), 0, false };
    crosshairs = 1;
    dragging = false;
    running = true;

    // create empty blank frame
    /*{
        JspFrame newFrame = { nullptr, 0, 0 };
        file.jsp.frames.push_back(newFrame);
    }*/

    int w = 100, g = 110, h = 22;
    int x = 6, y = 4;
    // top bar
    gui.addButton(rect(x, y, w, h), "Open", "Open JSP",
        { KMOD_LCTRL, SDL_SCANCODE_O },
        [this]() {
            if (file.unsaved && !dialog::showOkCancel("You have unsaved changes, do you want to open anyways?", false)) return;
            dialog::open("Open JSP", "jsp", [this](std::string str) { load(str); });
        });
    gui.addButton(rect(x += g, y, w, h), "Save", "Save JSP",
        { KMOD_LCTRL, SDL_SCANCODE_S },
        [this]() { save(); });
    gui.addButton(rect(x += g, y, w, h), "Save As", "Save JSP As",
        { KMOD_LCTRL | KMOD_LSHIFT, SDL_SCANCODE_S },
        std::bind(dialog::save, "Save JSP", "jsp", [this](std::string str) { saveAs(str); }));
    gui.addButton(rect(x += g, y, w, h), "Import", "Import from image",
        { KMOD_LCTRL, SDL_SCANCODE_I },
        std::bind(dialog::open, "Import Image", "png;bmp;tga;pcx", [this](std::string str) { import_frame(str); }));
    gui.addButton(rect(x += g, y, w, h), "Export", "Export to image",
        { KMOD_LCTRL, SDL_SCANCODE_E },
        std::bind(dialog::save, "Export Image", "png;bmp;tga;pcx", [this](std::string str) { export_frame(str); }));
    gui.addButton(rect(x += g, y, w, h), "Import All", "Import from folder",
        { KMOD_LCTRL | KMOD_LSHIFT, SDL_SCANCODE_I },
        std::bind(dialog::open, "Select first frame (0.png)", "png", [this](std::string str) { import_batch(str); }));
    gui.addButton(rect(x += g, y, w, h), "Export All", "Export to folder",
        { KMOD_LCTRL | KMOD_LSHIFT, SDL_SCANCODE_E },
        std::bind(dialog::save, "Export all frames", "png", [this](std::string str) { export_batch(str); }));

    x = DISPLAY_WIDTH - g;
    gui.addButton(rect(x, y, w, h), "Crosshairs", "Cycle crosshairs mode",
        { KMOD_LCTRL, SDL_SCANCODE_H },
        [this]() { crosshairs = (crosshairs + 1) % MAX_CROSSHAIRS; });

    x = 10; g = 34; y = 80;
    gui.addIconButton(x, y, FAChar::fast_backward, "First frame",
        { 0, SDL_SCANCODE_HOME },
        [this]() { file.curSprite = 0; });
    gui.addIconButton(x += g, y, FAChar::arrow_left, "Previous frame",
        { 0, SDL_SCANCODE_LEFT },
        [this]() { move(-1); });
    x += g; //gui.addIconButton(x += g, y, FAChar::minus);
    gui.addIconButton(x += g, y, FAChar::arrow_right, "Next frame",
        { 0, SDL_SCANCODE_RIGHT },
        [this]() { move(1); });
    gui.addIconButton(x += g, y, FAChar::fast_forward, "Last frame",
        { 0, SDL_SCANCODE_END },
        [this]() { file.curSprite = file.jsp.frames.size() - 1; });

    gui.addIconButton(146, 125, FAChar::undo, "Reset origin",
        { KMOD_LCTRL, SDL_SCANCODE_R },
        [this]() {
            JspFrame& frame = file.jsp.frames[file.curSprite];
            frame.ofsX = frame.ofsY = 0;
        });

    x = 10; g = 34; y = 150;
    gui.addIconButton(x, y, FAChar::backward, "Shift frame left",
        { KMOD_LSHIFT, SDL_SCANCODE_LEFT },
        [this]() { shift(-1); });
    gui.addIconButton(x += g, y, FAChar::plus, "Add frame",
        { KMOD_LCTRL, SDL_SCANCODE_INSERT },
        [this]() {
            JspFrame newFrame(32, 24);
            file.jsp.frames.insert(file.jsp.frames.begin() + file.curSprite, newFrame);
        });
    gui.addIconButton(x += g, y, FAChar::font, "Convert pink to alpha",
        { KMOD_LCTRL, SDL_SCANCODE_A },
        [this]() { convertAlpha(); });
    gui.addIconButton(x += g, y, FAChar::minus, "Delete frame",
        { KMOD_LCTRL, SDL_SCANCODE_DELETE },
        [this]() {
            if (file.jsp.frames.size() <= 1) return;

            file.jsp.frames.erase(file.jsp.frames.begin() + file.curSprite);
            if (file.curSprite >= file.jsp.frames.size()) {
                file.curSprite = file.jsp.frames.size() - 1;
            }
        });
    gui.addIconButton(x += g, y, FAChar::forward, "Shift frame right",
        { KMOD_LSHIFT, SDL_SCANCODE_RIGHT },
        [this]() { shift(1); });
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
        dialog::save("Save JSP", "jsp", [this](std::string str) { saveAs(str); });
        return;
    }

    cout << "Editor::save " << file.fname << endl;

    // show please wait dialog
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 196);
    SDL_RenderFillRect(renderer, NULL);
    DrawText(renderer, gFont, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 - 50, ALIGN_CENTER, { 0, 0, 0, 255 }, "Saving, please wait...");
    SDL_RenderPresent(renderer);

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

    std::shared_ptr<SDL_Surface> surface(IMG_Load(fname.c_str()), SDL_FreeSurface);
    if (!surface) {
        if (!batch) dialog::error("Failed to import");
        return false;
    }

    if (surface->format->format != SDL_PIXELFORMAT_ABGR8888) {
        surface = {SDL_ConvertSurfaceFormat(surface.get(), SDL_PIXELFORMAT_ABGR8888, 0), SDL_FreeSurface};
    }

    if (palette::reduceImage(surface.get()) && !batch) {
        dialog::showMessage("The image's colors were adjusted", nullptr);
    }

    std::shared_ptr<SDL_Texture> texture(
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, surface->w, surface->h),
        SDL_DestroyTexture);
    SDL_UpdateTexture(texture.get(), nullptr, surface->pixels, surface->pitch);
    SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);

    file.unsaved = true;
    file.jsp.frames[file.curSprite].texture = texture;
    file.jsp.frames[file.curSprite].surface = surface;
    return true;
}

bool Editor::export_frame(string fname, bool batch) {
    if (file.jsp.frames.size() == 0) return false;

    if (fname.find('.') == string::npos) {
        fname += ".png";
    }

    if (IMG_SavePNG(file.jsp.frames[file.curSprite].surface.get(), fname.c_str())) {
        if (!batch) dialog::error("Failed to export");
        return false;
    }
    return true;
}

/****************************************************************/
/* Batch import and export */

void Editor::import_batch(string fname) {
    if (fname.substr(fname.length() - 5) == "0.png") {
        fname.resize(fname.length() - 5);
    } else {
        dialog::error("Cannot batch import", "Must select file ending in 0.png");
        return;
    }

    int sprite = file.curSprite;
    for (file.curSprite = 0; file.curSprite < file.jsp.frames.size(); ++file.curSprite) {
        std::ostringstream s;
        s << fname << file.curSprite << ".png";
        string name = s.str();
        if (!import_frame(name, true)) {
            dialog::error("Batch import failed on file:", name.c_str());
            break;
        }
    }
    file.curSprite = sprite;
}

void Editor::export_batch(string fname) {
    if (fname.substr(fname.length() - 4) == ".png") {
        fname.resize(fname.length() - 4);
    }
    if (fname.back() == '0') {
        fname.resize(fname.length() - 1);
    }

    int sprite = file.curSprite;
    for (file.curSprite = 0; file.curSprite < file.jsp.frames.size(); ++file.curSprite) {
        std::ostringstream s;
        s << fname << file.curSprite << ".png";
        string name = s.str();
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

void Editor::convertAlpha() {
    JspFrame &frame = file.jsp.frames[file.curSprite];
    SDL_Surface *surface = frame.surface.get();

    SDL_Color *pixels = (SDL_Color*) surface->pixels;
    int pitch = surface->pitch / sizeof(SDL_Color);
    SDL_LockSurface(surface);
    SDL_Color key = palette::getColor(208);

    for (int y = 0; y < surface->h; ++y) {
        for (int x = 0; x < surface->h; ++x) {
            SDL_Color col = pixels[y * pitch + x];
            if (!memcmp(&col, &key, sizeof(SDL_Color))) {
                col = { 0, 0, 0, 0 };
            }
            pixels[y * pitch + x] = col;
        }
    }
    SDL_UnlockSurface(surface);
    frame.upload();
}

/****************************************************************/
/* Game loop overrides */

void Editor::render() {
    SDL_Color lightbg = {200, 200, 200, 255};
    SDL_Color black = {0, 0, 0, 255};
    SDL_Rect rect;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    const int CENTER_X = 180 + (DISPLAY_WIDTH - 180)/2, CENTER_Y = 30 + (DISPLAY_HEIGHT - 30)/2;

    // crosshairs in behind
    if (crosshairs == 1) {
        SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
        SDL_RenderDrawLine(renderer, 180, CENTER_Y, DISPLAY_WIDTH, CENTER_Y);
        SDL_RenderDrawLine(renderer, CENTER_X, 30, CENTER_X, DISPLAY_HEIGHT);
        SDL_SetRenderDrawColor(renderer, 0, 196, 0, 255);
        rect = {CENTER_X - 16, CENTER_Y - 12, 32, 24};
        SDL_RenderDrawRect(renderer, &rect);
    }

    // draw sprite
    if (file.jsp.frames.size() > 0) {
        JspFrame current = file.jsp.frames[file.curSprite];
        SDL_Rect rect = {
            CENTER_X - current.ofsX,
            CENTER_Y - current.ofsY,
            current.surface->w,
            current.surface->h,
        };
        SDL_RenderCopy(renderer, current.texture.get(), nullptr, &rect);
    }

    // crosshairs in front
    if (crosshairs == 2) {
        SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
        SDL_RenderDrawLine(renderer, 180, CENTER_Y, DISPLAY_WIDTH, CENTER_Y);
        SDL_RenderDrawLine(renderer, CENTER_X, 30, CENTER_X, DISPLAY_HEIGHT);
        SDL_SetRenderDrawColor(renderer, 0, 196, 0, 255);
        rect = {CENTER_X - 16, CENTER_Y - 12, 32, 24};
        SDL_RenderDrawRect(renderer, &rect);
    }

    // left bar
    SDL_SetRenderDrawColor(renderer, lightbg.r, lightbg.g, lightbg.b, lightbg.a);
    rect = {0, 0, 180, DISPLAY_HEIGHT};
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawLine(renderer, 180, 0, 180, DISPLAY_HEIGHT);
    SDL_RenderDrawLine(renderer, 0, 200, 180, 200);
    SDL_RenderDrawLine(renderer, 0, DISPLAY_HEIGHT - 24, 180, DISPLAY_HEIGHT - 24);

    // file stats
    vector<JspFrame> frames = file.jsp.frames;
    if (frames.size() == 0) {
        DrawText(renderer, gFont, 5, 35, ALIGN_LEFT, black, "No file open");
    } else {
        JspFrame current = frames[file.curSprite];
        DrawText(renderer, gFont, 5, 35, ALIGN_LEFT, black, file.shortname.c_str());

        std::ostringstream s;
        s << "Sprite count: " << frames.size();
        DrawText(renderer, gFont, 5, 55, ALIGN_LEFT, black, s.str().c_str());
        s.str(""); s << file.curSprite;
        DrawText(renderer, gFont, 88, 80, ALIGN_CENTER, black, s.str().c_str());
        s.str(""); s << "Size: (" << current.surface->w << ", " << current.surface->h << ")";
        DrawText(renderer, gFont, 5, 105, ALIGN_LEFT, black, s.str().c_str());
        s.str(""); s << "Origin: (" << current.ofsX << ", " << current.ofsY << ")";
        DrawText(renderer, gFont, 5, 125, ALIGN_LEFT, black, s.str().c_str());

        //s.str(""); s << FA::str(FAChar::check) << " " << FA::str(FAChar::adjust);
        //DrawText(renderer, gIconFont, 5, 145, ALIGN_LEFT, black, s.str().c_str());
    }

    // sprites
    if (frames.size() > 0) {
        SDL_Rect clip {0, 201, 180, DISPLAY_HEIGHT - 26 - 199};
        SDL_RenderSetClipRect(renderer, &clip);

        // setup
        SDL_Texture *curBmp = frames[file.curSprite].texture.get();
        SDL_Surface *surface = frames[file.curSprite].surface.get();
        int w = surface->w, h = surface->h;
        int spr_ = file.curSprite, y_ = 200 + (DISPLAY_HEIGHT - 200 - 24 - h) / 2;
        SDL_Rect rect = { 90 - w / 2, y_, w, h };
        SDL_RenderCopy(renderer, curBmp, nullptr, &rect);

        // frames before
        size_t spr = spr_;
        int y = y_;
        while (y > 200 && spr > 0) {
            --spr;
            SDL_Texture *bmp = frames[spr].texture.get();
            rect.w = frames[spr].surface->w;
            rect.h = frames[spr].surface->h;
            rect.x = 90 - rect.w / 2;
            rect.y = y -= rect.h;
            SDL_RenderCopy(renderer, bmp, nullptr, &rect);
        }

        // frames after
        spr = spr_, y = y_;
        while (y < DISPLAY_HEIGHT && spr < frames.size()) {
            SDL_Texture *bmp = frames[spr].texture.get();
            rect.w = frames[spr].surface->w;
            rect.h = frames[spr].surface->h;
            rect.x = 90 - rect.w / 2;
            rect.y = y;
            SDL_RenderCopy(renderer, bmp, nullptr, &rect);
            y += rect.h;
            ++spr;
        }

        // red box
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        rect = { 90 - w / 2 - 1, y_ - 1, 2 + w, 2 + h };
        SDL_RenderDrawRect(renderer, &rect);
    }
    SDL_RenderSetClipRect(renderer, nullptr);

    // top bar
    SDL_SetRenderDrawColor(renderer, lightbg.r, lightbg.g, lightbg.b, lightbg.a);
    rect = { 0, 0, DISPLAY_WIDTH, 30 };
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawLine(renderer, 0, 30, DISPLAY_WIDTH, 30);

    // buttons
    gui.render();

    /* int x = 200, y = 50, h = 0;
    for (size_t i = 0; i < file.jsp.frames.size(); ++i) {
        JspFrame frame = file.jsp.frames[i];
        if (x + frame.surface->w > DISPLAY_WIDTH) {
            y += h;
            x = 200;
            h = 0;
        }
        if (y >= DISPLAY_HEIGHT) {
            break;
        }
        rect = {x, y, frame.surface->w, frame.surface->h};
        SDL_RenderCopy(renderer, frame.texture.get(), nullptr, &rect);
        x += frame.surface->w;
        if (frame.surface->h > h) {
            h = frame.surface->h;
        }
    } */

    SDL_RenderPresent(renderer);
}

void Editor::handleEvent(const SDL_Event &event) {
    if (gui.handleEvent(event)) {
        return;
    }

    GuiRect dragArea = { 181, 31, DISPLAY_WIDTH, DISPLAY_HEIGHT };

    switch (event.type) {
    case SDL_QUIT:
        if (file.unsaved) {
            running = !dialog::showOkCancel("You have unsaved changes, do you want to quit anyways?", false);
        } else {
            running = false;
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
        if ((dragArea.left <= event.button.x && event.button.x <= dragArea.right && dragArea.top <= event.button.y && event.button.y <= dragArea.bottom) && event.button.button == 1) {
            dragging = true;
        }
        break;

    case SDL_MOUSEBUTTONUP:
        if (event.button.button == 1) {
            dragging = false;
        }
        break;

    case SDL_MOUSEMOTION:
        if (file.jsp.frames.size() == 0 || !dragging) break;
        file.jsp.frames[file.curSprite].ofsX -= event.motion.xrel;
        file.jsp.frames[file.curSprite].ofsY -= event.motion.yrel;
        break;

    case SDL_KEYDOWN:
        if (file.jsp.frames.size() == 0) break;

        int mods = SDL_GetModState();
        bool ctrl = mods & KMOD_CTRL;
        bool shift = mods & KMOD_SHIFT;

        int move = 1;
        if (shift) move = 10;

        switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_LEFT:
            if (ctrl) {
                file.jsp.frames[file.curSprite].ofsX += move;
                file.unsaved = true;
            }
            break;
        case SDL_SCANCODE_RIGHT:
            if (ctrl) {
                file.jsp.frames[file.curSprite].ofsX -= move;
                file.unsaved = true;
            }
            break;
        case SDL_SCANCODE_UP:
            if (ctrl) {
                file.jsp.frames[file.curSprite].ofsY += move;
                file.unsaved = true;
            }
            break;
        case SDL_SCANCODE_DOWN:
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
    while (ed.running) {
        ed.render();
        SDL_Event e;
        while (SDL_PollEvent(&e))
            ed.handleEvent(e);
    }
}

} // namespace editor
