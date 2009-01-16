# Copyright 1999-2004 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit games

DESCRIPTION="2D scrolling shooter, resembles old DOS game of same name"
HOMEPAGE=""
SRC_URI="http://geocities.com/fnorddaemon/snipes-1.0.0.tar.gz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

DEPEND="virtual/libc
	>=media-libs/libsdl-1.2.0"

src_unpack() {
	unpack ${A}
}

src_compile() {
	gcc ${CFLAGS} -I/usr/include/SDL -lSDL ${S}/snipes.c -o ${S}/snipes || die "Game didn't compile."
}

src_install() {
	dogamesbin snipes || die "dogamesbin failed."
}
