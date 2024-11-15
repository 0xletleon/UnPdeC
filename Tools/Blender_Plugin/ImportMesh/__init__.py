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


def read_mesh_head_info(data):
    """读取头部信息"""
    print("开始读取头部信息")

    # 文件中包含网格物体数量
    mesh_obj_number = struct.unpack("<I", data[24:28])[0]
    # 本网格变换矩阵数量
    mesh_matrices_number = struct.unpack("<I", data[32:36])[0]
    # 4个字节00标志
    zeros_tag = struct.unpack("<I", data[36:40])[0]
    # 1个字节01标志
    zeroone_tag = struct.unpack("<B", data[48:49])[0]
    # 本网格数据大小
    mesh_byte_size = struct.unpack("<I", data[49:53])[0]

    # 打印头部信息
    print(
        f"网格物体数量: {mesh_obj_number} 本网格变换矩阵数量: {mesh_matrices_number} 4个字节00标志: {zeros_tag} 1个字节01标志: {zeroone_tag} 本网格数据大小: {mesh_byte_size}"
    )

    # 返回头部信息
    return (mesh_obj_number, mesh_matrices_number, mesh_byte_size)


# 定义操作类
class ImportMeshClass(bpy.types.Operator):
    """Import a .mesh file"""

    bl_idname = "import.game_mesh"
    bl_label = "导入.mesh模型"
    bl_options = {"REGISTER", "UNDO"}

    # 使用bpy.props定义文件路径属性
    filepath: bpy.props.StringProperty(
        subtype="FILE_PATH",
        default="",
    )  # type: ignore

    # 定义invoke方法来显示文件选择对话框
    def invoke(self, context, event):
        # 调用文件选择对话框
        context.window_manager.fileselect_add(self)
        return {"RUNNING_MODAL"}

    def execute(self, context):
        # 清除当前场景中的所有物体
        bpy.ops.object.select_all(action="SELECT")
        bpy.ops.object.delete()

        # 定义数据文件的路径
        file_path = self.filepath

        # 检查文件是否存在
        if not os.path.exists(file_path):
            self.report({"ERROR"}, "文件不存在，请检查路径是否正确")
            return {"CANCELLED"}

        # 创建一个新的网格
        mesh_name = os.path.splitext(os.path.basename(file_path))[0]
        # mesh_name = "ImportedMesh"
        mesh = bpy.data.meshes.new(mesh_name)
        obj = bpy.data.objects.new(mesh_name, mesh)

        # 将对象添加到场景中
        context.collection.objects.link(obj)

        # 读取二进制文件
        data = None
        with open(file_path, "rb") as file:
            data = file.read()

        print(f"文件大小: {data[0:4]}")

        # 读取头部信息
        # 文件中包含网格物体数量, 本网格变换矩阵数量, 本网格数据大小
        mesh_obj_number, mesh_matrices_number, mesh_byte_size = read_mesh_head_info(
            data
        )

        # 数据起始位置
        data_start = 0x35
        # 数据块的大小 (0x34)
        block_size = int(mesh_byte_size / mesh_matrices_number)
        print(f"数据块的大小: {block_size}")

        # 物体变换矩阵数据
        mesh_matrices = []

        # 开始依次读取物体信息
        for i in range(mesh_obj_number):
            print(f"数据i: {i}")
            this_mesh_data = data[data_start : data_start + mesh_byte_size]
            print(f"数据块大小: {len(this_mesh_data)}")

            # 初始化mesh_matrices[i]
            mesh_matrices.append([])

            # 当前mesh索引
            this_mesh_index = 0
            for ti in range(mesh_matrices_number):
                # print(f"数据块ti: {ti} this_mesh_index: {this_mesh_index}")

                # 解析顶点坐标
                x = struct.unpack(
                    "f", this_mesh_data[this_mesh_index + 0 : this_mesh_index + 4]
                )[0]
                y = struct.unpack(
                    "f", this_mesh_data[this_mesh_index + 4 : this_mesh_index + 8]
                )[0]
                z = struct.unpack(
                    "f", this_mesh_data[this_mesh_index + 8 : this_mesh_index + 12]
                )[0]

                # print(f"顶点xyz: {x} {y} {z}")
                # 将顶点添加到顶点列表
                mesh_matrices[i].append((x, y, z))
                # 增加索引
                this_mesh_index += block_size
            
            # 
            
        print(f"数据块大小: {len(mesh_matrices[0])}")

        # 解析数据块
        vertices = []
        index = data_start
        # 0.309 0.450 0.029 0.0 2.152 -4.245 0.0 0.0078 0.0 1.119 2.069 3.056
        # for i in range(data_start, data_start + data_size, block_size):
        #     # 读取每个块中的顶点数据
        #     if index + block_size <= len(data):
        #         vertex_data = data[index : index + 12]  # 假设每个顶点数据占用12字节
        #         index += block_size

        #         # 解析顶点坐标
        #         x = struct.unpack("f", vertex_data[0:4])[0]
        #         y = struct.unpack("f", vertex_data[4:8])[0]
        #         z = struct.unpack("f", vertex_data[8:12])[0]

        #         vertices.append((x, y, z))

        # 创建顶点
        mesh.from_pydata(mesh_matrices[0], [], [])

        # 更新网格
        mesh.update()

        # 设置物体的位置
        obj.location = (0, 0, 0)

        self.report({"INFO"}, "模型加载成功")
        return {"FINISHED"}


def menu_func_import(self, context):
    self.layout.operator(ImportMeshClass.bl_idname, text="导入模型 (.mesh)")


# 注册和注销函数
def register():
    bpy.utils.register_class(ImportMeshClass)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)


def unregister():
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)
    bpy.utils.unregister_class(ImportMeshClass)


# 注册插件
if __name__ == "__main__":
    register()
