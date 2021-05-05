#include<opencv2/opencv.hpp>
#define IMAGES_PATH ""
#define STEP 6

int findColor(cv::Scalar color ) 
{
	cv::Scalar c[4] = { cv::Scalar(0, 0, 255),cv::Scalar(0, 255, 0),cv::Scalar(255, 0, 0),cv::Scalar(0, 255, 255) };
	cv::Scalar yellow = cv::Scalar(0, 255, 255);
	
	int colorIndex = 9999;
	int minVal = 9999;
	for (int i = 0; i < sizeof(c)/sizeof(cv::Scalar); i++)
	{
		if (cv::norm(color - c[i]) < minVal)
		{
			minVal = cv::norm(color - c[i]);
			colorIndex = i;
		}
	}
	/*
	switch (colorIndex)
	{
	case 0:		std::cout << "red" << std::endl;		break;
	case 1:		std::cout << "green" << std::endl;		break;
	case 2:		std::cout << "blue" << std::endl;		break;
	case 3:		std::cout << "yellow" << std::endl;		break;
	}*/
	return colorIndex;
}

void wait(int time = 0)
{
	if (cv::waitKey(time) == 'q') exit(0);
}

void konturowanie(const cv::Mat& img_c, std::vector<std::vector<cv::Point>>& contours, std::vector<cv::Vec4i>& hierarchy)
{
	cv::Mat img_gr;
	cv::cvtColor(img_c, img_gr, cv::COLOR_BGR2GRAY);
	cv::threshold(img_gr, img_gr, 250, 255, cv::THRESH_BINARY_INV);
	cv::findContours(img_gr, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
}

#if STEP<=5
void analiza(const cv::Mat& img_c, const std::vector<std::vector<cv::Point>>& contours, const std::vector<cv::Vec4i>& hierarchy)
{
	for (int i = 0; i < contours.size(); ++i)
	{
		if (hierarchy[i][3] < 0) //element nadrz�dny
		{
			cv::Mat maska(img_c.size(), CV_8UC1);
			maska = 0;
			cv::drawContours(maska, contours, i, cv::Scalar::all(255), -1, 8, hierarchy);
			cv::imshow("Maska", maska);
			cv::Mat img_tmp;
#if STEP==1
			img_tmp = img_c.clone();
			img_tmp.setTo(cv::Scalar(128, 128, 128), maska);
#endif
#if STEP==2
			img_tmp = img_c.clone();
			img_tmp.setTo(cv::Scalar(128, 128, 128), ~maska);
			cv::imshow("img_tmp", img_tmp);
#endif
#if STEP==3
			img_c.copyTo(img_tmp, maska);
			cv::imshow("img_tmp", img_tmp);
#endif
#if STEP==4
			cv::bitwise_not(img_c, img_tmp, maska);
			cv::imshow("img_tmp", img_tmp);
#endif
#if STEP==5
			cv::Scalar mean, stddev;
			cv::meanStdDev(img_c, mean, stddev, maska);
			std::cout << "Wartosc srednia" << mean << " Odchylenie: " << stddev << std::endl;
#endif
			wait();
		}
	}
}
#endif

#if STEP==6
void analiza(const cv::Mat& img_c, const std::vector<std::vector<cv::Point>>& contours, const std::vector<cv::Vec4i>& hierarchy)
{
	int colorTab[4] = { 0,0,0,0 };
	for (int i = 0; i < contours.size(); ++i)
	{
		if (hierarchy[i][3] < 0) //element nadrz�dny
		{
			cv::Rect r = cv::boundingRect(contours[i]);
			cv::Mat maska(r.size(), CV_8UC1);
			maska = 0;
			cv::drawContours(maska, contours, i, cv::Scalar::all(255), -1, 8, hierarchy, INT_MAX, -r.tl());
			cv::imshow("Maska", maska);
			cv::Mat img_tmp(img_c, r);
			cv::Scalar sredni_kolor = cv::mean(img_tmp, maska);
			//std::cout << sredni_kolor << std::endl;
			colorTab[findColor(sredni_kolor)]++;
		}
	}

		std::cout << "Number of red: " << colorTab[0] << std::endl;
		std::cout << "Number of green: " << colorTab[1] << std::endl;
		std::cout << "Number of blue: " << colorTab[2] << std::endl;
		std::cout << "Number of yellow: " << colorTab[3] << std::endl;

}
#endif

int main()
{
	for (int i = 1; i <= 3; ++i)
	{
		std::stringstream ss;
		ss << IMAGES_PATH << "" << i << ".png";
		cv::Mat img = cv::imread(ss.str());
		cv::imshow("Test", img);
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		konturowanie(img, contours, hierarchy);
		analiza(img, contours, hierarchy);
		wait();
	}
}