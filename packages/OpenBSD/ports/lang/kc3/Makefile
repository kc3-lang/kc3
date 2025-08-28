COMMENT =		KC3 graph-oriented programming language

VERSION =		0.1.15
DISTNAME =		kc3-${VERSION}
SITES =			https://kc3-lang.org/release/v${VERSION}/

CATEGORIES =		lang
LICENSE =		BSD
PERMIT_PACKAGE =	Yes

SHARED_LIBS =	kc3			0.0
SHARED_LIBS +=	ekc3			0.0 # 0.0
SHARED_LIBS +=	ekc3_debug		0.0 # 0.0
SHARED_LIBS +=	kc3_debug		0.0 # 0.0
SHARED_LIBS +=	kc3_event		0.0 # 0.0
SHARED_LIBS +=	kc3_event_debug		0.0 # 0.0
SHARED_LIBS +=	kc3_gtk4		0.0 # 0.0
SHARED_LIBS +=	kc3_gtk4_debug		0.0 # 0.0
SHARED_LIBS +=	kc3_http		0.0 # 0.0
SHARED_LIBS +=	kc3_http_debug		0.0 # 0.0
SHARED_LIBS +=	kc3_json		0.0 # 0.0
SHARED_LIBS +=	kc3_json_debug		0.0 # 0.0
SHARED_LIBS +=	kc3_markdown		0.0 # 0.0
SHARED_LIBS +=	kc3_markdown_debug	0.0 # 0.0
SHARED_LIBS +=	kc3_smtp		0.0 # 0.0
SHARED_LIBS +=	kc3_smtp_debug		0.0 # 0.0
SHARED_LIBS +=	kc3_socket		0.0 # 0.0
SHARED_LIBS +=	kc3_socket_debug	0.0 # 0.0
SHARED_LIBS +=	kc3_window		0.0 # 0.0
SHARED_LIBS +=	kc3_window_cairo	0.0 # 0.0
SHARED_LIBS +=	kc3_window_cairo_xcb	0.0 # 0.0
SHARED_LIBS +=	kc3_window_sdl2		0.0 # 0.0

TEST_TARGET =		test_debug

CONFIGURE_ENV =		GCOV=false
CONFIGURE_STYLE =	gnu dest

MAKE_FLAGS =		-j 20

.include <bsd.port.mk>
