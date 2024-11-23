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
    "version": (0, 3),
    "location": "File > Import",
    "warning": "仅为学习，严禁商用！",
    "category": "Import-Export",
}


def find_next_head(data, data_start):
    """查找下一个物体头部"""
    # 记录传入的 data_start
    temp_data_start = data_start
    print(">>>>>>>>>>>>>>>>>>>>>>>> :", hex(data_start))
    data_len = len(data)
    print("DATA大小:", hex(data_len))

    while True:
        if data_start >= data_len:
            print(
                "<<<<<<<<<<<<<<<<<<<<<<<!!! 没有找到下一个物体头部,开始查找地址: ",
                hex(temp_data_start),
            )
            return None

        # 读取下一个值
        # print("data_start:",hex(data_start))
        tag_ff = data[data_start : data_start + 1][0]
        # print(f"tag_ff: {hex(tag_ff)}")
        # data_start += 1
        if tag_ff == 0xFF:
            if data_start + 4 < len(data):  # 确保有足够的数据来解包一个整数
                tag_4ff = struct.unpack_from("<I", data, data_start)[0]
                # print("tag_4ff:",hex(tag_4ff))
                if tag_4ff != 0xFFFFFFFF:
                    data_start += 1
                    # print("!= 0xffffffff : ",hex(data_start))
                else:
                    # print(f"tag_4ff: {hex(tag_4ff)}")
                    data_start -= 0x30 + 0x1D
                    print(
                        "<<<<<<<<<<<<<<<<<<<<<<< 找到下一个物体第一个变换矩阵结尾",
                        hex(data_start),
                    )
                    return data_start
            else:
                print("数据不足，无法读取")
                return None  # 或者根据实际情况返回合适的值
        else:
            data_start += 1
            # print("!= 0xff : ",hex(data_start))


def read_map_first_head(self, data):
    """读取地图第一个头部信息"""
    print(">>> 开始读取地图第一个头部信息")
    # 读取位置
    data_index = 0

    # 确保有足够的数据 216CC5 287390
    # 29DEBC
    # 衔接地址 1BA288
    if len(data) < 24:
        print("Error: Not enough data")
        return

    # 读取相机位置?
    x1, y1, z1, x2, y2, z2 = struct.unpack_from("<ffffff", data)
    print(f"Camera 1 Position: x1={x1}, y1={y1}, z1={z1}")
    print(f"Camera 2 Position: x2={x2}, y2={y2}, z2={z2}")

    # 更新读取位置
    data_index += 24

    return data_index


# 定义读取头部信息函数
def read_head(self, data, start_index):
    """读取头部信息"""
    print(">>> 开始读取头部信息")

    # 检查是否有足够的字节数进行解包
    if len(data) < start_index + 29:
        print(f"! 头部信息解析失败: 不足的字节数在偏移量 {start_index}")
        # self.report({"ERROR"}, "头部信息解析失败")
        traceback.print_exc()
        # return {"CANCELLED"}
        return None

    # 文件中包含网格物体数量(仅头一个文件有用)
    mesh_obj_number = struct.unpack_from("<I", data, start_index)[0]
    # 本物体面数据组数量
    mesh_face_group_number = struct.unpack_from("<I", data, start_index + 4)[0]
    # 本网格变换矩阵数量
    mesh_matrices_number = struct.unpack_from("<I", data, start_index + 8)[0]
    # 4个字节00标志（注释掉）
    # zeros_tag = struct.unpack_from("<I", data, 36)[0]
    # 1个字节01标志（注释掉）
    # zeroone_tag = struct.unpack_from("<B", data, 48)[0]
    # 本网格字节总数
    mesh_byte_size = struct.unpack_from("<I", data, start_index + 25)[0]

    # 打印头部信息
    print(
        f"<<< 网格物体数量: {hex(mesh_obj_number)} 本物体面数据组数量 {hex(mesh_face_group_number)} 本网格变换矩阵数量: {hex(mesh_matrices_number)} 本网格字节总数: {hex(mesh_byte_size)}"
    )

    # 返回文件中包含网格物体数量, 本网格变换矩阵数量, 本网格字节总数
    return mesh_obj_number, mesh_matrices_number, mesh_byte_size


