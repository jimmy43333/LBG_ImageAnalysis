//
//  main.cpp
//  LBG_ImageAnalysis
//
//  Created by 宋題均 on 2017/11/26.
//  Copyright © 2017年 宋題均. All rights reserved.
//

#include <iostream>
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "CodeBookTrain.h"
using namespace std;
using namespace cv;

Mat readRawfile(const char* filename,int width,int height){
    Mat outputimage;
    //read the raw file
    FILE *fp = NULL;
    char *imagedata = NULL;
    int IMAGE_WIDTH = width;
    int IMAGE_HEIGHT = height;
    int framesize = IMAGE_WIDTH * IMAGE_HEIGHT;
    //Open raw Bayer image.
    fp = fopen(filename, "rb");
    if(!fp){
        cout << "read file failure";
        return outputimage;
    }
    //Memory allocation for bayer image data buffer.
    imagedata = (char*) malloc (sizeof(char) * framesize);
    //Read image data and store in buffer.
    fread(imagedata, sizeof(char), framesize, fp);
    //Create Opencv mat structure for image dimension. For 8 bit bayer, type should be CV_8UC1.
    outputimage.create(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);
    memcpy(outputimage.data, imagedata, framesize);
    free(imagedata);
    fclose(fp);
    return outputimage;
}

int main(int argc, const char * argv[]) {
    vector< vector<vtype> > codebook;
    Mat trainImg,trainImg2,trainImg3;
    trainImg = readRawfile("/Users/TGsung/Desktop/lenna.raw",512,512);
    trainImg2 = readRawfile("/Users/TGsung/Desktop/D23.raw",512,512);
    trainImg3 = readRawfile("/Users/TGsung/Desktop/mandrill.raw",512,512);
    //Train with codebook
    codebook = CodeBookTrain(codebook,trainImg);
    codebook = CodeBookTrain(codebook,trainImg2);
    codebook = CodeBookTrain(codebook,trainImg3);
    //Output images with codebook
    Mat img1;
    vector<int> compress;
    compress = Encode(codebook,trainImg);
    img1 = Decode(codebook,compress);
    imwrite("/Users/TGsung/Desktop/01.jpg",img1);
    imshow("window1",img1);
    waitKey(0);
    return 0;
}
