#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from tensorflow import keras
import numpy as np

# NN輸入檔名
nn_name = '180best.h5'

model = keras.models.load_model(nn_name)
# 準備 x_test 與 y_test 資料
def pre_processing(data):
    x_test = np.zeros((1, 147))  
    color_list = rest_color(data)
    for i in range(7):
        for j in range(7):
            num = -1
            space = 0
            for m in range(7):
                if data[i, m] == data[i, j]:
                    num+=1
                if data[i, m] == 0:
                    space+=1
                if data[m, j] == data[i, j]:
                    num+=1
                if data[m, j] == 0:
                    space+=1
            # 目前盤面同色個數
            x_test[0, i*21+j*3] = num
            
            if data[i, j]!=0:
                # 不是空格
                x_test[0, i*21+j*3+1] = space
                # 同色個數
                x_test[0, i*21+j*3+2] = color_list[int(data[i, j])-1]
            else:
                #是空格
                x_test[0, i*21+j*3+1] = num
                x_test[0, i*21+j*3+2] = 0
    return x_test

# 計算剩餘顏色
def rest_color(data):
    color_list = np.array([0, 0, 0, 0, 0, 0, 0])
    for i in range(7):
        for j in range(7):
            if data[i, j]==1:
                color_list[0]+=1
            elif data[i, j]==2:
                color_list[1]+=1
            elif data[i, j]==3:
                color_list[2]+=1
            elif data[i, j]==4:
                color_list[3]+=1
            elif data[i, j]==5:
                color_list[4]+=1
            elif data[i, j]==6:
                color_list[5]+=1
            elif data[i, j]==7:
                color_list[6]+=1
    for i in range(7):
        color_list[i] = 7-color_list[i]
    return color_list


def run_nn(data):
    #測試
    board = pre_processing(data)
    test_predictions = np.array(model(board, training=False))
    return np.round(test_predictions)

if __name__ == "__main__":
    data = np.array([(5, 6, 1, 3, 0, 3, 1), 
                    (0, 0, 0, 0, 0, 0, 0), 
                    (3, 6, 2, 4, 1, 1, 4), 
                    (0, 5, 7, 5, 6, 0, 7), 
                    (1, 6, 0, 0, 1, 0, 0), 
                    (0, 2, 4, 0, 4, 0, 6), 
                    (0, 0, 0, 0, 3, 0, 0)])

    y_test = np.array([106])
    print(run_nn(data))