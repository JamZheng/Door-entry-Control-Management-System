import datetime
import os


def get_time():
    now_time = datetime.datetime.now()
    time1_str = datetime.datetime.strftime(now_time, '%Y-%m-%d %H:%M:%S')
    return time1_str

def mkdir(path):
    # 去除首位空格
    path = path.strip()
    # 去除尾部 \ 符号
    path = path.rstrip("\\")
    isExists = os.path.exists(path)
    if not isExists:
        os.makedirs(path)
        print("创建成功")
        return True
    else:
        print(path + ' 目录已存在')
        return False


def create_file():
    the_time = get_time()
    the_path = str(os.getcwd())+'/time/' + str(the_time[0:10])
    the_path = the_path.replace('\\','/')
    mkdir(the_path)
    return the_time,the_path

if __name__ == "__main__":
    print(create_file())
    print(os.getcwd())
