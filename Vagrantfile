# -*- mode: ruby -*-
# vi: set ft=ruby :
VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  config.vm.define "operativos_vm" do |vm_config|
    vm_config.ssh.forward_agent = true
    vm_config.vm.box = "chef/ubuntu-14.04"
    vm_config.vm.network "private_network", ip: "192.168.33.50"
    vm_config.vm.synced_folder "./", "/var/vagrant", type: "nfs"
    vm_config.vm.provision :shell, privileged: false, path: "ci_jobs/vagrant_provision.sh"
  end
end
