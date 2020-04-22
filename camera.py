"""
WED 29 JAN

Computer Vision
"""
from picamera import PiCamera
from time import sleep

def main():
    print('Take pictures with this program')
    while True:
        print('Press p and enter to take 5 pictures')
        user_input = input()
        if user_input == 'p':
            continue
        else:
            break
main()

def takePic():
    camera.start_preview()
    sleep(5)
    camera.stop_preview()
    camera.capture('/home/pi/Desktop/image.jpg')
    camera.stop_preview()    
