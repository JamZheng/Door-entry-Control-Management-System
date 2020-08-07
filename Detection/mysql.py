# -*- coding:utf-8 -*-
import pymysql


#-------------------------MySQL数据库-------------------------
#查询记录
def lookup():
    try:
        db = pymysql.connect("localhost","root","123","first_mysql_test" ) 
        cursor = db.cursor()
        # SQL 查询语句
        sql = "SELECT * FROM table_data_mask"
        # 执行SQL语句
        cursor.execute(sql)
        # 获取所有记录列表
        results = cursor.fetchall()
        print ("ID   是否佩戴口罩   时间")
        for row in results:
            print(str(row[0])+' '+str(row[1])+' '+str(row[2])+'\n')
    except :
        print('无法连接数据库或数据库中暂无用户数据')
    db.close()

    
def store(flag,now_time):
    try:
        db = pymysql.connect("localhost","root","123","first_mysql_test" )
        cursor = db.cursor()
        
        if(flag==1):
            sql = "INSERT INTO table_data_mask (TIME,MASK) \
                VALUES ( '{timestr}', '0' )".format(timestr = now_time)#未佩戴
        elif(flag==0):
            sql = "INSERT INTO table_data_mask (TIME,MASK) \
                VALUES ( '{timestr}', '1' )".format(timestr = now_time)#已佩戴
        elif(flag==-1):
            sql = "INSERT INTO table_data_mask (TIME,MASK) \
                VALUES ( '{timestr}', '2' )".format(timestr = now_time)#无人脸
        
        cursor.execute(sql)
        
        db.commit()
    except:
        print("MySQL数据库存储失败")
    db.close() 
