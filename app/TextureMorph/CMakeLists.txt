FIND_PACKAGE (Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES (${QT_INCLUDES})

INCLUDE_DIRECTORIES (../../include/app/TextureMorph)

ADD_SUBDIRECTORY (ScaledCanvas)

FILE (GLOB CPP "*.cpp")
FILE (GLOB HEADERS "../../include/app/TextureMorph/*.hpp")
FILE (GLOB RESOURCES "*.qrc")

QT4_WRAP_CPP (QT_SRCS ${HEADERS})
QT4_ADD_RESOURCES (QT_RSRCS ${RESOURCES})

ADD_EXECUTABLE (TextureMorph ${CPP} ${QT_SRCS} ${QT_RSRCS})

TARGET_LINK_LIBRARIES (TextureMorph
	TextureMorph.ScaledCanvas
	MathExpression
	QtUtil
	${QT_LIBRARIES})
