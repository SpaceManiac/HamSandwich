#ifndef VANILLA_EXTRACT_VFS_H
#define VANILLA_EXTRACT_VFS_H

#include <stdio.h>
#include <memory>
#include <string>
#include <vector>
#include <set>

struct SDL_RWops;

namespace vanilla
{
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

#endif  // VANILLA_EXTRACT_VFS_H
