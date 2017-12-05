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
#include "readRawfile.h"
using namespace std;
using namespace cv;

string int2str(int& i);

int main(int argc, const char * argv[]) {
    vector< vector<vtype> > codebook;
    vector<Mat> Img(6);
    Img[0] = readRawfile("/Users/TGsung/Desktop/Dataset/880372.raw",128,128);
    Img[1] = readRawfile("/Users/TGsung/Desktop/Dataset/881530.raw",128,128);
    Img[2] = readRawfile("/Users/TGsung/Desktop/Dataset/882515.raw",128,128);
    Img[3] = readRawfile("/Users/TGsung/Desktop/Dataset/891538.raw",128,128);
    Img[4] = readRawfile("/Users/TGsung/Desktop/Dataset/891539.raw",128,128);
    Img[5] = readRawfile("/Users/TGsung/Desktop/Dataset/892539.raw",128,128);
    
    //Train with codebook
    codebook = CodeBookTrain(codebook,Img[0]);
    codebook = CodeBookTrain(codebook,Img[1]);
    codebook = CodeBookTrain(codebook,Img[2]);
    codebook = CodeBookTrain(codebook,Img[3]);
    
    //Output images with codebook
    Mat out;
    vector<int> compress;
    //print out the codebook
    compress.resize(256);
    for(int i=0;i<compress.size();i++){
        compress[i] = i;
    }
    out = Decode(codebook,compress);
    imwrite("/Users/TGsung/Desktop/codebook.jpg",out);
    //print images
    string name;
    double psnr =0;
    for(int i=0;i<Img.size();i++){
        compress = Encode(codebook,Img[i]);
        out = Decode(codebook,compress);
        psnr = calpsnr(Img[i],out);
        name = "/Users/TGsung/Desktop/0" + int2str(i) + ".jpg";
        cout << psnr << endl;
        imwrite(name,out);
    }
    
    cout << "Down!" << endl;
    return 0;
}

string int2str(int &i) {
    string s;
    stringstream ss(s);
    ss << i;
    return ss.str();
}
