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
    (void) title;
    char *chosen = nullptr;
    nfdresult_t result = NFD_OpenDialog(patterns, nullptr, &chosen);
    if (result == NFD_OKAY) {
        func(chosen);
    }
    free(chosen);
}

void save(const char* title, const char* patterns, callback func) {
    (void) title;
    char *chosen = nullptr;
    nfdresult_t result = NFD_SaveDialog(patterns, nullptr, &chosen);
    if (result == NFD_OKAY) {
        func(chosen);
    }
    free(chosen);
}

void showMessage(const char* body) {
    SDL_ShowSimpleMessageBox(0, "JspEdit", body, window);
}

void error(const char* head, const char* body = nullptr) {
    std::string buffer(head);
    buffer.append("\n\n");
    buffer.append(body);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "JspEdit", buffer.c_str(), window);
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
    std::vector<FileInfo> files;
    size_t curFile;
    Gui gui;

    int crosshairs;

    bool dragging;
    bool running;
    bool browsing;

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

    void working(const string& text);
    void render();
    void handleEvent(const SDL_Event &event);
};

static GuiRect rect(int x, int y, int w, int h) {
    return { x, y, x+w, y+h };
}

Editor::Editor()
{
    curFile = 0;
    crosshairs = 1;
    dragging = false;
    running = true;
    browsing = false;

    // create empty blank frame
    /*{
        JspFrame newFrame = { nullptr, 0, 0 };
        file.jsp.frames.push_back(newFrame);
    }*/
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
    working("Loading, please wait...");
    if (newFile.jsp.load(fname)) {
        cout << "Success" << endl;
        curFile = files.size();
        files.push_back(newFile);
    } else {
        cout << "Failure: " << newFile.jsp.error << endl;
        dialog::error("Failed to load:", newFile.jsp.error.c_str());
    }
}

void Editor::save() {
    if (files.empty()) return;
    FileInfo& file = files[curFile];
    if (file.fname == "") {
        // call save as which calls this instead
        dialog::save("Save JSP", "jsp", [this](std::string str) { saveAs(str); });
        return;
    }

    cout << "Editor::save " << file.fname << endl;
    working("Saving, please wait...");

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
    if (files.empty()) return;
    FileInfo& file = files[curFile];

    if (fname.find('.') == string::npos) {
        fname += ".jsp";
    }

    file.fname = fname;
    file.shortname = trimFilename(fname);
    save();
}

