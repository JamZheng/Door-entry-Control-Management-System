from ctypes import *  
dll = CDLL("./face.dll")
img1 = c_char_p(bytes("./facelib/t.jpg", 'utf-8'))
img2 = c_char_p(bytes("./facelib/t.jpg", 'utf-8')) 
print(dll.img_matching(img1,img2))