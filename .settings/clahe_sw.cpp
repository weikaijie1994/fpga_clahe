#include "clahe_sw.h"

using namespace std;

// このrangeはどう渡される?
template<class T, int histSize, int shift>
void my_CLAHE_CalcLut_Body<T, histSize, shift>::operator()(const cv::Range& range) const
{
	T* tileLut = lut_.ptr<T>(range.start);
	//lut_の行のサイズ maybe
	const size_t lut_step = lut_.step / sizeof(T);
	//const size_t lut_step = 256;

	//maybe: 一回のループで一つのタイルの累積ヒストグラムが作られる。
	// それがlut_配列の対応する行に保存される。
	for (int k = range.start; k < range.end; ++k, tileLut += lut_step) {
		const int ty = k / tilesX_;
		const int tx = k % tilesX_;

		//タイルを抽出
		cv::Rect tileROI;
		tileROI.x = tx * tileSize_.width;
		tileROI.y = ty * tileSize_.height;
		tileROI.width = tileSize_.width;
		tileROI.height = tileSize_.height;

		const cv::Mat tile = src_(tileROI);

		// calc histgram
		int tileHist[histSize] = { 0, };

		int height = tileROI.height;
		const size_t sstep = src_.step / sizeof(T);
		//const size_t sstep = 512;
		for (const T* ptr = tile.ptr<T>(0); height--; ptr += sstep)
		{
			int x = 0;
			for (; x <= tileROI.width - 4; x += 4) {
				int t0 = ptr[x], t1 = ptr[x + 1];
				tileHist[t0]++;
				tileHist[t1]++;
				t0 = ptr[x + 2]; t1 = ptr[x + 3];
				tileHist[t0]++;
				tileHist[t1]++;
			}

			for (; x < tileROI.width; ++x) {
				tileHist[ptr[x]]++;
			}
		}

		//clip histogram
		if (clipLimit_ > 0)
		{
			int clipped = 0;
			for (int i = 0; i < histSize; ++i)
			{
				if (tileHist[i] > clipLimit_)
				{
					clipped += tileHist[i] - clipLimit_;
					tileHist[i] = clipLimit_;
				}
			}


			// redistribute clipped pixel
			int redistBatch = clipped / histSize;
			int residual = clipped - redistBatch * histSize;

			//一様にredistBatchを足す　論文とは違う。
			for (int i = 0; i < histSize; ++i) {
				tileHist[i] += redistBatch;
			}

			if (residual != 0) {
				int residualStep = max(histSize / residual, 1);
				for (int i = 0; i < histSize && residual > 0; i += residualStep, residual--) {
					tileHist[i]++;
				}
			}
		}

		//calc lut
		int sum = 0;
		for (int i = 0; i < histSize; ++i) {
			sum += tileHist[i];
			tileLut[i] = cv::saturate_cast<T>(sum*lutScale_);
		}

	}
}

//この関数が実行される前にはlut_配列が完成している。

template<class T, int shift>
void my_CLAHE_Interpolation_Body<T, shift>::operator()(const cv::Range& range) const
{
	float inv_th = 1.0f / tileSize_.height;

	for (int y = range.start; y < range.end; ++y) {
		const T* srcRow = src_.ptr<T>(y);
		T* dstRow = dst_.ptr<T>(y);

		float tyf = y * inv_th - 0.5f;

		int ty1 = cvFloor(tyf);
		int ty2 = ty1 + 1;

		float ya = tyf - ty1, ya1 = 1.0f - ya;

		ty1 = max(ty1, 0);
		ty2 = min(ty2, tilesY_ - 1);

		//maybe: lutPlane1:現在のヒストグラム
		//       lutPlane2:tilesX_個先のタイルのヒストグラム
		//ptr<T>(int r)	r行目の先頭アドレスを取得
		const T* lutPlane1 = lut_.ptr<T>(ty1 * tilesX_);
		const T* lutPlane2 = lut_.ptr<T>(ty2 * tilesX_);

		for (int x = 0; x < src_.cols; ++x)
		{
			int srcVal = srcRow[x];

			int ind1 = ind1_p[x] + srcVal;
			int ind2 = ind2_p[x] + srcVal;

			//lutPlane1[ind1] lutPlane1[ind2] 隣り合ったヒストグラム
			//lutPlane2[ind1] lutPlsne2[ind2]
			// lutPlane1[ind1] lutPlsnr2[ind1] tilesX_個離れたhistogram
			float res = (lutPlane1[ind1] * xa1_p[x] + lutPlane1[ind2] * xa_p[x]) * ya1 +
				(lutPlane2[ind1] * xa1_p[x] + lutPlane2[ind2] * xa_p[x]) * ya;

			dstRow[x] = cv::saturate_cast<T>(res);
		}
	}
}


