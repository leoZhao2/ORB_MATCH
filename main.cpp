#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	//读取图像
	Mat img_1 = imread("1.png", CV_LOAD_IMAGE_COLOR);
	Mat img_2 = imread("2.png", CV_LOAD_IMAGE_COLOR);

	//初始化
	vector<KeyPoint> keypoints_1, keypoints_2;//关键点,指特征点在图像里的位置
	Mat descriptors_1, descriptors_2;//描述子,通常是向量
	Ptr<ORB> orb = ORB::create(500, 1.2f, 8, 31, 0, 2, ORB::HARRIS_SCORE, 31, 20);

	//第一步：检测OrientFAST角点位置
	orb->detect(img_1, keypoints_1);
	orb->detect(img_2, keypoints_2);

	//第2步：根据角点位置计算BRIEF描述子
	orb->compute(img_1, keypoints_1, descriptors_1);
	orb->compute(img_2, keypoints_2, descriptors_2);

	Mat outimg1;
	drawKeypoints(img_1, keypoints_1, outimg1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	imshow("1.png的ORB特征点", outimg1);
	Mat outimg2;
	drawKeypoints(img_2, keypoints_2, outimg2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	imshow("2.png的ORB特征点", outimg2);

	//第3步：对两幅图像中的BRIEF描述子进行匹配，使用Hamming距离
	vector<DMatch> matches;
	//特征匹配的方法：暴力匹配
	BFMatcher matcher(NORM_HAMMING);
	matcher.match(descriptors_1, descriptors_2, matches);
	//    for(auto it=matches.begin();it!=matches.end();++it)
	//    {
	//        cout<<*it<<" ";
	//    }
	//    cout<<endl;

		//第4步：匹配点对筛distance是min_dist

		double min_dist = 10000, max_dist = 0;

	//找出所有匹配之间的最小距离和最大距离，即最相似的和最不相似的和最不相似的两组点之间的距离
	for (int i = 0; i < descriptors_1.rows; ++i)
	{
		double dist = matches[i].distance;
		//        cout<<dist<<endl;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	printf("--Max dist:%f\n", max_dist);
	printf("--Min dist:%f\n", min_dist);

	//当描述子之间的距离大于两倍的最小距离时，即认为匹配有误
	//但有时候最小距离会非常小，设置一个经验值作为下限
	vector<DMatch> good_matches;
	for (int i = 0; i < descriptors_1.rows; ++i)
	{
		if (matches[i].distance <= max(2 * min_dist, 30.0))
		{
			good_matches.push_back(matches[i]);
		}
	}

	//第5步：绘制匹配结果
	Mat img_match;
	Mat img_goodmatch;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_match);
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, good_matches, img_goodmatch);
	imshow("所有匹配点对", img_match);
	imshow("优化后匹配点对", img_goodmatch);
	waitKey(0);

	return 0;
}