 
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>


using namespace cv;
using namespace std;


//  Variables required for Callback functions (Run-time Variables)
struct Runtime_Variables
{    
        int N = 0;      //  Count for number of points of a rectangle (for mouse input)
        Mat img_perm;

//      Instantiate contours and its iterators
        vector<vector<Point>> contours;

        vector<RotatedRect> rects_green;
        vector<RotatedRect>::iterator itc_green;
    
        vector<RotatedRect> rects_red;
        vector<RotatedRect>::iterator itc_red;

//      Instantiate co-ordinates (coords)
        vector<Point> coords;
};



//  User-defined functions:


//  Searches for duplication of 'rect' regions in vector 'rects' (only checks for centers and angle)
bool Detect_Rect_In_Rects(const RotatedRect &_rect, const vector<RotatedRect> _rects)
{
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


//  Draws a single "RotatedRect" region named 'rect'
void Draw_Rotated_Rect(Mat &img, const RotatedRect &rect, Scalar color =  Scalar(255, 255, 255))
{
    Point2f rect_points[4]; rect.points(rect_points);

    for(int j = 0; j < 4; j++)
        line(img, rect_points[j], rect_points[(j+1)%4], color, 2, 8);
}


//  Draws every "RotatedRect" region in 'rects'
void Draw_Rotated_Rects(Mat &img, vector<RotatedRect> &rects, Scalar color =  Scalar(75, 150, 0))
{
    for(int i = 0; i < rects.size(); i++)
    {
        Point2f rect_points[4]; rects[i].points(rect_points);

        for(int j = 0; j < 4; j++)
            line(img, rect_points[j], rect_points[(j+1)%4], color, 2, 8);
    }
}



//  Checks if the co-ordinate lies inside the region "RotatedRect"
bool Does_Rectangle_Contain_Point(const RotatedRect &_rect, const Point2f &_point)
{
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


//  Checks if the co-ordinate lies inside any region "RotatedRect" of '_rects'
bool Does_Rectangles_Contain_Point(const vector<RotatedRect> &_rects, const Point2f &_point)
{  
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



//  Adds a "RotatedRect" from 'all_rects' to 'destination_rects' that matches the co-ordinate 'center'
void Add_Region_From_Coordinate(vector<RotatedRect> &all_rects, vector<RotatedRect> &destination_rects, vector<RotatedRect>::iterator &itc, const  Point2f &coordinate)
{

    for(itc = all_rects.begin(); itc <= all_rects.end(); itc++)
    {
        if ( Does_Rectangle_Contain_Point( RotatedRect(*itc), coordinate ) )
        {
            destination_rects.push_back(RotatedRect(*itc));
            cout << "Region Reserved! " << endl;
        }
    }
}


//  Removes a detected "RotatedRect" region from co-ordinate 'center'
void Remove_Region_From_Coordinate(vector<RotatedRect> &rects, vector<RotatedRect>::iterator &itc, const  Point2f &coordinate, Mat &img_perm)
{
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



//  Adds "RotatedRect" region on mouse-clicked 'coords'
void Mouse_To_Region(vector<RotatedRect> &rects, vector<Point> &coords, Mat &img_perm, int &N)
{
    int x[4]; int y[4];

    x[0] = coords.at(0).x;     y[0] = coords.at(0).y;
    x[1] = coords.at(1).x;     y[1] = coords.at(1).y;
    x[2] = coords.at(2).x;     y[2] = coords.at(2).y;
    x[3] = coords.at(3).x;     y[3] = coords.at(3).y;

    int center_x = (x[0] + x[1] + x[2] + x[3]) / 4;
    int center_y = (y[0] + y[1] + y[2] + y[3]) / 4;

    float angle  = (atan2(y[1] - y[2], x[1] - x[2]));
    float height = abs(y[1] - y[2]);
    float width  = abs(x[1] - x[0]);

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



//  Mouse setback function definition
void Mouse_Event(int event, int x, int y, int flags, void* userdata)
{

    Runtime_Variables* runtime = ((Runtime_Variables*)userdata);


    if (event == EVENT_LBUTTONDOWN)
    {
//      Adds co-ordinates from mouse input (Left-Click)
        runtime->coords.push_back(Point(x,y));
        runtime->N++;
        cout << "Click! " << runtime->N << endl;
        circle(runtime->img_perm, Point(x,y), 2, Scalar(0,200,255));
    }


    if (event == EVENT_RBUTTONDOWN)
    {
//      Removes red as well as green regions (Right-Click)
        Remove_Region_From_Coordinate(runtime->rects_green, runtime->itc_green, Point2f(x,y), runtime->img_perm);
        Remove_Region_From_Coordinate(runtime->rects_red, runtime->itc_red, Point2f(x,y), runtime->img_perm);
    }


    if (event == EVENT_MBUTTONDOWN)
    {
//      Swaps red to green and vice versa

//      If Rectangles 'rects_red' does not contain Point (x,y)    
        if(!Does_Rectangles_Contain_Point(runtime->rects_red, Point2f(x,y)))
            Add_Region_From_Coordinate(runtime->rects_green, runtime->rects_red, runtime->itc_green, Point2f(x,y) );   // Only Works for ticketing mode (See bottom)
        else
            Remove_Region_From_Coordinate(runtime->rects_red, runtime->itc_red, Point2f(x,y), runtime->img_perm);
    }


    if(runtime->N >= 4)
    {
//      Check for 4 clicks (of a rectangle) for LBUTTON event
        Mouse_To_Region(runtime->rects_green, runtime->coords, runtime->img_perm, runtime->N);
    }
}



/*  Check for aspect ratio with error margin of 64%  (Fine-tuned)                       ***
    Average car dimension: 1845x570  i.e. aspect ratio: ~3.2 (Fine-tuned to 3.0)        ***/
bool Verify_Aspect_Ratio(RotatedRect candidate, float error = 0.64, const float aspect = 3.0)
{

//  Set a min and max area; all other patchs are discarded
    int min = 8 * aspect * 8;     // min area                                           ***
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



//  OS independent clearing the screen method
void Clear_Screen()
{
    cout << "\033[2J\033[1;1H" << endl << endl << endl;
}