bool Editor::import_frame(string fname, bool batch) {
    if (files.empty()) return false;
    FileInfo& file = files[curFile];

    if (file.jsp.frames.size() == 0) return false;

    working("Importing...");

    std::shared_ptr<SDL_Surface> surface(IMG_Load(fname.c_str()), SDL_FreeSurface);
    if (!surface) {
        if (!batch) dialog::error("Failed to import", IMG_GetError());
        return false;
    }

    if (surface->format->format != SDL_PIXELFORMAT_ABGR8888) {
        surface = {SDL_ConvertSurfaceFormat(surface.get(), SDL_PIXELFORMAT_ABGR8888, 0), SDL_FreeSurface};
    }

    if (palette::reduceImage(surface.get()) && !batch) {
        dialog::showMessage("The image's colors were adjusted");
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
    if (files.empty()) return false;
    FileInfo& file = files[curFile];

    if (file.jsp.frames.size() == 0) return false;

    if (fname.find('.') == string::npos) {
        fname += ".png";
    }

    if (IMG_SavePNG(file.jsp.frames[file.curSprite].surface.get(), fname.c_str())) {
        if (!batch) dialog::error("Failed to export", IMG_GetError());
        return false;
    }
    return true;
}

/****************************************************************/
/* Batch import and export */

void Editor::import_batch(string fname) {
    if (files.empty()) return;
    FileInfo& file = files[curFile];

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
    if (files.empty()) return;
    FileInfo& file = files[curFile];

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
    if (files.empty()) return;
    FileInfo& file = files[curFile];

    if (file.jsp.frames.empty()) return;

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
    if (files.empty()) return;
    FileInfo& file = files[curFile];

    if (file.jsp.frames.empty()) return;

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
    if (files.empty()) return;
    FileInfo& file = files[curFile];

    if (file.jsp.frames.empty()) return;

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
                file.unsaved = true;
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

void Editor::working(const std::string& text) {
    render();
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 196);
    SDL_RenderFillRect(renderer, NULL);
    DrawText(renderer, gFont, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 - 50, ALIGN_CENTER, { 0, 0, 0, 255 }, text.c_str());
    SDL_RenderPresent(renderer);
}

void Editor::render() {
    SDL_Color lightbg = {200, 200, 200, 255};
    SDL_Color black = {0, 0, 0, 255};
    SDL_Rect region;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    const int CENTER_X = 180 + (DISPLAY_WIDTH - 180)/2, CENTER_Y = 30 + (DISPLAY_HEIGHT - 30)/2;

    if (browsing) {
        if (!files.empty()) {
            FileInfo& file = files[curFile];
            int x = 200, y = 50, h = 0;
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
                region = {x, y, frame.surface->w, frame.surface->h};
                SDL_RenderCopy(renderer, frame.texture.get(), nullptr, &region);

                if (i == file.curSprite) {
                    // red box
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    region.x -= 1; region.y -= 1; region.w += 2; region.h += 2;
                    SDL_RenderDrawRect(renderer, &region);
                }

                x += frame.surface->w;
                if (frame.surface->h > h) {
                    h = frame.surface->h;
                }
            }
        }
    } else {
        // crosshairs in behind
        if (crosshairs == 1) {
            SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
            SDL_RenderDrawLine(renderer, 180, CENTER_Y, DISPLAY_WIDTH, CENTER_Y);
            SDL_RenderDrawLine(renderer, CENTER_X, 30, CENTER_X, DISPLAY_HEIGHT);
            SDL_SetRenderDrawColor(renderer, 0, 196, 0, 255);
            region = {CENTER_X - 16, CENTER_Y - 12, 32, 24};
            SDL_RenderDrawRect(renderer, &region);
        }

        // draw sprite
        if (!files.empty()) {
            FileInfo& file = files[curFile];
            if (file.jsp.frames.size() > 0) {
                JspFrame current = file.jsp.frames[file.curSprite];
                region = {
                    CENTER_X - current.ofsX,
                    CENTER_Y - current.ofsY,
                    current.surface->w,
                    current.surface->h,
                };
                SDL_RenderCopy(renderer, current.texture.get(), nullptr, &region);
            }
        }

        // crosshairs in front
        if (crosshairs == 2) {
            SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
            SDL_RenderDrawLine(renderer, 180, CENTER_Y, DISPLAY_WIDTH, CENTER_Y);
            SDL_RenderDrawLine(renderer, CENTER_X, 30, CENTER_X, DISPLAY_HEIGHT);
            SDL_SetRenderDrawColor(renderer, 0, 196, 0, 255);
            region = {CENTER_X - 16, CENTER_Y - 12, 32, 24};
            SDL_RenderDrawRect(renderer, &region);
        }
    }

    // left bar
    SDL_SetRenderDrawColor(renderer, lightbg.r, lightbg.g, lightbg.b, lightbg.a);
    region = {0, 0, 180, DISPLAY_HEIGHT};
    SDL_RenderFillRect(renderer, &region);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawLine(renderer, 180, 0, 180, DISPLAY_HEIGHT);
    SDL_RenderDrawLine(renderer, 0, 200, 180, 200);
    SDL_RenderDrawLine(renderer, 0, DISPLAY_HEIGHT - 24, 180, DISPLAY_HEIGHT - 24);

    // file stats
    if (!files.empty()) {
        FileInfo& file = files[curFile];

        std::string name = file.shortname;
        if (name.empty()) {
            name = "untitled";
        }
        if (file.unsaved) {
            name += " *";
        }
        DrawText(renderer, gFont, 5, 35, ALIGN_LEFT, black, name.c_str());

        const vector<JspFrame> &frames = file.jsp.frames;
        std::ostringstream s;
        s << "Sprite count: " << frames.size();
        DrawText(renderer, gFont, 5, 55, ALIGN_LEFT, black, s.str().c_str());
        if (!frames.empty()) {
            JspFrame current = frames[file.curSprite];

            s.str(""); s << file.curSprite;
            DrawText(renderer, gFont, 88, 80, ALIGN_CENTER, black, s.str().c_str());
            s.str(""); s << "Size: (" << current.surface->w << ", " << current.surface->h << ")";
            DrawText(renderer, gFont, 5, 105, ALIGN_LEFT, black, s.str().c_str());
            s.str(""); s << "Origin: (" << current.ofsX << ", " << current.ofsY << ")";
            DrawText(renderer, gFont, 5, 125, ALIGN_LEFT, black, s.str().c_str());
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
    }

    // top bar
    SDL_SetRenderDrawColor(renderer, lightbg.r, lightbg.g, lightbg.b, lightbg.a);
    region = { 0, 0, DISPLAY_WIDTH, 30 };
    SDL_RenderFillRect(renderer, &region);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawLine(renderer, 0, 30, DISPLAY_WIDTH, 30);

    // buttons
    int x = 6, y = 4, w = 80, g = 90, h = 22;
    if (gui.button(rect(x, y, w, h), "Open", "Open JSP (Ctrl+O)", { KMOD_LCTRL, SDL_SCANCODE_O })) {
        dialog::open("Open JSP", "jsp", [this](std::string str) { load(str); });
    }
    if (gui.button(rect(x += g, y, w, h), "Close", "Close file (Ctrl+W)", { KMOD_LCTRL, SDL_SCANCODE_W })) {
        if (files.empty()) {
            running = false;
            return;
        }
        FileInfo& file = files[curFile];

        if (file.unsaved && !dialog::showOkCancel("Really close without saving?", false)) return;

        files.erase(files.begin() + curFile);
        if (files.empty()) {
            curFile = 0;
        } else if (curFile >= files.size()) {
            curFile = files.size() - 1;
        }
    }
    if (gui.button(rect(x += g, y, w, h), "Save", "Save JSP (Ctrl+S)", { KMOD_LCTRL, SDL_SCANCODE_S })) {
        save();
    }
    if (gui.button(rect(x += g, y, w, h), "Save As", "Save JSP As (Ctrl+Shift+S)", { KMOD_LCTRL | KMOD_LSHIFT, SDL_SCANCODE_S })) {
        if (files.empty()) return;
        dialog::save("Save JSP", "jsp", [this](std::string str) { saveAs(str); });
    }
    if (gui.button(rect(x += g, y, w, h), "Import", "Import from image (Ctrl+I)", { KMOD_LCTRL, SDL_SCANCODE_I })) {
        if (files.empty()) return;
        if (files[curFile].jsp.frames.size() == 0) return;
        dialog::open("Import Image", "png;bmp;tga;pcx", [this](std::string str) { import_frame(str); });
    }
    if (gui.button(rect(x += g, y, w, h), "Export", "Export to image (Ctrl+E)", { KMOD_LCTRL, SDL_SCANCODE_E })) {
        if (files.empty()) return;
        if (files[curFile].jsp.frames.size() == 0) return;
        dialog::save("Export Image", "png;bmp;tga;pcx", [this](std::string str) { export_frame(str); });
    }
    if (gui.button(rect(x += g, y, w, h), "Import All", "Import from folder (Ctrl+Shift+I)", { KMOD_LCTRL | KMOD_LSHIFT, SDL_SCANCODE_I })) {
        if (files.empty()) return;
        dialog::open("Select first frame (0.png)", "png", [this](std::string str) { import_batch(str); });
    }
    if (gui.button(rect(x += g, y, w, h), "Export All", "Export to folder (Ctrl+Shift+E)", { KMOD_LCTRL | KMOD_LSHIFT, SDL_SCANCODE_E })) {
        if (files.empty()) return;
        dialog::save("Export all frames", "png", [this](std::string str) { export_batch(str); });
    }

    //x = DISPLAY_WIDTH - g;
    if (gui.button(rect(x += g, y, w, h), "Crosshairs", "Cycle crosshairs mode (Ctrl+H)", { KMOD_LCTRL, SDL_SCANCODE_H })) {
        crosshairs = (crosshairs + 1) % MAX_CROSSHAIRS;
    }
    if (gui.button(rect(x += g, y, w, h), "Browse", "Show all frames at once (Ctrl+B)", { KMOD_LCTRL, SDL_SCANCODE_B })) {
        browsing = !browsing;
    }

    x = 10; g = 34; y = 80;
    if (gui.iconButton(x, y, FAChar::fast_backward, "First frame (Home)", { 0, SDL_SCANCODE_HOME })) {
        if (files.empty()) return;
        files[curFile].curSprite = 0;
    }
    if (gui.iconButton(x += g, y, FAChar::arrow_left, "Previous frame (Left)", { 0, SDL_SCANCODE_LEFT })) {
        move(-1);
    }
    x += g;
    if (gui.iconButton(x += g, y, FAChar::arrow_right, "Next frame (Right)", { 0, SDL_SCANCODE_RIGHT })) {
        move(1);
    }
    if (gui.iconButton(x += g, y, FAChar::fast_forward, "Last frame (End)", { 0, SDL_SCANCODE_END })) {
        if (files.empty()) return;
        FileInfo& file = files[curFile];
        file.curSprite = file.jsp.frames.size() - 1;
    }

    if (gui.iconButton(146, 125, FAChar::undo, "Reset origin (Ctrl+R)", { KMOD_LCTRL, SDL_SCANCODE_R })) {
        if (files.empty()) return;
        FileInfo& file = files[curFile];
        if (file.jsp.frames.empty()) return;
        JspFrame& frame = file.jsp.frames[file.curSprite];
        frame.ofsX = frame.ofsY = 0;
    }

    x = 10; g = 34; y = 150;
    if (gui.iconButton(x, y, FAChar::backward, "Shift frame left (Shift+Left)", { KMOD_LSHIFT, SDL_SCANCODE_LEFT })) {
        shift(-1);
    }
    if (gui.iconButton(x += g, y, FAChar::plus, "Add frame (Ctrl+Insert)", { KMOD_LCTRL, SDL_SCANCODE_INSERT })) {
        if (files.empty()) return;
        FileInfo& file = files[curFile];
        JspFrame newFrame(32, 24);
        file.jsp.frames.insert(file.jsp.frames.begin() + file.curSprite, newFrame);
    }
    if (gui.iconButton(x += g, y, FAChar::font, "Convert pink to alpha (Ctrl+A)", { KMOD_LCTRL, SDL_SCANCODE_A })) {
        convertAlpha();
    }
    if (gui.iconButton(x += g, y, FAChar::minus, "Delete frame (Ctrl+Delete)", { KMOD_LCTRL, SDL_SCANCODE_DELETE })) {
        if (files.empty()) return;
        FileInfo& file = files[curFile];
        if (file.jsp.frames.empty()) return;

        file.jsp.frames.erase(file.jsp.frames.begin() + file.curSprite);

        if (file.jsp.frames.empty()) {
            file.curSprite = 0;
        } else if (file.curSprite >= file.jsp.frames.size()) {
            file.curSprite = file.jsp.frames.size() - 1;
        }
    }
    if (gui.iconButton(x += g, y, FAChar::forward, "Shift frame right (Shift+Right)", { KMOD_LSHIFT, SDL_SCANCODE_RIGHT })) {
        shift(1);
    }

    gui.render();
}

void Editor::handleEvent(const SDL_Event &event) {
    if (gui.handleEvent(event)) {
        return;
    }

    GuiRect dragArea = { 181, 31, DISPLAY_WIDTH, DISPLAY_HEIGHT };

    switch (event.type) {
    case SDL_QUIT:
        // TODO: check all files, not just current
        if (!files.empty() && files[curFile].unsaved) {
            running = !dialog::showOkCancel("You have unsaved changes, do you want to quit anyways?", false);
        } else {
            running = false;
        }
        break;

    case SDL_DROPFILE:
        load(event.drop.file);
        SDL_free(event.drop.file);
        break;

    case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
            DISPLAY_WIDTH = event.window.data1;
            DISPLAY_HEIGHT = event.window.data2;
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
        if (!browsing && (dragArea.left <= event.button.x && event.button.x <= dragArea.right && dragArea.top <= event.button.y && event.button.y <= dragArea.bottom) && event.button.button == 1) {
            dragging = true;
        }
        break;

    case SDL_MOUSEBUTTONUP:
        if (event.button.button == 1) {
            dragging = false;
        }
        break;

    case SDL_MOUSEMOTION: {
        if (files.empty()) break;
        FileInfo& file = files[curFile];
        if (file.jsp.frames.empty() || !dragging) break;
        file.jsp.frames[file.curSprite].ofsX -= event.motion.xrel;
        file.jsp.frames[file.curSprite].ofsY -= event.motion.yrel;
        break;
    }

    case SDL_MOUSEWHEEL:
        if (event.wheel.y > 0) {
            move(-1);
        } else if (event.wheel.y < 0) {
            move(1);
        }
        break;

    case SDL_KEYDOWN: {
        if (files.empty()) break;
        FileInfo& file = files[curFile];
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
        default:
            break;
        }
        break;
    }
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
        SDL_RenderPresent(renderer);
        SDL_Event e;
        while (SDL_PollEvent(&e))
            ed.handleEvent(e);
    }
}

} // namespace editor
