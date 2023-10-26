#ifndef VFS_H
#define VFS_H

#include <stdio.h>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include "owned_stdio.h"
#include "owned_sdl.h"

struct SDL_RWops;

namespace vanilla
{
	bool ends_with(std::string_view haystack, std::string_view suffix);

	struct CaseInsensitive
	{
		bool operator() (const std::string& lhs, const std::string& rhs) const;
	};

	enum class VfsSourceKind
	{
		Unspecified,
		BaseGame,  // Base game content that is known to always be present.
		Addon,     // An optional addon of some type, from an external source.
		Appdata,   // The user's own data, including work-in-progress addons.
	};

	// Description of where a VFS and the files in it came from.
	struct VfsMeta
	{
		VfsSourceKind kind = VfsSourceKind::Unspecified;
		uint64_t steamWorkshopId = 0;
	};

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
		virtual owned::FILE open_stdio(const char* filename);
		virtual bool list_dir(const char* directory, std::set<std::string, CaseInsensitive>& output) = 0;
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
	struct Mount final
	{
		std::unique_ptr<Vfs> vfs;
		std::string mountpoint;
		VfsMeta meta;

		Mount(std::unique_ptr<Vfs>&& vfs, std::string&& mountpoint = "", VfsMeta meta = {})
			: vfs(std::move(vfs))
			, mountpoint(mountpoint)
			, meta(meta)
		{
			// Strip trailing '/' from mountpoint.
			if (!this->mountpoint.empty() && this->mountpoint.back() == '/')
			{
				this->mountpoint.erase(this->mountpoint.size() - 1);
			}
		}

		const char* matches(const char* filename) const;
		owned::SDL_RWops open_sdl(const char* filename);
		owned::FILE open_stdio(const char* filename);
	};

	// A full filesystem, including a list of mounts and the write (appdata) mount.
	class VfsStack final
	{
	public:
		std::vector<Mount> mounts;
		std::unique_ptr<WriteVfs> write_mount;

		VfsStack() {}

		void push_back(Mount mount)
		{
			mounts.push_back(std::move(mount));
		}
		void push_back(std::unique_ptr<Vfs>&& entry, std::string&& mountpoint = "", VfsMeta meta = {})
		{
			mounts.push_back(Mount { std::move(entry), std::move(mountpoint), meta });
		}
		void push_back(std::unique_ptr<Vfs>&& entry, std::string&& mountpoint, VfsSourceKind kind)
		{
			mounts.push_back(Mount { std::move(entry), std::move(mountpoint), VfsMeta { kind, 0 } });
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
		void list_dir(const char* directory, std::set<std::string, CaseInsensitive>& output);
		owned::SDL_RWops open_write_sdl(const char* filename);
		owned::FILE open_write_stdio(const char* filename);
		bool delete_file(const char* filename);

		// Query metadata for a given path.
		bool query_bottom(const char* filename, VfsMeta* meta);
	};
}

#endif  // VFS_H
