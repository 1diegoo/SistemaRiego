#ifndef DBG_UTILS_H_PROY8
#define DBG_UTILS_H_PROY8

#ifdef DEBUG_MODE
#define RUN_DBG_ONLY(expression) expression
#else
#define RUN_DBG_ONLY(expression)
#endif

#endif