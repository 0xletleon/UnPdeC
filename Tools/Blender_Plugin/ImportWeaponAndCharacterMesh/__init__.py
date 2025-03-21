import math
import traceback
import bpy
import struct
import os


def read_dynamic_head(self, data):
    """读取头部信息"""
    print(">>> 开始读取头部信息")

    try:
        # 网格信息
        mesh_info = []
        # 文件读取位置
        data_index = 0
        # 读取包含的对象数量1
        include_obj_number1 = struct.unpack_from("<I", data, data_index)[0]
        # 修正数据读取位置
        data_index += 4

        # 读取包含的对象名称
        for i in range(include_obj_number1):
            # 读取物体名称长度
            name_length = struct.unpack_from("<I", data, data_index)[0]
            # 修正数据读取位置
            data_index += 4
            # 读取物体名称
            obj_name = struct.unpack_from(f"<{name_length}s", data, data_index)[
                0
            ].decode("utf-8")
            print(f"对象名称: {obj_name}")
            # 保存名称
            mesh_info.append(obj_name)
            # 修正数据读取位置
            data_index += name_length

        # 读取包含的对象数量2
        include_obj_number2 = struct.unpack_from("<I", data, data_index)[0]
        # 修正数据读取位置
        data_index += 4

        # 检查头部数量是否一致
        if include_obj_number1 != include_obj_number2:
            print("! 头部信息解析失败: 包含的对象数量不一致")
            self.report({"ERROR"}, "头部信息解析失败")
            traceback.print_exc()
            return {"CANCELLED"}

        # 跳过物体的 初始位置和相机初始位置？这个不确定！
        skip_len = include_obj_number1 * 0x18
        print(f"data_index: {hex(data_index)}")

        # 修正数据读取位置
        data_index += skip_len

        print(f"data_index: {hex(data_index)} skip_len: {hex(skip_len)}")

        # 返回 物体开始位置，物体信息
        return data_index, mesh_info
    except Exception as e:
        print("! 读取头部信息失败:", e)
        self.report({"ERROR"}, "读取头部信息失败")
        traceback.print_exc()
        return {"CANCELLED"}


# 定义读取头部信息函数
def read_head(data, start_index):
    """读取头部信息"""
    print(">>> 开始读取头部信息:", hex(start_index))

    # 检查是否有足够的字节数进行解包
    if len(data) < start_index + 0x1D:
        print(f"! 头部信息解析失败: 不足的字节数在偏移量 {start_index}")
        traceback.print_exc()
        return None
        # self.report({"ERROR"}, "头部信息解析失败")
        # return {"CANCELLED"}

    print("start_index:", hex(start_index))
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

    # 返回文件中包含网格物体数量, 本物体面数据组数量, 本网格变换矩阵数量, 本网格字节总数
    return mesh_obj_number, mesh_face_group_number, mesh_matrices_number, mesh_byte_size


# 定义解析顶点数据函数
def read_vertices(self, vertices_data, mesh_matrices_number, mesh_byte_size):
    """解析顶点数据"""
    print(">>> 开始解析顶点数据")
    # 顶点数据
    vertices = []
    # UV 坐标数据
    uv_coords = []
    # 法线数据
    # normals = []
    # 切线数据
    tangents = []

    # 数据块的大小 (0x34 是一个猜测的大小，可能需要根据实际情况动态计算)
    block_size = int(mesh_byte_size / mesh_matrices_number)
    if block_size <= 0:
        print(f"! 数据块的大小计算失败: {block_size}")
        self.report({"ERROR"}, "数据块的大小计算失败")
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
        # traceback.print_exc()
        # return {"CANCELLED"}
        return None

    print(f"<<< 顶点数据解析完成: {hex(len(vertices))} 组")
    # 返回 顶点数据, UV坐标数据, 切线数据
    return vertices, uv_coords, tangents


# 定义解析面数据函数
def read_faces(self, faces_data_block, index_length):
    """解析面数据"""
    print(f">>> 开始解析面数据 {hex(index_length)}")
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

    print(f"<<< 面数据读取完毕: {hex(len(faces))} 组")

    return faces


