#ifndef _CLAHE_CONFIG_
#define _CLAHE_CONFIG_

#include "hls_stream.h"
#include "hls_math.h"
#include "ap_int.h"
#include "parameter.h"

struct axis_t {
    ap_uint<8> data;
    ap_int<1> last;
};


#define stream_u8 hls::stream<unsigned char>

typedef ap_uint<16> HIST;
typedef ap_uint<16> LOOP;

void f_prepare_buf(int* ind1_p, int* ind2_p, float *xa_p, float *xa1_p);
unsigned char saturate_cast(float v);
void axis2streams(axis_t *src,
stream_u8 &st0,
stream_u8 &st1,
stream_u8 &st2,
stream_u8 &st3,
stream_u8 &st4,
stream_u8 &st5,
stream_u8 &st6,
stream_u8 &st7,
stream_u8 &st8,
stream_u8 &st9,
stream_u8 &st10,
stream_u8 &st11,
stream_u8 &st12,
stream_u8 &st13,
stream_u8 &st14,
stream_u8 &st15,
stream_u8 &src_buf
);

void calc_lut_stream(
stream_u8 &st0,
stream_u8 &st1,
stream_u8 &st2,
stream_u8 &st3,
stream_u8 &st4,
stream_u8 &st5,
stream_u8 &st6,
stream_u8 &st7,
stream_u8 &st8,
stream_u8 &st9,
stream_u8 &st10,
stream_u8 &st11,
stream_u8 &st12,
stream_u8 &st13,
stream_u8 &st14,
stream_u8 &st15,
stream_u8 &lut0,
stream_u8 &lut1,
stream_u8 &lut2,
stream_u8 &lut3,
stream_u8 &lut4,
stream_u8 &lut5,
stream_u8 &lut6,
stream_u8 &lut7,
stream_u8 &lut8,
stream_u8 &lut9,
stream_u8 &lut10,
stream_u8 &lut11,
stream_u8 &lut12,
stream_u8 &lut13,
stream_u8 &lut14,
stream_u8 &lut15,
int clipLimit);
void calc_lut_body_stream(
stream_u8 &st,
stream_u8 &lut,
int clipLimit
);
void interpolation_stream(stream_u8 &src, stream_u8 &dst,
stream_u8 &lut0,
stream_u8 &lut1,
stream_u8 &lut2,
stream_u8 &lut3,
stream_u8 &lut4,
stream_u8 &lut5,
stream_u8 &lut6,
stream_u8 &lut7,
stream_u8 &lut8,
stream_u8 &lut9,
stream_u8 &lut10,
stream_u8 &lut11,
stream_u8 &lut12,
stream_u8 &lut13,
stream_u8 &lut14,
stream_u8 &lut15,
int *ind1_p, int *ind2_p, float *xa_p, float *xa1_p);
void stream2axis(stream_u8 &dst_in, axis_t *dst);
void clahe_top(axis_t *src, axis_t *dst);
#endif
