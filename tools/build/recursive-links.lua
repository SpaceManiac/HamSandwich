--[[ recursive-links.lua
Apply `links` configurations recursively. ]]

local _project = project
local _links = links
local _dependson = dependson

local _recursive_links = {}
local _current_recursive_links = nil

function project(name)
	_project(name)

	_current_recursive_links = {}
	_recursive_links[name] = _current_recursive_links
end

local function flatten(inner_func)
	return function(name)
		if type(name) == 'table' then
			for _, v in ipairs(name) do
				inner_func(v)
			end
		else
			inner_func(name)
		end
	end
end

local function wrap_recurse(_original)
	return function(name)
		_original(name)

		table.insert(_current_recursive_links, name)
		local their_links = _recursive_links[name]
		if their_links ~= nil then
			includedirs { "source/" .. name }
			for _, v in ipairs(their_links) do
				_original(v)
				table.insert(_current_recursive_links, v)
			end
		end
	end
end

-- Link X, and also everything X depends on.
links = flatten(wrap_recurse(_links))
-- Depend on X, and also everything X depends on.
dependson = flatten(wrap_recurse(_dependson))
