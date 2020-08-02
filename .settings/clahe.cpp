#include "clahe.h"

void f_prepare_buf(int* ind1_p, int* ind2_p, float *xa_p, float *xa1_p) {

	const float inv_tw = 1.0f / F_tileWIDTH;

	for (int x = 0; x < WIDTH; ++x) {
		float txf = x * inv_tw - 0.5f;

		int tx1 = floor(txf);
		int tx2 = tx1 + 1;


		xa_p[x] = txf - tx1;
		xa1_p[x] = 1.0f - xa_p[x];

		tx1 = hls::max(tx1, 0);
		tx2 = hls::min(tx2, F_tilesX-1);

		ind1_p[x] = tx1;
		ind2_p[x] = tx2;

	}
}

unsigned char saturate_cast(float v) {
	int iv = round(v);
	return (unsigned char)(hls::min(iv, 255));
}

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
)
{
#pragma HLS inline off
	for (int i = 0; i < HEIGHT; ++i) {
		int w = 0;
		for (int j = 0; j < F_tileWIDTH; ++j, ++w) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			unsigned char tmp =  src[i*WIDTH + w].data;
			st0 << tmp;
			src_buf << tmp;
		}
		for (int j = 0; j < F_tileWIDTH; ++j, ++w) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			unsigned char tmp =  src[i*WIDTH + w].data;
			st1 << tmp;
			src_buf << tmp;
		}
		for (int j = 0; j < F_tileWIDTH; ++j, ++w) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			unsigned char tmp =  src[i*WIDTH + w].data;
			st2 << tmp;
			src_buf << tmp;
		}
		for (int j = 0; j < F_tileWIDTH; ++j, ++w) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			unsigned char tmp =  src[i*WIDTH + w].data;
			st3 << tmp;
			src_buf << tmp;
		}
		for (int j = 0; j < F_tileWIDTH; ++j, ++w) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			unsigned char tmp =  src[i*WIDTH + w].data;
			st4 << tmp;
			src_buf << tmp;
		}
		for (int j = 0; j < F_tileWIDTH; ++j, ++w) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			unsigned char tmp =  src[i*WIDTH + w].data;
			st5 << tmp;
			src_buf << tmp;
		}
		for (int j = 0; j < F_tileWIDTH; ++j, ++w) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			unsigned char tmp =  src[i*WIDTH + w].data;
			st6 << tmp;
			src_buf << tmp;
		}
		for (int j = 0; j < F_tileWIDTH; ++j, ++w) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			unsigned char tmp =  src[i*WIDTH + w].data;
			st7 << tmp;
			src_buf << tmp;
		}
		for (int j = 0; j < F_tileWIDTH; ++j, ++w) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			unsigned char tmp =  src[i*WIDTH + w].data;
			st8 << tmp;
			src_buf << tmp;
		}
		for (int j = 0; j < F_tileWIDTH; ++j, ++w) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			unsigned char tmp =  src[i*WIDTH + w].data;
			st9 << tmp;
			src_buf << tmp;
		}
		for (int j = 0; j < F_tileWIDTH; ++j, ++w) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			unsigned char tmp =  src[i*WIDTH + w].data;
			st10 << tmp;
			src_buf << tmp;
		}
		for (int j = 0; j < F_tileWIDTH; ++j, ++w) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			unsigned char tmp =  src[i*WIDTH + w].data;
			st11 << tmp;
			src_buf << tmp;
		}
		for (int j = 0; j < F_tileWIDTH; ++j, ++w) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			unsigned char tmp =  src[i*WIDTH + w].data;
			st12 << tmp;
			src_buf << tmp;
		}
		for (int j = 0; j < F_tileWIDTH; ++j, ++w) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			unsigned char tmp =  src[i*WIDTH + w].data;
			st13 << tmp;
			src_buf << tmp;
		}
		for (int j = 0; j < F_tileWIDTH; ++j, ++w) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			unsigned char tmp =  src[i*WIDTH + w].data;
			st14 << tmp;
			src_buf << tmp;
		}
		for (int j = 0; j < F_tileWIDTH; ++j, ++w) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			unsigned char tmp =  src[i*WIDTH + w].data;
			st15 << tmp;
			src_buf << tmp;
		}
	}
}

