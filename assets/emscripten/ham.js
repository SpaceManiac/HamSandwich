// ----------------------------------------------------------------------------
// General functionality
window.onerror = function(event) {
	var status = document.getElementById('status');
	if (status) {
		status.innerText = "Error! See browser console.";
	}
};

var HamSandwich = (function () {
	var metadata = JSON.parse(document.getElementById('ham-meta').textContent);

	// ------------------------------------------------------------
	// File system syncing
	var FS_IDLE = 0, FS_WORKING = 1, FS_PENDING = 2;
	var appdataMount, installerMount, needInstallerSync;
	var fsStatus = FS_WORKING;  // waiting for preInit to run

	function fsCallback(err) {
		if (err) {
			Module.printErr('FS sync error:', err);
		}
		if (fsStatus == FS_PENDING) {
			fsSyncInner();
		} else {
			fsStatus = FS_IDLE;
			Module.setStatus("");
		}
	}

	function fsSyncInner() {
		fsStatus = FS_WORKING;
		Module.setStatus("Saving...");
		if (needInstallerSync) {
			needInstallerSync = false;
			FS.syncfs(false, fsCallback);
		} else {
			appdataMount.type.syncfs(appdataMount, false, fsCallback);
		}
	}

	function fsSync(options) {
		needInstallerSync = needInstallerSync || options?.installers;
		if (fsStatus) {
			fsStatus = FS_PENDING;
		} else {
			fsSyncInner();
		}
	}

	function fsInitCallback(err) {
		fsCallback(err);
		InstallerUpload.onFsInit();
	}

	function fsInit() {
		FS.mkdir('/appdata');
		appdataMount = FS.mount(IDBFS, {}, '/appdata').mount;
		FS.mkdir('/installers');
		installerMount = FS.mount(IDBFS, {}, '/installers').mount;
		FS.syncfs(true, fsInitCallback);

		Module.ENV['HSW_APPDATA'] = 'appdata/' + HamSandwich.metadata.appdataName;
		pushAssets(null, 'stdio', '.');
	}

	var assetCounter = 0;
	function pushAssets(mountpoint) {
		mountpoint = mountpoint || '';
		Module.ENV['HSW_ASSETS_' + (assetCounter++)] = Array.prototype.join.call(arguments, '@');
	}

	// ------------------------------------------------------------
	// Module overrides
	function setWindowTitle(text) {
		document.title = text;
		var title = document.getElementById('title');
		title.hidden = !text;
		title.innerText = text;
	}
	setWindowTitle(metadata.title);

	function setRunStatus(text, callback) {
		var notice = document.getElementById('runstatus-notice');
		if (notice) {
			notice.hidden = !text;
			notice.innerText = text;
			if (callback) {
				notice.ariaRoleDescription = 'button';
				notice.style.cursor = 'pointer';
				notice.addEventListener('click', callback);
			}
			return notice;
		}
	}

	function toggleMenu(state) {
		var menuWindow = document.getElementById('menu');
		menuWindow.hidden = !(state ?? menuWindow.hidden);
		document.body.classList.toggle('menu-open', !menuWindow.hidden);
	}

	return {
		metadata,
		fsSync,
		fsInit,
		setWindowTitle,
		pushAssets,
		setRunStatus,
		toggleMenu,
	};
})();