# 定义解析顶点数据函数
def read_vertices(self, vertices_data, mesh_matrices_number, mesh_byte_size):
    """解析顶点数据"""
    print(">>> 开始解析顶点数据")
    # 顶点数据
    vertices = []
    # UV 坐标数据
    uv_coords = []
    # 法线数据
    normals = []
    # 切线数据
    tangents = []

    # 数据块的大小 (0x34)
    block_size = int(mesh_byte_size / mesh_matrices_number)
    if block_size != 52:
        print(f"! 数据块的大小计算失败: {block_size}")
        # self.report({"ERROR"}, "数据块的大小计算失败")
        traceback.print_exc()
        # return {"CANCELLED"}
        return None

    print(f"> 数据块的大小: {hex(block_size)}")

    # 解析顶点数据
    try:
        for mni in range(mesh_matrices_number):
            # 计算当前块的起始位置
            mniv = block_size * mni
            # 确保有足够的字节进行解包
            if mniv + 12 <= mesh_byte_size:
                vx = struct.unpack_from("f", vertices_data, mniv)[0]
                vy = struct.unpack_from("f", vertices_data, mniv + 4)[0]
                vz = struct.unpack_from("f", vertices_data, mniv + 8)[0]
                # 将顶点添加到顶点列表
                vertices.append((vx, vy, vz))

                # 读取UV坐标 !实验
                # u = struct.unpack_from("f", vertices_data, mniv + 0x1C)[0]
                # v = struct.unpack_from("f", vertices_data, mniv + 0x28)[0]
                # uv_coords.append((u, v))

                # 读取切线 !实验
                # tx = struct.unpack_from("f", vertices_data, mniv + 0x24)[0]
                # ty = struct.unpack_from("f", vertices_data, mniv + 0x28)[0]
                # tz = struct.unpack_from("f", vertices_data, mniv + 0x2C)[0]
                # tw = struct.unpack_from("f", vertices_data, mniv + 0x30)[0]
                # tangents.append((tx, ty, tz, tw))
            else:
                print(f"! 顶点数据解析失败: 不足的字节数在偏移量 {mniv}")
                break
    except Exception as e:
        print(f"! 顶点数据解析失败: {e}")
        # self.report({"ERROR"}, f"顶点数据解析失败 : {e}")
        traceback.print_exc()
        # return {"CANCELLED"}
        return None

    print(f"<<< 顶点数据解析完成: {len(vertices)} 组")

    return vertices, uv_coords, tangents


# 定义解析面数据函数
def read_faces(self, faces_data_block, index_length):
    """解析面数据"""
    print(f">>> 开始解析面数据 {index_length}")
    faces = []
    try:
        # 确保有足够的字节进行解包
        for i in range(0, index_length, 12):
            f0 = struct.unpack_from("H", faces_data_block, i)[0]
            f1 = struct.unpack_from("H", faces_data_block, i + 4)[0]
            f2 = struct.unpack_from("H", faces_data_block, i + 8)[0]
            faces.append((f0, f1, f2))
            # print(f"> 解析面: {i} -> {f0} {f1} {f2}")
    except Exception as e:
        print(f"! 面数据解析失败: {e}")
        # self.report({"ERROR"}, f"面数据解析失败 : {e}")
        traceback.print_exc()
        # return {"CANCELLED"}
        return None

    print(f"<<< 面数据读取完毕: {len(faces)} 组")

    return faces


# 定义分割网格数据函数
def split_mesh(self, data):
    """分割网格数据"""
    print(">>> 开始分割网格数据")
    # 数据起始位置
    data_start = 0
    # 网格对象
    mesh_obj = []

    # 读取动态头部
    data_index = read_map_first_head(self, data)
    # 修正数据起始位置
    data_start = data_index
    print("> fix数据起始位置:", hex(data_start))

    # 临时计数
    temp_num = 0

    try:
        while True:
            temp_num += 1

            # 读取头部信息
            read_head_temp = read_head(self, data, data_start)
            # 判断是否读取失败
            if read_head_temp is None:
                print("! 读取头部信息失败")
                # return mesh_obj
                break
            # 解析头部信息 -> 文件中包含网格物体数量, 本网格变换矩阵数量, 本网格字节总数
            mesh_obj_number, mesh_matrices_number, mesh_byte_size = read_head_temp

            # 获取顶点数据长度
            vertices_data = data[data_start + 0x1D : data_start + 0x1D + mesh_byte_size]
            print("> 获取顶点数据长度:", hex(len(vertices_data)))
            if len(vertices_data) <= 0:
                print("! 获取顶点数据长度失败")
                # self.report({"ERROR"}, "获取顶点数据长度失败")
                traceback.print_exc()
                # return {"CANCELLED"}
                break
            # 解析顶点数据块
            read_vertices_temp = read_vertices(
                self, vertices_data, mesh_matrices_number, mesh_byte_size
            )
            # 判断是否读取失败
            if read_vertices_temp is None:
                print("! 解析顶点数据失败")
                # return mesh_obj
                break
            # 顶点数据, UV坐标数据, 切线数据
            vertices_array, uv_coords, tangents = read_vertices_temp

            # 获取面数据块大小
            faces_data_size = struct.unpack(
                "<I",
                data[
                    data_start
                    + 0x1D
                    + mesh_byte_size : data_start
                    + 0x1D
                    + mesh_byte_size
                    + 4
                ],
            )[0]
            print(f"> 获取面数据块大小: {hex(faces_data_size)}")
            if faces_data_size >= len(data):
                print(f"! 获取面数据块失败,遇到还未识别的数据块！ 开始地址:{hex(data_start + 0x1D)} 偏移地址: hex(data_start + 0x1D + mesh_byte_size)")
                break
            # 获取面数据块
            faces_data_block = data[
                data_start
                + 0x1D
                + mesh_byte_size
                + 4 : data_start
                + 0x1D
                + mesh_byte_size
                + 4
                + faces_data_size
            ]
            print(f"> 索引地址: {hex(data_start + 0x1d + mesh_byte_size + 4)}")
            print(f"> 获取面数据块: {hex(len(faces_data_block))}")
            # 解析面数据块
            faces_array = read_faces(self, faces_data_block, len(faces_data_block))
            # 判断是否读取失败
            if faces_array is None:
                print("! 解析面数据失败")
                # return mesh_obj
                break

            # 向mesh_obj中添加数据
            mesh_obj.append(
                {
                    "vertices": {
                        "mesh_obj_number": mesh_obj_number,
                        "mesh_matrices_number": mesh_matrices_number,
                        "mesh_byte_size": mesh_byte_size,
                        "data": vertices_array,
                        "uv_coords": uv_coords,  # xx
                        "tangents": tangents,
                    },
                    "faces": {"size": faces_data_size, "data": faces_array},
                }
            )

            # 结束位置,也是新的开始
            data_start += 0x1D + mesh_byte_size + 4 + faces_data_size
            print("> data_start:", hex(data_start))

            # 读取剩余数据(着色器,贴图,动画等) -> 是否存在另一个物体, 另一个物体头部地址
            find_start = find_next_head(data, data_start)
            if find_start is None:
                print("! 未找到下一个物体头部地址, mesh_obj len:", hex(len(mesh_obj)))
                break
            data_start = find_start

            print("完成获取下一个物体头部地址！！！！！！！！！")
            # if not nextobj:
            #     print("! 读取其他物体数据失败")
            #     # self.report({"ERROR"}, "读取其他物体数据失败")
            #     traceback.print_exc()
            #     # return {"CANCELLED"}
            #     break
            # # 修正数据起始位置
            # data_start = next_data_start

            # 检查是否到达文件末尾
            if len(mesh_obj) >= mesh_obj[0]["vertices"]["mesh_obj_number"] - 1:
                print("<<< 数据到达尾部")
                break

        print("返回 mesh_obj！！！")
        return mesh_obj
    except Exception as e:
        print("! 分割网格数据失败:", e)
        # self.report({"ERROR"}, f"分割网格数据失败: {e}")
        traceback.print_exc()
        # return {"CANCELLED"}
        return mesh_obj


