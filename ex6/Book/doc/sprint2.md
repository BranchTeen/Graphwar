
# 设计文档

## 第二轮迭代

从文件加载书的数据。

### 命令

| 名字         | 参数          | 返回值   |
|:-------------|:-------------|:---------|
| Load         | const char*  | int      |

### common层

命令参数类 LoadParameter 即 `TypeParameter<const char*>`。

### Model层

InfoModel类增加加载文件的方法，并触发通知。

```mermaid

classDiagram

class InfoModel {
...
	+load(const char* file) bool
}

```

### ViewModel层

BookViewModel类增加加载命令的成员对象，及获取该对象接口的方法。

```mermaid

classDiagram

class BookViewModel {
...
	-LoadCommand m_load_command

	+get_load_command() ICommandBase* |
}

class LoadCommand {
	-BookViewModel *m_pvm

	+exec(ICommandParameter *p) int*
}
LoadCommand ..|> ICommandBase
LoadCommand o-- BookViewModel

BookViewModel *-- LoadCommand

```

### View层

MainWindow类增加加载命令的成员变量和设置方法。
增加加载按钮和回调事件。

```mermaid

classDiagram

class MainWindow {
...
	-ICommandBase* m_load_command

	+set_load_command(ICommandBase *p) void

	-btn_load_cb(Fl_Widget *, void *) void$
}
MainWindow o-- ICommandBase

```

### app层

增加加载命令的组装。

```

m_main_wnd <-- m_sp_book_viewmodel.get_load_command

```
