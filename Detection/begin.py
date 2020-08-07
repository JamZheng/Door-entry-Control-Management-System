# -*- coding:utf-8 -*-
import os
import time
import datetime
import data_base as db
import mask_detection as md
#import mysql

#初始界面
def enter_command():
    cmd=input('请输入命令(?帮助):')
    cmd=cmd.strip().lower()
    return cmd


#操作指南
def print_help():
    print('可用操作:')
    print('start: 开始监测识别')
    print('lookup: 查询记录')
    print('quit: 保存并退出')
    print('?: 操作指南')


def main():
    try:
        while True:
            cmd=enter_command()
            if cmd=='start':
                md.start()    
            elif cmd=='quit':
                return
            elif cmd=='lookup':
                db.lookup()

                #mysql.lookup()
                
            elif cmd=='?':
                print_help()
    finally:
        return

if __name__=='__main__':main()
