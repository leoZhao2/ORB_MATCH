#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	//��ȡͼ��
	Mat img_1 = imread("1.png", CV_LOAD_IMAGE_COLOR);
	Mat img_2 = imread("2.png", CV_LOAD_IMAGE_COLOR);

	//��ʼ��
	vector<KeyPoint> keypoints_1, keypoints_2;//�ؼ���,ָ��������ͼ�����λ��
	Mat descriptors_1, descriptors_2;//������,ͨ��������
	Ptr<ORB> orb = ORB::create(500, 1.2f, 8, 31, 0, 2, ORB::HARRIS_SCORE, 31, 20);

	//��һ�������OrientFAST�ǵ�λ��
	orb->detect(img_1, keypoints_1);
	orb->detect(img_2, keypoints_2);

	//��2�������ݽǵ�λ�ü���BRIEF������
	orb->compute(img_1, keypoints_1, descriptors_1);
	orb->compute(img_2, keypoints_2, descriptors_2);

	Mat outimg1;
	drawKeypoints(img_1, keypoints_1, outimg1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	imshow("1.png��ORB������", outimg1);
	Mat outimg2;
	drawKeypoints(img_2, keypoints_2, outimg2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	imshow("2.png��ORB������", outimg2);

	//��3����������ͼ���е�BRIEF�����ӽ���ƥ�䣬ʹ��Hamming����
	vector<DMatch> matches;
	//����ƥ��ķ���������ƥ��
	BFMatcher matcher(NORM_HAMMING);
	matcher.match(descriptors_1, descriptors_2, matches);
	//    for(auto it=matches.begin();it!=matches.end();++it)
	//    {
	//        cout<<*it<<" ";
	//    }
	//    cout<<endl;

		//��4����ƥ����ɸdistance��min_dist

		double min_dist = 10000, max_dist = 0;

	//�ҳ�����ƥ��֮�����С����������룬�������Ƶĺ�����Ƶĺ�����Ƶ������֮��ľ���
	for (int i = 0; i < descriptors_1.rows; ++i)
	{
		double dist = matches[i].distance;
		//        cout<<dist<<endl;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	printf("--Max dist:%f\n", max_dist);
	printf("--Min dist:%f\n", min_dist);

	//��������֮��ľ��������������С����ʱ������Ϊƥ������
	//����ʱ����С�����ǳ�С������һ������ֵ��Ϊ����
	vector<DMatch> good_matches;
	for (int i = 0; i < descriptors_1.rows; ++i)
	{
		if (matches[i].distance <= max(2 * min_dist, 30.0))
		{
			good_matches.push_back(matches[i]);
		}
	}

	//��5��������ƥ����
	Mat img_match;
	Mat img_goodmatch;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_match);
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, good_matches, img_goodmatch);
	imshow("����ƥ����", img_match);
	imshow("�Ż���ƥ����", img_goodmatch);
	waitKey(0);

	return 0;
}