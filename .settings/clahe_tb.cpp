#include "xf_headers.h"
#include "clahe.h"
#include "clahe_sw.h"

cv::Mat do_clahe(cv::Mat src);
cv::Mat do_myclahe(cv::Mat src);

int main(int argc,char **argv){
	cv::Mat img, img2, result, result_ocv, error;

	if(argc != 2){
		printf("Usage : <executable> <input image> \n");
		return -1;
	}

	img = cv::imread(argv[1],cv::IMREAD_GRAYSCALE);

	if(!img.data){
		printf("no image \n");
		return -1;
	}

	cv::resize(img, img, cv::Size(WIDTH, HEIGHT));
	img2 = img.clone();

	result_ocv.create(img.size(), img.type());
	result.create(img.size(), img.type());
	error.create(img.size(), img.type());

	cv::imwrite("input.png",img);
	uint16_t height = img.rows;
	uint16_t width = img.cols;

	axis_t *input = new axis_t [height*width];
	axis_t *output = new axis_t [height*width];

	// input
	for (int i = 0; i < HEIGHT; ++i) {
		for (int j = 0; j < WIDTH; ++j) {
			input[i*WIDTH+j].data = img.at<unsigned char>(i,j);
		}
	}

	// FPGA
	clahe_top(input, output);

	// output
	for (int i = 0; i < HEIGHT; ++i) {
			for (int j = 0; j < WIDTH; ++j) {
				result.at<unsigned char>(i,j) = output[i*WIDTH+j].data;
			}
	}

	delete[] input;
	delete[] output;


	//OpenCV execution
	//result_ocv = do_clahe(img2);
	//Our SW execution
	result_ocv = do_myclahe(img2);

	//check diff
	cv::absdiff(result,result_ocv,error);

	cv::Mat color;
	double minval=256,maxval=0;
	int cnt = 0;
	for (int i=0;i<error.rows;i++){
		for(int j=0;j<error.cols;j++){
			uchar v = error.at<uchar>(i,j);
			if (v!=0) {
				error.at<uchar>(i,j)  = 255 ;
				cnt++;
			}
			if (minval > v )
				minval = v;
			if (maxval < v)
				maxval = v;
		}
	}
	float err_per = 100.0*(float)cnt/(error.rows*error.cols);

	fprintf(stderr,"Minimum error in intensity = %f\n Maximum error in intensity = %f\n Percentage of pixels above error threshold = %f\n",minval,maxval,err_per);
	fprintf(stderr, "cnt is %d\n", cnt);

	cv::imwrite("output_hls.png", result);
	cv::imwrite("resize_ocv.png", result_ocv);
	cv::imwrite("error.png", error);

	if(maxval>2){
		printf("\nTest Failed\n");
		return -1;
	}

	return 0;
}

cv::Mat do_clahe(cv::Mat src) {
	cv::Mat dst;
	dst.create(src.size(), src.type());
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	clahe->setClipLimit(CLIP_LIMIT);
	clahe->setTilesGridSize(cv::Size(F_tilesX, F_tilesY));
	clahe->apply(src, dst);

	return dst;
}

cv::Mat do_myclahe(cv::Mat src) {
	cv::Mat dst;
	dst.create(src.size(), src.type());
	cv::Ptr<cv::CLAHE> clahe = mycreateCLAHE(CLIP_LIMIT, cv::Size(F_tilesX, F_tilesY));
	clahe->apply(src, dst);

	return dst;
}



