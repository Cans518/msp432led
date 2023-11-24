import cv2
from pyzbar.pyzbar import decode
import serial
import numpy as np

# 初始化串口
ser = serial.Serial('COM6', 115200)  

def read_qr_code_and_send_serial():
    # 打开摄像头
    cap = cv2.VideoCapture(0)

    while True:
        ret, frame = cap.read()
        if not ret:
            print("无法读取摄像头")
            break

        # 在帧上进行二维码解码
        decoded_objects = decode(frame)

        for obj in decoded_objects:
            qr_data = obj.data.decode('utf-8')
            print(f"二维码内容: {qr_data}")

            # 发送数据到串口
            if qr_data == "on":
                ser.write(b'1')  # 发送字节'1'
                print("向串口发送: 1")
            elif qr_data == "off":
                ser.write(b'2')  # 发送字节'2'
                print("向串口发送: 2")

            # 在图像上绘制边界框
            points = obj.polygon
            if len(points) > 4:
                hull = cv2.convexHull(np.array([point for point in points], dtype=np.float32))
                cv2.polylines(frame, [hull], True, (0, 255, 0), 2)

        # 显示摄像头图像
        cv2.imshow("QR Code Scanner", frame)

        # 检测按键事件，如果按下 "q" 键则退出循环
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # 释放摄像头、关闭串口和窗口
    cap.release()
    ser.close()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    read_qr_code_and_send_serial()
