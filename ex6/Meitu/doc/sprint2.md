
# 设计文档

## 第二轮迭代

灰度化图片。

### 命令

| 名字         | 参数          | 返回值   |
|:-------------|:-------------|:---------|
| gray         |              | int      |

### Model层

ImageModel类增加灰度化方法，触发通知。

```mermaid

classDiagram

class ImageModel {
...
	+gray() void
}

```

### ViewModel层

ImageViewModel类增加灰度化命令，提供获取该命令对象接口的方法。

```mermaid

classDiagram

class ImageViewModel {
...
	+get_gray_command() ICommandBase* |
}
ImageViewModel --|> GrayCommand~ImageViewModel~

class GrayCommand~T~ {
	+exec(const std::any& v) int*
}
GrayCommand~T~ ..|> ICommandBase

```

### View层

MainWindow类增加灰度化命令的成员变量和设置方法。
增加灰度化按钮和回调事件。

```mermaid

classDiagram

class MainWindow {
...
	-ICommandBase *m_gray_command

	+set_gray_command(ICommandBase *p) void

	-btn_gray_cb(Fl_Widget *, void *) void$
}
MainWindow o-- ICommandBase

```

### app层

增加灰度化命令的组装。

```

m_main_wnd <-- m_image_viewmodel.get_gray_command

```