void axis2stream(
axis_t *src,
stream_u8 &st,
stream_u8 &src_buf
)
{
#pragma HLS inline off
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			unsigned char tmp =  src[i*WIDTH + j].data;
			st << tmp;
			src_buf << tmp;
		}
	}
}
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
int clipLimit) {
	const int tileSizeTotal = F_tileWIDTH * F_tileHEIGHT;
	const float lutScale = (float) (F_HISTSIZE - 1) / tileSizeTotal;


	outer: for (ap_uint<8> k = 0; k < F_tilesY; ++k) {
#pragma HLS dataflow
		calc_lut_body_stream(st0, lut0, clipLimit);
		calc_lut_body_stream(st1, lut1, clipLimit);
		calc_lut_body_stream(st2, lut2, clipLimit);
		calc_lut_body_stream(st3, lut3, clipLimit);
		calc_lut_body_stream(st4, lut4, clipLimit);
		calc_lut_body_stream(st5, lut5, clipLimit);
		calc_lut_body_stream(st6, lut6, clipLimit);
		calc_lut_body_stream(st7, lut7, clipLimit);
		calc_lut_body_stream(st8, lut8, clipLimit);
		calc_lut_body_stream(st9, lut9, clipLimit);
		calc_lut_body_stream(st10, lut10, clipLimit);
		calc_lut_body_stream(st11, lut11, clipLimit);
		calc_lut_body_stream(st12, lut12, clipLimit);
		calc_lut_body_stream(st13, lut13, clipLimit);
		calc_lut_body_stream(st14, lut14, clipLimit);
		calc_lut_body_stream(st15, lut15, clipLimit);
	}
}

