#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;



bool verifySizes(RotatedRect candidate)
{

    // Check for aspect ratio with error margin of 35%
    float error = 0.35;


    // Average car dimension: 1845x570  i.e. aspect ratio: ~2.8
    const float aspect = 2.8;

    
    // Set a min and max area; all other patchs are discarded
    int min = 20 * aspect * 20;     // min area
    int max = 224 * aspect * 224;   // max area

    
    // Get only patches that match to a aspect ratio.
    float rmin = aspect - aspect * error;
    float rmax = aspect + aspect * error;


    int area = candidate.size.height * candidate.size.width;
    float r = (float)candidate.size.width / (float)candidate.size.height;


    if(r<1)
    {
        // if height greater than width (check for rotation)
        r = 1/r;
    }


    if((area < min || area > max) || (r < rmin || r > rmax))
    {
        return false;
    }
    else
    {
        return true;
    }
}



int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        cout <<" Enter two images as command-line arguments" << endl;
        return -1;
    }


    // Read the file
    Mat img, img_perm, img2;
    img = imread(argv[1], IMREAD_COLOR); 
    img2 = imread(argv[2], IMREAD_COLOR);  
    img_perm = img2;  

    
    if(!img.data)                              
    {
        // Check for invalid input
        cout << "Could not open or find the image" << endl;
        return -1;
    }


    // Converts image to gray
    cvtColor(img, img, COLOR_BGR2GRAY);        // Intput and output image names
    blur(img, img, Size(5,5));              // Gaussian blur 5,5


    // Apply Sobel filter
    Mat img_horizontal, img_vertical;
    Sobel(img, img_horizontal, CV_8U, 0, 1, 3, 1, 0);  // Vertical Edges
    Sobel(img, img_vertical, CV_8U, 1, 0, 3, 1, 0);    // Horizontal Edges


    // Add Horizontal + Vertical edges
    img = img_horizontal + img_vertical;


    // Threshold
    threshold(img, img, 0, 255, THRESH_OTSU+THRESH_BINARY);


    if(!img2.data)                              
    {
        // Check for invalid input
        cout << "Could not open or find the image" << endl ;
        return -1;
    }


    // Converts image to gray
    cvtColor(img2, img2, COLOR_BGR2GRAY);
    blur(img2, img2, Size(5,5));              // Gaussian blur 5x5


    // Aapply Sobel filter
    Mat img_horizontal2, img_vertical2;
    Sobel(img2, img_horizontal2, CV_8U, 0, 1, 3, 1, 0);  // Vertical Edges
    Sobel(img2, img_vertical2, CV_8U, 1, 0, 3, 1, 0);    // Horizontal Edges


   // Add Horizontal + Vertical edges
    img2 = img_horizontal2 + img_vertical2;


    // Threshold
    threshold(img2, img2, 0, 255, THRESH_OTSU+THRESH_BINARY);



    // Rect r = Rect(115,215,95,95);    // Create a Rect with top-left vertex at (115,215), of width 95 and height 95 pixels.
    // rectangle(img,r,Scalar(27,127,0),4);    // Draw the rect defined by r with line thickness 4 and Green color


    // Create output matrice(s)
    Mat img3;
    img3 = img2 - img;


    // Morphology (Perform after substraction)
    Mat kernel = getStructuringElement(MORPH_RECT, Size(40, 40));  // Create kernel
    morphologyEx(img3, img3, CV_MOP_CLOSE, kernel);    // Remove internal noise


    // Substract original edges from later ones
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));    
    img3 -= img;
    // erode(img3, img3, kernel);


    // instantiate contours
    vector<vector<Point>> contours;     // vector of vectors (contours)
    cv::findContours(img3, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE); // retrieve all external contours, all pixels of it


    // Start to iterate to each contour found
    vector<vector<Point>>::iterator itc = contours.begin();
    vector<RotatedRect> rects;



    while(itc!=contours.end())
    {
        
        // Remove patch that has no inside limits of aspect ratio and area.


        // Create bounding rect of object
        RotatedRect mr = minAreaRect(Mat(*itc));


        if(!verifySizes(mr))
        {
            itc = contours.erase(itc);
        }
        else
        {
            ++itc;
            rects.push_back(mr);
        }      
    }


    Scalar color = Scalar(75, 150, 0);

    
    for(int i = 0; i < contours.size(); i++)
    {      
       
        // rotated rectangle
        Point2f rect_points[4]; rects[i].points(rect_points);
        for(int j = 0; j < 4; j++)
            line(img_perm, rect_points[j], rect_points[(j+1)%4], color, 3, 8);
    }    
    



    // Output Operations
    
    namedWindow("img", WINDOW_AUTOSIZE);    // Create a window for display.
    imshow("img", img);                     // Show our image inside it.
    
    namedWindow("img2", WINDOW_AUTOSIZE);
    imshow("img2", img2);                

    namedWindow("img3", WINDOW_AUTOSIZE);
    imshow("img3", img3);

    namedWindow("img_perm", WINDOW_AUTOSIZE);
    imshow("img_perm", img_perm);


    waitKey(0);             // Wait for a keystroke in the window
    return 0;
}



// Compiled with:      g++ init.cpp `pkg-config --cflags --libs opencv`
