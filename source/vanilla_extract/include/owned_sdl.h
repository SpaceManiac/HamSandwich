#ifndef OWNED_SDL_H
#define OWNED_SDL_H

#include <memory>
#include <SDL.h>
#include "rw_functions.h"
#include "owned_stdio.h"

// ----------------------------------------------------------------------------
// SDL_RWops

namespace owned
{
	namespace _deleter
	{
		struct SDL_RWops
		{
			void operator()(::SDL_RWops* ptr)
			{
				if (ptr && SDL_RWclose(ptr) != 0)
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_RWclose: %s", SDL_GetError());
				}
			}
		};
	}

	typedef std::unique_ptr<::SDL_RWops, _deleter::SDL_RWops> SDL_RWops;

	inline SDL_RWops SDL_RWFromFP(::FILE* fp)
	{
		return SDL_RWops { ::SDL_RWFromFP(fp, SDL_FALSE) };
	}

	inline SDL_RWops SDL_RWFromFP(owned::FILE fp)
	{
		return SDL_RWops { ::SDL_RWFromFP(fp.release(), SDL_TRUE) };
	}

	inline SDL_RWops SDL_RWFromConstMem(const void *mem, int size)
	{
		return SDL_RWops { ::SDL_RWFromConstMem(mem, size) };
	}

	inline SDL_RWops SDL_RWFromFile(const char *file, const char *mode)
	{
		return SDL_RWops { ::SDL_RWFromFile(file, mode) };
	}
}

inline Sint64 SDL_RWsize(const owned::SDL_RWops& context)
{
	return context->size(context.get());
}

inline Sint64 SDL_RWseek(const owned::SDL_RWops& context, Sint64 offset, int whence)
{
	return context->seek(context.get(), offset, whence);
}

inline Sint64 SDL_RWtell(const owned::SDL_RWops& context)
{
	return context->seek(context.get(), 0, RW_SEEK_CUR);
}

inline size_t SDL_RWread(const owned::SDL_RWops& context, void *ptr, size_t size, size_t maxnum)
{
	return context->read(context.get(), ptr, size, maxnum);
}

inline size_t SDL_RWwrite(const owned::SDL_RWops& context, const void *ptr, size_t size, size_t num)
{
	return context->write(context.get(), ptr, size, num);
}

[[nodiscard]] // If you don't care about the return value, use .reset() instead.
inline int SDL_RWclose(owned::SDL_RWops context)
{
	return SDL_RWclose(context.release());
}

// ----------------------------------------------------------------------------
// SDL_Surface

namespace owned
{
	namespace _deleter
	{
		struct SDL_Surface
		{
			void operator()(::SDL_Surface* ptr) { return SDL_FreeSurface(ptr); }
		};
	}

	typedef std::unique_ptr<::SDL_Surface, _deleter::SDL_Surface> SDL_Surface;

	inline SDL_Surface SDL_LoadBMP_RW(::SDL_RWops* rw)
	{
		return SDL_Surface { SDL_LoadBMP_RW(rw, SDL_FALSE) };
	}

	inline SDL_Surface SDL_LoadBMP_RW(owned::SDL_RWops rw)
	{
		return SDL_Surface { SDL_LoadBMP_RW(rw.release(), SDL_TRUE) };
	}
}

// ----------------------------------------------------------------------------
// SDL_Window

namespace owned
{
	namespace _deleter
	{
		struct SDL_Window
		{
			void operator()(::SDL_Window* ptr) { return SDL_DestroyWindow(ptr); }
		};
	}

	typedef std::unique_ptr<::SDL_Window, _deleter::SDL_Window> SDL_Window;
}

// ----------------------------------------------------------------------------
// SDL_Renderer

namespace owned
{
	namespace _deleter
	{
		struct SDL_Renderer
		{
			void operator()(::SDL_Renderer* ptr) { return SDL_DestroyRenderer(ptr); }
		};
	}

	typedef std::unique_ptr<::SDL_Renderer, _deleter::SDL_Renderer> SDL_Renderer;
}

// ----------------------------------------------------------------------------
// SDL_Texture

namespace owned
{
	namespace _deleter
	{
		struct SDL_Texture
		{
			void operator()(::SDL_Texture* ptr) { return SDL_DestroyTexture(ptr); }
		};
	}

	typedef std::unique_ptr<::SDL_Texture, _deleter::SDL_Texture> SDL_Texture;
}

// ----------------------------------------------------------------------------
// SDL_GLContext

namespace owned
{
	namespace _deleter
	{
		struct SDL_GLContext
		{
			void operator()(::SDL_GLContext ptr) { return SDL_GL_DeleteContext(ptr); }
		};
	}

	typedef std::unique_ptr<void, _deleter::SDL_GLContext> SDL_GLContext;
}

// ----------------------------------------------------------------------------
// SDL_Joystick

namespace owned
{
	namespace _deleter
	{
		struct SDL_Joystick
		{
			void operator()(::SDL_Joystick* ptr) { return SDL_JoystickClose(ptr); }
		};
	}

	typedef std::unique_ptr<::SDL_Joystick, _deleter::SDL_Joystick> SDL_Joystick;

	inline SDL_Joystick SDL_JoystickOpen(int device_index)
	{
		return SDL_Joystick { ::SDL_JoystickOpen(device_index) };
	}

	// NB: SDL_JoystickFromInstanceID does not return an owned ptr
}

// ----------------------------------------------------------------------------
// SDL_GameController

namespace owned
{
	namespace _deleter
	{
		struct SDL_GameController
		{
			void operator()(::SDL_GameController* ptr) { return SDL_GameControllerClose(ptr); }
		};
	}

	typedef std::unique_ptr<::SDL_GameController, _deleter::SDL_GameController> SDL_GameController;

	inline SDL_GameController SDL_GameControllerOpen(int device_index)
	{
		return SDL_GameController { ::SDL_GameControllerOpen(device_index) };
	}
}

#endif
