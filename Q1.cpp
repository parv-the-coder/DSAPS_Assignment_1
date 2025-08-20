#include<iostream>
#include<cmath>
#include<opencv2/opencv.hpp>
using namespace std;


int *** convimg(cv::Mat &img,int &height,int &width)
{
    height = img.rows;
    width = img.cols;


    int ***image = new int**[height];
    for (int i = 0; i < height; i++) 
    {
        image[i] = new int*[width];
        
        for (int j = 0; j < width; j++) 
        {
            image[i][j] = new int[3];   //RGB
        }
    }

    //filling the pixel value
   for (int i = 0; i < height; i++) 
   {
        for (int j = 0; j < width; j++) 
        {
            cv::Vec3b pixel = img.at<cv::Vec3b>(i, j);
            image[i][j][0] = pixel[0];  // B
            image[i][j][1] = pixel[1];  // G
            image[i][j][2] = pixel[2];  // R
        }
    }

    return image;
}

int main()
{
    string inppath;
    cout<<"Enter Path of image: ";
    cin>>inppath;               // taking path from user

    cv::Mat img = cv::imread(inppath);      // reading image

    //checking imaged loaded properly
    if(img.empty())
    {
        cout<<"Error in loading image"<<endl;
        return -1;
    }

    int nheight,nwidth;
    cout<<"Enter Height: ";
    cin>>nheight;
    cout<<"Enter Width: ";
    cin>>nwidth;

    cout<<"Original: "<<img.rows <<" x "<< img.cols<<endl;
    cout<<"Target: "<<nheight<<" x "<< nwidth<<endl;


    int height, width;
    int*** image = convimg(img, height, width);


    return 0;
}