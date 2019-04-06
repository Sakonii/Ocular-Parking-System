#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;



int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        cout <<" Enter two images as command-line arguments" << endl;
        return -1;
    }

    Mat img;
    img = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

    if(!img.data)                              // Check for invalid input
    {
        cout << "Could not open or find the image" << endl ;
        return -1;
    }


    //converts image to gray
    cvtColor(img, img, CV_BGR2GRAY);        //intput and output image names
    blur(img, img, Size(5,5));              //Gaussian blur 5x5

    //apply Sobel filter
    Mat img_horizontal, img_vertical;
    Sobel(img, img_horizontal, CV_8U, 0, 1, 3, 1, 0);  //1st: vertical, second: horizontal
    Sobel(img, img_vertical, CV_8U, 1, 0, 3, 1, 0);

    img = img_horizontal + img_vertical;


    //Threshold
    threshold(img, img, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);

    //Morphology (Perform after substraction maybe)
    //Mat kernel = getStructuringElement(MORPH_RECT, Size(20, 20));  //Create kernel
    //morphologyEx(img, img, CV_MOP_CLOSE, kernel);    //remove internal noise

    //Canny(img, img, 100, 100, 3, false);      //Direct edge detection





    Mat img2;
    img2 = imread(argv[2], CV_LOAD_IMAGE_COLOR);   // Read the file

    if(!img2.data)                              // Check for invalid input
    {
        cout << "Could not open or find the image" << endl ;
        return -1;
    }


    //converts image to gray
    cvtColor(img2, img2, CV_BGR2GRAY);
    blur(img2, img2, Size(5,5));              //Gaussian blur 5x5

    //apply Sobel filter
    Mat img_horizontal2, img_vertical2;
    Sobel(img2, img_horizontal2, CV_8U, 0, 1, 3, 1, 0);  //1st: vertical, second: horizontal
    Sobel(img2, img_vertical2, CV_8U, 1, 0, 3, 1, 0);

    img2 = img_horizontal2 + img_vertical2;


    //Threshold
    threshold(img2, img2, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);






    //Rect r = Rect(115,215,95,95);    //create a Rect with top-left vertex at (115,215), of width 95 and height 95 pixels.
    //rectangle(img,r,Scalar(27,127,0),4);    //draw the rect defined by r with line thickness 4 and Green color


    namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.
    imshow("Display window", img);                // Show our image inside it.
    

    Mat img3;
    img3 = img2 - img;
    //imshow("Display window", img3);


    waitKey(0);             // Wait for a keystroke in the window
    return 0;
}



// g++ init.cpp `pkg-config --cflags --libs opencv`
