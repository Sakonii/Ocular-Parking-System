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
      
3.  Switch to ticket mode:
     Comment out lines marked around line 280 in "init.cpp"
