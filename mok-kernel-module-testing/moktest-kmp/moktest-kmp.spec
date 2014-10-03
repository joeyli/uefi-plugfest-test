# needssslcertforbuild

Name: moktest
Version: 1
Release: 0
BuildRequires: kernel-source kernel-syms module-init-tools
#BuildRequires: %kernel_module_package_buildreqs
Summary: Dummy Module for MOK testing
Group: System/Tests
# License: GPLv2
License: GPL-2.0
Source0: Makefile
Source1: moktest.c
BuildRoot: /var/tmp/%{name}-%{version}-root
%kernel_module_package default

%description
An dummy module for MOK testing.

%prep
%setup -T -c
cp %{SOURCE0} .
cp %{SOURCE1} .

%build
for flavor in %flavors_to_build; do
	make -C %{kernel_source $flavor} M=$PWD
done

%install
export INSTALL_MOD_PATH=$RPM_BUILD_ROOT
export INSTALL_MOD_DIR=%kernel_module_package_moddir/%{name}
for flavor in %flavors_to_build; do
	make -C %{kernel_source $flavor} M=$PWD modules_install
done

%clean

%changelog
