Name: katy
Summary: A KDE Text Editor with similar features to UltraEdit(tm)
Version: 0.3
Release: 1mdk
Copyright: GPL
Group: Editors
Source: ftp.kde.org/pub/kde/unstable/apps/utils/katy-%{version}.tar.gz
Packager: Gordon Tyler <gordon@doxxx.net>
BuildRoot: /tmp/katy-%{version}
Prefix: /usr
Requires: kdelibs >= 2.0

%description
Katy is a general purpose text editor for KDE, inspired by the Windows text editor, UltraEdit(tm).

%prep
rm -rf $RPM_BUILD_ROOT
%setup -n katy-%{version}

%build
make -f admin/Makefile.common
./configure
make

%install
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%{prefix}/bin/katy
%{prefix}/lib/menu/katy
%{prefix}/share/apps/katy/
#%{prefix}/share/doc/HTML/en/katy/
%{prefix}/share/icons/hicolor/32x32/apps/katy.png
%{prefix}/share/icons/hicolor/48x48/apps/katy.png
%{prefix}/share/icons/hicolor/16x16/apps/katy.png
%{prefix}/share/icons/locolor/16x16/apps/katy.png
%{prefix}/share/icons/locolor/32x32/apps/katy.png                                    
