 
#include <iostream>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "functions.hpp"


using namespace cv;
using namespace std;


int main(int argc, char* argv[])
{

    if(argc != 3)
    {
//      Check for correct input
        cout <<"\n   Enter empty image and video PATH as command-line arguments \n\n   Example: ./a.out img_empty4.jpg vid4.mp4" << endl << endl;
        return -1;
    }


//  Necessary declarations

    Mat img_empty, temp;

    Mat dilate_element = getStructuringElement(MORPH_RECT, Size(4, 4));
    Mat erode_element  = getStructuringElement(MORPH_RECT, Size(4, 4));

    Scalar colors = Scalar(128, 128, 128);
    Runtime_Variables runtime;


//  Read the file
    img_empty = imread(argv[1], IMREAD_COLOR);
    runtime.img_perm = img_empty.clone();

 
    if(img_empty.empty())
    {
//      Check for invalid input
        cout << "Could not open or find the image" << endl;
        return -1;
    }


//  Converts image to gray
    cvtColor(img_empty, img_empty, COLOR_BGR2GRAY);          // Intput and output image names
    blur(img_empty, img_empty, Size(5,5));                  // Gaussian blur 5,5


//  Apply Sobel filter
    Mat img_horizontal, img_vertical;

    Sobel(img_empty, img_horizontal, CV_8U, 0, 1, 3, 1, 0);   // Vertical Edges
    Sobel(img_empty, img_vertical, CV_8U, 1, 0, 3, 1, 0);    // Horizontal Edges


//  Add Horizontal + Vertical edges
    img_empty = img_horizontal + img_vertical;


//  Copy of Detected Edges
    temp = img_empty.clone();


//  Threshold
    threshold(img_empty, img_empty, 0, 255, THRESH_OTSU+THRESH_BINARY);


//  Dilate before substraction
    dilate(temp, temp, dilate_element);


//  Substract original edges from later ones again
    temp -= img_empty;


//  Morphology (Perform after substraction)
    Mat kernel = getStructuringElement(MORPH_RECT, Size(45, 45));    // Create kernel           ***
    morphologyEx(temp, temp, MORPH_CLOSE, kernel);                 // Remove internal noise


//  Substract original edges from later ones again
    temp -= img_empty;


//  Erode and dilation to separate detected sections clearly                                    ***

    erode_element  = getStructuringElement(MORPH_RECT, Size(15, 15));
    dilate_element = getStructuringElement(MORPH_RECT, Size(13, 13));

    erode(temp, img_empty, erode_element);
    dilate(img_empty, img_empty, dilate_element);


//  Retrieve all external contours, all pixels of it
    cv::findContours(img_empty, runtime.contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);


//  Iterator counter for contours
    vector<vector<Point>>::iterator itc_contour = runtime.contours.begin();
    

    while(itc_contour!=runtime.contours.end())
    {

//      Remove patch that has no inside limits of aspect ratio and area.
        RotatedRect mr = minAreaRect(Mat(*itc_contour));    //  Create bounding rect of object

        if(!Verify_Aspect_Ratio(mr, 0.64, 3.7))             //  Error of 64%    Aspect ratio: 3.7 : 1
        {
            itc_contour = runtime.contours.erase(itc_contour);
        }

        else
        {
            ++itc_contour;
            runtime.rects_green.push_back(mr);
        }

    }


//  Draw detected parkable regions
    Draw_Rotated_Rects(runtime.img_perm, runtime.rects_green);



//  I/O operations

    cout << "Usage:\n\t1.) Register 4 Left-clicks (Rectangle co-ordinates) to add parking regions \n\t2.) Right-click to delete a region  \n\t3.) Press 'Enter' to continue" << endl << endl;

    namedWindow("Ocular Parking System", WINDOW_AUTOSIZE);
    setMouseCallback("Ocular Parking System", Mouse_Event, &runtime);

    while(1)
    {
//      Hold the screen (60 secs for "Enter" key)
        imshow("Ocular Parking System", runtime.img_perm);
        if(waitKey(60) == 10)
            break;
    }



        ////////////////////////////////////////////////////////////////////////////
        //////////////////////// Video Starts Here /////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////


//  Necessary re-difinitions and other declarations

    Mat frame, frame_perm;
    int sensitivity = 6, ticket_mode = 0, fps = 15;

    dilate_element = getStructuringElement(MORPH_RECT, Size(8,8));   //                ***
    erode_element = getStructuringElement(MORPH_RECT, Size(2, 2));

    Mat erode_element2 = getStructuringElement(MORPH_RECT, Size(15, 15));
    Mat dilate_element2 = getStructuringElement(MORPH_RECT, Size(7, 7));

    kernel = getStructuringElement(MORPH_RECT, Size(20, 20));        // Create kernel   ***

    vector<vector<Point>>::iterator itc_vehicles_contour;
    vector<RotatedRect>::iterator itc_vehicles;

    vector<RotatedRect> rects_vehicles;
    vector<vector<Point>> contours_vehicles;

    auto time_start = std::chrono::high_resolution_clock::now();



//  Read the files
    img_empty = imread(argv[1], IMREAD_COLOR);
    VideoCapture frames(argv[2]);


    if(!frames.isOpened() || img_empty.empty())
    {
//      Check for invalid input
        cout << "\n   Invalid Path for video file. \n   Example: ./a.out img_empty4.jpg vid4.mp4" << endl;
        return -1;
    }



//  Converts image to gray
    cvtColor(img_empty, img_empty, COLOR_BGR2GRAY);        // Intput and output image names
    blur(img_empty, img_empty, Size(5,5));                // Gaussian blur 5,5


//  Edge Detection operations
    Sobel(img_empty, img_horizontal, CV_8U, 0, 1, 3, sensitivity, 0);   // Vertical Edges             ****
    Sobel(img_empty, img_vertical, CV_8U, 1, 0, 3, sensitivity, 0);    // Horizontal Edges            ****


//  Add Horizontal + Vertical edges
    img_empty = img_horizontal + img_vertical;


//  Dilation
    dilate(img_empty, img_empty, dilate_element);
 


    while(true)
    {
//      Loop for all the frames of the video

//      Get each frame
        frames >> frame;
        frame_perm = frame.clone();


//      Time commands for FPS        
        auto time_now = std::chrono::high_resolution_clock::now();

        if(std::chrono::duration<float>(time_now - time_start).count() > (1/float(fps)))
        {
//          If time difference is in accordance with FPS
            time_start = std::chrono::high_resolution_clock::now();


//          Converts frame to gray
            cvtColor(frame, frame, COLOR_BGR2GRAY);
            blur(frame, frame, Size(5,5));                        // Gaussian blur 5x5


//          Apply Sobel filter
            Mat img_horizontal2, img_vertical2;
            Sobel(frame, img_horizontal2, CV_8U, 0, 1, 3, sensitivity * 0.8, 0);   // Vertical Edges          ****
            Sobel(frame, img_vertical2, CV_8U, 1, 0, 3, sensitivity * 0.8, 0);    // Horizontal Edges         ****


//          Add Horizontal + Vertical edges
            frame = img_horizontal2 + img_vertical2;


//          Create output matrice(s)
            frame -= img_empty;


//          Threshold
            threshold(frame, frame, 0, 255, THRESH_OTSU+THRESH_BINARY);


//          Dilate to remove small marks
            erode(frame, frame, erode_element);


//          Morphology (Perform after substraction)
            morphologyEx(frame, frame, MORPH_CLOSE, kernel);       // Remove internal noise


//          Substract original edges from later ones
            frame -= img_empty;
            dilate(frame, frame, dilate_element2);


//          Instantiate contours
            contours_vehicles.clear();
            cv::findContours(frame, contours_vehicles, RETR_EXTERNAL, CHAIN_APPROX_NONE); // retrieve all external contours, all pixels of it


//          Iterator for contours

            rects_vehicles.clear();
            itc_vehicles_contour = contours_vehicles.begin();


        
            while(itc_vehicles_contour!=contours_vehicles.end())
            {

//              Remove patch that has no inside limits of aspect ratio and area.

//              Create bounding rect of object
                RotatedRect mr_vehicles = minAreaRect(Mat(*itc_vehicles_contour));

                if(!Verify_Aspect_Ratio(mr_vehicles))
                    itc_vehicles_contour = contours_vehicles.erase(itc_vehicles_contour);
                else
                {
                    ++itc_vehicles_contour;
                    rects_vehicles.push_back(mr_vehicles);
                }
            }



        ////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////// Ticketing Region (See Bottom) /////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////
        
            if(!ticket_mode)
            {
                runtime.itc_red = runtime.rects_red.begin();
                while(runtime.itc_red != runtime.rects_red.end())
                {
//                  Delete red regions "rects_red" on removal of obstruction (center of vehicles doesn't lie on the red region)
                    bool obstruction_remains = false;

                    for(itc_vehicles = rects_vehicles.begin(); itc_vehicles <= rects_vehicles.end(); itc_vehicles++)
                    {
                        if(rects_vehicles.empty() || runtime.rects_red.empty())
//                      Prevent SegFaults
                            break;
                        else if ( Does_Rectangle_Contain_Point( RotatedRect(*runtime.itc_red), RotatedRect(*itc_vehicles).center ) )
                            obstruction_remains = true;
                    }

                    if(obstruction_remains)
                        ++runtime.itc_red;

                    else
                        runtime.itc_red = runtime.rects_red.erase(runtime.itc_red);
                }
            }

        //////////////////////////////////////////////////////////////////////////////////
        //////////////////////// End of Ticketing Region /////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////


            for(runtime.itc_green = runtime.rects_green.begin(); runtime.itc_green <= runtime.rects_green.end(); runtime.itc_green++)
            {
//              Convert green regions "rects_green" to red regions "rects_red" on detection of obstruction (center of vehicles lies on the green region)
                bool red_already_contains_this_obstruction = false;

                for(itc_vehicles = rects_vehicles.begin(); itc_vehicles <= rects_vehicles.end(); itc_vehicles++)
                {
//                  Prevent duplication of same regions
                    red_already_contains_this_obstruction = Detect_Rect_In_Rects( RotatedRect(*runtime.itc_green), runtime.rects_red );

                    if(rects_vehicles.empty() || runtime.rects_green.empty())
//                      Prevent SegFaults
                        break;
                    else if ( Does_Rectangle_Contain_Point( RotatedRect(*runtime.itc_green), RotatedRect(*itc_vehicles).center ) &&  !red_already_contains_this_obstruction)
                        runtime.rects_red.push_back(RotatedRect(*runtime.itc_green));
                }
            }

        }



//      Draw the vehicle regions
        Draw_Rotated_Rects(temp, rects_vehicles);
        Draw_Rotated_Rects(frame_perm, rects_vehicles, Scalar(0,215,255));


//      Draw the Parking regions (Green)
        Draw_Rotated_Rects(frame_perm, runtime.rects_green);


//      Draw obstruction regions (Red)
        Draw_Rotated_Rects(frame_perm, runtime.rects_red, Scalar(0,0,255));




//      Output Operations

        namedWindow("Ocular Parking System", WINDOW_AUTOSIZE);
        createTrackbar("Ticket Mode", "Ocular Parking System", &ticket_mode, 1);
        createTrackbar("FPS", "Ocular Parking System", &fps, 40);
        createTrackbar("Sensitivity", "Ocular Parking System", &sensitivity, 15);        
        imshow("Ocular Parking System", frame_perm);

        Clear_Screen();
        cout << "Total parking Spots   = " << runtime.rects_green.size() << endl;
        cout << "No. of Spots Occupied = " << runtime.rects_red.size() << endl;
        cout << "No. of Empty Spaces   = " << (runtime.rects_green.size()- runtime.rects_red.size()) << endl;


        if (waitKey(20) == 10)
//          Wait 'Enter' key input for 20 secs
            break;

    }


//  Wait for a keystroke in the window
    waitKey(0);
    return 0;
}


//  Dependencies:           sudo apt-get install gcc   
//                          sudo apt-get install libopencv-dev
//  Compilation :           g++ init.cpp `pkg-config --cflags --libs opencv`
//  Execution:              ./a.out "empty_images/img_empty4.jpg" "video/vid4.mp4"