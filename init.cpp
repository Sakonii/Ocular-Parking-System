#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;



//  Global Variables required for Callback functions (Run-time Variables)

    int N = 0;                          //  Count for number of points of a rectangle (for mouse input)
    Mat img_perm;

//  Instantiate contours and its iterators
    vector<vector<Point>> contours;     //  Instantiate vector of vectors of points (contours)

    vector<RotatedRect> rects_green;
    vector<RotatedRect>::iterator itc_green;
  
    vector<RotatedRect> rects_red;
    vector<RotatedRect>::iterator itc_red;

//  Instantiate co-ordinates (coords)
    vector<Point> coords;



//  User-defined functions:


bool Detect_Rect_In_Rects(const cv::RotatedRect &_rect, const vector<cv::RotatedRect> &_rects)
{
//  Searches for duplication of 'rect' regions in vector 'rects' (only checks for centers and angle)
    vector<RotatedRect>::const_iterator _itc;

    for(_itc  = _rects.begin(); _itc <= _rects.end(); _itc++)
    {
        if(_rects.empty())
//          Prevent Segfaults
            break;
        else if ( (_rect.center.x == RotatedRect(*_itc).center.x) && (_rect.center.y == RotatedRect(*_itc).center.y) && (_rect.angle == RotatedRect(*_itc).angle) )
            return true;
    }

    return false;
}


void Draw_Rotated_Rect(Mat &img, const RotatedRect &rect, Scalar color =  Scalar(255, 255, 255))
{
//  Draws a single "RotatedRect" region named 'rect'
    Point2f rect_points[4]; rect.points(rect_points);

    for(int j = 0; j < 4; j++)
        line(img, rect_points[j], rect_points[(j+1)%4], color, 2, 8);
}


void Draw_Rotated_Rects(Mat &img, vector<RotatedRect> &rects, Scalar color =  Scalar(75, 150, 0))
{
//  Draws every "RotatedRect" region in 'rects'
    for(int i = 0; i < rects.size(); i++)
    {
        Point2f rect_points[4]; rects[i].points(rect_points);

        for(int j = 0; j < 4; j++)
            line(img, rect_points[j], rect_points[(j+1)%4], color, 2, 8);
    }
}



bool Does_Rectangle_Contain_Point(const RotatedRect &_rect, const Point2f &_point)
{
//  Checks if the co-ordinate lies inside the region "RotatedRect"

//  Get the co-ordinates of corners of "RotatedRect"
    Point2f corners[4];
    _rect.points(corners);

//  Convert the point array to a vector.
    Point2f* lastItemPointer = (corners + sizeof corners / sizeof corners[0]);
    vector<Point2f> contour(corners, lastItemPointer);

//  Check if the point is within the rectangle.
    double indicator = pointPolygonTest(contour, _point, false);
    bool rectangleContainsPoint = (indicator >= 0);
    return rectangleContainsPoint;
}


bool Does_Rectangles_Contain_Point(const vector<RotatedRect> &_rects, const Point2f &_point)
{
//  Checks if the co-ordinate lies inside any region "RotatedRect" of '_rects'  
    for(vector<RotatedRect>::const_iterator _itc = _rects.begin(); _itc <= _rects.end(); _itc++)
    {
        if(_rects.empty())
//          Prevent SegFaults
            break;
        else if(Does_Rectangle_Contain_Point( RotatedRect(*_itc), _point ))
            return 1;
    }

    return 0;
}



void Add_Region_From_Coordinate(vector<RotatedRect> &all_rects, vector<RotatedRect> &destination_rects, vector<RotatedRect>::iterator &itc, const  Point2f &coordinate)
{
//  Adds a "RotatedRect" from 'all_rects' to 'destination_rects' that matches the co-ordinate 'center'

    for(itc = all_rects.begin(); itc <= all_rects.end(); itc++)
    {
        if ( Does_Rectangle_Contain_Point( RotatedRect(*itc), coordinate ) )
        {
            destination_rects.push_back(RotatedRect(*itc));
            cout << "Region Reserved! " << endl;
        }
    }
}


void Remove_Region_From_Coordinate(vector<RotatedRect> &rects, vector<RotatedRect>::iterator &itc, const  Point2f &coordinate)
{
//  Removes a detected "RotatedRect" region from co-ordinate 'center'
    itc = rects.begin();

    while(itc != rects.end())
    {
        if ( Does_Rectangle_Contain_Point( RotatedRect(*itc), coordinate ) )
        {
            Draw_Rotated_Rect(img_perm, RotatedRect(*itc), Scalar(255, 255, 255));
            itc = rects.erase(itc);
            cout << "Detected Region Deleted! " << endl;
        }

        else
            ++itc;
    }
}



