# needssslcertforbuild

Name: moktest
Version: 1
Release: 0
BuildRequires: kernel-source kernel-syms module-init-tools
# Do not check post scripts because building environment doesn't support efi
BuildRequires: -post-build-checks
Summary: Dummy Module for MOK testing
Group: System/Tests
# License: GPLv2
License: GPL-2.0
Source0: Makefile
Source1: moktest.c
BuildRoot: /var/tmp/%{name}-%{version}-root
Requires: moktest-kmp
Requires(post): mokutil
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
if test -e %{_sourcedir}/_projectcert.crt ; then
	openssl x509 -inform PEM -in %{_sourcedir}/_projectcert.crt -outform DER -out ./uefi-plugfest.der
	install -d %{buildroot}/%{_sysconfdir}/uefi/certs/
	install -m 444 ./uefi-plugfest.der %{buildroot}/%{_sysconfdir}/uefi/certs/uefi-plugfest.der
fi

%clean

%post
/usr/bin/mokutil --root-pw --import %{_sysconfdir}/uefi/certs/uefi-plugfest.der

%preun
/usr/bin/mokutil --root-pw --delete %{_sysconfdir}/uefi/certs/uefi-plugfest.der

%files
%dir %{_sysconfdir}/uefi/
%dir %{_sysconfdir}/uefi/certs/
%{_sysconfdir}/uefi/certs/uefi-plugfest.der

%changelog