# 定义分割网格数据函数
def split_mesh(self, data):
    """分割网格数据"""
    print(">>> 开始分割网格数据")

    # 数据起始位置
    data_start = 0
    # 是否为首次读取
    # first_read = True
    # 网格对象
    mesh_obj = []

    # 读取动态头部
    data_index, mesh_info = read_dynamic_head(self, data)
    # 修正数据起始位置
    data_start = data_index
    print("> fix数据起始位置:", hex(data_start))

    try:
        for mi_name in mesh_info:
            print(f">>> 读取网格信息名称: {mi_name}")
            # if first_read:
            #     data_start += 24
            #     first_read = False

            # 读取头部信息
            read_head_temp = read_head(data, data_start)

            if read_head_temp is None:
                print("! 读取头部信息失败")
                # return mesh_obj
                break

            # 返回文件中包含网格物体数量, 本物体面数据组数量, 本网格变换矩阵数量, 本网格字节总数
            (
                mesh_obj_number,
                mesh_face_group_number,
                mesh_matrices_number,
                mesh_byte_size,
            ) = read_head_temp

            # 获取顶点数据长度
            vertices_data = data[data_start + 0x1D : data_start + 0x1D + mesh_byte_size]
            print("> 获取顶点数据长度:", hex(len(vertices_data)))
            if len(vertices_data) <= 0:
                print("! 获取顶点数据长度失败")
                # self.report({"ERROR"}, "获取顶点数据长度失败")
                # traceback.print_exc()
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
                    "name": str(mi_name),
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

            # 检查是否到达文件末尾
            if len(mesh_obj) >= mesh_obj[0]["vertices"]["mesh_obj_number"] - 1:
                print("<<< 数据到达尾部")
                break

        return mesh_obj
    except Exception as e:
        print("! 分割网格数据失败:", e)
        # self.report({"ERROR"}, f"分割网格数据失败: {e}")
        traceback.print_exc()
        # return {"CANCELLED"}
        return mesh_obj


# 定义操作类
class ImporWeaponAndCharacterMeshClass(bpy.types.Operator):
    """Import a .mesh file"""

    bl_idname = "import.game_weapon_and_character_mesh"
    bl_label = "导入武器和人物模型(.mesh)"
    bl_options = {"REGISTER", "UNDO"}
    # 使用bpy.props定义文件路径属性
    filepath: bpy.props.StringProperty(subtype="FILE_PATH", default="")  # type: ignore
    # 文件扩展名过滤
    filename_ext = ".mesh"
    filter_glob: bpy.props.StringProperty(default="*.mesh", options={"HIDDEN"})  # type: ignore

    # 定义invoke方法来显示文件选择对话框
    def invoke(self, context, event):
        # 调用文件选择对话框
        context.window_manager.fileselect_add(self)
        return {"RUNNING_MODAL"}

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
            # data = None
            with open(file_path, "rb") as file:
                data = file.read()

            # 获得文件名(不带后缀)
            mesh_name = os.path.splitext(os.path.basename(file_path))[0]
            print(f"<<< 读取到的模型名称: {mesh_name}")

            # 分割网格数据
            mesh_obj = split_mesh(self, data)

            # 循环索引
            idx = 0
            # 读取数据块
            for mesh_item in mesh_obj:
                # 物体名称
                obj_name = mesh_item["name"]
                # 读取顶点数据
                vertices = mesh_item["vertices"]["data"]
                # 读取面数据
                faces = mesh_item["faces"]["data"]

                # 读取UV坐标
                # uv_coords = this_obj["vertices"]["uv_coords"]
                # 读取切线
                # tangents = this_obj["vertices"]["tangents"]

                # 创建新网格
                new_mesh = bpy.data.meshes.new(f"{mesh_name}_{obj_name}_{idx}")
                new_obj = bpy.data.objects.new(
                    f"{mesh_name}_{obj_name}_{idx}", new_mesh
                )

                # 将对象添加到场景中
                context.collection.objects.link(new_obj)

                # 创建顶点 点, 线, 面
                new_mesh.from_pydata(vertices, [], faces)

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
            self.report({"ERROR"}, f"模型加载失败: {e}")
            traceback.print_exc()
            return {"CANCELLED"}


def menu_func_import(self, context):
    self.layout.operator(
        ImporWeaponAndCharacterMeshClass.bl_idname, text="导入武器和人物模型(.mesh)"
    )


# 注册和注销函数
def register():
    bpy.utils.register_class(ImporWeaponAndCharacterMeshClass)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)


def unregister():
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)
    bpy.utils.unregister_class(ImporWeaponAndCharacterMeshClass)


# 注册插件
if __name__ == "__main__":
    register()
