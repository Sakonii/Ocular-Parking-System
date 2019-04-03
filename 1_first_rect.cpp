#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;



int main(int argc, char** argv)
{
    if(argc != 2)
    {
        cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
        return -1;
    }

    Mat img;
    img = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

    if(!img.data)                              // Check for invalid input
    {
        cout << "Could not open or find the image" << endl ;
        return -1;
    }



    Rect r = Rect(115,215,95,95);    //create a Rect with top-left vertex at (115,215), of width 95 and height 95 pixels.
    rectangle(img,r,Scalar(127,127,0),4);    //draw the rect defined by r with line thickness 4 and Blue color


    namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.
    imshow("Display window", img);                // Show our image inside it.

    waitKey(0);             // Wait for a keystroke in the window
    return 0;
}

