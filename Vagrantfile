# -*- mode: ruby -*-
# vi: set ft=ruby :

# Documentation: https://docs.vagrantup.com
Vagrant.configure("2") do |config|

	# Windows Server 2016 + VS Build Tools 2022
	config.vm.define "windows" do |node|
		node.vm.box = "mwrock/Windows2016"
		node.vm.provision "shell", path: "tools/windows-vm/provision.ps1"
	end

	# Disable GUI, because it's on by default for the Windows2016 box?
	config.vm.provider "virtualbox" do |vb|
		vb.gui = false
	end

end
