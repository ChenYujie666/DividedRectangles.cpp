import numpy as np
import matplotlib.pyplot as plt

# 生成示例矩阵 (0-1)
matrix = np.zeros((100, 100))


x1,y1 = 40,40
x2,y2 = 60,80

assert(y1 < y2)


for x in range(100):
    for y in range(100):
        # if y > y2:
        #     matrix[x][y] = 0.4
        if x == x2:
            if y > y2:
                matrix[x][y] = 0.4
            continue
        if(y1 <= (y2-y)/(x2-x)*(x1-x)+y):
            matrix[x][y] = 0.3
        if matrix[x][y]  and y < y1:
            matrix[x][y] = 0.05
        if y > y2:
            matrix[x][y] += 0.4

matrix[x1][y1] = 1
matrix[x2][y2] = 0.7



x_interval1, y_interval1 = 20,60
x_interval2, y_interval2 = 45,20
x_interval3, y_interval3 = 50,50
x_interval4, y_interval4 = 20,20
x_interval5, y_interval5 = 80,20

matrix[x_interval1][y_interval1] = 0.5
matrix[x_interval2][y_interval2] = 0.5
matrix[x_interval3][y_interval3] = 0.5
matrix[x_interval4][y_interval4] = 0.5
matrix[x_interval5][y_interval5] = 0.5

matrix = matrix.T



plt.figure(figsize=(6, 6))
plt.imshow(matrix, cmap="binary", interpolation="none", origin="lower") # 使用黑白颜色映射



plt.text(x1, y1, " interval 1", c = "b")
plt.text(x2, y2-2, " interval 2")

plt.text(x_interval1, y_interval1, " keep interval1 ")
plt.text(x_interval2, y_interval2, " pop interval1",c="r")
plt.text(x_interval3, y_interval3, " pop interval1",c="r")
plt.text(x_interval4, y_interval4, " keep interval1",c="r")
plt.text(x_interval5, y_interval5, " keep interval1",c="r")


# 显示图像
plt.show()