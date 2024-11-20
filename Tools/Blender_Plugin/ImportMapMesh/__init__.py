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

def read_shader_03(data,data_start,type_tag):
    """读取着色器数据"""
    print(">>> 开始读取着色器数据")

    # 读取着色器名称长度
    s_name_len = struct.unpack_from("<I", data, data_start)[0]
    print(f"1 着色器名字长度: {hex(s_name_len)}")
    # 此次不修改 data_start

    if s_name_len > 0:
        for type_tag_i in range(type_tag):
            # 读取着色器名称长度
            s_name_len = struct.unpack_from("<I", data, data_start)[0]
            print(f"2 着色器名字长度: {hex(s_name_len)}")
            data_start += 4
            print("DEBUG -> data_start:",hex(data_start))

            # 是ColorMap
            if s_name_len == 1:
                print("是ColorMap")
                # data_start += 4
                # print("DEBUG -> data_start:",hex(data_start))

                # 修正名称长度值
                s_name_len = struct.unpack_from("<I", data, data_start)[0]
                print(f"3 着色器名字长度: {hex(s_name_len)}")
                data_start += 4
                print("DEBUG -> data_start:",hex(data_start))
                
                # 读取着色器名称
                s_name = struct.unpack_from(f"<{s_name_len}s", data, data_start)[0].decode("utf-8")
                print(f"读取着色器名称: {s_name}")
                data_start += s_name_len
                print("DEBUG -> data_start:",hex(data_start))

                # 读取贴图地址长度
                c_dir_len = struct.unpack_from("<I", data, data_start)[0]
                print(f"读取贴图地址长度: {c_dir_len}")
                data_start += 4
                print("DEBUG -> data_start:",hex(data_start))

                # 读取贴图地址
                c_dir = struct.unpack_from(f"<{c_dir_len}s", data, data_start)[0].decode("utf-8")
                print(f"读取贴图地址: {c_dir}")
                data_start += c_dir_len
                print("DEBUG -> data_start:",hex(data_start))


                # # 读取下一个值
                # next_s_name_len = struct.unpack_from("<I", data, data_start)[0]
                # print(f"读取下一个值: {hex(next_s_name_len)}")

                # # 像是 动画控制？
                # if next_s_name_len == 1:
                #     print("next_s_name_len == 1")
                #     data_start += 8
                #     continue
            else:
                print("非ColorMap")
                # 读取着色器名称
                print("读取着色器：",hex(data_start))
                s_name = struct.unpack_from(f"<{s_name_len}s", data, data_start)[0].decode("utf-8")
                print(f"读取着色器名称: {s_name}")
                data_start += s_name_len
                print("DEBUG -> data_start:",hex(data_start))

                # 读取下一个值
                next_s_name_len = struct.unpack_from("<I", data, data_start)[0]
                print(f"读取下一个值: {hex(next_s_name_len)}")

                # 像是 动画控制？
                if next_s_name_len == 1:
                    print("next_s_name_len == 1")
                    data_start += 8
                    print("DEBUG -> data_start:",hex(data_start))

                    continue
    else:
        print("> 空03 + 12")
        data_start += 12
        print("DEBUG -> data_start:",hex(data_start))


    return data_start


def read_other_data(data,data_start,temp_num):
    """读取其他数据"""
    print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 开始读取其他数据 ",temp_num)
    # 是否存在另一个物体
    nextobj = False

    # 判断第一个字节是否为 0x01
    tag_1 = data[data_start:data_start + 1]
    data_start += 1
    print("DEBUG -> data_start:",hex(data_start))
    print(f"tag_1: {tag_1}")

    while True:
        new_01 = data[data_start:data_start + 1]
        if new_01 == b"\x01":
            newtype_tag = struct.unpack_from("<I", data, data_start + 1)[0]
            if newtype_tag == 67108864:
                print("找到新的物体头部开始地址:",data_start)
                break

        # 类型标签
        type_tag = struct.unpack_from("<I", data, data_start)[0]
        data_start += 4
        print(f"DEBUG -> data_start: {hex(data_start)} type_tag: {hex(type_tag)}")
        # print(f"data_start: {data_start} type_tag: {type_tag}")

        # 可以放外面
        if tag_1 == b'\x01':
            print(">>> tag_1 == 01  type_tag -> ",hex(type_tag))

            # == 3 是着色器数据
            if type_tag == 3:
                print(">>> type_tag == 3 : ",hex(data_start))

                # 类型标签
                next_type_tag = struct.unpack_from("<I", data, data_start)[0]
                if next_type_tag == type_tag:
                    print("结束,或开始新的类型数据了！")
                    data_start += 4
                    print("DEBUG -> data_start:",hex(data_start))
                    data_start = read_shader_03(data,data_start,type_tag)
                    
                    nextobj = True
                else:
                    print("正常类型内数据")
                    data_start = read_shader_03(data,data_start,type_tag)

                    nextobj = True


                # for idx in range(1024):
                #     # 读取下一个字节
                #     tag_next = data[data_start:data_start + 1]
                #     # 数据位置
                #     data_start += 1
                #     # print(f"tag_next: {tag_next}")
                #     if tag_next == b'\x01':
                #         test_0 = data[data_start:data_start + 1][0]
                #         # print(f"test_0: {test_0}")
                #         if test_0 != 0:
                #             print(">>> 是 0x01:",hex(data_start))
                #             print(">>> 找到标志地址")
                #             # 修正数据读取位置
                #             data_start -= 0x19
                #             # 是否存在另一个物体
                #             nextobj = True
                #             break
                    
                #     if idx == 1023:
                #         print(">>> 未找到下一个物体头部地址")
                #         break
                
                # print("找到下一个物体头部地址data_start:",hex(data_start))
            
            # == 4
            elif type_tag == 4:
                print(">>> tag_2 == 4: ",hex(data_start))
                data_start -= 5
            
            # == else
            # else:
                # print(f">>> tag_2!= else: {type_tag} data_start: {hex(data_start)}")
        else:
            print(">>> 非0x01 错误")


    # 返回 -> 是否存在另一个物体, 另一个物体头部地址
    print("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 结束读取其他数据: ",temp_num)
    return nextobj, data_start



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

