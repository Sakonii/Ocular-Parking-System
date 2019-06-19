## Ocular-Parking-System

Takes parking lot camera footage / images and turns it into a grid showing empty parking spaces.

## Requirements:

* [GNU c/c++ compiler]


## Dependencies:         
                  
* [OpenCV >= 3.0]    
  > sudo apt-get install libopencv-dev


## Usage:

1.  Compilation:
   g++ init.cpp `pkg-config --cflags --libs opencv`   

2.  Execution:
  > ./a.out

    Command-line arguements:
    
      * argv[1]   : Path to empty image         
      * argv[2]   : Path to test image


  Example: ./a.out img_empty.jpg