# 定义操作类
class ImportMapMeshClass(bpy.types.Operator):
    """Import a .mesh file"""

    bl_idname = "import.game_map_mesh"
    bl_label = "导入.mesh地图模型"
    bl_options = {"REGISTER", "UNDO"}
    # 使用bpy.props定义文件路径属性
    filepath: bpy.props.StringProperty(subtype="FILE_PATH", default="")  # type: ignore
    # 文件扩展名过滤
    filename_ext = ".mesh"
    filter_glob: bpy.props.StringProperty(default="*.mesh", options={"HIDDEN"})  # type: ignore

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
            mesh_obj = split_mesh(self, data)

            # 循环索引
            idx = 0
            # 读取数据块
            for this_obj in mesh_obj:
                # 读取顶点数据
                vertices = this_obj["vertices"]["data"]
                # 读取面数据
                faces = this_obj["faces"]["data"]

                # 读取UV坐标
                # uv_coords = this_obj["vertices"]["uv_coords"]
                # 读取切线
                # tangents = this_obj["vertices"]["tangents"]

                # 创建新网格
                new_mesh = bpy.data.meshes.new(f"{mesh_name}_{idx}")
                new_obj = bpy.data.objects.new(f"{mesh_name}_{idx}", new_mesh)

                # 将对象添加到场景中
                context.collection.objects.link(new_obj)

                # 创建顶点 点, 线, 面
                new_mesh.from_pydata(vertices, [], faces)

                # 设置UV坐标 !实验
                # if uv_coords:
                #     uv_layer = new_mesh.uv_layers.new(name="UVMap")
                #     for loop_idx, face in enumerate(new_mesh.loops):
                #         vert_idx = face.vertex_index
                #         u, v = uv_coords[vert_idx]
                #         uv_layer.data[loop_idx].uv = (u, v)

                # 设置切线 !实验
                # if tangents:
                #     # 创建切线层
                #     tangent_layer = new_mesh.attributes.new(name="Tangent", type='FLOAT_VECTOR', domain='CORNER')
                #     for loop_idx, face in enumerate(new_mesh.loops):
                #         tan = tangents[face.vertex_index]
                #         tangent_layer.data[loop_idx].vector = (tan[0], tan[1], tan[2])

                # 更新网格
                new_mesh.update()

                # 设置物体的位置
                new_obj.location = (0, 0, 0)
                # 首先，设置旋转模式为欧拉角
                new_obj.rotation_mode = "XYZ"
                # 设置X轴的旋转值为90度（转换为弧度）
                new_obj.rotation_euler = (math.radians(90), 0, 0)

                # 循环索引+1
                idx += 1

            self.report({"INFO"}, "模型加载成功")
            return {"FINISHED"}
        except Exception as e:
            self.report({"ERROR"}, f"地图模型加载失败: {e}")
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
