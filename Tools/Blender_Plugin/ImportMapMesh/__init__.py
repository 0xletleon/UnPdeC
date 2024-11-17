import math
import traceback
import bpy
import struct
import os

# 插件元数据
bl_info = {
    "name": "导入.mesh地图模型",
    "author": "letleon",
    "description": "导入.mesh地图模型",
    "blender": (4, 1, 0),
    "version": (0, 2),
    "location": "File > Import",
    "warning": "仅为学习，严禁商用！",
    "category": "Import-Export",
}


# 定义读取头部信息函数
def read_mesh_head_info(self,data, start_index):
    """读取头部信息"""
    print(">>> 开始读取头部信息")

    # 检查是否有足够的字节数进行解包
    if len(data) < start_index + 29:
        print(f"! 头部信息解析失败: 不足的字节数在偏移量 {start_index}")
        self.report({"ERROR"}, "头部信息解析失败")
        # 打印堆栈跟踪信息
        traceback.print_exc()
        return {"CANCELLED"}

    # 文件中包含网格物体数量(仅头一个文件有用)
    mesh_obj_number = struct.unpack_from("<I", data, start_index)[0]
    # 本网格变换矩阵数量
    mesh_matrices_number = struct.unpack_from("<I", data, start_index + 8)[0]
    # 4个字节00标志（注释掉）
    # zeros_tag = struct.unpack_from("<I", data, 36)[0]
    # 1个字节01标志（注释掉）
    # zeroone_tag = struct.unpack_from("<B", data, 48)[0]
    # 本网格数据大小
    mesh_byte_size = struct.unpack_from("<I", data, start_index + 25)[0]

    # 打印头部信息
    print(f"<<< 网格物体数量: {mesh_obj_number} 本网格变换矩阵数量: {mesh_matrices_number} 本网格数据大小: {mesh_byte_size}")

    # 返回头部信息
    return mesh_obj_number, mesh_matrices_number, mesh_byte_size


# 定义解析顶点数据函数
def read_vertex_value(self,vertex_data_block, mesh_matrices_number, mesh_byte_size):
    """解析顶点数据"""
    print(">>> 开始解析顶点数据")
    # 顶点数据
    this_vertexs = []
    # 数据块的大小 (0x34)
    block_size = int(mesh_byte_size / mesh_matrices_number)
    if block_size <= 0:
        print(f"! 数据块的大小计算失败: {block_size}")
        self.report({"ERROR"}, "数据块的大小计算失败")
        # 打印堆栈跟踪信息
        traceback.print_exc()
        return {"CANCELLED"}

    print(f"> 数据块的大小: {block_size}")

    # 解析顶点数据
    try:
        for mni in range(mesh_matrices_number):
            # 计算当前块的起始位置
            mniv = block_size * mni
            # 确保有足够的字节进行解包
            if mniv + 12 <= mesh_byte_size:
                vx = struct.unpack_from("f", vertex_data_block, mniv)[0]
                vy = struct.unpack_from("f", vertex_data_block, mniv + 4)[0]
                vz = struct.unpack_from("f", vertex_data_block, mniv + 8)[0]
                # 将顶点添加到顶点列表
                this_vertexs.append((vx, vy, vz))
            else:
                print(f"! 顶点数据解析失败: 不足的字节数在偏移量 {mniv}")
                break
    except Exception as e:
        print(f"! 顶点数据解析失败: {e}")
        self.report({"ERROR"}, f"顶点数据解析失败 : {e}")
        # 打印堆栈跟踪信息
        traceback.print_exc()
        return {"CANCELLED"}

    print(f"<<< 顶点数据解析完成: {len(this_vertexs)} 组")

    return this_vertexs


# 定义解析顶点索引数据函数
def read_vertex_index_value(self,vertex_index_data_block, index_length):
    """解析顶点索引数据"""
    print(f">>> 开始解析顶点索引数据 {index_length}")
    vertex_indices = []
    try:
        # todo: 这里可能不对！！！有空再搞！
        for i in range(0, index_length - 7, 8):  # 确保有足够的字节进行解包
            e0 = struct.unpack_from("H", vertex_index_data_block, i)[0]
            e1 = struct.unpack_from("H", vertex_index_data_block, i + 4)[0]
            vertex_indices.append((e0, e1))
            # print(f"> 解析顶点索引: {e0} {e1}")
    except Exception as e:
        print(f"! 顶点索引数据解析失败: {e}")
        self.report({"ERROR"}, f"顶点索引数据解析失败 : {e}")
        # 打印堆栈跟踪信息
        traceback.print_exc()
        return {"CANCELLED"}

    print(f"<<< 顶点索引数据读取完毕: {len(vertex_indices)} 组")
    
    return vertex_indices


