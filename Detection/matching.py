# -*- coding:utf-8 -*-
import os
from ctypes import *

def matching(i,the_path):
    i = i-1
    #print("00")
    dllpath = os.getcwd();
    dllpath = dllpath.replace('\\','/')
    #print(dllpath)
    facedll = CDLL(os.getcwd() + "/face.dll")
    #print("11")
    img = c_char_p(bytes(str(the_path) + '/' + 'demo' + str(i) + '.jpg', 'utf-8'))
    flagmatch = 0
    for root, dirs, files in os.walk("./facelib/"):
        for file in files:
            print("验证图片：" + "./facelib/" + file)
            img0 = c_char_p(bytes("./facelib/" + file, 'utf-8'))
            if facedll.img_matching(img, img0):
                flagmatch = 1
                break
    if flagmatch == 1:
        print("\n验证成功！请通过")
    else:
        print("\n验证失败，请调整角度，摘下口罩，重新识别")