void Mouse_To_Region(vector<RotatedRect> &rects, vector<Point> &coords)
{
//  Adds "RotatedRect" region on mouse-clicked 'coords'
    int x[4]; int y[4];

    x[0] = coords.at(0).x;     y[0] = coords.at(0).y;
    x[1] = coords.at(1).x;     y[1] = coords.at(1).y;
    x[2] = coords.at(2).x;     y[2] = coords.at(2).y;
    x[3] = coords.at(3).x;     y[3] = coords.at(3).y;

    int center_x = (x[0] + x[1] + x[2] + x[3]) / 4;
    int center_y = (y[0] + y[1] + y[2] + y[3]) / 4;

    float angle = (atan2(y[1] - y[2], x[1] - x[2]));
    float height = abs(y[1] - y[2]);
    float width = abs(x[1] - x[0]);

    RotatedRect temp = RotatedRect(Point2f(center_x, center_y), Size2f(width, height), angle);
    rects.push_back(temp);


    coords.clear();
    cout << "\nCo-ordinates Added!"<<endl;
    cout << "Center: " << center_x << " , " << center_y << endl;
    cout << "Angle: " << angle <<endl;
    cout << "Dimension: " << width << " x " << height << endl << endl;
    cout << "Usage:\n\t1.) Register 4 Left-clicks (Rectangle co-ordinates) to add parking regions \n\t2.) Right-click to delete a region  \n\t3.) Press 'Enter' to continue" << endl << endl;
    N = 0;

    Draw_Rotated_Rects(img_perm, rects);

}



void mouse_event(int event, int x, int y, int flags, void* userdata)
{
//  Mouse setback function definition
    rects_green = *((vector<RotatedRect>*)userdata);


    if (event == EVENT_LBUTTONDOWN)
    {
//      Adds co-ordinates from mouse input (Left-Click)
        coords.push_back(Point(x,y));
        N++;
        cout << "Click! " << N << endl;
        circle(img_perm, Point(x,y), 2, Scalar(0,200,255));
    }


    if (event == EVENT_RBUTTONDOWN)
    {
//      Removes red as well as green regions (Right-Click)
        Remove_Region_From_Coordinate(rects_green, itc_green, Point2f(x,y) );
        Remove_Region_From_Coordinate(rects_red, itc_red, Point2f(x,y) );
    }


    if (event == EVENT_MBUTTONDOWN)
    {
//      Swaps red to green and vice versa

//      If Rectangles 'rects_red' does not contain Point (x,y)    
        if(!Does_Rectangles_Contain_Point(rects_red, Point2f(x,y)))
            Add_Region_From_Coordinate(rects_green, rects_red, itc_green, Point2f(x,y) );   // Only Works for ticketing mode (See bottom)
        else
            Remove_Region_From_Coordinate(rects_red, itc_red, Point2f(x,y) );
    }


    if(N>=4)
    {
//      Check for 4 clicks (of a rectangle) for LBUTTON event
        Mouse_To_Region(rects_green, coords);
    }
}



bool Verify_Aspect_Ratio(RotatedRect candidate, float error = 0.64, const float aspect = 3.0)
{

//  Check for aspect ratio with error margin of 64%  (Fine-tuned)                       ***
//  Average car dimension: 1845x570  i.e. aspect ratio: ~3.2 (Fine-tuned to 3.0)        ***


//  Set a min and max area; all other patchs are discarded
int min = 8 * aspect * 8;     // min area                                               ***
    int max = 40 * aspect * 40;   // max area                                           ***


//  Get only patches that match to a aspect ratio.
    float rmin = aspect - aspect * error;
    float rmax = aspect + aspect * error;


    int area = candidate.size.height * candidate.size.width;
    float r = (float)candidate.size.width / (float)candidate.size.height;


    if(r<1)
    {
//      if height greater than width (check for rotation)
        r = 1/r;
    }


    if((area < min || area > max) || (r < rmin || r > rmax))
        return false;
    else
        return true;
}



void Clear_Screen()
{
//  OS independent clearing the screen method
    cout << "\033[2J\033[1;1H" << endl << endl << endl;
}




/*====================================              MAIN FUNCTION          ============================================*/
/*=====================================================================================================================*/



