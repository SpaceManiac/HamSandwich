#include "vec_rw.h"
#include <SDL3/SDL_iostream.h>
#include <utility>
#include <algorithm>
#include <stddef.h>

namespace vanilla
{

	struct Vec_RWops
	{
		std::vector<uint8_t> buffer;
		ptrdiff_t cursor;
		SDL_PropertiesID props;
	};

	static int64_t vec_size(void* ud)
	{
		Vec_RWops* vec = (Vec_RWops*)ud;
		return vec->buffer.size() - vec->cursor;
	}

	static int64_t vec_seek(void* ud, int64_t offset, SDL_IOWhence whence)
	{
		Vec_RWops* vec = (Vec_RWops*)ud;
		switch (whence)
		{
			case SDL_IO_SEEK_SET:
				vec->cursor = offset;
				break;
			case SDL_IO_SEEK_CUR:
				vec->cursor += offset;
				break;
			case SDL_IO_SEEK_END:
				vec->cursor = vec->buffer.size() + offset;
				break;
			default:
				SDL_SetError("Unknown value for 'whence'");
				return -1;
		}
		vec->cursor = std::clamp(vec->cursor, (ptrdiff_t)0, (ptrdiff_t)vec->buffer.size());
		return vec->cursor;
	}

	static size_t vec_read(void* ud, void* dest, size_t size, SDL_IOStatus* status)
	{
		Vec_RWops* vec = (Vec_RWops*)ud;
		size_t available = vec->buffer.size() - vec->cursor;
		if (size > available)
		{
			size = available;
			*status = SDL_IO_STATUS_EOF;
		}
		memcpy(dest, &vec->buffer[vec->cursor], size);
		vec->cursor += size;
		return size;
	}

	static bool vec_close(void* ud)
	{
		Vec_RWops* vec = (Vec_RWops*)ud;
		delete vec;
		return true;
	}

	static const SDL_IOStreamInterface VEC_IO =
	{
		.version = sizeof(SDL_IOStreamInterface),
		.size = vec_size,
		.seek = vec_seek,
		.read = vec_read,
		.write = nullptr,
		.flush = nullptr,
		.close = vec_close,
	};

	owned::SDL_IOStream create_vec_rwops(std::vector<uint8_t> &&buffer)
	{
		uint8_t* data = buffer.data();
		size_t size = buffer.size();
		Vec_RWops* vec = new Vec_RWops{std::move(buffer), 0, 0};
		owned::SDL_IOStream rw { SDL_OpenIO(&VEC_IO, vec) };
		if (!rw)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_OpenIO(Vec_RWops): %s", SDL_GetError());
			delete vec;
			return nullptr;
		}
        SDL_PropertiesID props = SDL_GetIOProperties(rw.get());
        if (props) {
            vec->props = props;
            SDL_SetPointerProperty(props, SDL_PROP_IOSTREAM_MEMORY_POINTER, data);
            SDL_SetNumberProperty(props, SDL_PROP_IOSTREAM_MEMORY_SIZE_NUMBER, size);
        }
		return rw;
	}

} // namespace vanilla
