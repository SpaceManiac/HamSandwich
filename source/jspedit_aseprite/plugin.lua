-- Plugin entry point. Registers commands which activate jspedit.lua on use.
function init(plugin)
	plugin:newCommand {
		id = "jsp_import",
		title = "Import JSP or JFT",
		group = "file_import_1",
		onclick = function()
			dofile(app.fs.joinPath(plugin.path, "jspedit.lua")).import(plugin)
		end,
	}

	plugin:newCommand {
		id = "jsp_export",
		title = "Export JSP",
		group = "file_export_1",
		onclick = function()
			dofile(app.fs.joinPath(plugin.path, "jspedit.lua")).export(plugin)
		end,
		onenabled = function ()
			return app.sprite ~= nil
		end,
	}
end
