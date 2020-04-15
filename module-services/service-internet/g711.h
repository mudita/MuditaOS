/*
 * g711.h
 *
 * u-law, A-law and linear PCM conversions.
 */

#ifndef _G711_H_
#define _G711_H_

#ifdef __cplusplus
extern "C"
{
#endif

    unsigned char linear2alaw(short pcm_val);
    short alaw2linear(unsigned char a_val);
    unsigned char linear2ulaw(short pcm_val);
    short ulaw2linear(unsigned char u_val);
    unsigned char alaw2ulaw(unsigned char aval);
    unsigned char ulaw2alaw(unsigned char uval);

#ifdef __cplusplus
}
#endif

#endif /* _G711_H_ */