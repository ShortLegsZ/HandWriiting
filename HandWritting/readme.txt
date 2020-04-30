环境：
win10+vs2017+opencv4.1.1

opencv配置环境变量
1、包含目录
路径opencv\build\include\opencv2和路径opencv\build\include添加到通用属性->VC++目录->包含目录
2、库目录
路径opencv\build\x64\vc15\lib添加到通用属性->VC++目录->库目录
3、依赖项（本次添加有opencv版本有关）
opencv_world411d.lib（debug版本）、opencv_world411.lib（release版本）添加至链接器->输入->附加依赖项

附链接（VS2017配置opencv4.1.1）
https://www.jianshu.com/p/84029f59c3a0