my_CLAHE_Impl::my_CLAHE_Impl(double clipLimit, int tilesX, int tilesY) :
	clipLimit_(clipLimit), tilesX_(tilesX), tilesY_(tilesY)
{

}

void my_CLAHE_Impl::apply(cv::InputArray _src, cv::OutputArray _dst) {
	//前提　分割は割り切れる　i.e. src.width%tilesX == 0 && src.height%tilesY == 0
	int histSize = 256;

	cv::Size tileSize;
	cv::_InputArray _srcForLut;

	tileSize = cv::Size(_src.size().width / tilesX_, _src.size().height / tilesY_);
	_srcForLut = _src;




	const int tileSizeTotal = tileSize.area();
	const float lutScale = static_cast<float>(histSize - 1) / tileSizeTotal;

	int clipLimit = 0;
	if (clipLimit_ > 0) {
		clipLimit = static_cast<int>(clipLimit_ * tileSizeTotal / histSize);
		clipLimit = max(clipLimit, 1);
	}

	cv::Mat src = _src.getMat();
	_dst.create(src.size(), src.type());
	cv::Mat dst = _dst.getMat();
	cv::Mat srcForLut = _srcForLut.getMat();
	lut_.create(tilesX_ * tilesY_, histSize, _src.type());

	cv::Ptr<cv::ParallelLoopBody> calcLutBody;

	//clipLimitとlutScaleの役割は?
	//calcLutBody = cv::makePtr <my_CLAHE_CalcLut_Body<uchar, 256, 0>>(srcForLut, lut_, tileSize, tilesX_, clipLimit, lutScale);
	//cout << "here?" << endl;

	//タイルの数まで並列で実行可能。
	//calcLutBody = cv::makePtr < my_CLAHE_CalcLut_Body<uchar, 256, 0 >>(srcForLut, lut_, tileSize, tilesX_, clipLimit, lutScale);
	//cv::parallel_for_(cv::Range(0, tilesX_ * tilesY_), *calcLutBody);
	calc_lut_fpga(srcForLut, lut_, tileSize, tilesX_, clipLimit, lutScale);


	cv::Ptr<cv::ParallelLoopBody> interpolationBody;


	//srcの行数まで並列で実行可能
	//interpolationBody = cv::makePtr<my_CLAHE_Interpolation_Body<uchar, 0>>(src, dst, lut_, tileSize, tilesX_, tilesY_);
	//cv::parallel_for_(cv::Range(0, src.rows), *interpolationBody);

	const int width = WIDTH;
	int ind1_p[width], ind2_p[width];
	float xa_p[width], xa1_p[width];
	prepare_buf(ind1_p, ind2_p, xa_p, xa1_p);
	interpolation_fpga(src, dst, lut_, tileSize, tilesX_, tilesY_, ind1_p, ind2_p, xa_p, xa1_p);

}

////// FPGA version //////////////

void calc_lut_fpga(cv::Mat src, cv::Mat lut, cv::Size tileSize, int tilesX, int clipLimit, float lutScale) {
	const size_t lut_step = 256;
	const int tileSizeTotal = (WIDTH / F_tilesX) * (HEIGHT / F_tilesY);
	const int histsize = 256;

	unsigned char *tileLut;
	for (int k = 0; k < F_tilesX * F_tilesY; ++k) {
		const int ty = k / tilesX;
		const int tx = k % tilesX;
		const int sy = ty * tileSize.height;
		const int sx = tx * tileSize.width;
		int tileHist[histsize] = { 0, };

		tileLut = lut.ptr(k);

		int height = tileSize.height;
		const size_t sstep = WIDTH;

		for (const uchar *ptr = src.ptr(sy, sx); height--; ptr += sstep) {
			int x = 0;
			for (; x <= tileSize.width - 4; x += 4) {
				int t0 = ptr[x], t1 = ptr[x + 1];
				tileHist[t0]++;
				tileHist[t1]++;
				t0 = ptr[x + 2]; t1 = ptr[x + 3];
				tileHist[t0]++;
				tileHist[t1]++;
			}

			for (; x < tileSize.width; ++x) {
				tileHist[ptr[x]]++;
			}
		}
		//clip histogram
		if (clipLimit > 0) {
			int clipped = 0;
			for (int i = 0; i < histsize; ++i) {
				if (tileHist[i] > clipLimit) {
					clipped += tileHist[i] - clipLimit;
					tileHist[i] = clipLimit;
				}
			}

			//redistribute clipped pixel
			int redistBatch = clipped / histsize;
			int residual = clipped - redistBatch * histsize;
			for (int i = 0; i < histsize; ++i) {
				tileHist[i] += redistBatch;
			}

			if (residual != 0) {
				int residualStep = max(histsize / residual, 1);
				for (int i = 0; i < histsize && residual > 0; i += residualStep, residual--) {
					tileHist[i]++;
				}
			}
		}
		//cout << "here?" << endl;
		//calc lut
		int sum = 0;
		for (int i = 0; i < histsize; ++i) {
			sum += tileHist[i];
			//tileLut[i] = cv::saturate_cast<uchar>(sum * lutScale);
			tileLut[i] = saturate_cast_sw(sum * lutScale);
			//tileLut[i] = (uchar)(sum * lutScale);
			//(uchar)のキャストだと結果が一致しない　（詳しく見る)
			//結果はかなり速くなる（なぜ？）
		}
		//cout << "after?" << endl;
	}
}

