INCLUDE_DIRECTORIES (../../include/lib/MathExpression)

FILE (GLOB CPP "*.cpp")

ADD_LIBRARY (MathExpression ${CPP})
