//
//  CodeBookTrain.h
//  LBG_ImageAnalysis
//
//  Created by 宋題均 on 2017/11/26.
//  Copyright © 2017年 宋題均. All rights reserved.
//
#include <vector>
#include <math.h>
using namespace std;
using namespace cv;

#ifndef CodeBookTrain_h
#define CodeBookTrain_h
#define vtype unsigned int
//Add two vectors
vector<vtype> vectorAdd(vector<vtype> &V1, vector<vtype> &V2){
    vector<vtype> result;
    if(V1.size()!=V2.size()){
        cout<< "add error";
        return V1;
    }
    for(int i =0;i<V1.size();i++){
        result.push_back(V1.at(i)+V2.at(i));
    }
    return result;
}
//Sub two vectors
vector<vtype> vectorSub(vector<vtype> &V1, vector<vtype> &V2){
    vector<vtype> result;
    if(V1.size()!=V2.size()){
        cout<< "sub error";
        return V1;
    }
    for(int i =0;i<V1.size();i++){
        result.push_back(V1.at(i)-V2.at(i));
    }
    return result;
}
//MeanValue of the vector set
vector<vtype> vectorMean(vector<vector<vtype>> &input){
    vector<vtype> result;
    result.resize(input[0].size());
    for(int i = 0;i<input.size();i++){
        result = vectorAdd(result,input[i]);
    }
    for(int j = 0;j<result.size();j++){
        result[j] = result[j]/input.size();
    }
    return result;
}
//Distance of two vector
float vectorDistance(vector<vtype> V1, vector<vtype> V2){
    float result=0;
    vector<vtype> tmp = vectorSub(V1,V2);
    for(int i = 0 ;i< tmp.size();i++){
        result = result + tmp[i]*tmp[i];
    }
    return sqrt(result);
}
//Training the Codebook
vector<vector<vtype>> CodeBookTrain(vector<vector<vtype>> &incodebook , Mat &input){
    int d = 16;
    int k = 256;
    vector<vector<vtype>> codebook;
    vector<vector<vtype>> InputDataSet;             //Input data of size 16384 * 16
    vector<vtype> tmp;
    vector<vector<vtype>> cluster;
    float D=0;                                 //the distortion to decided training is over or not
    float distortion = 1000;                   //the distortion of previous round
    float threshold = 0.05;
    int iteration = 5;                         //the max iteration of training
    
    //Create the training dataset of input image
    for(int i =0;i <= 512-4;i=i+4){
        for(int j=0;j <= 512-4;j=j+4){
            for(int k = 0; k < 4 ;k++){
                for(int l=0;l< 4 ;l++){
                    tmp.push_back(input.at<uchar>(i+k,j+l));
                }
            }
            InputDataSet.push_back(tmp);
            tmp.clear();
        }
    }
    //Initial the codebook
    if(incodebook.empty()){
        //allocate the size of codebook
        codebook.resize(k);
        for(int i = 0; i != k;i++){
            codebook[i].resize(d);
        }
        //choose initial data
        unsigned long jump = InputDataSet.size()/codebook.size();
        unsigned long iter =0;
        for(int i=0; i < codebook.capacity();i++){
            cluster.assign(InputDataSet.begin()+iter,InputDataSet.begin()+iter+jump);
            if(!cluster.empty()){
                vector<vtype> v = vectorMean(cluster);
                codebook[i].assign(v.begin(),v.end());
                iter = iter + jump;
                cluster.clear();
            }else{
                cout << "Cluster empty";
            }
        }
    }else{
        codebook = incodebook;
    }
    //LBG Algorithm
    vector<int> index(InputDataSet.size(),0);  //the index of which cluster the data distribute to
    do{
        if(D != 0){
            distortion = D;
            cout << "Training remain : " << iteration << endl;
        }
        //Classify dataset into K cluster (here k = 256)
        for(int i =0;i<InputDataSet.size();i++){
            int tmpdis = vectorDistance(InputDataSet[i],codebook[0]);
            index[i] = 0;
            for(int j=1;j<codebook.size();j++){
                if(vectorDistance(InputDataSet[i],codebook[j]) <= tmpdis){
                    index[i] = j;
                    tmpdis = vectorDistance(InputDataSet[i],codebook[j]);
                }
            }
        }
        //Update codebook
        for(int i =0;i<codebook.size();i++){
            for(int j=0;j<index.size();j++){
                if(index[j] == i){
                    cluster.push_back(InputDataSet[j]);
                }
            }
            if(cluster.empty()){
                continue;
            }
            codebook[i] = vectorMean(cluster);
            cluster.clear();
        }
        //Calculate Distortion D
        for(int i=0;i<index.size();i++){
            D = D + vectorDistance(InputDataSet[i],codebook[index[i]]);
        }
        D = D/index.size();
        iteration--;
    }while(((distortion-D)/D) > threshold && 0 < iteration);
    cout << "Training Down!" << endl;
    return codebook;
}
//Encode the Image
vector<int> Encode(vector<vector<vtype>> &codebook , Mat &input){
    vector<vtype> partofimg;
    vector<int> outputindex(input.rows * input.cols /16,0);
    int index = 0;
    int tmpdis;
    for(int i =0;i <= input.rows-4;i=i+4){
        for(int j=0;j <= input.cols-4;j=j+4){
            for(int k = 0; k < 4 ;k++){
                for(int l=0;l< 4 ;l++){
                    partofimg.push_back(input.at<uchar>(i+k,j+l));
                }
            }
            tmpdis = vectorDistance(partofimg,codebook[0]);
            outputindex[index]=0;
            for(int j=1;j<codebook.size();j++){
                if(vectorDistance(partofimg,codebook[j]) <= tmpdis){
                    outputindex[index] = j;
                    tmpdis = vectorDistance(partofimg,codebook[j]);
                }
            }
            index++;
            partofimg.clear();
        }
    }
    return outputindex;
}
//Decode the Image
Mat Decode(vector<vector<vtype>> &codebook , vector<int> &input){
    int r = sqrt(input.size())*4;
    Mat output(r,r,CV_8U);
    vector<vtype> tmp;
    int index = 0;
    for(int i =0;i <= r-4;i=i+4){
        for(int j=0;j <= r-4;j=j+4){
            tmp.clear();
            tmp = codebook[input[index]];
            for(int k = 3; 0 <= k ;k--){
                for(int l=3; 0 <= l  ;l--){
                    output.at<uchar>(i+k,j+l) = tmp.back();
                    tmp.pop_back();
                }
            }
            index++;
        }
    }
    return output;
}
#endif /* CodeBookTrain_h */