void interpolation_fpga(cv::Mat src, cv::Mat dst, cv::Mat lut, cv::Size tileSize, int tilesX, int tilesY, int *ind1_p, int *ind2_p, float *xa_p, float *xa1_p) {
	float inv_th = 1.0f / tileSize.height;
	float maximum = 0.0;
	for (int y = 0; y < src.rows; ++y) {
		const uchar *srcRow = src.ptr<uchar>(y);
		uchar *dstRow = dst.ptr<uchar>(y);

		float tyf = y * inv_th - 0.5f;

		int ty1 = cvFloor(tyf);
		int ty2 = ty1 + 1;

		float ya = tyf - ty1, ya1 = 1.0f - ya;

		ty1 = max(ty1, 0);
		ty2 = min(ty2, tilesY - 1);

		const uchar *lutPlane1 = lut.ptr<uchar>(ty1 * tilesX);
		const uchar *lutPlane2 = lut.ptr<uchar>(ty2 * tilesX);

		for (int x = 0; x < src.cols; ++x) {
			int srcVal = srcRow[x];

			int ind1 = ind1_p[x] + srcVal;
			int ind2 = ind2_p[x] + srcVal;

			float res = (lutPlane1[ind1] * xa1_p[x] + lutPlane1[ind2] * xa_p[x]) * ya1 +
				(lutPlane2[ind1] * xa1_p[x] + lutPlane2[ind2] * xa_p[x]) * ya;
			if (maximum < res) {
				maximum = res;
			}
			//dstRow[x] = cv::saturate_cast<uchar>(res);
			//dstRow[x] = (uchar)(res); //4倍くらい速い
			dstRow[x] = saturate_cast_sw(res);
		}
	}
	cout << "max: " << maximum << endl;
}

void prepare_buf(int *ind1_p, int *ind2_p, float *xa_p, float *xa1_p) {
	int width = WIDTH;
	int tilesX = 16;
	int tileSizeWidth = WIDTH/tilesX;


	int lut_step = 256;
	float inv_tw = 1.0f / tileSizeWidth;

	for (int x = 0; x < width; ++x) {
		float txf = x * inv_tw - 0.5f;

		int tx1 = cvFloor(txf);
		int tx2 = tx1 + 1;

		xa_p[x] = txf - tx1;
		xa1_p[x] = 1.0f - xa_p[x];

		tx1 = max(tx1, 0);
		tx2 = min(tx2, tilesX - 1);

		ind1_p[x] = tx1 * lut_step;
		ind2_p[x] = tx2 * lut_step;
	}
}

unsigned char saturate_cast_sw(float v) {
	int iv = round(v);
	return (unsigned char)(hls::min(iv, 255));
}

////////// FPGA version end ///////////////
void my_CLAHE_Impl::setClipLimit(double clipLimit)
{
	clipLimit_ = clipLimit;
}

double my_CLAHE_Impl::getClipLimit() const
{
	return clipLimit_;
}

void my_CLAHE_Impl::setTilesGridSize(cv::Size tileGridSize)
{
	tilesX_ = tileGridSize.width;
	tilesY_ = tileGridSize.height;
}

cv::Size my_CLAHE_Impl::getTilesGridSize() const
{
	return cv::Size(tilesX_, tilesY_);
}

void my_CLAHE_Impl::collectGarbage()
{
	lut_.release();
}

cv::Ptr <cv::CLAHE> mycreateCLAHE(double clipLimit, cv::Size tileGridSize) {
	//return cv::makePtr<my_CLAHE_Impl>(clipLimit, tileGridSize.width, tileGridSize.height);
	return cv::Ptr<my_CLAHE_Impl>(new my_CLAHE_Impl(clipLimit, tileGridSize.width, tileGridSize.height));

}
