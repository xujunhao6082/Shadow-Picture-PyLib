### Description

利用alpha通道,通过最小二乘法使图片在不同背景下呈现不同图像
mix函数本身接受PIL产生的bytes作为图片, 最后一个参数为float,表示最佳拟合点,可返回可由PIL读取的bytes

### Compile

`setting.json`中应设定你的依赖位置, 包括Python,OpenCL,
此外相应库目录下需有GNU格式的链接库
