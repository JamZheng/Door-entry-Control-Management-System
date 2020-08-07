# -*- coding:utf-8 -*-
import datetime
import sqlite3, re


#创建数据库
def create():
    conn = sqlite3.connect('Record.db')
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS COMPANY
           (ID INTEGER PRIMARY KEY AUTOINCREMENT,
           TIME           TEXT    NOT NULL,
           MASK           TEXT    NOT NULL);
           ''')
    conn.commit()
    conn.close()


#存储记录
def store(flag,time_now):
    conn = sqlite3.connect('Record.db')
    c = conn.cursor()
    if(flag==1):
        str = "INSERT INTO COMPANY (TIME,MASK) \
          VALUES ( '{timestr}', '未佩戴口罩' )".format(timestr = time_now)
    elif(flag==0):
        str = "INSERT INTO COMPANY (TIME,MASK) \
          VALUES ( '{timestr}', '已佩戴口罩' )".format(timestr = time_now)
    elif(flag==-1):
        str = "INSERT INTO COMPANY (TIME,MASK) \
          VALUES ( '{timestr}', '未识别到人像' )".format(timestr = time_now)
    c.execute(str)
    conn.commit()
    conn.close()


#查询记录
def lookup():
    conn = sqlite3.connect('Record.db')
    try:
        cursor =conn.execute("SELECT * from COMPANY")
        print ("ID   时间   是否佩戴口罩")
        for it in cursor:
            s = ''
            for i in range(len(it)):
                s = s + str(it[i]) + '   '
            print(s + '\n')
    except sqlite3.OperationalError:
        print('数据库中暂无用户数据')
    conn.close()
    
