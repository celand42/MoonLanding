#if !defined GAME_HEADER
#define GAME_HEADER

typedef unsigned int uint32;

#ifndef ASSERT
#define ASSERT(expr) if (!expr) {game_manager->logProblem(__FILE__, __LINE__);}
#endif

#endif