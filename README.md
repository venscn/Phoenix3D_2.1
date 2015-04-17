Phoenix3D
=======
欢迎来到Phoenix3D源代码

Phoenix3D是一款古典的，优雅的，跨平台的开源3D引擎。

引擎使用传统引擎的“NodeBased”架构，将场景物件进行层级连接，形成有向无环“场景树”。“裁剪器”遍历“场景
树”，获得“可见渲染体列表”。“渲染器”对排序后的“可见渲染体列表”进行遍历渲染。

引擎的底层源于David Eberly的开源3D引擎WildMagic5。我们继承了WildMagic优秀的编码排版习惯，使得引擎
代码，清晰，严谨，易读。引擎中整体架构中，事件机制贯穿全程，你可以立即，轻松地通知“对象”开始你所需
要的工作。

引擎天生拥有跨平台特性，所有和平台相关的调用都被优雅的封装。引擎通过“MacroDefine”进行区别编译。目前
支持的平台有，WindowsPC，Android，IOS。对其他平台的支持，我们也在开发之中。

引擎核心层以Boost Software License进行发布。编辑器“Nirvana”以GPL协议进行发布。在Boost协议下，你拥有
非常宽松的政策，可以自由修改引擎核心，甚至可以形成自己的一个版本；在GPL协议下，你不可以引用编辑器的
代码形成一个商业版本，闭源版本，必须也是以GPL协议进行开源发布。凤凰社拥有引擎的著作权。

GitHub：https://github.com/PhenixFoundation/Phoenix3D
QQ交流群：336273110
微信公众帐号：Phoenix3d
