Name: katy
Summary: Katy -- An UltraEdit-like text editor for KDE
Version: 0.3
Release: 1
Copyright: GPL
Group: X11/KDE/Utilities
Source: ftp.kde.org/pub/kde/unstable/apps/utils/katy-${version}.tar.gz
Packager: Gordon Tyler <gordon@doxxx.net>
BuildRoot: /tmp/katy-${versoion}
Prefix: /opt/kde

%description
Katy is a general purpose text editor for KDE which is inspired by the Windows text editor, UltraEdit.

%prep
rm -rf $RPM_BUILD_ROOT
%setup -n katy-${version}

%build
./configure
make

%install
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%{prefix}/bin/katy
%{prefix}/bin/katy_client
%{prefix}/share/applnk/Utilities/katy.desktop
%{prefix}/share/apps/katy/
%{prefix}/share/doc/HTML/en/katy/
%{prefix}/share/icons/medium/locolor/apps/katy.png
%{prefix}/share/icons/small/locolor/apps/katy.png
