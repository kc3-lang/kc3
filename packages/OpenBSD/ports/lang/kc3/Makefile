COMMENT =		KC3 graph-oriented programming language

VERSION =		0.1.15
DISTNAME =		kc3-${VERSION}
SITES =			https://kc3-lang.org/release/v${VERSION}/

CATEGORIES =		lang
LICENSE =		BSD
PERMIT_PACKAGE =	Yes

SHARED_LIBS =		kc3	0.0

TEST_TARGET =		test_debug

CONFIGURE_ENV =		GCOV=false
CONFIGURE_STYLE =	gnu dest

MAKE_FLAGS =		-j 20

.include <bsd.port.mk>
