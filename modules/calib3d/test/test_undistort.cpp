/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "test_precomp.hpp"
#include "opencv2/imgproc/imgproc_c.h"

namespace opencv_test { namespace {

class CV_DefaultNewCameraMatrixTest : public cvtest::ArrayTest
{
public:
    CV_DefaultNewCameraMatrixTest();
protected:
    int prepare_test_case (int test_case_idx);
    void prepare_to_validation( int test_case_idx );
    void get_test_array_types_and_sizes( int test_case_idx, vector<vector<Size> >& sizes, vector<vector<int> >& types );
    void run_func();

private:
    cv::Size img_size;
    cv::Mat camera_mat;
    cv::Mat new_camera_mat;

    int matrix_type;

    bool center_principal_point;

    static const int MAX_X = 2048;
    static const int MAX_Y = 2048;
    //static const int MAX_VAL = 10000;
};

CV_DefaultNewCameraMatrixTest::CV_DefaultNewCameraMatrixTest()
{
    test_array[INPUT].push_back(NULL);
    test_array[OUTPUT].push_back(NULL);
    test_array[REF_OUTPUT].push_back(NULL);

    matrix_type = 0;
    center_principal_point = false;
}

void CV_DefaultNewCameraMatrixTest::get_test_array_types_and_sizes( int test_case_idx, vector<vector<Size> >& sizes, vector<vector<int> >& types )
{
    cvtest::ArrayTest::get_test_array_types_and_sizes(test_case_idx,sizes,types);
    RNG& rng = ts->get_rng();
    matrix_type = types[INPUT][0] = types[OUTPUT][0]= types[REF_OUTPUT][0] = cvtest::randInt(rng)%2 ? CV_64F : CV_32F;
    sizes[INPUT][0] = sizes[OUTPUT][0] = sizes[REF_OUTPUT][0] = cvSize(3,3);
}

int CV_DefaultNewCameraMatrixTest::prepare_test_case(int test_case_idx)
{
    int code = cvtest::ArrayTest::prepare_test_case( test_case_idx );

    if (code <= 0)
        return code;

    RNG& rng = ts->get_rng();

    img_size.width = cvtest::randInt(rng) % MAX_X + 1;
    img_size.height = cvtest::randInt(rng) % MAX_Y + 1;

    center_principal_point = ((cvtest::randInt(rng) % 2)!=0);

    // Generating camera_mat matrix
    double sz = MAX(img_size.width, img_size.height);
    double aspect_ratio = cvtest::randReal(rng)*0.6 + 0.7;
    double a[9] = {0,0,0,0,0,0,0,0,1};
    Mat _a(3,3,CV_64F,a);
    a[2] = (img_size.width - 1)*0.5 + cvtest::randReal(rng)*10 - 5;
    a[5] = (img_size.height - 1)*0.5 + cvtest::randReal(rng)*10 - 5;
    a[0] = sz/(0.9 - cvtest::randReal(rng)*0.6);
    a[4] = aspect_ratio*a[0];

    Mat& _a0 = test_mat[INPUT][0];
    cvtest::convert(_a, _a0, _a0.type());
    camera_mat = _a0;

    return code;

}

void CV_DefaultNewCameraMatrixTest::run_func()
{
    new_camera_mat = cv::getDefaultNewCameraMatrix(camera_mat,img_size,center_principal_point);
}

void CV_DefaultNewCameraMatrixTest::prepare_to_validation( int /*test_case_idx*/ )
{
    const Mat& src = test_mat[INPUT][0];
    Mat& dst = test_mat[REF_OUTPUT][0];
    Mat& test_output = test_mat[OUTPUT][0];
    Mat& output = new_camera_mat;
    cvtest::convert( output, test_output, test_output.type() );
    if (!center_principal_point)
    {
        cvtest::copy(src, dst);
    }
    else
    {
        double a[9] = {0,0,0,0,0,0,0,0,1};
        Mat _a(3,3,CV_64F,a);
        if (matrix_type == CV_64F)
        {
            a[0] = src.at<double>(0,0);
            a[4] = src.at<double>(1,1);
        }
        else
        {
            a[0] = src.at<float>(0,0);
            a[4] = src.at<float>(1,1);
        }
        a[2] = (img_size.width - 1)*0.5;
        a[5] = (img_size.height - 1)*0.5;
        cvtest::convert( _a, dst, dst.type() );
    }
}

//---------

class CV_GetOptimalNewCameraMatrixNoDistortionTest : public cvtest::ArrayTest
{
public:
    CV_GetOptimalNewCameraMatrixNoDistortionTest();
protected:
    int prepare_test_case (int test_case_idx);
    void prepare_to_validation(int test_case_idx);
    void get_test_array_types_and_sizes(int test_case_idx, vector<vector<Size> >& sizes, vector<vector<int> >& types);
    void run_func();

private:
    cv::Mat camera_mat;
    cv::Mat distortion_coeffs;
    cv::Mat new_camera_mat;

    cv::Size img_size;
    double alpha;
    bool center_principal_point;

    int matrix_type;

