#ifndef VANILLA_EXTRACT_H
#define VANILLA_EXTRACT_H

#include <stdio.h>
#include <memory>
#include <string>
#include <vector>
#include <set>

struct SDL_RWops;

namespace vanilla
{
	// Forward declarations
	class Vfs;
	class WriteVfs;

	// Helpers
	int mkdir_parents(const char* path);
	FILE* fp_from_bundle(const char* file, const char* mode, SDL_RWops* rw, const char* tempdir, bool reuse_safe);

	// Available providers
	std::unique_ptr<WriteVfs> open_stdio(const char* prefix);
	std::unique_ptr<Vfs> open_nsis(SDL_RWops* rw);
	std::unique_ptr<Vfs> open_inno(FILE* fp);
	std::unique_ptr<Vfs> open_zip(const char* filename);
#if defined(__ANDROID__) && __ANDROID__
	std::unique_ptr<Vfs> open_android();
#endif

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

		virtual FILE* open_stdio(const char* filename) = 0;
		virtual SDL_RWops* open_sdl(const char* filename) = 0;
		virtual bool list_dir(const char* directory, std::set<std::string>& output) = 0;
	};

	// A single writeable VFS provider.
	class WriteVfs : public Vfs
	{
	public:
		WriteVfs() {}
		virtual ~WriteVfs() {}

		virtual FILE* open_write_stdio(const char* filename) = 0;
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
	struct VfsStack
	{
		std::vector<Mount> mounts;
		std::unique_ptr<WriteVfs> write_mount;
	public:
		VfsStack() {}

		void push_back(std::unique_ptr<Vfs>&& entry, std::string mountpoint = "");

		// Returns true if this VfsStack is empty and therefore not useable.
		bool empty() const { return mounts.empty() && !write_mount; }

		// Get the Vfs that should be used for writing.
		WriteVfs* appdata() { return write_mount.get(); }

		// Forward to children
		SDL_RWops* open_sdl(const char* filename);
	};
}

#endif  // VANILLA_EXTRACT_H
