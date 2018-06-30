# Graphics

一个中学物理老师翻译的DX11龙书：
http://shiba.hpe.sh.cn/jiaoyanzu/WULI/Soft/NotXNA

如何使龙书demo运行起来：
https://blog.csdn.net/pobber/article/details/51971939?_t=t

大神翻译的OpenGL：
https://learnopengl-cn.github.io/


### 解惑：别人的Direct3D 11 demo 可能在自己机子上build失败？


从Win8开始，DirectX SDK 被包含在了 Windows SDK中。  
微软的解释是：DirectX技术和Windows系统结合得越来越紧密，而Windows SDK是Windows上最主要的SDK。  
如果仅仅只是把以前独立发布的DirectX SDK移入Windows SDK中，可能也不会有上面的问题。  
问题在于：微软不再发布单独的DirectX SDK，而且剔除了所有的D3DX库（包括D3DX11）。微软去掉了d3dx*.h和d3dx*.lib等等，把剩下的合并到了Windows SDK中。  
（注：D3DX和D3D并不是一个东西。D3DX，即Direct3D Extension是一套比较上层的API库，用于给微软的Direct3D图形API提供辅助。D3DX已经被微软抛弃。）  
https://docs.microsoft.com/zh-cn/windows/desktop/directx-sdk--august-2009-  
上面的链接里面，描述了曾经属于DirectX SDK，而现在属于Windows SDK的工具库。也就是说，这里列出的工具库依然可用。  
如果依然想用被抛弃的D3DX库，一般也是可以的，因为微软已经把很多D3DX开源了。比如FX11、DirectXTex，都已经开源了。  
  
比较容易混淆的还有D3DXMath。Direct3D 9和Direct3D 10都有D3DXMath库，但是Direct3D 11不包含D3DXMath库，取而代之的是XNAMath。  
然后，随着DirectX SDK移入Windows SDK, XNAMath也不再存在，取而代之的是DirectXMath。其实基本上只是XNAMath改了名字为DirectXMath。  
当然，头文件、类型名、API都是有改动的，只是替换起来也容易。另外，DirectXMath增加了命名空间DirectX。  
  
可以参考：Living without D3DX( https://blogs.msdn.microsoft.com/chuckw/2013/08/20/living-without-d3dx/ )  
这里讲述了怎么替换老版本的API。  
对于D3DXMath，更详细的还可以参考Working with D3DXMath ( https://docs.microsoft.com/zh-cn/windows/desktop/dxmath/pg-xnamath-migration-d3dx )  
这里专门讲述了怎么替换老的D3DXMath，包括API，数据结构等等。  

### Code Tech
1、Direct3D编程过程中，经常需要通过日志分析问题，这里是一个简单好用的输出日志到DebugView的代码示例：  
#define Direct3DDebugString(msg) { std::wstringstream ss; ss << L"[Direct3D] "<< msg; ::OutputDebugString(ss.str().c_str()); }  
之后代码里面就可以这样输出日志：  
Direct3DDebugString(L"ResizeBuffers width=" << width_ << L" height=" << height_);  
开启DebugView工具即可查看日志输出过程。  