def read_map_first_head(self, data):
    """读取地图第一个头部信息"""
    print(">>> 开始读取地图第一个头部信息")    
    # 读取位置
    data_index = 0

    # 确保有足够的数据
    if len(data) < 24:
        print("Error: Not enough data")
        return

    # 读取相机位置?
    x1, y1, z1, x2, y2, z2 = struct.unpack_from('<ffffff', data)
    print(f'Camera 1 Position: x1={x1}, y1={y1}, z1={z1}')
    print(f'Camera 2 Position: x2={x2}, y2={y2}, z2={z2}')

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
        # traceback.print_exc()
        # return {"CANCELLED"}
        return None

    # 文件中包含网格物体数量(仅头一个文件有用)
    mesh_obj_number = struct.unpack_from("<I", data, start_index)[0]
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
        f"<<< 网格物体数量: {mesh_obj_number} 本网格变换矩阵数量: {mesh_matrices_number} 本网格字节总数: {mesh_byte_size}"
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

    print(f"<<< 顶点数据解析完成: {len(vertices)} 组")

    return vertices, uv_coords, tangents


# 定义解析面数据函数
def read_faces(self, faces_data_block, index_length):
    """解析面数据"""
    print(f">>> 开始解析面数据 {index_length}")
    faces = []
    try:
        # 确保有足够的字节进行解包
        for i in range(0, index_length - 12, 12):
            f0 = struct.unpack_from("H", faces_data_block, i)[0]
            f1 = struct.unpack_from("H", faces_data_block, i + 4)[0]
            f2 = struct.unpack_from("H", faces_data_block, i + 8)[0]
            faces.append((f0, f1, f2))
            # print(f"> 解析面: {i} -> {f0} {f1} {f2}")
    except Exception as e:
        print(f"! 面数据解析失败: {e}")
        # self.report({"ERROR"}, f"面数据解析失败 : {e}")
        # traceback.print_exc()
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
            print("> 获取顶点数据长度:", len(vertices_data))
            if len(vertices_data) <= 0:
                print("! 获取顶点数据长度失败")
                self.report({"ERROR"}, "获取顶点数据长度失败")
                traceback.print_exc()
                return {"CANCELLED"}
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
            vertices_array, uv_coords, tangents  = read_vertices_temp

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
            print(f"> 获取面数据块大小: {faces_data_size}")
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
            print(f"> 获取面数据块: {len(faces_data_block)}")
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
                        "uv_coords": uv_coords,# xx
                        "tangents": tangents,
                    },
                    "faces": {"size": faces_data_size, "data": faces_array},
                }
            )

            # 结束位置,也是新的开始
            data_start += 0x1D + mesh_byte_size + 4 + faces_data_size
            print("> data_start:", hex(data_start))

            # 读取剩余数据(着色器,贴图,动画等) -> 是否存在另一个物体, 另一个物体头部地址
            nextobj, next_data_start = read_other_data(data,data_start,temp_num)
            if not nextobj:
                print("! 读取其他物体数据失败")
                # self.report({"ERROR"}, "读取其他物体数据失败")
                traceback.print_exc()
                # return {"CANCELLED"}
                break
            # 修正数据起始位置
            data_start = next_data_start

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
                uv_coords = this_obj["vertices"]["uv_coords"]
                # 读取切线
                tangents = this_obj["vertices"]["tangents"]

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
