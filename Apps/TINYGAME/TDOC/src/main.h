
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

INLINE int _abs(int n) { return (n < 0) ? -n : n; }

// Game setup
void setup();

// Main program loop
void loop();

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