// ----------------------------------------------------------------------------
// Installer upload
var InstallerUpload = (function () {
	var installerUpload = document.getElementById('installer-upload');
	var table = installerUpload.querySelector('table');
	var meta = HamSandwich.metadata.installers;

	function setSpinner(td) {
		var outer = document.createElement('div');
		outer.className = 'spinner';
		var inner = document.createElement('div');
		inner.className = 'loading-pulse';
		outer.appendChild(inner);
		td.innerText = '';
		td.appendChild(outer);
	}

	var status = {};
	installerUpload.hidden = true;
	for (let installer of meta) {
		installerUpload.hidden = false;

		let statusTd = document.createElement('td');
		setSpinner(statusTd);

		var fnameTd = document.createElement('td');
		if (installer.optional)
			fnameTd.append('Optional: ');

		var a = document.createElement('a');
		a.href = installer.link;
		a.innerText = installer.filename;
		a.target = '_blank';
		fnameTd.appendChild(a);

		var fileTd = document.createElement('td');
		let fileInput = document.createElement('input');
		fileInput.type = 'file';
		fileInput.addEventListener('change', async () => {
			setSpinner(statusTd);
			let buf = await fileInput.files[0].arrayBuffer();
			FS.writeFile('/installers/' + installer.filename, new Uint8Array(buf));
			checkFsFile(installer, true);
		});
		fileTd.appendChild(fileInput);

		var tr = document.createElement('tr');
		tr.appendChild(statusTd);
		tr.appendChild(fnameTd);
		tr.appendChild(fileTd);
		table.appendChild(tr);

		status[installer.filename] = {
			statusTd,
			fileInput,
			accepted: false,
			sha256sum: installer.sha256sum,
		};
	}

	async function downloadInstaller(installer) {
		try {
			status[installer.filename].fileInput.disabled = true;
			let link = installer.link;
			if (installer.file_id) {
				link = `https://wombat.platymuus.com/hamsandwich/itch_proxy.php?url=${installer.link}/file/${installer.file_id}`;
			}

			let response = await fetch(link);
			let buf = await response.arrayBuffer();
			FS.writeFile('/installers/' + installer.filename, new Uint8Array(buf));
			checkFsFile(installer, true);
		} catch {
			status[installer.filename].fileInput.disabled = false;
		}
	}

	async function checkFsFile(installer, sync) {
		let fname = installer.filename;

		let buffer;
		try {
			buffer = FS.readFile('/installers/' + fname);
		} catch (e) {
			if (!sync && !installer.optional) {
				downloadInstaller(installer);
				return;
			}

			status[fname].statusTd.innerText = '\u2014';
			if (!installer.optional) {
				HamSandwich.toggleMenu(true);
				HamSandwich.setRunStatus('Provide the installer to play.', () => HamSandwich.toggleMenu(true));
			}
			return;
		}

		// Warning: `crypto.subtle` is only available on HTTPS sites.
		let hashBuffer = await crypto.subtle.digest('SHA-256', buffer);
		let hexdigest = Array.from(new Uint8Array(hashBuffer)).map(b => b.toString(16).padStart(2, '0')).join('');
		if (hexdigest === status[fname].sha256sum) {
			status[fname].accepted = true;
			status[fname].statusTd.innerText = 'Ok';
			status[fname].fileInput.disabled = true;
			status[fname].fileInput.hidden = true;
			if (sync) {
				HamSandwich.fsSync({ installers: true });
			}
			if (!installer.optional) {
				Module.removeRunDependency('installer ' + fname);
			}
		} else {
			status[fname].statusTd.innerText = 'Bad';
			HamSandwich.toggleMenu(true);
			HamSandwich.setRunStatus('Provide the installer to play.', () => HamSandwich.toggleMenu(true));
		}
	}

	function preInit() {
		for (var installer of meta) {
			if (!installer.optional) {
				Module.addRunDependency('installer ' + installer.filename);
			}
		}
	}

	function onFsInit() {
		for (var installer of meta) {
			checkFsFile(installer, false);
			if (installer.optional) {
				try {
					FS.stat('/installers/' + installer.filename);
					HamSandwich.pushAssets(installer.mountpoint, installer.kind, '/installers/' + installer.filename);
				} catch {}
			} else {
				HamSandwich.pushAssets(installer.mountpoint, installer.kind, '/installers/' + installer.filename);
			}
		}
	}

	return {
		preInit,
		onFsInit,
	}
})();