# 定义分割网格数据函数
def split_mesh(self,data):
    """分割网格数据"""
    print(">>> 开始分割网格数据")
    # 数据起始位置
    data_start = 0
    # 网格对象
    mesh_obj = []
    try:
        while True:
            # 读取头部信息
            mesh_heads = read_mesh_head_info(self,data,data_start)
            # 文件中包含网格物体数量, 本网格变换矩阵数量, 本网格数据大小
            mesh_obj_number, mesh_matrices_number, mesh_byte_size = mesh_heads
            
            # 获取顶点索引数据长度
            vertex_data_len = data[data_start + 0x1d : data_start + 0x1d + mesh_byte_size]
            print("> 获取顶点索引数据长度:",len(vertex_data_len))
            if len(vertex_data_len) <= 0:
                print("! 获取顶点索引数据长度失败")
                self.report({"ERROR"}, "获取顶点索引数据长度失败")
                # 打印堆栈跟踪信息
                traceback.print_exc()
                return {"CANCELLED"}
                # break
            
            # 解析顶点数据块
            this_vertexs = read_vertex_value(self,vertex_data_len,mesh_matrices_number,mesh_byte_size)
            
            # 读取顶点索引大小
            vertex_index_data_size = struct.unpack("<I", data[data_start + 0x1d + mesh_byte_size : data_start + 0x1d + mesh_byte_size + 4])[0]
            print(f"> 读取顶点索引大小: {vertex_index_data_size}")
            # 获取顶点索引数据块
            vertex_index_data_block = data[data_start + 0x1d + mesh_byte_size + 4 : data_start + 0x1d + mesh_byte_size + 4 + vertex_index_data_size]
            print(f"> 索引地址: {hex(data_start + 0x1d + mesh_byte_size + 4)}")
            print(f"> 获取顶点索引数据块: {len(vertex_index_data_block)}")
            # 解析顶点索引数据块
            this_vertex_indexs = read_vertex_index_value(self,vertex_index_data_block,len(vertex_index_data_block))

            # 向mesh_obj中添加数据
            mesh_obj.append({
                "vertex": {
                    "mesh_obj_number": mesh_obj_number,
                    "mesh_matrices_number": mesh_matrices_number,
                    "mesh_byte_size": mesh_byte_size,
                    "data": this_vertexs
                },
                "vertexindex":{
                    "size":vertex_index_data_size,
                    "data": this_vertex_indexs
                }
            })

            # 结束位置,也是新的开始
            data_start += 0x1d + mesh_byte_size + 4 + vertex_index_data_size
            print("> data_start:",data_start)

            # 检查是否到达文件末尾
            if len(mesh_obj) >= mesh_obj[0]["vertex"]["mesh_obj_number"] - 1:
                print("<<< 数据到达尾部")
                break

        return mesh_obj
    except Exception as e:
        print("! 分割网格数据失败:",e)
        self.report({"ERROR"}, f"分割网格数据失败: {e}")
        # 打印堆栈跟踪信息
        traceback.print_exc()
        return {"CANCELLED"}

# 定义操作类
class ImportMapMeshClass(bpy.types.Operator):
    """Import a .mesh file"""
    bl_idname = "import.game_map_mesh"
    bl_label = "导入.mesh地图模型"
    bl_options = {"REGISTER", "UNDO"}
    # 使用bpy.props定义文件路径属性
    filepath: bpy.props.StringProperty(subtype="FILE_PATH",default="") # type: ignore
    # 文件扩展名过滤
    filename_ext = ".mesh"
    filter_glob: bpy.props.StringProperty(default="*.mesh",options={'HIDDEN'}) # type: ignore

    def execute(self, context):
        # 清除当前场景中的所有物体
        # bpy.ops.object.select_all(action="SELECT")
        # bpy.ops.object.delete()

        try:
            # 定义数据文件的路径
            file_path = self.filepath
            # 检查文件是否存在
            if not os.path.exists(file_path):
                self.report({"ERROR"}, "文件不存在，请检查路径是否正确")
                return {"CANCELLED"}

            # 读取二进制文件
            data = None
            with open(file_path, "rb") as file:
                data = file.read()

            # 获得文件名(不带后缀)
            mesh_name = os.path.splitext(os.path.basename(file_path))[0]

            # 分割网格数据
            mesh_obj = split_mesh(self,data)

            # 循环索引
            idx = 0
            # 读取数据块
            for thsi_vertex in mesh_obj:
                # 读取顶点数据
                vertices = thsi_vertex["vertex"]["data"]
                # 读取顶点索引数据
                vertex_index = thsi_vertex["vertexindex"]["data"]

                # 创建新网格
                new_mesh = bpy.data.meshes.new(f"{mesh_name}_{idx}")
                new_obj = bpy.data.objects.new(f"{mesh_name}_{idx}", new_mesh)

                # 将对象添加到场景中
                context.collection.objects.link(new_obj)

                # 创建顶点 点, 线, 面
                new_mesh.from_pydata(vertices, vertex_index, [])

                # 更新网格
                new_mesh.update()

                # 设置物体的位置
                new_obj.location = (0, 0, 0)
                # 首先，设置旋转模式为欧拉角
                new_obj.rotation_mode = 'XYZ'
                # 设置X轴的旋转值为90度（转换为弧度）
                new_obj.rotation_euler = (math.radians(90), 0, 0)

                # 循环索引+1
                idx += 1

            self.report({"INFO"}, "模型加载成功")
            return {"FINISHED"}
        except Exception as e:
            self.report({"ERROR"}, f"地图模型加载失败: {e}")
            # 打印堆栈跟踪信息
            traceback.print_exc()
            return {"CANCELLED"}

    # 定义invoke方法来显示文件选择对话框
    def invoke(self, context, event):
        # 调用文件选择对话框
        context.window_manager.fileselect_add(self)
        return {"RUNNING_MODAL"}

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