    static const int MAX_X = 2048;
    static const int MAX_Y = 2048;
};

CV_GetOptimalNewCameraMatrixNoDistortionTest::CV_GetOptimalNewCameraMatrixNoDistortionTest()
{
    test_array[INPUT].push_back(NULL); // camera_mat
    test_array[INPUT].push_back(NULL); // distortion_coeffs
    test_array[OUTPUT].push_back(NULL); // new_camera_mat
    test_array[REF_OUTPUT].push_back(NULL);

    alpha = 0.0;
    center_principal_point = false;
    matrix_type = 0;
}

void CV_GetOptimalNewCameraMatrixNoDistortionTest::get_test_array_types_and_sizes(int test_case_idx, vector<vector<Size> >& sizes, vector<vector<int> >& types)
{
    cvtest::ArrayTest::get_test_array_types_and_sizes(test_case_idx, sizes, types);
    RNG& rng = ts->get_rng();
    matrix_type = types[INPUT][0] = types[INPUT][1] = types[OUTPUT][0] = types[REF_OUTPUT][0] = cvtest::randInt(rng)%2 ? CV_64F : CV_32F;
    sizes[INPUT][0] = sizes[OUTPUT][0] = sizes[REF_OUTPUT][0] = cvSize(3,3);
    sizes[INPUT][1] = cvSize(1,4);
}

int CV_GetOptimalNewCameraMatrixNoDistortionTest::prepare_test_case(int test_case_idx)
{
    int code = cvtest::ArrayTest::prepare_test_case( test_case_idx );

    if (code <= 0)
        return code;

    RNG& rng = ts->get_rng();

    alpha = cvtest::randReal(rng);
    center_principal_point = ((cvtest::randInt(rng) % 2)!=0);

    // Generate random camera matrix. Use floating point precision for source to avoid precision loss
    img_size.width = cvtest::randInt(rng) % MAX_X + 1;
    img_size.height = cvtest::randInt(rng) % MAX_Y + 1;
    const float aspect_ratio = static_cast<float>(img_size.width) / img_size.height;
    float cam_array[9] = {0,0,0,0,0,0,0,0,1};
    cam_array[2] = static_cast<float>((img_size.width - 1)*0.5);  // center
    cam_array[5] = static_cast<float>((img_size.height - 1)*0.5); // center
    cam_array[0] = static_cast<float>(MAX(img_size.width, img_size.height)/(0.9 - cvtest::randReal(rng)*0.6));
    cam_array[4] = aspect_ratio*cam_array[0];

    Mat& input_camera_mat = test_mat[INPUT][0];
    cvtest::convert(Mat(3, 3, CV_32F, cam_array), input_camera_mat, input_camera_mat.type());
    camera_mat = input_camera_mat;

    // Generate zero distortion matrix
    const Mat zero_dist_coeffs = Mat::zeros(1, 4, CV_32F);
    Mat& input_dist_coeffs = test_mat[INPUT][1];
    cvtest::convert(zero_dist_coeffs, input_dist_coeffs, input_dist_coeffs.type());
    distortion_coeffs = input_dist_coeffs;

    return code;
}

void CV_GetOptimalNewCameraMatrixNoDistortionTest::run_func()
{
    new_camera_mat = cv::getOptimalNewCameraMatrix(camera_mat, distortion_coeffs, img_size, alpha, img_size, NULL, center_principal_point);
}

void CV_GetOptimalNewCameraMatrixNoDistortionTest::prepare_to_validation(int /*test_case_idx*/)
{
    const Mat& src = test_mat[INPUT][0];
    Mat& dst = test_mat[REF_OUTPUT][0];
    cvtest::copy(src, dst);

    Mat& output = test_mat[OUTPUT][0];
    cvtest::convert(new_camera_mat, output, output.type());
}

//---------

class CV_UndistortPointsTest : public cvtest::ArrayTest
{
public:
    CV_UndistortPointsTest();
protected:
    int prepare_test_case (int test_case_idx);
    void prepare_to_validation( int test_case_idx );
    void get_test_array_types_and_sizes( int test_case_idx, vector<vector<Size> >& sizes, vector<vector<int> >& types );
    double get_success_error_level( int test_case_idx, int i, int j );
    void run_func();
    void distortPoints(const CvMat* _src, CvMat* _dst, const CvMat* _cameraMatrix,
                       const CvMat* _distCoeffs, const CvMat* matR, const CvMat* matP);

private:
    bool useCPlus;
    bool useDstMat;
    static const int N_POINTS = 10;
    static const int MAX_X = 2048;
    static const int MAX_Y = 2048;

    bool zero_new_cam;
    bool zero_distortion;
    bool zero_R;

    cv::Size img_size;
    cv::Mat dst_points_mat;