void calc_lut_body_stream(
stream_u8 &st,
stream_u8 &lut,
int clipLimit
)
{
	const int tileSizeTotal = F_tileWIDTH * F_tileHEIGHT;
	const float lutScale = (float) (F_HISTSIZE - 1) / tileSizeTotal;

	HIST tileHist[F_HISTSIZE];

		histinit: for(LOOP i = 0; i < F_HISTSIZE; ++i) {
		#pragma HLS pipeline
			tileHist[i] = 0;
		}

	//make histogram from one tile
	HIST clipped = 0;
	HIST acc = 0;
		unsigned char val;
		unsigned char old;

		// first loops behavior
		st >> old;
		acc  = 1;
#pragma HLS dependence variable=tileHist intra RAW false
		make_histogram: for (LOOP h = 0;  h < F_tileHEIGHT*F_tileWIDTH-1; ++h) {
#pragma HLS pipeline
				st >> val;
				if (old == val) {
					acc = acc + 1;
				} else {
					if (acc >= clipLimit) {
						tileHist[old] = clipLimit;
						clipped += acc - clipLimit;
					} else {
						tileHist[old] = acc;
					}
					acc = tileHist[val] + 1;
				}
				old = val;
			}

		if (acc >= clipLimit) {
			tileHist[old] = clipLimit;
			clipped += acc - clipLimit;
		} else {
			tileHist[old] = acc;
		}

			//redistribute clipped pixel
			HIST redistBatch = clipped / F_HISTSIZE;
			int residual = clipped - redistBatch * F_HISTSIZE;

			redistribute: for (LOOP i = 0; i < F_HISTSIZE; ++i) {
#pragma HLS pipeline
				tileHist[i] += redistBatch;
			}

			if (residual != 0) {
				int residualStep = hls::max(F_HISTSIZE/residual , 1);
				HIST oldi = 0;
				HIST oldval = tileHist[0];
//#pragma HLS dependence variable=tileHist intra RAW false
				resdistribute2: for (LOOP i = residualStep; i < F_HISTSIZE && residual > 0; i +=residualStep, residual--) {
#pragma HLS loop_tripcount min=256 = max=256 avg=256
#pragma HLS pipeline
					tileHist[oldi] = oldval + 1;
					oldi = i;
					oldval  = tileHist[i];
				}

				tileHist[oldi] = oldval + 1;
			}

			//calc lut
			HIST sum = 0;
			calc_lut: for (LOOP i = 0; i < F_HISTSIZE; ++i) {
			#pragma HLS pipeline
					sum += tileHist[i];
					//lut << (unsigned char)(sum * lutScale);
					lut << saturate_cast(sum * lutScale);
			}
}


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
int *ind1_p, int *ind2_p, float *xa_p, float *xa1_p) {
	const float inv_th = 1.0f / F_tileHEIGHT;
	//buf[0][][]: A, buf[1][][]: B, buf[2][][]: C
	unsigned char buf[3][F_tilesX][F_HISTSIZE];

#pragma HLS array_partition variable=buf complete dim=1
#pragma HLS array_partition variable=buf complete dim=2

	unsigned char srcVal;
	////////// yy is col's index /////////////////////
	int yy = 0;
	////////// yy is col's index /////////////////////

	/// tmp is buf's index
	int _tmp = 0;
	int tmp1, tmp2;

	//////////////prologue start /////////////////////////////
	//////////////update tmp ///////////////////////
	tmp1 = _tmp%3;
	//////////////update tmp ///////////////////////
	prologue_data_in: for (int x = 0; x < F_HISTSIZE; ++x) {
#pragma HLS pipeline
		lut0 >> buf[tmp1][0][x];
		lut1 >> buf[tmp1][1][x];
		lut2 >> buf[tmp1][2][x];
		lut3 >> buf[tmp1][3][x];
		lut4 >> buf[tmp1][4][x];
		lut5 >> buf[tmp1][5][x];
		lut6 >> buf[tmp1][6][x];
		lut7 >> buf[tmp1][7][x];
		lut8 >> buf[tmp1][8][x];
		lut9 >> buf[tmp1][9][x];
		lut10 >> buf[tmp1][10][x];
		lut11 >> buf[tmp1][11][x];
		lut12 >> buf[tmp1][12][x];
		lut13 >> buf[tmp1][13][x];
		lut14 >> buf[tmp1][14][x];
		lut15 >> buf[tmp1][15][x];
	}
	prologue_calc: for (int y = 0; y < F_tileHEIGHT/2; ++y, ++yy) {
		float tyf = yy * inv_th - 0.5f;

		int ty1 = floor(tyf);

		float ya = tyf - ty1, ya1 = 1.0f - ya;
		for (int x = 0; x < WIDTH; ++x) {
#pragma HLS pipeline
			src >> srcVal;
			int ind1 = ind1_p[x];
			int ind2 = ind2_p[x];

			float res = (buf[tmp1][ind1][srcVal] * xa1_p[x] + buf[tmp1][ind2][srcVal] * xa_p[x]);

			dst << saturate_cast(res);
		}
	}
		//////////////prologue end /////////////////////////////

		//////////////body start   /////////////////////////////
		body: for (int i = 0; i < HEIGHT/F_tileHEIGHT-1; ++i) {
			//////////////update tmp ///////////////////////
			_tmp++;
			tmp2 = tmp1;
			tmp1 = _tmp%3;
			//////////////update tmp ///////////////////////
			body_data_in: for (int x = 0; x < F_HISTSIZE; ++x) {
			#pragma HLS pipeline
				lut0 >> buf[tmp1][0][x];
				lut1 >> buf[tmp1][1][x];
				lut2 >> buf[tmp1][2][x];
				lut3 >> buf[tmp1][3][x];
				lut4 >> buf[tmp1][4][x];
				lut5 >> buf[tmp1][5][x];
				lut6 >> buf[tmp1][6][x];
				lut7 >> buf[tmp1][7][x];
				lut8 >> buf[tmp1][8][x];
				lut9 >> buf[tmp1][9][x];
				lut10 >> buf[tmp1][10][x];
				lut11 >> buf[tmp1][11][x];
				lut12 >> buf[tmp1][12][x];
				lut13 >> buf[tmp1][13][x];
				lut14 >> buf[tmp1][14][x];
				lut15 >> buf[tmp1][15][x];
			}
			body_calc: for (int y = 0; y < F_tileHEIGHT; ++y, ++yy) {
					float tyf = yy * inv_th - 0.5f;

					int ty1 = floor(tyf);

					float ya = tyf - ty1, ya1 = 1.0f - ya;
					for (int x = 0; x < WIDTH; ++x) {
			#pragma HLS pipeline
						src >> srcVal;
						int ind1 = ind1_p[x];
						int ind2 = ind2_p[x];

						float res = (buf[tmp2][ind1][srcVal] * xa1_p[x] + buf[tmp2][ind2][srcVal] * xa_p[x]) * ya1
									+ (buf[tmp1][ind1][srcVal] * xa1_p[x] + buf[tmp1][ind2][srcVal] * xa_p[x]) * ya;

						dst << saturate_cast(res);
					}
			}
		}
		//////////////body end   /////////////////////////////
		//////////////epilogue start  /////////////////////////////
		epilogue_calc: for (int y = 0; y < F_tileHEIGHT/2; ++y, ++yy) {
				float tyf = yy * inv_th - 0.5f;
				int ty1 = floor(tyf);
				float ya = tyf - ty1, ya1 = 1.0f - ya;

				for (int x = 0; x < WIDTH; ++x) {
		#pragma HLS pipeline
						src >> srcVal;
						int ind1 = ind1_p[x];
						int ind2 = ind2_p[x];

						float res = (buf[tmp1][ind1][srcVal] * xa1_p[x] + buf[tmp1][ind2][srcVal] * xa_p[x]);

						dst << saturate_cast(res);
					}
			}
		//////////////epilogue start  /////////////////////////////
}

void stream2axis(stream_u8 &dst_in, axis_t *dst) {
#pragma HLS inline off
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
#pragma HLS pipeline
#pragma HLS loop_flatten off
			ap_uint<1> tmp = 0;
			unsigned char val;
			if ((i==HEIGHT-1) && (j== WIDTH-1)) {
				tmp = 1;
			}
			dst[i*WIDTH+j].last = tmp;
			dst_in >> val;
			dst[i*WIDTH+j].data = val;
		}
	}
}



