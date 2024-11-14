import bpy
import struct
import os

# 插件元数据
bl_info = {
    "name": "导入.mesh模型",
    "author": "letleon",
    "version": (1, 0),
    "blender": (4, 1, 0),
    "location": "File > Import",
    "description": "导入.mesh模型",
    "warning": "仅为学习，严禁商用！",
    "category": "Import-Export",
}

# 定义操作类
class ImportMeshOperator(bpy.types.Operator):
    """Import a .mesh file"""
    bl_idname = "import.game_mesh"
    bl_label = "导入.mesh模型"
    bl_options = {'REGISTER', 'UNDO'}

    # 使用bpy.props定义文件路径属性
    filepath: bpy.props.StringProperty(
        subtype='FILE_PATH',
        default="",
    ) # type: ignore

    # 定义invoke方法来显示文件选择对话框
    def invoke(self, context, event):
        # 调用文件选择对话框
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}

    def execute(self, context):
        # 清除当前场景中的所有物体
        bpy.ops.object.select_all(action='SELECT')
        bpy.ops.object.delete()

        # 定义数据文件的路径
        file_path = self.filepath

        # 检查文件是否存在
        if not os.path.exists(file_path):
            self.report({'ERROR'}, "文件不存在，请检查路径是否正确")
            return {'CANCELLED'}

        # 创建一个新的网格
        mesh_name = "ImportedMesh"
        mesh = bpy.data.meshes.new(mesh_name)
        obj = bpy.data.objects.new(mesh_name, mesh)

        # 将对象添加到场景中
        context.collection.objects.link(obj)

        # 读取二进制文件
        with open(file_path, 'rb') as file:
            data = file.read()

        # 数据块的大小和起始位置
        data_start = 0x35
        data_size = 0x1CD8
        block_size = 0x34

        # 确保数据大小正确
        assert len(data) >= data_start + data_size, "Data size is less than expected."

        # 解析数据块
        vertices = []
        index = data_start
        # 0.309 0.450 0.029 0.0 2.152 -4.245 0.0 0.0078 0.0 1.119 2.069 3.056
        for i in range(data_start, data_start + data_size, block_size):
            # 读取每个块中的顶点数据
            if index + block_size <= len(data):
                vertex_data = data[index:index+12]  # 假设每个顶点数据占用12字节
                index += block_size
                
                # 解析顶点坐标
                x = struct.unpack('f', vertex_data[0:4])[0]
                y = struct.unpack('f', vertex_data[4:8])[0]
                z = struct.unpack('f', vertex_data[8:12])[0]
                
                vertices.append((x, y, z))

        # 创建顶点
        mesh.from_pydata(vertices, [], [])

        # 更新网格
        mesh.update()

        # 设置物体的位置
        obj.location = (0, 0, 0)

        self.report({'INFO'}, "模型加载成功")
        return {'FINISHED'}

def menu_func_import(self, context):
    self.layout.operator(ImportMeshOperator.bl_idname, text="导入模型 (.mesh)")

# 注册和注销函数
def register():
    bpy.utils.register_class(ImportMeshOperator)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)

def unregister():
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)
    bpy.utils.unregister_class(ImportMeshOperator)

# 注册插件
if __name__ == "__main__":
    register()