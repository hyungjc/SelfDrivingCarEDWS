"""
WED 29 JAN

Computer Vision
"""
def main():
    print('Take pictures with this program')
    while True:
        print('Press p to take 5 pictures')
        user_input = input()
        if user_input == 'p':
            continue
        else:
            break

def takePic():
    camera.start_preview()
    sleep(5)
    camera.capture('/home/pi/Desktop/image.jpg')
    camera.stop_preview()    
