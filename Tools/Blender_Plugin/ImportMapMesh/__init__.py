import bpy
import struct
import os

# 插件元数据
bl_info = {
    "name": "导入.mesh地图模型",
    "author": "letleon",
    "description": "导入.mesh地图模型",
    "blender": (4, 1, 0),
    "version": (0, 1),
    "location": "File > Import",
    "warning": "仅为学习，严禁商用！",
    "category": "Import-Export",
}
def read_mesh_head_info(data,start_index):
    """读取头部信息"""
    print("开始读取头部信息")

    # 文件中包含网格物体数量(仅头一个文件有用)
    mesh_obj_number = struct.unpack("<I", data[start_index : start_index + 4])[0]
    # 本网格变换矩阵数量
    mesh_matrices_number = struct.unpack("<I", data[start_index + 8 : start_index + 12])[0]
    # 4个字节00标志
    # zeros_tag = struct.unpack("<I", data[36:40])[0]
    # 1个字节01标志
    # zeroone_tag = struct.unpack("<B", data[48:49])[0]
    # 本网格数据大小
    mesh_byte_size = struct.unpack("<I", data[start_index + 25 : start_index + 29])[0]

    # 打印头部信息
    print(f"网格物体数量: {mesh_obj_number} 本网格变换矩阵数量: {mesh_matrices_number} 本网格数据大小: {mesh_byte_size}")

    # 返回头部信息
    return (mesh_obj_number, mesh_matrices_number, mesh_byte_size)

def get_shader_animation_texture(data,new_data_index):
    zeroone_tag = struct.unpack("<B", data[new_data_index:new_data_index+1])[0]
    print(f"0x01标志: {zeroone_tag}")
    if zeroone_tag != 1:
        print("0x01标志错误")
        return
    
    # tag_03 = struct.unpack("<B", data[new_data_index:new_data_index+1])[0]

def read_vertex_data(vertex_data,mesh_matrices_number,mesh_byte_size):
    """读取顶点数据"""
    # 顶点数据
    this_matrices = []
    # 数据块的大小 (0x34)
    block_size = int(mesh_byte_size / mesh_matrices_number)
    print(f"数据块的大小: {block_size}")

    # 解析顶点数据
    for mni in range(mesh_matrices_number):
        # 解析顶点坐标
        mniv = block_size * mni
        vx = struct.unpack("f", vertex_data[mniv : mniv + 4])[0]
        vy = struct.unpack("f", vertex_data[mniv + 4 : mniv + 8])[0]
        vz = struct.unpack("f", vertex_data[mniv + 8 : mniv + 12])[0]
        # print(f"顶点坐标: {vx} {vy} {vz}")
        # 将顶点添加到顶点列表
        this_matrices.append((vx, vy, vz))

    print(f"Mesh顶点数据读取完成: {mni} this_matrices len:{len(this_matrices)}")
           
    return this_matrices

def split_mesh(data):
    """分割网格数据"""

    # 数据起始位置
    data_start = 0
    # 网格对象
    mesh_obj = []
    # 循环条件
    continue_flag = True
    # 循环计数
    c_i = 0

    try:
        while continue_flag:
            print(f"循环次数: {c_i}")
            # 读取头部信息
            # 文件中包含网格物体数量, 本网格变换矩阵数量, 本网格数据大小
            mesh_obj_number, mesh_matrices_number, mesh_byte_size = read_mesh_head_info(data,data_start)
            # 读取顶点数据
            vertex_data = data[data_start + 0x1d : data_start + 0x1d + mesh_byte_size]
            print("vertex_data legth:",len(vertex_data))
            this_matrices = read_vertex_data(vertex_data,mesh_matrices_number,mesh_byte_size)


            # 读取顶点索引数据大小
            vertex_index_data_size = struct.unpack("<I", data[data_start + 0x1d + mesh_byte_size : data_start + 0x1d + mesh_byte_size + 4])[0]
            print(f"读取顶点索引数据大小: {vertex_index_data_size}")
            # 读取顶点索引数据
            vertex_index_data = data[data_start + 0x1d + mesh_byte_size + 4 : data_start + 0x1d + mesh_byte_size + 4 + vertex_index_data_size]
            print(f"读取顶点索引数据: {len(vertex_index_data)}")
            # 初始化 mesh_obj[c_i]
            mesh_obj.append({
                "vertex": {
                    "mesh_obj_number": mesh_obj_number, 
                    "mesh_matrices_number": mesh_matrices_number, 
                    "mesh_byte_size": mesh_byte_size,
                    "data": this_matrices
                },
                "vertexindex":{
                    "size":vertex_index_data_size,
                    "data": vertex_index_data
                }
            })

            # 结束位置,也是新的开始
            data_start += 0x1d + mesh_byte_size + 4 + vertex_index_data_size
            print("data_start:",data_start)

            # 循环计数+1
            c_i += 1
            print("已循环次数:",c_i)
            if c_i >= mesh_obj[0]["vertex"]["mesh_obj_number"] - 1:
                print("数据到达尾部")
                continue_flag = False

        return mesh_obj
    except Exception as e:
        print("循环结束:",e)
        return mesh_obj



# 定义操作类
class ImportMapMeshClass(bpy.types.Operator):
    """Import a .mesh file"""

    bl_idname = "import.game_map_mesh"
    bl_label = "导入.mesh地图模型"
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
        # mesh = bpy.data.meshes.new(mesh_name)
        # obj = bpy.data.objects.new(mesh_name, mesh)

        # # 将对象添加到场景中
        # context.collection.objects.link(obj)

        # 读取二进制文件
        data = None
        with open(file_path, "rb") as file:
            data = file.read()

        # print(f"文件大小: {data[0:4]}")

        # mesh_obj.append({
        #     "vertex": {
        #         "mesh_obj_number": mesh_obj_number, 
        #         "mesh_matrices_number": mesh_matrices_number, 
        #         "mesh_byte_size": mesh_byte_size,
        #         "data": vertex_data
        #     },
        #     "vertexindex":{
        #         "size":vertex_index_data_size,
        #         "data": vertex_index_data
        #     }
        # })

        # 分割网格数据
        mesh_obj = split_mesh(data)

        # 循环索引
        for_i = 0
        # 读取数据块
        for thsi_vertex in mesh_obj:
            # 读取顶点数据
            vertices = thsi_vertex["vertex"]["data"]

            # 创建新网格
            new_mesh = bpy.data.meshes.new(mesh_name + str(for_i))
            new_obj = bpy.data.objects.new(mesh_name, new_mesh)

            # 将对象添加到场景中
            context.collection.objects.link(new_obj)

            # 创建顶点 点线面
            new_mesh.from_pydata(vertices, [], [])

            # 更新网格
            new_mesh.update()

            # 设置物体的位置
            new_obj.location = (0, 0, 0)
            
            # 循环索引+1
            for_i += 1



        # 解析数据块
        # vertices = []


        # # 创建顶点
        # mesh.from_pydata(vertices[0], [], [])

        # # 更新网格
        # mesh.update()

        # # 设置物体的位置
        # obj.location = (0, 0, 0)

        self.report({"INFO"}, "模型加载成功")
        return {"FINISHED"}


def menu_func_import(self, context):
    self.layout.operator(ImportMapMeshClass.bl_idname, text="导入地图模型 (.mesh)")


# 注册和注销函数
def register():
    bpy.utils.register_class(ImportMapMeshClass)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)


def unregister():
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)
    bpy.utils.unregister_class(ImportMapMeshClass)


# 注册插件
if __name__ == "__main__":
    register()