    cv::Mat camera_mat;
    cv::Mat R;
    cv::Mat P;
    cv::Mat distortion_coeffs;
    cv::Mat src_points;
    std::vector<cv::Point2f> dst_points;
};

CV_UndistortPointsTest::CV_UndistortPointsTest()
{
    test_array[INPUT].push_back(NULL); // points matrix
    test_array[INPUT].push_back(NULL); // camera matrix
    test_array[INPUT].push_back(NULL); // distortion coeffs
    test_array[INPUT].push_back(NULL); // R matrix
    test_array[INPUT].push_back(NULL); // P matrix
    test_array[OUTPUT].push_back(NULL); // distorted dst points
    test_array[TEMP].push_back(NULL); // dst points
    test_array[REF_OUTPUT].push_back(NULL);

    useCPlus = useDstMat = false;
    zero_new_cam = zero_distortion = zero_R = false;
}

void CV_UndistortPointsTest::get_test_array_types_and_sizes( int test_case_idx, vector<vector<Size> >& sizes, vector<vector<int> >& types )
{
    cvtest::ArrayTest::get_test_array_types_and_sizes(test_case_idx,sizes,types);
    RNG& rng = ts->get_rng();
    useCPlus = ((cvtest::randInt(rng) % 2)!=0);
    //useCPlus = 0;
    if (useCPlus)
    {
        types[INPUT][0] = types[OUTPUT][0] = types[REF_OUTPUT][0] = types[TEMP][0]= CV_32FC2;
    }
    else
    {
        types[INPUT][0] = types[OUTPUT][0] = types[REF_OUTPUT][0] = types[TEMP][0]= cvtest::randInt(rng)%2 ? CV_64FC2 : CV_32FC2;
    }
    types[INPUT][1] = cvtest::randInt(rng)%2 ? CV_64F : CV_32F;
    types[INPUT][2] = cvtest::randInt(rng)%2 ? CV_64F : CV_32F;
    types[INPUT][3] = cvtest::randInt(rng)%2 ? CV_64F : CV_32F;
    types[INPUT][4] = cvtest::randInt(rng)%2 ? CV_64F : CV_32F;

    sizes[INPUT][0] = sizes[OUTPUT][0] = sizes[REF_OUTPUT][0] = sizes[TEMP][0]= cvtest::randInt(rng)%2 ? cvSize(1,N_POINTS) : cvSize(N_POINTS,1);
    sizes[INPUT][1] = sizes[INPUT][3] = cvSize(3,3);
    sizes[INPUT][4] = cvtest::randInt(rng)%2 ? cvSize(3,3) : cvSize(4,3);

    if (cvtest::randInt(rng)%2)
    {
        if (cvtest::randInt(rng)%2)
        {
            sizes[INPUT][2] = cvSize(1,4);
        }
        else
        {
            sizes[INPUT][2] = cvSize(1,5);
        }
    }
    else
    {
        if (cvtest::randInt(rng)%2)
        {
            sizes[INPUT][2] = cvSize(4,1);
        }
        else
        {
            sizes[INPUT][2] = cvSize(5,1);
        }
    }
}

int CV_UndistortPointsTest::prepare_test_case(int test_case_idx)
{
    RNG& rng = ts->get_rng();
    int code = cvtest::ArrayTest::prepare_test_case( test_case_idx );

    if (code <= 0)
        return code;

    useDstMat = (cvtest::randInt(rng) % 2) == 0;

    img_size.width = cvtest::randInt(rng) % MAX_X + 1;
    img_size.height = cvtest::randInt(rng) % MAX_Y + 1;
    int dist_size = test_mat[INPUT][2].cols > test_mat[INPUT][2].rows ? test_mat[INPUT][2].cols : test_mat[INPUT][2].rows;
    double cam[9] = {0,0,0,0,0,0,0,0,1};
    vector<double> dist(dist_size);
    vector<double> proj(test_mat[INPUT][4].cols * test_mat[INPUT][4].rows);
    vector<Point2d> points(N_POINTS);

    Mat _camera(3,3,CV_64F,cam);
    Mat _distort(test_mat[INPUT][2].rows,test_mat[INPUT][2].cols,CV_64F,&dist[0]);
    Mat _proj(test_mat[INPUT][4].size(), CV_64F, &proj[0]);
    Mat _points(test_mat[INPUT][0].size(), CV_64FC2, &points[0]);

    _proj = Scalar::all(0);

    //Generating points
    for( int i = 0; i < N_POINTS; i++ )
    {
        points[i].x = cvtest::randReal(rng)*img_size.width;
        points[i].y = cvtest::randReal(rng)*img_size.height;
    }

    //Generating camera matrix
    double sz = MAX(img_size.width,img_size.height);
    double aspect_ratio = cvtest::randReal(rng)*0.6 + 0.7;
    cam[2] = (img_size.width - 1)*0.5 + cvtest::randReal(rng)*10 - 5;
    cam[5] = (img_size.height - 1)*0.5 + cvtest::randReal(rng)*10 - 5;
    cam[0] = sz/(0.9 - cvtest::randReal(rng)*0.6);
    cam[4] = aspect_ratio*cam[0];

    //Generating distortion coeffs
    dist[0] = cvtest::randReal(rng)*0.06 - 0.03;
    dist[1] = cvtest::randReal(rng)*0.06 - 0.03;
    if( dist[0]*dist[1] > 0 )
        dist[1] = -dist[1];
    if( cvtest::randInt(rng)%4 != 0 )
    {
        dist[2] = cvtest::randReal(rng)*0.004 - 0.002;
        dist[3] = cvtest::randReal(rng)*0.004 - 0.002;
        if (dist_size > 4)
            dist[4] = cvtest::randReal(rng)*0.004 - 0.002;
    }
    else
    {
        dist[2] = dist[3] = 0;
        if (dist_size > 4)
            dist[4] = 0;
    }

    //Generating P matrix (projection)
    if( test_mat[INPUT][4].cols != 4 )
    {
        proj[8] = 1;
        if (cvtest::randInt(rng)%2 == 0) // use identity new camera matrix
        {
            proj[0] = 1;
            proj[4] = 1;
        }
        else
        {
            proj[0] = cam[0] + (cvtest::randReal(rng) - (double)0.5)*0.2*cam[0]; //10%
            proj[4] = cam[4] + (cvtest::randReal(rng) - (double)0.5)*0.2*cam[4]; //10%
            proj[2] = cam[2] + (cvtest::randReal(rng) - (double)0.5)*0.3*img_size.width; //15%
            proj[5] = cam[5] + (cvtest::randReal(rng) - (double)0.5)*0.3*img_size.height; //15%
        }
    }
    else
    {
        proj[10] = 1;
        proj[0] = cam[0] + (cvtest::randReal(rng) - (double)0.5)*0.2*cam[0]; //10%
        proj[5] = cam[4] + (cvtest::randReal(rng) - (double)0.5)*0.2*cam[4]; //10%
        proj[2] = cam[2] + (cvtest::randReal(rng) - (double)0.5)*0.3*img_size.width; //15%
        proj[6] = cam[5] + (cvtest::randReal(rng) - (double)0.5)*0.3*img_size.height; //15%

        proj[3] = (img_size.height + img_size.width - 1)*0.5 + cvtest::randReal(rng)*10 - 5;
        proj[7] = (img_size.height + img_size.width - 1)*0.5 + cvtest::randReal(rng)*10 - 5;
        proj[11] = (img_size.height + img_size.width - 1)*0.5 + cvtest::randReal(rng)*10 - 5;
    }

    //Generating R matrix
    Mat _rot(3,3,CV_64F);
    Mat rotation(1,3,CV_64F);
    rotation.at<double>(0) = CV_PI*(cvtest::randReal(rng) - (double)0.5); // phi
    rotation.at<double>(1) = CV_PI*(cvtest::randReal(rng) - (double)0.5); // ksi
    rotation.at<double>(2) = CV_PI*(cvtest::randReal(rng) - (double)0.5); //khi
    cvtest::Rodrigues(rotation, _rot);

    //copying data
    //src_points = &_points;
    _points.convertTo(test_mat[INPUT][0], test_mat[INPUT][0].type());
    _camera.convertTo(test_mat[INPUT][1], test_mat[INPUT][1].type());
    _distort.convertTo(test_mat[INPUT][2], test_mat[INPUT][2].type());
    _rot.convertTo(test_mat[INPUT][3], test_mat[INPUT][3].type());
    _proj.convertTo(test_mat[INPUT][4], test_mat[INPUT][4].type());

    zero_distortion = (cvtest::randInt(rng)%2) == 0 ? false : true;
    zero_new_cam = (cvtest::randInt(rng)%2) == 0 ? false : true;
    zero_R = (cvtest::randInt(rng)%2) == 0 ? false : true;

    if (useCPlus)
    {
        _points.convertTo(src_points, CV_32F);

        camera_mat = test_mat[INPUT][1];
        distortion_coeffs = test_mat[INPUT][2];
        R = test_mat[INPUT][3];
        P = test_mat[INPUT][4];
    }

    return code;
}

void CV_UndistortPointsTest::prepare_to_validation(int /*test_case_idx*/)
{
    int dist_size = test_mat[INPUT][2].cols > test_mat[INPUT][2].rows ? test_mat[INPUT][2].cols : test_mat[INPUT][2].rows;
    double cam[9] = {0,0,0,0,0,0,0,0,1};
    double rot[9] = {1,0,0,0,1,0,0,0,1};

    double* dist = new double[dist_size ];
    double* proj = new double[test_mat[INPUT][4].cols * test_mat[INPUT][4].rows];
    double* points = new double[N_POINTS*2];
    double* r_points = new double[N_POINTS*2];
    //Run reference calculations
    CvMat ref_points= cvMat(test_mat[INPUT][0].rows,test_mat[INPUT][0].cols,CV_64FC2,r_points);
    CvMat _camera = cvMat(3,3,CV_64F,cam);
    CvMat _rot = cvMat(3,3,CV_64F,rot);
    CvMat _distort = cvMat(test_mat[INPUT][2].rows,test_mat[INPUT][2].cols,CV_64F,dist);
    CvMat _proj = cvMat(test_mat[INPUT][4].rows,test_mat[INPUT][4].cols,CV_64F,proj);
    CvMat _points= cvMat(test_mat[TEMP][0].rows,test_mat[TEMP][0].cols,CV_64FC2,points);

    Mat __camera = cvarrToMat(&_camera);
    Mat __distort = cvarrToMat(&_distort);
    Mat __rot = cvarrToMat(&_rot);
    Mat __proj = cvarrToMat(&_proj);
    Mat __points = cvarrToMat(&_points);
    Mat _ref_points = cvarrToMat(&ref_points);

    cvtest::convert(test_mat[INPUT][1], __camera, __camera.type());
    cvtest::convert(test_mat[INPUT][2], __distort, __distort.type());
    cvtest::convert(test_mat[INPUT][3], __rot, __rot.type());
    cvtest::convert(test_mat[INPUT][4], __proj, __proj.type());

    if (useCPlus)
    {
        if (useDstMat)
        {
            CvMat temp = cvMat(dst_points_mat);
            for (int i=0;i<N_POINTS*2;i++)
            {
                points[i] = temp.data.fl[i];
            }
        }
        else
        {
            for (int i=0;i<N_POINTS;i++)
            {
                points[2*i] = dst_points[i].x;
                points[2*i+1] = dst_points[i].y;
            }
        }
    }
    else
    {
        cvtest::convert(test_mat[TEMP][0],__points, __points.type());
    }

    CvMat* input2 = zero_distortion ? 0 : &_distort;
    CvMat* input3 = zero_R ? 0 : &_rot;
    CvMat* input4 = zero_new_cam ? 0 : &_proj;
    distortPoints(&_points,&ref_points,&_camera,input2,input3,input4);

    Mat& dst = test_mat[REF_OUTPUT][0];
    cvtest::convert(_ref_points, dst, dst.type());

    cvtest::copy(test_mat[INPUT][0], test_mat[OUTPUT][0]);

    delete[] dist;
    delete[] proj;
    delete[] points;
    delete[] r_points;
}

void CV_UndistortPointsTest::run_func()
{

    if (useCPlus)
    {
        cv::Mat input2,input3,input4;
        input2 = zero_distortion ? cv::Mat() : cv::Mat(test_mat[INPUT][2]);
        input3 = zero_R ? cv::Mat() : cv::Mat(test_mat[INPUT][3]);
        input4 = zero_new_cam ? cv::Mat() : cv::Mat(test_mat[INPUT][4]);

        if (useDstMat)
        {
            //cv::undistortPoints(src_points,dst_points_mat,camera_mat,distortion_coeffs,R,P);
            cv::undistortPoints(src_points,dst_points_mat,camera_mat,input2,input3,input4);
        }
        else
        {
            //cv::undistortPoints(src_points,dst_points,camera_mat,distortion_coeffs,R,P);
            cv::undistortPoints(src_points,dst_points,camera_mat,input2,input3,input4);
        }
    }
    else
    {
        CvMat _input0 = cvMat(test_mat[INPUT][0]), _input1 = cvMat(test_mat[INPUT][1]), _input2, _input3, _input4;
        CvMat _output = cvMat(test_mat[TEMP][0]);
        if(!zero_distortion)
            _input2 = cvMat(test_mat[INPUT][2]);
        if(!zero_R)
            _input3 = cvMat(test_mat[INPUT][3]);
        if(!zero_new_cam)
            _input4 = cvMat(test_mat[INPUT][4]);
        cvUndistortPoints(&_input0, &_output, &_input1,
                          zero_distortion ? 0 : &_input2,
                          zero_R ? 0 : &_input3,
                          zero_new_cam ? 0 : &_input4);
    }
}

void CV_UndistortPointsTest::distortPoints(const CvMat* _src, CvMat* _dst, const CvMat* _cameraMatrix,
                                            const CvMat* _distCoeffs,
                                            const CvMat* matR, const CvMat* matP)
{
    double a[9];

    CvMat* __P;
    if ((!matP)||(matP->cols == 3))
        __P = cvCreateMat(3,3,CV_64F);
    else
        __P = cvCreateMat(3,4,CV_64F);
    if (matP)
    {
        cvtest::convert(cvarrToMat(matP), cvarrToMat(__P), -1);
    }
    else
    {
        cvZero(__P);
        __P->data.db[0] = 1;
        __P->data.db[4] = 1;
        __P->data.db[8] = 1;
    }
    CvMat* __R = cvCreateMat(3,3,CV_64F);
    if (matR)
    {
        cvCopy(matR,__R);
    }
    else
    {
        cvZero(__R);
        __R->data.db[0] = 1;
        __R->data.db[4] = 1;
        __R->data.db[8] = 1;
    }
    for (int i=0;i<N_POINTS;i++)
    {
        int movement = __P->cols > 3 ? 1 : 0;
        double x = (_src->data.db[2*i]-__P->data.db[2])/__P->data.db[0];
        double y = (_src->data.db[2*i+1]-__P->data.db[5+movement])/__P->data.db[4+movement];
        CvMat inverse = cvMat(3,3,CV_64F,a);
        cvInvert(__R,&inverse);
        double w1 = x*inverse.data.db[6]+y*inverse.data.db[7]+inverse.data.db[8];
        double _x = (x*inverse.data.db[0]+y*inverse.data.db[1]+inverse.data.db[2])/w1;
        double _y = (x*inverse.data.db[3]+y*inverse.data.db[4]+inverse.data.db[5])/w1;

        //Distortions

        double __x = _x;
        double __y = _y;
        if (_distCoeffs)
        {
            double r2 = _x*_x+_y*_y;

            __x = _x*(1+_distCoeffs->data.db[0]*r2+_distCoeffs->data.db[1]*r2*r2)+
            2*_distCoeffs->data.db[2]*_x*_y+_distCoeffs->data.db[3]*(r2+2*_x*_x);
            __y = _y*(1+_distCoeffs->data.db[0]*r2+_distCoeffs->data.db[1]*r2*r2)+
            2*_distCoeffs->data.db[3]*_x*_y+_distCoeffs->data.db[2]*(r2+2*_y*_y);
            if ((_distCoeffs->cols > 4) || (_distCoeffs->rows > 4))
            {
                __x+=_x*_distCoeffs->data.db[4]*r2*r2*r2;
                __y+=_y*_distCoeffs->data.db[4]*r2*r2*r2;
            }
        }


        _dst->data.db[2*i] = __x*_cameraMatrix->data.db[0]+_cameraMatrix->data.db[2];
        _dst->data.db[2*i+1] = __y*_cameraMatrix->data.db[4]+_cameraMatrix->data.db[5];

    }

    cvReleaseMat(&__R);
    cvReleaseMat(&__P);

}


double CV_UndistortPointsTest::get_success_error_level( int /*test_case_idx*/, int /*i*/, int /*j*/ )
{
    return 5e-2;
}

//------------------------------------------------------

class CV_InitUndistortRectifyMapTest : public cvtest::ArrayTest
{
public:
    CV_InitUndistortRectifyMapTest();
protected:
    int prepare_test_case (int test_case_idx);
    void prepare_to_validation( int test_case_idx );
    void get_test_array_types_and_sizes( int test_case_idx, vector<vector<Size> >& sizes, vector<vector<int> >& types );
    double get_success_error_level( int test_case_idx, int i, int j );
    void run_func();

private:
    bool useCPlus;
    static const int N_POINTS = 100;
    static const int MAX_X = 2048;
    static const int MAX_Y = 2048;
    bool zero_new_cam;
    bool zero_distortion;
    bool zero_R;


