#ifndef OWNED_SDL_H
#define OWNED_SDL_H

#include <memory>
#include <SDL3/SDL.h>

// ----------------------------------------------------------------------------
// SDL_malloc / SDL_free

namespace owned
{
	namespace _deleter
	{
		struct SdlMem
		{
			void operator()(void* ptr) { return ::SDL_free(ptr); }
		};
	}

	template<class T>
	using SdlMem = std::unique_ptr<T, _deleter::SdlMem>;
}

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
				if (ptr && !SDL_CloseIO(ptr))
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CloseIO: %s", SDL_GetError());
				}
			}
		};
	}

	typedef std::unique_ptr<::SDL_IOStream, _deleter::SDL_IOStream> SDL_IOStream;

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
	return SDL_GetIOSize(context.get());
}

inline Sint64 SDL_SeekIO(const owned::SDL_IOStream& context, Sint64 offset, SDL_IOWhence whence)
{
	return SDL_SeekIO(context.get(), offset, whence);
}

inline Sint64 SDL_TellIO(const owned::SDL_IOStream& context)
{
	return SDL_TellIO(context.get());
}

inline size_t SDL_ReadIO(const owned::SDL_IOStream& context, void *ptr, size_t size)
{
	return SDL_ReadIO(context.get(), ptr, size);
}

inline size_t SDL_WriteIO(const owned::SDL_IOStream& context, const void *ptr, size_t size)
{
	return SDL_WriteIO(context.get(), ptr, size);
}

inline bool SDL_FlushIO(const owned::SDL_IOStream& context)
{
	return SDL_FlushIO(context.get());
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
// SDL_Palette

namespace owned
{
	namespace _deleter
	{
		struct SDL_Palette
		{
			void operator()(::SDL_Palette* ptr) { return SDL_DestroyPalette(ptr); }
		};
	}

	typedef std::unique_ptr<::SDL_Palette, _deleter::SDL_Palette> SDL_Palette;

	inline SDL_Palette SDL_CreatePalette(int ncolors)
	{
		return SDL_Palette { ::SDL_CreatePalette(ncolors) };
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
			void operator()(::SDL_GLContext ptr)
			{
				if (ptr && !SDL_GL_DestroyContext(ptr))
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_DestroyContext: %s", SDL_GetError());
				}
			}
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