void clahe_top(axis_t *src, axis_t *dst) {
#pragma HLS INTERFACE axis port=src depth=1920*1080 // Added depth for C/RTL cosimulation
#pragma HLS INTERFACE axis port=dst depth=1920*1080 // Added depth for C/RTL cosimulation
#pragma HLS INTERFACE s_axilite port=return

	const int tileSizeTotal = F_tileWIDTH* F_tileHEIGHT;
	const int clipLimit = static_cast<int>(CLIP_LIMIT * tileSizeTotal / F_HISTSIZE);
	int ind1_p[WIDTH], ind2_p[WIDTH];
	float xa_p[WIDTH], xa1_p[WIDTH];

	stream_u8 st0("st0"), st1("st1"), st2("st2"), st3("st3"), st4("st4"), st5("st5"), st6("st6"), st7("st7"),
							st8("st8"), st9("st9"), st10("st10"), st11("st11"), st12("st12"), st13("st13"), st14("st14"), st15("st15");
	stream_u8 lut0, lut1, lut2, lut3, lut4, lut5, lut6, lut7, lut8, lut9, lut10, lut11,
						lut12, lut13, lut14, lut15;
	stream_u8 src_buf("src_buf");
	stream_u8 dst_stream("dst_stream");
/*
#pragma HLS stream variable=st0 depth=1080*90
#pragma HLS stream variable=st1 depth=1080*90
#pragma HLS stream variable=st2 depth=1080*90
#pragma HLS stream variable=st3 depth=1080*90
#pragma HLS stream variable=st4 depth=1080*90
#pragma HLS stream variable=st5 depth=1080*90
#pragma HLS stream variable=st6 depth=1080*90
#pragma HLS stream variable=st7 depth=1080*90
#pragma HLS stream variable=st8 depth=1080*90
#pragma HLS stream variable=st9 depth=1080*90
#pragma HLS stream variable=st10 depth=1080*90
#pragma HLS stream variable=st11 depth=1080*90
#pragma HLS stream variable=st12 depth=1080*90
#pragma HLS stream variable=st13 depth=1080*90
#pragma HLS stream variable=st14 depth=1080*90
#pragma HLS stream variable=st15 depth=1080*90
*/
/*
#pragma HLS stream variable=lut0 depth=16*256
#pragma HLS stream variable=lut1 depth=16*256
#pragma HLS stream variable=lut2 depth=16*256
#pragma HLS stream variable=lut3 depth=16*256
#pragma HLS stream variable=lut4 depth=16*256
#pragma HLS stream variable=lut5 depth=16*256
#pragma HLS stream variable=lut6 depth=16*256
#pragma HLS stream variable=lut7 depth=16*256
#pragma HLS stream variable=lut8 depth=16*256
#pragma HLS stream variable=lut9 depth=16*256
#pragma HLS stream variable=lut10 depth=16*256
#pragma HLS stream variable=lut11 depth=16*256
#pragma HLS stream variable=lut12 depth=16*256
#pragma HLS stream variable=lut13 depth=16*256
#pragma HLS stream variable=lut14 depth=16*256
#pragma HLS stream variable=lut15 depth=16*256
*/
#pragma HLS stream variable=src_buf depth=1920*135
//#pragma HLS stream variable=dst_stream depth=1920*135

#pragma HLS dataflow
	f_prepare_buf(ind1_p, ind2_p, xa_p, xa1_p);

	axis2streams(src,
		st0,
		st1,
		st2,
		st3,
		st4,
		st5,
		st6,
		st7,
		st8,
		st9,
		st10,
		st11,
		st12,
		st13,
		st14,
		st15,
		src_buf);

	calc_lut_stream(
			st0,
			st1,
			st2,
			st3,
			st4,
			st5,
			st6,
			st7,
			st8,
			st9,
			st10,
			st11,
			st12,
			st13,
			st14,
			st15,
			lut0,
			lut1,
			lut2,
			lut3,
			lut4,
			lut5,
			lut6,
			lut7,
			lut8,
			lut9,
			lut10,
			lut11,
			lut12,
			lut13,
			lut14,
			lut15,
	clipLimit);

	/*
	axis2stream(src,st0,src_buf);
	calc_lut_stream_re(
				st0,
				lut0,
				lut1,
				lut2,
				lut3,
				lut4,
				lut5,
				lut6,
				lut7,
				lut8,
				lut9,
				lut10,
				lut11,
				lut12,
				lut13,
				lut14,
				lut15,
		clipLimit);
		*/
	interpolation_stream(src_buf, dst_stream,
			lut0,
			lut1,
			lut2,
			lut3,
			lut4,
			lut5,
			lut6,
			lut7,
			lut8,
			lut9,
			lut10,
			lut11,
			lut12,
			lut13,
			lut14,
			lut15,
			ind1_p, ind2_p, xa_p, xa1_p);
	stream2axis(dst_stream, dst);
}