// ----------------------------------------------------------------------------
// Emscripten imports and exports
var Module = (function() {
	var status = document.getElementById('status');
	var spinner = document.getElementById('spinner');
	var canvas = document.getElementById('canvas');

	var totalDependencies = 0, unsatisfied = 0;
	var hasContext = true;

	for (let button of document.getElementsByClassName('js-fullscreen')) {
		button.addEventListener('click', function() {
			Module.canvas.requestFullscreen();
			setTimeout(() => button.blur());
		});
	}

	document.getElementById('export')?.addEventListener('click', async function() {
		function saveAs(file, filename) {
			if (window.navigator.msSaveOrOpenBlob) { // IE10+
				window.navigator.msSaveOrOpenBlob(file, filename);
			} else { // Others
				var a = document.createElement("a"),
						url = URL.createObjectURL(file);
				a.href = url;
				a.download = filename;
				document.body.appendChild(a);
				a.click();
				setTimeout(function() {
					document.body.removeChild(a);
					window.URL.revokeObjectURL(url);
				}, 0);
			}
		}

		function collect(zip, fsPath) {
			let list;
			try {
				list = FS.readdir(fsPath);
			} catch {
				alert('No save to export!');
				return false;
			}
			for (var item of list) {
				if (item == "." || item == "..") continue;
				var fullItem = fsPath + "/" + item;
				if (FS.isDir(FS.stat(fullItem).mode)) {
					collect(zip.folder(item), fullItem);
				} else {
					zip.file(item, FS.readFile(fullItem));
				}
			}
			return true;
		}

		Module.setStatus("Zipping...");
		var zip = new JSZip();
		if (!collect(zip, '/appdata/' + HamSandwich.metadata.appdataName)) {
			// If there's nothing to export.
			Module.setStatus("");
			return;
		}
		let content = await zip.generateAsync({ type: "blob" });
		saveAs(content, "HamSandwich Saves.zip");
		Module.setStatus("");
	});

	// As a default initial behavior, pop up an alert when webgl context is lost. To make your
	// application robust, you may want to override this behavior before shipping!
	// See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
	canvas.addEventListener('webglcontextlost', function(e) {
		if (hasContext) alert("WebGL context lost. You will need to reload the page.");
		hasContext = false; // only show this debug message once
		e.preventDefault();
	});

	// Only items which Emscripten actually interfaces with should be set here.
	return {
		canvas: canvas,

		print: function(text) {
			if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(" ");
			console.log(text);
		},
		printErr: function(text) {
			if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(" ");
			if (!text.startsWith('Preparing... (')) {
				console.warn(text);
			}
		},
		setStatus: function(text) {
			var matches = /^Downloading data\.\.\. \((\d+)\/(\d+)\)$/.exec(text);
			if (matches) {
				var loaded = parseInt(matches[1]), total = parseInt(matches[2]);
				var pct = Math.round(loaded * 1000 / total) / 10;
				text = "Downloading data (" + pct + "%)";
			}

			if (spinner) spinner.hidden = !text;
			status.innerText = text;
		},
		monitorRunDependencies: function(left) {
			totalDependencies += Math.max(0, left - unsatisfied);
			unsatisfied = left;
			Module.setStatus("Preparing... (" + (totalDependencies - left) + "/" + totalDependencies + ")");
		},

		hamQuit: function () {
			if (history.length > 1) {
				// If there's history to go back to, go back (probably to index.html).
				history.back();
			} else if (!location.pathname.endsWith('/') && !location.pathname.endsWith('/index.html')) {
				// If we're not index.html or /, go to index.html.
				location = '.';  // Go to index.html.
			}
		},

		preInit: [
			function() {
				Module.requestFullscreen = function() {
					Module.canvas.requestFullscreen();
				};
				setWindowTitle = function(title) {
					HamSandwich.setWindowTitle(title);
					canvas.style.opacity = '1';
					canvas.style.pointerEvents = 'auto';
				};
				quit_ = function(code) {
					if (!code) {
						Module.hamQuit();
					}

					// If the above didn't actually navigate us anywhere...
					canvas.style.opacity = '0';
					canvas.style.pointerEvents = 'none';

					HamSandwich.setRunStatus('Refresh or click here to play again.', () => {
						window.location.reload();
					});
				};
			},
			InstallerUpload.preInit,
			HamSandwich.fsInit,
		],

		postRun: function() {
			HamSandwich.toggleMenu(false);
		},
	};
})();

Module.setStatus("Downloading...");
window.onerror = function(event) {
	Module.setStatus("Error! See browser console.");
	var spinner = document.getElementById('spinner');
	if (spinner) spinner.hidden = false;
	Module.setStatus = Module.printErr;
};

for (let close of document.getElementsByClassName('js-close')) {
	close.addEventListener('click', () => {
		Module.hamQuit();
		close.remove();
	});
	if (history.length <= 1 && (location.pathname.endsWith('/') || location.pathname.endsWith('/index.html'))) {
		// Hide the close button if Module.hamQuit() definitely wouldn't succeed.
		close.remove();
	}
}