    cv::Size img_size;

    cv::Mat camera_mat;
    cv::Mat R;
    cv::Mat new_camera_mat;
    cv::Mat distortion_coeffs;
    cv::Mat mapx;
    cv::Mat mapy;
    CvMat* _mapx;
    CvMat* _mapy;
    int mat_type;
};

CV_InitUndistortRectifyMapTest::CV_InitUndistortRectifyMapTest()
{
    test_array[INPUT].push_back(NULL); // test points matrix
    test_array[INPUT].push_back(NULL); // camera matrix
    test_array[INPUT].push_back(NULL); // distortion coeffs
    test_array[INPUT].push_back(NULL); // R matrix
    test_array[INPUT].push_back(NULL); // new camera matrix
    test_array[OUTPUT].push_back(NULL); // distorted dst points
    test_array[REF_OUTPUT].push_back(NULL);

    useCPlus = false;
    zero_distortion = zero_new_cam = zero_R = false;
    _mapx = _mapy = NULL;
    mat_type = 0;
}

void CV_InitUndistortRectifyMapTest::get_test_array_types_and_sizes( int test_case_idx, vector<vector<Size> >& sizes, vector<vector<int> >& types )
{
    cvtest::ArrayTest::get_test_array_types_and_sizes(test_case_idx,sizes,types);
    RNG& rng = ts->get_rng();
    useCPlus = ((cvtest::randInt(rng) % 2)!=0);
    //useCPlus = 0;
    types[INPUT][0] = types[OUTPUT][0] = types[REF_OUTPUT][0] = CV_64FC2;

    types[INPUT][1] = cvtest::randInt(rng)%2 ? CV_64F : CV_32F;
    types[INPUT][2] = cvtest::randInt(rng)%2 ? CV_64F : CV_32F;
    types[INPUT][3] = cvtest::randInt(rng)%2 ? CV_64F : CV_32F;
    types[INPUT][4] = cvtest::randInt(rng)%2 ? CV_64F : CV_32F;

    sizes[INPUT][0] = sizes[OUTPUT][0] = sizes[REF_OUTPUT][0] = cvSize(N_POINTS,1);
    sizes[INPUT][1] = sizes[INPUT][3] = cvSize(3,3);
    sizes[INPUT][4] = cvSize(3,3);

    if (cvtest::randInt(rng)%2)
    {
        if (cvtest::randInt(rng)%2)
        {
            sizes[INPUT][2] = cvSize(1,4);
        }
        else
        {
            sizes[INPUT][2] = cvSize(1,5);
        }
    }
    else
    {
        if (cvtest::randInt(rng)%2)
        {
            sizes[INPUT][2] = cvSize(4,1);
        }
        else
        {
            sizes[INPUT][2] = cvSize(5,1);
        }
    }
}


int CV_InitUndistortRectifyMapTest::prepare_test_case(int test_case_idx)
{
    RNG& rng = ts->get_rng();
    int code = cvtest::ArrayTest::prepare_test_case( test_case_idx );

    if (code <= 0)
        return code;

    img_size.width = cvtest::randInt(rng) % MAX_X + 1;
    img_size.height = cvtest::randInt(rng) % MAX_Y + 1;

    if (useCPlus)
    {
        mat_type = (cvtest::randInt(rng) % 2) == 0 ? CV_32FC1 : CV_16SC2;
        if ((cvtest::randInt(rng) % 4) == 0)
            mat_type = -1;
        if ((cvtest::randInt(rng) % 4) == 0)
            mat_type = CV_32FC2;
        _mapx = 0;
        _mapy = 0;
    }
    else
    {
        int typex = (cvtest::randInt(rng) % 2) == 0 ? CV_32FC1 : CV_16SC2;
        //typex = CV_32FC1; ///!!!!!!!!!!!!!!!!
        int typey = (typex == CV_32FC1) ? CV_32FC1 : CV_16UC1;

        _mapx = cvCreateMat(img_size.height,img_size.width,typex);
        _mapy = cvCreateMat(img_size.height,img_size.width,typey);


    }

    int dist_size = test_mat[INPUT][2].cols > test_mat[INPUT][2].rows ? test_mat[INPUT][2].cols : test_mat[INPUT][2].rows;
    double cam[9] = {0,0,0,0,0,0,0,0,1};
    vector<double> dist(dist_size);
    vector<double> new_cam(test_mat[INPUT][4].cols * test_mat[INPUT][4].rows);
    vector<Point2d> points(N_POINTS);

    Mat _camera(3,3,CV_64F,cam);
    Mat _distort(test_mat[INPUT][2].size(),CV_64F,&dist[0]);
    Mat _new_cam(test_mat[INPUT][4].size(),CV_64F,&new_cam[0]);
    Mat _points(test_mat[INPUT][0].size(),CV_64FC2, &points[0]);

    //Generating points
    for (int i=0;i<N_POINTS;i++)
    {
        points[i].x = cvtest::randReal(rng)*img_size.width;
        points[i].y = cvtest::randReal(rng)*img_size.height;
    }

    //Generating camera matrix
    double sz = MAX(img_size.width,img_size.height);
    double aspect_ratio = cvtest::randReal(rng)*0.6 + 0.7;
    cam[2] = (img_size.width - 1)*0.5 + cvtest::randReal(rng)*10 - 5;
    cam[5] = (img_size.height - 1)*0.5 + cvtest::randReal(rng)*10 - 5;
    cam[0] = sz/(0.9 - cvtest::randReal(rng)*0.6);
    cam[4] = aspect_ratio*cam[0];

    //Generating distortion coeffs
    dist[0] = cvtest::randReal(rng)*0.06 - 0.03;
    dist[1] = cvtest::randReal(rng)*0.06 - 0.03;
    if( dist[0]*dist[1] > 0 )
        dist[1] = -dist[1];
    if( cvtest::randInt(rng)%4 != 0 )
    {
        dist[2] = cvtest::randReal(rng)*0.004 - 0.002;
        dist[3] = cvtest::randReal(rng)*0.004 - 0.002;
        if (dist_size > 4)
            dist[4] = cvtest::randReal(rng)*0.004 - 0.002;
    }
    else
    {
        dist[2] = dist[3] = 0;
        if (dist_size > 4)
            dist[4] = 0;
    }

    //Generating new camera matrix
    _new_cam = Scalar::all(0);
    new_cam[8] = 1;

    //new_cam[0] = cam[0];
    //new_cam[4] = cam[4];
    //new_cam[2] = cam[2];
    //new_cam[5] = cam[5];

    new_cam[0] = cam[0] + (cvtest::randReal(rng) - (double)0.5)*0.2*cam[0]; //10%
    new_cam[4] = cam[4] + (cvtest::randReal(rng) - (double)0.5)*0.2*cam[4]; //10%
    new_cam[2] = cam[2] + (cvtest::randReal(rng) - (double)0.5)*0.3*img_size.width; //15%
    new_cam[5] = cam[5] + (cvtest::randReal(rng) - (double)0.5)*0.3*img_size.height; //15%


    //Generating R matrix
    Mat _rot(3,3,CV_64F);
    Mat rotation(1,3,CV_64F);
    rotation.at<double>(0) = CV_PI/8*(cvtest::randReal(rng) - (double)0.5); // phi
    rotation.at<double>(1) = CV_PI/8*(cvtest::randReal(rng) - (double)0.5); // ksi
    rotation.at<double>(2) = CV_PI/3*(cvtest::randReal(rng) - (double)0.5); //khi
    cvtest::Rodrigues(rotation, _rot);

    //cvSetIdentity(_rot);
    //copying data
    cvtest::convert( _points, test_mat[INPUT][0], test_mat[INPUT][0].type());
    cvtest::convert( _camera, test_mat[INPUT][1], test_mat[INPUT][1].type());
    cvtest::convert( _distort, test_mat[INPUT][2], test_mat[INPUT][2].type());
    cvtest::convert( _rot, test_mat[INPUT][3], test_mat[INPUT][3].type());
    cvtest::convert( _new_cam, test_mat[INPUT][4], test_mat[INPUT][4].type());

    zero_distortion = (cvtest::randInt(rng)%2) == 0 ? false : true;
    zero_new_cam = (cvtest::randInt(rng)%2) == 0 ? false : true;
    zero_R = (cvtest::randInt(rng)%2) == 0 ? false : true;

    if (useCPlus)
    {
        camera_mat = test_mat[INPUT][1];
        distortion_coeffs = test_mat[INPUT][2];
        R = test_mat[INPUT][3];
        new_camera_mat = test_mat[INPUT][4];
    }

    return code;
}

void CV_InitUndistortRectifyMapTest::prepare_to_validation(int/* test_case_idx*/)
{
#if 0
    int dist_size = test_mat[INPUT][2].cols > test_mat[INPUT][2].rows ? test_mat[INPUT][2].cols : test_mat[INPUT][2].rows;
    double cam[9] = {0,0,0,0,0,0,0,0,1};
    double rot[9] = {1,0,0,0,1,0,0,0,1};
    vector<double> dist(dist_size);
    vector<double> new_cam(test_mat[INPUT][4].cols * test_mat[INPUT][4].rows);
    vector<Point2d> points(N_POINTS);
    vector<Point2d> r_points(N_POINTS);
    //Run reference calculations
    Mat ref_points(test_mat[INPUT][0].size(),CV_64FC2,&r_points[0]);
    Mat _camera(3,3,CV_64F,cam);
    Mat _rot(3,3,CV_64F,rot);
    Mat _distort(test_mat[INPUT][2].size(),CV_64F,&dist[0]);
    Mat _new_cam(test_mat[INPUT][4].size(),CV_64F,&new_cam[0]);
    Mat _points(test_mat[INPUT][0].size(),CV_64FC2,&points[0]);

    cvtest::convert(test_mat[INPUT][1],_camera,_camera.type());
    cvtest::convert(test_mat[INPUT][2],_distort,_distort.type());
    cvtest::convert(test_mat[INPUT][3],_rot,_rot.type());
    cvtest::convert(test_mat[INPUT][4],_new_cam,_new_cam.type());

    //Applying precalculated undistort rectify map
    if (!useCPlus)
    {
        mapx = cv::Mat(_mapx);
        mapy = cv::Mat(_mapy);
    }
    cv::Mat map1,map2;
    cv::convertMaps(mapx,mapy,map1,map2,CV_32FC1);
    CvMat _map1 = map1;
    CvMat _map2 = map2;
    const Point2d* sptr = (const Point2d*)test_mat[INPUT][0].data;
    for( int i = 0;i < N_POINTS; i++ )
    {
        int u = saturate_cast<int>(sptr[i].x);
        int v = saturate_cast<int>(sptr[i].y);
        points[i].x = _map1.data.fl[v*_map1.cols + u];
        points[i].y = _map2.data.fl[v*_map2.cols + u];
    }

    //---

    cv::undistortPoints(_points, ref_points, _camera,
                        zero_distortion ? Mat() : _distort,
                        zero_R ? Mat::eye(3,3,CV_64F) : _rot,
                        zero_new_cam ? _camera : _new_cam);
    //cvTsDistortPoints(&_points,&ref_points,&_camera,&_distort,&_rot,&_new_cam);
    cvtest::convert(ref_points, test_mat[REF_OUTPUT][0], test_mat[REF_OUTPUT][0].type());
    cvtest::copy(test_mat[INPUT][0],test_mat[OUTPUT][0]);

    cvReleaseMat(&_mapx);
    cvReleaseMat(&_mapy);
#else
    int dist_size = test_mat[INPUT][2].cols > test_mat[INPUT][2].rows ? test_mat[INPUT][2].cols : test_mat[INPUT][2].rows;
    double cam[9] = {0,0,0,0,0,0,0,0,1};
    double rot[9] = {1,0,0,0,1,0,0,0,1};
    double* dist = new double[dist_size ];
    double* new_cam = new double[test_mat[INPUT][4].cols * test_mat[INPUT][4].rows];
    double* points = new double[N_POINTS*2];
    double* r_points = new double[N_POINTS*2];
    //Run reference calculations
    CvMat ref_points= cvMat(test_mat[INPUT][0].rows,test_mat[INPUT][0].cols,CV_64FC2,r_points);
    CvMat _camera = cvMat(3,3,CV_64F,cam);
    CvMat _rot = cvMat(3,3,CV_64F,rot);
    CvMat _distort = cvMat(test_mat[INPUT][2].rows,test_mat[INPUT][2].cols,CV_64F,dist);
    CvMat _new_cam = cvMat(test_mat[INPUT][4].rows,test_mat[INPUT][4].cols,CV_64F,new_cam);
    CvMat _points= cvMat(test_mat[INPUT][0].rows,test_mat[INPUT][0].cols,CV_64FC2,points);

    CvMat _input1 = cvMat(test_mat[INPUT][1]);
    CvMat _input2 = cvMat(test_mat[INPUT][2]);
    CvMat _input3 = cvMat(test_mat[INPUT][3]);
    CvMat _input4 = cvMat(test_mat[INPUT][4]);

    cvtest::convert(cvarrToMat(&_input1), cvarrToMat(&_camera), -1);
    cvtest::convert(cvarrToMat(&_input2), cvarrToMat(&_distort), -1);
    cvtest::convert(cvarrToMat(&_input3), cvarrToMat(&_rot), -1);
    cvtest::convert(cvarrToMat(&_input4), cvarrToMat(&_new_cam), -1);

    //Applying precalculated undistort rectify map
    if (!useCPlus)
    {
        mapx = cv::cvarrToMat(_mapx);
        mapy = cv::cvarrToMat(_mapy);
    }
    cv::Mat map1,map2;
    cv::convertMaps(mapx,mapy,map1,map2,CV_32FC1);
    CvMat _map1 = cvMat(map1);
    CvMat _map2 = cvMat(map2);
    for (int i=0;i<N_POINTS;i++)
    {
        double u = test_mat[INPUT][0].ptr<double>()[2*i];
        double v = test_mat[INPUT][0].ptr<double>()[2*i+1];
        _points.data.db[2*i] = (double)_map1.data.fl[(int)v*_map1.cols+(int)u];
        _points.data.db[2*i+1] = (double)_map2.data.fl[(int)v*_map2.cols+(int)u];
    }

    //---

    cvUndistortPoints(&_points,&ref_points,&_camera,
                      zero_distortion ? 0 : &_distort, zero_R ? 0 : &_rot, zero_new_cam ? &_camera : &_new_cam);
    //cvTsDistortPoints(&_points,&ref_points,&_camera,&_distort,&_rot,&_new_cam);
    CvMat dst = cvMat(test_mat[REF_OUTPUT][0]);
    cvtest::convert(cvarrToMat(&ref_points), cvarrToMat(&dst), -1);

    cvtest::copy(test_mat[INPUT][0],test_mat[OUTPUT][0]);

    delete[] dist;
    delete[] new_cam;
    delete[] points;
    delete[] r_points;
    cvReleaseMat(&_mapx);
    cvReleaseMat(&_mapy);
#endif
}

void CV_InitUndistortRectifyMapTest::run_func()
{
    if (useCPlus)
    {
        cv::Mat input2,input3,input4;
        input2 = zero_distortion ? cv::Mat() : test_mat[INPUT][2];
        input3 = zero_R ? cv::Mat() : test_mat[INPUT][3];
        input4 = zero_new_cam ? cv::Mat() : test_mat[INPUT][4];
        cv::initUndistortRectifyMap(camera_mat,input2,input3,input4,img_size,mat_type,mapx,mapy);
    }
    else
    {
        CvMat input1 = cvMat(test_mat[INPUT][1]), input2, input3, input4;
        if( !zero_distortion )
            input2 = cvMat(test_mat[INPUT][2]);
        if( !zero_R )
            input3 = cvMat(test_mat[INPUT][3]);
        if( !zero_new_cam )
            input4 = cvMat(test_mat[INPUT][4]);
        cvInitUndistortRectifyMap(&input1,
                                  zero_distortion ? 0 : &input2,
                                  zero_R ? 0 : &input3,
                                  zero_new_cam ? 0 : &input4,
                                  _mapx,_mapy);
    }
}

double CV_InitUndistortRectifyMapTest::get_success_error_level( int /*test_case_idx*/, int /*i*/, int /*j*/ )
{
    return 8;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

TEST(Calib3d_DefaultNewCameraMatrix, accuracy) { CV_DefaultNewCameraMatrixTest test; test.safe_run(); }
TEST(Calib3d_GetOptimalNewCameraMatrixNoDistortion, accuracy) { CV_GetOptimalNewCameraMatrixNoDistortionTest test; test.safe_run(); }
TEST(Calib3d_UndistortPoints, accuracy) { CV_UndistortPointsTest test; test.safe_run(); }
TEST(Calib3d_InitUndistortRectifyMap, accuracy) { CV_InitUndistortRectifyMapTest test; test.safe_run(); }

TEST(Calib3d_UndistortPoints, inputShape)
{
    //https://github.com/opencv/opencv/issues/14423
    Matx33d cameraMatrix = Matx33d::eye();
    {
        //2xN 1-channel
        Mat imagePoints(2, 3, CV_32FC1);
        imagePoints.at<float>(0,0) = 320; imagePoints.at<float>(1,0) = 240;
        imagePoints.at<float>(0,1) = 0;   imagePoints.at<float>(1,1) = 240;
        imagePoints.at<float>(0,2) = 320; imagePoints.at<float>(1,2) = 0;

        vector<Point2f> normalized;
        undistortPoints(imagePoints, normalized, cameraMatrix, noArray());
        EXPECT_EQ(static_cast<int>(normalized.size()), imagePoints.cols);
        for (int i = 0; i < static_cast<int>(normalized.size()); i++) {
            EXPECT_NEAR(normalized[i].x, imagePoints.at<float>(0,i), std::numeric_limits<float>::epsilon());
            EXPECT_NEAR(normalized[i].y, imagePoints.at<float>(1,i), std::numeric_limits<float>::epsilon());
        }
    }
    {
        //Nx2 1-channel
        Mat imagePoints(3, 2, CV_32FC1);
        imagePoints.at<float>(0,0) = 320; imagePoints.at<float>(0,1) = 240;
        imagePoints.at<float>(1,0) = 0;   imagePoints.at<float>(1,1) = 240;
        imagePoints.at<float>(2,0) = 320; imagePoints.at<float>(2,1) = 0;

        vector<Point2f> normalized;
        undistortPoints(imagePoints, normalized, cameraMatrix, noArray());
        EXPECT_EQ(static_cast<int>(normalized.size()), imagePoints.rows);
        for (int i = 0; i < static_cast<int>(normalized.size()); i++) {
            EXPECT_NEAR(normalized[i].x, imagePoints.at<float>(i,0), std::numeric_limits<float>::epsilon());
            EXPECT_NEAR(normalized[i].y, imagePoints.at<float>(i,1), std::numeric_limits<float>::epsilon());
        }
    }
    {
        //1xN 2-channel
        Mat imagePoints(1, 3, CV_32FC2);
        imagePoints.at<Vec2f>(0,0) = Vec2f(320, 240);
        imagePoints.at<Vec2f>(0,1) = Vec2f(0, 240);
        imagePoints.at<Vec2f>(0,2) = Vec2f(320, 0);

        vector<Point2f> normalized;
        undistortPoints(imagePoints, normalized, cameraMatrix, noArray());
        EXPECT_EQ(static_cast<int>(normalized.size()), imagePoints.cols);
        for (int i = 0; i < static_cast<int>(normalized.size()); i++) {
            EXPECT_NEAR(normalized[i].x, imagePoints.at<Vec2f>(0,i)(0), std::numeric_limits<float>::epsilon());
            EXPECT_NEAR(normalized[i].y, imagePoints.at<Vec2f>(0,i)(1), std::numeric_limits<float>::epsilon());
        }
    }
    {
        //Nx1 2-channel
        Mat imagePoints(3, 1, CV_32FC2);
        imagePoints.at<Vec2f>(0,0) = Vec2f(320, 240);
        imagePoints.at<Vec2f>(1,0) = Vec2f(0, 240);
        imagePoints.at<Vec2f>(2,0) = Vec2f(320, 0);

        vector<Point2f> normalized;
        undistortPoints(imagePoints, normalized, cameraMatrix, noArray());
        EXPECT_EQ(static_cast<int>(normalized.size()), imagePoints.rows);
        for (int i = 0; i < static_cast<int>(normalized.size()); i++) {
            EXPECT_NEAR(normalized[i].x, imagePoints.at<Vec2f>(i,0)(0), std::numeric_limits<float>::epsilon());
            EXPECT_NEAR(normalized[i].y, imagePoints.at<Vec2f>(i,0)(1), std::numeric_limits<float>::epsilon());
        }
    }
    {
        //vector<Point2f>
        vector<Point2f> imagePoints;
        imagePoints.push_back(Point2f(320, 240));
        imagePoints.push_back(Point2f(0,   240));
        imagePoints.push_back(Point2f(320, 0));

        vector<Point2f> normalized;
        undistortPoints(imagePoints, normalized, cameraMatrix, noArray());
        EXPECT_EQ(normalized.size(), imagePoints.size());
        for (int i = 0; i < static_cast<int>(normalized.size()); i++) {
            EXPECT_NEAR(normalized[i].x, imagePoints[i].x, std::numeric_limits<float>::epsilon());
            EXPECT_NEAR(normalized[i].y, imagePoints[i].y, std::numeric_limits<float>::epsilon());
        }
    }
    {
        //vector<Point2d>
        vector<Point2d> imagePoints;
        imagePoints.push_back(Point2d(320, 240));
        imagePoints.push_back(Point2d(0,   240));
        imagePoints.push_back(Point2d(320, 0));

        vector<Point2d> normalized;
        undistortPoints(imagePoints, normalized, cameraMatrix, noArray());
        EXPECT_EQ(normalized.size(), imagePoints.size());
        for (int i = 0; i < static_cast<int>(normalized.size()); i++) {
            EXPECT_NEAR(normalized[i].x, imagePoints[i].x, std::numeric_limits<double>::epsilon());
            EXPECT_NEAR(normalized[i].y, imagePoints[i].y, std::numeric_limits<double>::epsilon());
        }
    }
}

TEST(Calib3d_UndistortPoints, outputShape)
{
    Matx33d cameraMatrix = Matx33d::eye();
    {
        vector<Point2f> imagePoints;
        imagePoints.push_back(Point2f(320, 240));
        imagePoints.push_back(Point2f(0,   240));
        imagePoints.push_back(Point2f(320, 0));

        //Mat --> will be Nx1 2-channel
        Mat normalized;
        undistortPoints(imagePoints, normalized, cameraMatrix, noArray());
        EXPECT_EQ(static_cast<int>(imagePoints.size()), normalized.rows);
        for (int i = 0; i < normalized.rows; i++) {
            EXPECT_NEAR(normalized.at<Vec2f>(i,0)(0), imagePoints[i].x, std::numeric_limits<float>::epsilon());
            EXPECT_NEAR(normalized.at<Vec2f>(i,0)(1), imagePoints[i].y, std::numeric_limits<float>::epsilon());
        }
    }
    {
        vector<Point2f> imagePoints;
        imagePoints.push_back(Point2f(320, 240));
        imagePoints.push_back(Point2f(0,   240));
        imagePoints.push_back(Point2f(320, 0));

        //Nx1 2-channel
        Mat normalized(static_cast<int>(imagePoints.size()), 1, CV_32FC2);
        undistortPoints(imagePoints, normalized, cameraMatrix, noArray());
        EXPECT_EQ(static_cast<int>(imagePoints.size()), normalized.rows);
        for (int i = 0; i < normalized.rows; i++) {
            EXPECT_NEAR(normalized.at<Vec2f>(i,0)(0), imagePoints[i].x, std::numeric_limits<float>::epsilon());
            EXPECT_NEAR(normalized.at<Vec2f>(i,0)(1), imagePoints[i].y, std::numeric_limits<float>::epsilon());
        }
    }
    {
        vector<Point2f> imagePoints;
        imagePoints.push_back(Point2f(320, 240));
        imagePoints.push_back(Point2f(0,   240));
        imagePoints.push_back(Point2f(320, 0));

        //1xN 2-channel
        Mat normalized(1, static_cast<int>(imagePoints.size()), CV_32FC2);
        undistortPoints(imagePoints, normalized, cameraMatrix, noArray());
        EXPECT_EQ(static_cast<int>(imagePoints.size()), normalized.cols);
        for (int i = 0; i < normalized.rows; i++) {
            EXPECT_NEAR(normalized.at<Vec2f>(0,i)(0), imagePoints[i].x, std::numeric_limits<float>::epsilon());
            EXPECT_NEAR(normalized.at<Vec2f>(0,i)(1), imagePoints[i].y, std::numeric_limits<float>::epsilon());
        }
    }
    {
        vector<Point2f> imagePoints;
        imagePoints.push_back(Point2f(320, 240));
        imagePoints.push_back(Point2f(0,   240));
        imagePoints.push_back(Point2f(320, 0));

        //vector<Point2f>
        vector<Point2f> normalized;
        undistortPoints(imagePoints, normalized, cameraMatrix, noArray());
        EXPECT_EQ(imagePoints.size(), normalized.size());
        for (int i = 0; i < static_cast<int>(normalized.size()); i++) {
            EXPECT_NEAR(normalized[i].x, imagePoints[i].x, std::numeric_limits<float>::epsilon());
            EXPECT_NEAR(normalized[i].y, imagePoints[i].y, std::numeric_limits<float>::epsilon());
        }
    }
    {
        vector<Point2d> imagePoints;
        imagePoints.push_back(Point2d(320, 240));
        imagePoints.push_back(Point2d(0,   240));
        imagePoints.push_back(Point2d(320, 0));

        //vector<Point2d>
        vector<Point2d> normalized;
        undistortPoints(imagePoints, normalized, cameraMatrix, noArray());
        EXPECT_EQ(imagePoints.size(), normalized.size());
        for (int i = 0; i < static_cast<int>(normalized.size()); i++) {
            EXPECT_NEAR(normalized[i].x, imagePoints[i].x, std::numeric_limits<double>::epsilon());
            EXPECT_NEAR(normalized[i].y, imagePoints[i].y, std::numeric_limits<double>::epsilon());
        }
    }
}

TEST(Imgproc_undistort, regression_15286)
{
    double kmat_data[9] = { 3217, 0, 1592, 0, 3217, 1201, 0, 0, 1 };
    Mat kmat(3, 3, CV_64F, kmat_data);
    double dist_coeff_data[5] = { 0.04, -0.4, -0.01, 0.04, 0.7 };
    Mat dist_coeffs(5, 1, CV_64F, dist_coeff_data);

    Mat img = Mat::zeros(512, 512, CV_8UC1);
    img.at<uchar>(128, 128) = 255;
    img.at<uchar>(128, 384) = 255;
    img.at<uchar>(384, 384) = 255;
    img.at<uchar>(384, 128) = 255;

    Mat ref = Mat::zeros(512, 512, CV_8UC1);
    ref.at<uchar>(Point(24, 98)) = 78;
    ref.at<uchar>(Point(24, 99)) = 114;
    ref.at<uchar>(Point(25, 98)) = 36;
    ref.at<uchar>(Point(25, 99)) = 60;
    ref.at<uchar>(Point(27, 361)) = 6;
    ref.at<uchar>(Point(28, 361)) = 188;
    ref.at<uchar>(Point(28, 362)) = 49;
    ref.at<uchar>(Point(29, 361)) = 44;
    ref.at<uchar>(Point(29, 362)) = 16;
    ref.at<uchar>(Point(317, 366)) = 134;
    ref.at<uchar>(Point(317, 367)) = 78;
    ref.at<uchar>(Point(318, 366)) = 40;
    ref.at<uchar>(Point(318, 367)) = 29;
    ref.at<uchar>(Point(310, 104)) = 106;
    ref.at<uchar>(Point(310, 105)) = 30;
    ref.at<uchar>(Point(311, 104)) = 112;
    ref.at<uchar>(Point(311, 105)) = 38;

    Mat img_undist;
    undistort(img, img_undist, kmat, dist_coeffs);

    ASSERT_EQ(0.0, cvtest::norm(img_undist, ref, cv::NORM_INF));
}

TEST(Calib3d_initUndistortRectifyMap, regression_14467)
{
    Size size_w_h(512 + 3, 512);
    Matx33f k(
        6200, 0, size_w_h.width / 2.0f,
        0, 6200, size_w_h.height / 2.0f,
        0, 0, 1
    );

    Mat mesh_uv(size_w_h, CV_32FC2);
    for (int i = 0; i < size_w_h.height; i++)
    {
        for (int j = 0; j < size_w_h.width; j++)
        {
            mesh_uv.at<Vec2f>(i, j) = Vec2f((float)j, (float)i);
        }
    }

    Matx<double, 1, 14> d(
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0,
        0.09, 0.0
    );
    Mat mapxy, dst;
    initUndistortRectifyMap(k, d, noArray(), k, size_w_h, CV_32FC2, mapxy, noArray());
    undistortPoints(mapxy.reshape(2, (int)mapxy.total()), dst, k, d, noArray(), k);
    dst = dst.reshape(2, mapxy.rows);
    EXPECT_LE(cvtest::norm(dst, mesh_uv, NORM_INF), 1e-3);
}

}} // namespace
