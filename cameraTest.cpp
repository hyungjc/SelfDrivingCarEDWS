#include <opencv2/opencv.hpp>
#include <raspicam_cv.h>
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;
using namespace cv;
using namespace raspicam;

RaspiCam_Cv camera;

Mat frame;
Mat displayTranformation;
Mat framPers;
Mat frameGray;
Mat frameThresh;
Mat frameEdge;
Mat frameFinal;
Mat ROILane;
Mat frameFinalDuplicate;

int leftLanePosition;
int rightLanePosition;
int roadCenter;
int frameCenter;
int deviationFromCenterLine;

stringstream stringStream;

vector<int> histogramLane;


//points of interest, change the values based on the region required
Point2f boxInFrame[] = {Point2f(25, 160), Point2f(330, 160), Point2f(0, 210), Point2f(360, 210)};
Point2f boxTo[] = {Point2f(60, 0), Point2f(300, 0), Point2f(60, 240), Point2f(300, 240)};


// camera setup. Change camera settings if needed
 void setup ( int argc,char **argv, RaspiCam_Cv &camera ) {
	 
    camera.set ( CAP_PROP_FRAME_WIDTH,  ( "-w",argc,argv,360 ) );
    camera.set ( CAP_PROP_FRAME_HEIGHT,  ( "-h",argc,argv,240 ) );
    camera.set ( CAP_PROP_BRIGHTNESS, ( "-br",argc,argv,50 ) );
    camera.set ( CAP_PROP_CONTRAST ,( "-co",argc,argv,50 ) );
    camera.set ( CAP_PROP_SATURATION,  ( "-sa",argc,argv,50 ) );
    camera.set ( CAP_PROP_GAIN,  ( "-g",argc,argv ,50 ) );
    camera.set ( CAP_PROP_FPS,  ( "-fps",argc,argv,100));
}


// get the zoomed in box of the road, this is where we will find the lanes from
void perspective() {
	
	line(frame, boxInFrame[0], boxInFrame[1], Scalar(255,0,255), 2);
	line(frame, boxInFrame[1], boxInFrame[3], Scalar(255,0,255), 2);
	line(frame, boxInFrame[2], boxInFrame[0], Scalar(255,0,255), 2);
	line(frame, boxInFrame[3], boxInFrame[2], Scalar(255,0,255), 2);
	
	//remove 
	//line(frame, boxTo[0], boxTo[1], Scalar(0,255,0), 2);
	//line(frame, boxTo[1], boxTo[3], Scalar(0,255,0), 2);
	//line(frame, boxTo[2], boxTo[0], Scalar(0,255,0), 2);
	//line(frame, boxTo[3], boxTo[2], Scalar(0,255,0), 2);
	
	displayTranformation = getPerspectiveTransform(boxInFrame, boxTo);
	
	warpPerspective(frame, framPers, displayTranformation, Size(360,240));
}


// make the perspective black and white and do the canny edge detection
void threshold() {
	
	cvtColor(framPers, frameGray, COLOR_RGB2GRAY);
	
	// change values based on what workes
	// white intencity, black intencity. White should be higher outdoors/ when it is bright 
	inRange(frameGray, 150, 255, frameThresh);
	

	// change the treshhold for what workes 
	Canny(frameThresh, frameEdge, 300, 700, 3, false);
	
	add(frameThresh, frameEdge, frameFinal);
	
	cvtColor(frameFinal, frameFinal, COLOR_GRAY2RGB);
	cvtColor(frameFinal, frameFinalDuplicate, COLOR_RGB2BGR);
}


// divide the screen into x number of bars and put the color intensity of each of those bars into an array. Black 0, White 255
void histogram() {
	
	histogramLane.resize(360);
	histogramLane.clear();
	
	for(int i = 0; i < frame.size().width; i++) {
		
		ROILane = frameFinalDuplicate(Rect(i, 140 , 1, 100));	
		divide(255, ROILane, ROILane);
		
		histogramLane.push_back((int)(sum(ROILane)[0]));
	}
}


// find where the array and the bars changes from black to white, aka the lines
void laneFinder() {
	
	vector<int>:: iterator leftPtr;
	leftPtr = max_element(histogramLane.begin(), histogramLane.begin() + frame.size().width / 2);
	leftLanePosition = distance(histogramLane.begin(), leftPtr);
	
	vector<int>:: iterator rightPtr;
	rightPtr = max_element(histogramLane.begin() + frame.size().width / 2, histogramLane.end());
	rightLanePosition = distance(histogramLane.begin(), rightPtr);
	
	line(frameFinal, Point2f(leftLanePosition, 0), Point2f(leftLanePosition, 240), Scalar(0, 255, 0), 2);
	line(frameFinal, Point2f(rightLanePosition, 0), Point2f(rightLanePosition, 240), Scalar(0, 255, 0), 2);
}


// find the center of the road and the center of the frame
void centerOfRoad() {
	
	roadCenter = (rightLanePosition-leftLanePosition) / 2 + leftLanePosition;
	frameCenter = frame.size().width / 2;
	
	line(frameFinal, Point2f(roadCenter, 0), Point2f(roadCenter, 240), Scalar(255, 0, 255), 3);
	
	// should overlap, so make changes to the frameCenter
	line(frameFinal, Point2f(frameCenter, 0), Point2f(frameCenter, 240), Scalar(255, 165, 0), 3);
	
	// calculate how much the center of the frame is from the center of the road
	deviationFromCenterLine = roadCenter-frameCenter;
}


void capture() {
	camera.grab();
    camera.retrieve(frame);
	cvtColor(frame, frame, COLOR_BGR2RGB);
}


int main(int argc,char **argv) {
	
    // camera init
	setup(argc, argv, camera);
	cout<<"Connecting to camera..."<<endl;
	if (!camera.open()) {
		cout<<"Failed to connect!!!"<<endl;
    }
     
    cout<<"Camera Id = "<<camera.getId()<<endl;
     
     
    while(1) {
		
		auto start = std::chrono::system_clock::now();
		
		// function calls
		capture();
		perspective();
		threshold();
		histogram();
		laneFinder();
		centerOfRoad();
		
		
		// displaying the images
		stringStream.str(" ");
		stringStream.clear();
		
		stringStream<<"Deviation: "<< deviationFromCenterLine;
		putText(frame, stringStream.str(), Point2f(1, 30), 0, 1, Scalar(255, 165, 0), 1);

		
		namedWindow("Original view", WINDOW_KEEPRATIO);
		moveWindow("Original view", 0, 150);
		resizeWindow("Original view", 640, 480);
		imshow("Original view", frame);
		
		
		namedWindow("Perspective", WINDOW_KEEPRATIO);
		moveWindow("Perspective", 640, 150);
		resizeWindow("Perspective", 640, 480);
		imshow("Perspective", framPers);
		
		
		namedWindow("Lane detection", WINDOW_KEEPRATIO);
		moveWindow("Lane detection", 1280, 150);
		resizeWindow("Lane detection", 640, 480);
		imshow("Lane detection", frameFinal);
	   
	   
	    // print FPS to the terminal
		waitKey(1);
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end-start;
		
		float t = elapsed_seconds.count();
		int FPS = 1/t;
		cout<<"FPS = "<<FPS<<endl;
    }

    return 0;  
}
