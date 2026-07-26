
#ifndef _MAIN_H
#define _MAIN_H

//#ifdef __cplusplus
//extern "C" {
//#endif

// Game setup
void setup();

// Main program loop
void loop();

INLINE void Sound_TDDUG(uint8_t freq_,uint8_t dur) { Sound(freq_, dur); }

//#ifdef __cplusplus
//}
//#endif

#endif // _MAIN_H