int main(int argc, char* argv[])
{


    if(argc != 3)
    {
//      Check for correct input
        cout <<"\n   Enter empty image and video PATH as command-line arguments \n\n   Example: ./a.out img_empty4.jpg vid4.mp4" << endl << endl;
        return -1;
    }

//  Read the file
    Mat img_empty, temp, temp2;
    img_empty = imread(argv[1], IMREAD_COLOR);

//  img2 = imread(argv[1], IMREAD_COLOR);
    img_perm = img_empty.clone();


//  Necessary declarations
    Mat dilate_element = getStructuringElement(MORPH_RECT, Size(4, 4));
    Mat erode_element = getStructuringElement(MORPH_RECT, Size(4, 4));
    Scalar colors = Scalar(128, 128, 128);
    //rects_red.push_back( RotatedRect(Point2f(0, 1), Size2f(0, 0), 0) );         // 'rects_red' initialized (prevent SegFaults)


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
    morphologyEx(temp, temp, CV_MOP_CLOSE, kernel);                 // Remove internal noise


//  Substract original edges from later ones again
    temp -= img_empty;


//  Erode and dilation to separate detected sections clearly                                    ***

    erode_element = getStructuringElement(MORPH_RECT, Size(15, 15));
    dilate_element = getStructuringElement(MORPH_RECT, Size(13, 13));

    erode(temp, img_empty, erode_element);
    dilate(img_empty, img_empty, dilate_element);


//  Retrieve all external contours, all pixels of it
    cv::findContours(img_empty, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);


//  Iterator counter for contours
    vector<vector<Point>>::iterator itc_contour = contours.begin();
    

    while(itc_contour!=contours.end())
    {

//      Remove patch that has no inside limits of aspect ratio and area.
        RotatedRect mr = minAreaRect(Mat(*itc_contour));    //  Create bounding rect of object

        if(!Verify_Aspect_Ratio(mr, 0.64, 3.7))             //  Error of 64%    Aspect ratio: 3.7 : 1
        {
            itc_contour = contours.erase(itc_contour);
        }

        else
        {
            ++itc_contour;
            rects_green.push_back(mr);
        }

    }


//  Draw detected parkable regions
    Draw_Rotated_Rects(img_perm, rects_green);



//  I/O operations

    cout << "Usage:\n\t1.) Register 4 Left-clicks (Rectangle co-ordinates) to add parking regions \n\t2.) Right-click to delete a region  \n\t3.) Press 'Enter' to continue" << endl << endl;

    namedWindow("Debug Screen", WINDOW_AUTOSIZE);
    setMouseCallback("Debug Screen", mouse_event, &rects_green);

    while(1)
    {
//      Hold the screen (60 secs for "Enter" key)
        imshow("Debug Screen", img_perm);
        if(waitKey(60) == 10)
            break;
    }



/*=================================                 VIDEO STARTS HERE      ============================================*/
/*=====================================================================================================================*/



//  Read the files
    Mat frame, frame_perm;
    img_empty = imread(argv[1], IMREAD_COLOR);
    VideoCapture frames(argv[2]);


    if(!frames.isOpened() || img_empty.empty())
    {
//      Check for invalid input
        cout << "\n   Invalid Path for video file. \n   Example: ./a.out img_empty4.jpg vid4.mp4" << endl;
        return -1;
    }


//  Necessary re-difinitions and other declarations
    dilate_element = getStructuringElement(MORPH_RECT, Size(8,8));   //                ***
    erode_element = getStructuringElement(MORPH_RECT, Size(2, 2));

    Mat erode_element2 = getStructuringElement(MORPH_RECT, Size(15, 15));
    Mat dilate_element2 = getStructuringElement(MORPH_RECT, Size(7, 7));

    kernel = getStructuringElement(MORPH_RECT, Size(20, 20));        // Create kernel   ***

    vector<vector<Point>>::iterator itc_vehicles_contour;
    vector<RotatedRect>::iterator itc_vehicles;



//  Converts image to gray
    cvtColor(img_empty, img_empty, COLOR_BGR2GRAY);        // Intput and output image names
    blur(img_empty, img_empty, Size(5,5));                // Gaussian blur 5,5


//  Edge Detection operations
    Sobel(img_empty, img_horizontal, CV_8U, 0, 1, 3, 5, 0);  // Vertical Edges              ****
    Sobel(img_empty, img_vertical, CV_8U, 1, 0, 3, 5, 0);    // Horizontal Edges            ****


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


//      Converts frame to gray
        cvtColor(frame, frame, COLOR_BGR2GRAY);
        blur(frame, frame, Size(5,5));                        // Gaussian blur 5x5


//      Apply Sobel filter
        Mat img_horizontal2, img_vertical2;
        Sobel(frame, img_horizontal2, CV_8U, 0, 1, 3, 4, 0);  // Vertical Edges              ****
        Sobel(frame, img_vertical2, CV_8U, 1, 0, 3, 4, 0);    // Horizontal Edges            ****


//      Add Horizontal + Vertical edges
        frame = img_horizontal2 + img_vertical2;


//      Create output matrice(s)
        frame -= img_empty;


//      Threshold
        threshold(frame, frame, 0, 255, THRESH_OTSU+THRESH_BINARY);


//      Dilate to remove small marks
        erode(frame, frame, erode_element);


//      Morphology (Perform after substraction)
        morphologyEx(frame, frame, CV_MOP_CLOSE, kernel);       // Remove internal noise


//      Substract original edges from later ones
        frame -= img_empty;
        dilate(frame, frame, dilate_element2);


//      Instantiate contours
        vector<vector<Point>> contours_vehicles;                 // vector of vectors (contours)
        cv::findContours(frame, contours_vehicles, RETR_EXTERNAL, CHAIN_APPROX_NONE); // retrieve all external contours, all pixels of it


//      Iterator for contours
        vector<RotatedRect> rects_vehicles;                      // New initialization needed for every new frame
        itc_vehicles_contour = contours_vehicles.begin();


    
        while(itc_vehicles_contour!=contours_vehicles.end())
        {

//          Remove patch that has no inside limits of aspect ratio and area.

//          Create bounding rect of object
            RotatedRect mr_vehicles = minAreaRect(Mat(*itc_vehicles_contour));

            if(!Verify_Aspect_Ratio(mr_vehicles))
                itc_vehicles_contour = contours_vehicles.erase(itc_vehicles_contour);
            else
            {
                ++itc_vehicles_contour;
                rects_vehicles.push_back(mr_vehicles);
            }
        }



        for(itc_green = rects_green.begin(); itc_green <= rects_green.end(); itc_green++)
        {
//          Convert green regions "rects_green" to red regions "rects_red" on detection of obstruction (center of vehicles lies on the green region)
            bool red_already_contains_this_obstruction = false;

            for(itc_vehicles = rects_vehicles.begin(); itc_vehicles <= rects_vehicles.end(); itc_vehicles++)
            {
//              Prevent duplication of same regions
                red_already_contains_this_obstruction = Detect_Rect_In_Rects( RotatedRect(*itc_green), rects_red );

                if(rects_vehicles.empty() || rects_green.empty())
//                  Prevent SegFaults
                    break;
                else if ( Does_Rectangle_Contain_Point( RotatedRect(*itc_green), RotatedRect(*itc_vehicles).center ) &&  !red_already_contains_this_obstruction) 
                    rects_red.push_back(RotatedRect(*itc_green));        
            }
        }



/*================== Ticketing Region (See Bottom) ==================*/


//         itc_red = rects_red.begin();
//         while(itc_red != rects_red.end())
//         {
// //          Delete red regions "rects_red" on removal of obstruction (center of vehicles doesn't lie on the red region)
//             bool obstruction_remains = false;

//             for(itc_vehicles = rects_vehicles.begin(); itc_vehicles <= rects_vehicles.end(); itc_vehicles++)
//             {
//                 if(rects_vehicles.empty() || rects_red.empty())
// //              Prevent SegFaults
//                     break;
//                 else if ( Does_Rectangle_Contain_Point( RotatedRect(*itc_red), RotatedRect(*itc_vehicles).center ) )
//                     obstruction_remains = true;
//             }

//             if(obstruction_remains)
//                 ++itc_red;

//             else
//                 itc_red = rects_red.erase(itc_red);
//         }

/*===================== End of Ticketing Region =====================*/



        temp = frame.clone();

//      Draw the vehicle regions
        Draw_Rotated_Rects(temp, rects_vehicles);
        Draw_Rotated_Rects(frame_perm, rects_vehicles, Scalar(0,215,255));


//      Draw the Parking regions (Green)
        Draw_Rotated_Rects(frame_perm, rects_green);


//      Draw obstruction regions (Red)
        Draw_Rotated_Rects(frame_perm, rects_red, Scalar(0,0,255));




//      Output Operations

//      Debugging Screen
//      namedWindow("img4", WINDOW_AUTOSIZE);
//      imshow("img4", temp);

        namedWindow("frame_perm", WINDOW_AUTOSIZE);
        setMouseCallback("frame_perm", mouse_event, &rects_green);
        imshow("frame_perm", frame_perm);

        Clear_Screen();
        cout << "Total parking Spots = \t" << rects_green.size() << endl;
        cout << "No. of Spots Occupied =\t" << rects_red.size() << endl;
        cout << "No. of Empty Spaces =\t" << (rects_green.size()-rects_red.size()) << endl;


        if (waitKey(20) == 10)
//          Wait 'Enter' key input for 20 secs
            break;

    }


//  Wait for a keystroke in the window
    waitKey(0);
    return 0;
}


//  Dependencies:           sudo apt-get install libopencv-dev
//  Compiled with:          g++ init.cpp `pkg-config --cflags --libs opencv`
//  Executed with:          ./a.out img_empty4.jpg vid4.mp4
//  Switch to ticket mode:  Comment out lines marked around line 555