#ifndef OWNED_SDL_H
#define OWNED_SDL_H

#include <memory>
#include <SDL3/SDL.h>
#include "rw_functions.h"
#include "owned_stdio.h"

// ----------------------------------------------------------------------------
// SDL_IOStream

namespace owned
{
	namespace _deleter
	{
		struct SDL_IOStream
		{
			void operator()(::SDL_IOStream* ptr)
			{
				if (ptr && SDL_CloseIO(ptr) != 0)
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CloseIO: %s", SDL_GetError());
				}
			}
		};
	}

	typedef std::unique_ptr<::SDL_IOStream, _deleter::SDL_IOStream> SDL_IOStream;

	inline SDL_IOStream SDL_RWFromFP(::FILE* fp)
	{
		return SDL_IOStream { ::SDL_RWFromFP(fp, false) };
	}

	inline SDL_IOStream SDL_RWFromFP(owned::FILE fp)
	{
		return SDL_IOStream { ::SDL_RWFromFP(fp.release(), true) };
	}

	inline SDL_IOStream SDL_IOFromConstMem(const void *mem, int size)
	{
		return SDL_IOStream { ::SDL_IOFromConstMem(mem, size) };
	}

	inline SDL_IOStream SDL_IOFromFile(const char *file, const char *mode)
	{
		return SDL_IOStream { ::SDL_IOFromFile(file, mode) };
	}
}

inline Sint64 SDL_GetIOSize(const owned::SDL_IOStream& context)
{
	return context->size(context.get());
}

inline Sint64 SDL_SeekIO(const owned::SDL_IOStream& context, Sint64 offset, int whence)
{
	return context->seek(context.get(), offset, whence);
}

inline Sint64 SDL_TellIO(const owned::SDL_IOStream& context)
{
	return context->seek(context.get(), 0, SDL_IO_SEEK_CUR);
}

inline size_t SDL_ReadIO(const owned::SDL_IOStream& context, void *ptr, size_t size, size_t maxnum)
{
	return context->read(context.get(), ptr, size, maxnum);
}

inline size_t SDL_WriteIO(const owned::SDL_IOStream& context, const void *ptr, size_t size, size_t num)
{
	return context->write(context.get(), ptr, size, num);
}

[[nodiscard]] // If you don't care about the return value, use .reset() instead.
inline int SDL_CloseIO(owned::SDL_IOStream context)
{
	return SDL_CloseIO(context.release());
}

// ----------------------------------------------------------------------------
// SDL_Surface

namespace owned
{
	namespace _deleter
	{
		struct SDL_Surface
		{
			void operator()(::SDL_Surface* ptr) { return SDL_DestroySurface(ptr); }
		};
	}

	typedef std::unique_ptr<::SDL_Surface, _deleter::SDL_Surface> SDL_Surface;

	inline SDL_Surface SDL_LoadBMP_IO(::SDL_IOStream* rw)
	{
		return SDL_Surface { SDL_LoadBMP_IO(rw, false) };
	}

	inline SDL_Surface SDL_LoadBMP_IO(owned::SDL_IOStream rw)
	{
		return SDL_Surface { SDL_LoadBMP_IO(rw.release(), true) };
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
			void operator()(::SDL_GLContext ptr) { return SDL_GL_DestroyContext(ptr); }
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
			void operator()(::SDL_Joystick* ptr) { return SDL_CloseJoystick(ptr); }
		};
	}

	typedef std::unique_ptr<::SDL_Joystick, _deleter::SDL_Joystick> SDL_Joystick;

	inline SDL_Joystick SDL_OpenJoystick(int device_index)
	{
		return SDL_Joystick { ::SDL_OpenJoystick(device_index) };
	}

	// NB: SDL_GetJoystickFromID does not return an owned ptr
}

// ----------------------------------------------------------------------------
// SDL_Gamepad

namespace owned
{
	namespace _deleter
	{
		struct SDL_Gamepad
		{
			void operator()(::SDL_Gamepad* ptr) { return SDL_CloseGamepad(ptr); }
		};
	}

	typedef std::unique_ptr<::SDL_Gamepad, _deleter::SDL_Gamepad> SDL_Gamepad;

	inline SDL_Gamepad SDL_OpenGamepad(int device_index)
	{
		return SDL_Gamepad { ::SDL_OpenGamepad(device_index) };
	}
}

#endif
