--[[ recursive-links.lua
Apply `links` configurations recursively. ]]

local _project = project
local _links = links

local _current_project = nil
local _recursive_links = {}

function project(name)
	_current_project = name
	_project(name)
end

function links(name)
	if type(name) == 'table' then
		for _, v in ipairs(name) do
			links(v)
		end
		return
	end

	local our_links = _recursive_links[_current_project]
	if our_links == nil then
		our_links = {}
		_recursive_links[_current_project] = our_links
	end
	table.insert(our_links, name)

	_links(name)
	local their_links = _recursive_links[name]
	if their_links ~= nil then
		includedirs { "source/" .. name }
		for _, v in ipairs(their_links) do
			_links(v)
			table.insert(our_links, v)
		end
	end
end
