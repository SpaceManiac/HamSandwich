#ifndef VANILLA_EXTRACT_H
#define VANILLA_EXTRACT_H

#include <stdio.h>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include "owned.h"

struct SDL_RWops;

namespace vanilla
{
	// Helpers
	int mkdir_parents(std::string_view path);
	owned::FILE fp_from_bundle(std::string_view filename, SDL_RWops* rw);

	// A single VFS provider, read-only by default.
	class Vfs
	{
		Vfs(const Vfs&) = delete;
		Vfs(Vfs&&) = delete;
		Vfs& operator=(const Vfs&) = delete;
		Vfs& operator=(Vfs&&) = delete;
	public:
		Vfs() {}
		virtual ~Vfs() {}

		virtual owned::SDL_RWops open_sdl(const char* filename) = 0;
		virtual owned::FILE open_stdio(const char* filename)
		{
			auto sdl = open_sdl(filename);
			return fp_from_bundle(filename, sdl.get());
		}
		virtual bool list_dir(const char* directory, std::set<std::string>& output) = 0;
	};

	// A single writeable VFS provider.
	class WriteVfs : public Vfs
	{
	public:
		WriteVfs() {}
		virtual ~WriteVfs() {}

		virtual owned::FILE open_write_stdio(const char* filename) = 0;
		virtual owned::SDL_RWops open_write_sdl(const char* filename) = 0;
		virtual bool delete_file(const char* filename) = 0;
	};

	// A pair of Vfs and mountpoint.
	struct Mount
	{
		std::unique_ptr<Vfs> vfs;
		std::string mountpoint;

		Mount(std::unique_ptr<Vfs>&& vfs, const std::string& mountpoint = "")
			: vfs(std::move(vfs))
			, mountpoint(mountpoint)
		{
			// Strip trailing '/' from mountpoint.
			if (!this->mountpoint.empty() && this->mountpoint.back() == '/')
			{
				this->mountpoint.erase(this->mountpoint.size() - 1);
			}
		}

		const char* matches(const char* filename) const;
	};

	// A full filesystem, including a list of mounts and the write (appdata) mount.
	class VfsStack
	{
		std::vector<Mount> mounts;
		std::unique_ptr<WriteVfs> write_mount;
	public:
		VfsStack() {}

		void push_back(Mount mount)
		{
			mounts.push_back(std::move(mount));
		}
		void push_back(std::unique_ptr<Vfs>&& entry, std::string mountpoint = "")
		{
			mounts.push_back(Mount { std::move(entry), mountpoint });
		}

		// Returns true if this VfsStack is empty and therefore not useable.
		bool empty() const
		{
			return mounts.empty() && !write_mount;
		}

		// Set the Vfs that should be used for writing.
		std::unique_ptr<WriteVfs> set_appdata(std::unique_ptr<WriteVfs> new_value);

		// Forward to children
		owned::SDL_RWops open_sdl(const char* filename);
		owned::FILE open_stdio(const char* filename);
		void list_dir(const char* directory, std::set<std::string>& output);
		owned::SDL_RWops open_write_sdl(const char* filename);
		owned::FILE open_write_stdio(const char* filename);
		bool delete_file(const char* filename);
	};

	// Available providers
	std::unique_ptr<WriteVfs> open_stdio(std::string_view prefix);
	std::unique_ptr<Vfs> open_nsis(owned::SDL_RWops rw);
	std::unique_ptr<Vfs> open_inno(SDL_RWops* rw);
	std::unique_ptr<Vfs> open_zip(owned::SDL_RWops rw);
#if defined(__ANDROID__) && __ANDROID__
	std::unique_ptr<Vfs> open_android(const char* prefix = nullptr);
#endif
}

#endif  // VANILLA_EXTRACT_H
