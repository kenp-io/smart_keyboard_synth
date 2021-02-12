import os
import time 

os.system("sudo ip link set can0 down")
time.sleep(1)
os.system("sudo ip link set can0 up type can bitrate 500000")
time.sleep(1)

os.system("./BootCommander -s=xcp -t=xcp_can -d=can0 ../../key-firmware/example_app.srec -tid=021 -rid=011")
os.system("./BootCommander -s=xcp -t=xcp_can -d=can0 ../../key-firmware/example_app.srec -tid=022 -rid=012")
os.system("./BootCommander -s=xcp -t=xcp_can -d=can0 ../../key-firmware/example_app.srec -tid=023 -rid=013")
os.system("./BootCommander -s=xcp -t=xcp_can -d=can0 ../../key-firmware/example_app.srec -tid=024 -rid=014")
os.system("./BootCommander -s=xcp -t=xcp_can -d=can0 ../../key-firmware/example_app.srec -tid=025 -rid=015")
os.system("./BootCommander -s=xcp -t=xcp_can -d=can0 ../../key-firmware/example_app.srec -tid=026 -rid=016")
os.system("./BootCommander -s=xcp -t=xcp_can -d=can0 ../../key-firmware/example_app.srec -tid=027 -rid=017")
os.system("./BootCommander -s=xcp -t=xcp_can -d=can0 ../../key-firmware/example_app.srec -tid=028 -rid=018")
