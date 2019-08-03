## Ocular-Parking-System

Takes parking lot camera / video footage and turns it into a grid showing empty parking spaces.

## Requirements and Dependencies:

* [GNU c/c++ compiler]
  > sudo apt-get install gcc

* [OpenCV >= 3.0]
  > sudo apt-get install libopencv-dev


## Usage:

1.  Compilation:
   > g++ init.cpp \`pkg-config --cflags --libs opencv\`

2.  Execution:
  > ./a.out "empty_images/img_empty4.jpg" "video/vid4.mp4"

    Command-line arguements:
    
      * argv[1]   : Path to empty image         
      * argv[2]   : Path to test video
      
      
## Input:

* Reference Image

![Reference Image](https://github.com/Skelliger7/Ocular-Parking-System/blob/master/empty_images/img_empty4.jpg)

* A Frame Of Video

![Frame](https://github.com/Skelliger7/Ocular-Parking-System/blob/master/process/0_input/vid_frame.png)


## Output:

* Final Output:

![Final Output](https://github.com/Skelliger7/Ocular-Parking-System/blob/master/process/5_output/target4.png)
