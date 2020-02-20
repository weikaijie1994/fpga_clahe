#ifndef _CLAHE_SW_CONFIG_
#define _CLAHE_SW_CONFIG_

#include "xf_headers.h"
#include "hls_stream.h"
#include "hls_math.h"
#include "ap_int.h"

#include "parameter.h"

class my_CLAHE_Impl final : public cv::CLAHE
{
public:
	my_CLAHE_Impl(double clipLimit = 40.0, int tilesX = 8, int tilesY = 8);

	void apply(cv::InputArray src, cv::OutputArray dst) override;

	void setClipLimit(double cliplimit) override;
	double getClipLimit() const override;

	void setTilesGridSize(cv::Size tileGridSize) override;
	cv::Size getTilesGridSize() const override;

	void collectGarbage() override;

private:
	double clipLimit_;
	int tilesX_;
	int tilesY_;

	cv::Mat lut_;
};

template<class T, int histSize, int shift>
class my_CLAHE_CalcLut_Body : public cv::ParallelLoopBody
{
public:
	my_CLAHE_CalcLut_Body(const cv::Mat& src, const cv::Mat& lut, const cv::Size& tileSize, const int& tilesX, const int& clipLimit, const float& lutScale) :
		src_(src), lut_(lut), tileSize_(tileSize), tilesX_(tilesX), clipLimit_(clipLimit), lutScale_(lutScale)
	{

	}

	void operator ()(const cv::Range& range) const override;

private:
	cv::Mat src_;
	mutable  cv::Mat lut_;

	cv::Size tileSize_;
	int tilesX_;
	int clipLimit_;
	float lutScale_;
};

template<class T, int shift>
class my_CLAHE_Interpolation_Body : public cv::ParallelLoopBody
{
public:
	// buf(src.col * 4)にind1_p,ind2_p, xa_p xa1_pを用意
	my_CLAHE_Interpolation_Body(const cv::Mat& src, const cv::Mat& dst, const cv::Mat& lut, const cv::Size& tileSize, const int& tilesX, const int& tilesY) :
		src_(src), dst_(dst), lut_(lut), tileSize_(tileSize), tilesX_(tilesX), tilesY_(tilesY)
	{
		buf.allocate(src.cols << 2);
		//ind1_p = buf.data();
		ind1_p = buf.ptr();
		ind2_p = ind1_p + src.cols;
		xa_p = (float *)(ind2_p + src.cols);
		xa1_p = xa_p + src.cols;

		int lut_step = static_cast<int>(lut_.step / sizeof(T));
		//int lut_step = 256;
		float inv_tw = 1.0f / tileSize_.width;

		for (int x = 0; x < src.cols; ++x)
		{
			float txf = x * inv_tw - 0.5f;

			int tx1 = cvFloor(txf);
			int tx2 = tx1 + 1;

			xa_p[x] = txf - tx1;
			xa1_p[x] = 1.0f - xa_p[x];

			tx1 = std::max(tx1, 0);
			tx2 = std::min(tx2, tilesX_ - 1);

			ind1_p[x] = tx1 * lut_step;
			ind2_p[x] = tx2 * lut_step;
		}

	}

	void operator()(const cv::Range& range) const override;

private:
	cv::Mat src_;
	mutable cv::Mat dst_;
	cv::Mat lut_;

	cv::Size tileSize_;
	int tilesX_;
	int tilesY_;

	cv::AutoBuffer<int> buf;
	//これらのポインタは何?
	//なぜxa_pはfloat *
	int *ind1_p, *ind2_p;
	float *xa_p, *xa1_p;
};

cv::Ptr <cv::CLAHE> mycreateCLAHE(double clipLimit, cv::Size tileGridSize);
void prepare_buf(int *ind1_p, int *ind2_p, float *xa_p, float *xa1_p);
void calc_lut_fpga(cv::Mat src, cv::Mat lut, cv::Size tileSize, int tilesX, int clipLimit, float lutScale);
void interpolation_fpga(cv::Mat src, cv::Mat dst, cv::Mat lut, cv::Size tileSize, int tilesX, int tilesY, int *ind1_p, int *ind2_p, float *xa_p, float *xa1_p);
unsigned char saturate_cast_sw(float v);
#endif
