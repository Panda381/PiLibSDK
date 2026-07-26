
#ifndef _MAIN_H
#define _MAIN_H

//#ifdef __cplusplus
//extern "C" {
//#endif

// Game setup
void setup();

// Main program loop
void loop();

INLINE int abs(int n) { return (n < 0) ? -n : n; }

//#ifdef __cplusplus
//}
//#endif

#endif // _MAIN_H
