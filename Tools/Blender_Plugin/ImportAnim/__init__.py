import bpy
import struct
import os
from mathutils import Quaternion, Euler
import bmesh
import re

# 插件元数据
bl_info = {
    "name": "导入.anim动画",
    "author": "letleon",
    "version": (1, 1),
    "blender": (4, 1, 0),
    "location": "File > Import",
    "description": "导入.anim动画",
    "warning": "仅供学习，严禁商用！",
    "category": "Import-Export",
}


# 顶义操作类
class ImportAnimOperator(bpy.types.Operator):
    """Import an game .anim file"""

    bl_idname = "import.game_anim"
    bl_label = "导入.anim动画"
    bl_options = {"REGISTER", "UNDO"}

    # 使用bpy.props顶义文件路径属性
    filepath: bpy.props.StringProperty(
        subtype="FILE_PATH",
        default="",
    ) # type: ignore

    # 顶义invoke方法来显示文件选择对话框
    def invoke(self, context, event):
        # 设置文件选择对话框的属性
        # selffilepath = bpy.props.StringProperty(
        #     subtype="FILE_PATH", default=self.filepath
        # )
        # 设置文件过滤器为.anim后缀
        self.filter_glob = "*.anim;"
        context.window_manager.fileselect_add(self)
        return {"RUNNING_MODAL"}

    # run code
    def execute(self, context):
        # 文件的路径
        file_path = self.filepath

        # 检查文件是否存在
        if not os.path.exists(file_path):
            self.report({"ERROR"}, "文件不存在，请检查路径是否正确")
            return {"CANCELLED"}

        # 读取文件
        with open(file_path, "rb") as file:
            data = file.read()

        # 从文件路径中提取文件名（不包括扩展名）
        file_name = os.path.splitext(os.path.basename(file_path))[0]

        # 解析并获得帧数据
        vertex_groups = self.parse_anim_file(data, file_name)

        # 获取总帧数
        total_frames = max(len(group_data) for group_data in vertex_groups.values())
        print(f"!!!!!!!!!!! 总帧数: {total_frames}")

        # 设置Blender场景的帧数
        bpy.context.scene.frame_start = 1
        bpy.context.scene.frame_end = total_frames

        # 创建动画
        for group_name, group_data in vertex_groups.items():
            # 创建一个新的立方体网格
            mesh = bpy.data.meshes.new(name=group_name)
            # 创建一个新的立方体物体
            obj = bpy.data.objects.new(name=group_name, object_data=mesh)
            # 将物体添加到场景中
            context.collection.objects.link(obj)

            # 设置为活动物体
            bpy.context.view_layer.objects.active = obj
            # 保存当前模式
            current_mode = bpy.context.object.mode
            # 确保处于物体模式
            bpy.ops.object.mode_set(mode="OBJECT")

            # 添加立方体数据到网格
            bm = bmesh.new()
            # 设置尺寸为 0.1
            bmesh.ops.create_cube(bm, size=0.1)
            bm.to_mesh(mesh)
            bm.free()

            # 恢复之前的模式
            bpy.ops.object.mode_set(mode=current_mode)
            # 更新网格数据
            mesh.update()

            # 设置关键帧 location rotation
            for frame, transform in enumerate(group_data):
                # 设置位置关键帧
                obj.location = transform["location"]
                obj.keyframe_insert(data_path="location", frame=frame)

                # 设置旋转关键帧(欧拉角)
                # 假设旋转数据是以度为单位的欧拉角，需要转换为弧度
                obj.rotation_euler = transform["rotation"]
                obj.keyframe_insert(data_path="rotation_euler", frame=frame)

        self.report({"INFO"}, f"{file_name} 动画文件加载成功")
        return {"FINISHED"}

    # 解析并获得帧数据
    def parse_anim_file(self, data, file_name):
        print(f"开始处理 {file_name}")

        # 所有顶点组信息
        all_group = []
        # 当前顶点组数据结束的地址
        group_eoffset = 0
        # 第一个特征
        frist_feature = 0
        # 当前文件大小，用作结束位置
        file_size = len(data)
        # 可能的结束位置
        # 有些文件中会出现自己文件的名字，右面的数据暂时是未知的
        # 所以要找到他然后设置正确的结束位置
        possible_end_offset = data.find(file_name.encode("utf-8"), 0)
        if possible_end_offset != -1:
            # 跳到名字大小前
            file_size = possible_end_offset - 4

        # 查找顶点组
        print("开始 查找顶点组")
        while True:
            # 文件分析 https://www.cnblogs.com/letleon/p/18511408
            try:
                # 获取 顶点组名称大小
                group_name_length = struct.unpack(
                    "I", data[group_eoffset : group_eoffset + 4]
                )[0]
                # 检查 长度是否超长
                if group_name_length > 63:
                    print(
                        f"!名称长度: {group_name_length} 超过了Blender的限制 63个字符"
                    )
                    break
                print(f"顶点组名称大小: {group_name_length}")

                # 获取 顶点组名字
                group_name = data[
                    group_eoffset + 4 : group_eoffset + 4 + group_name_length
                ].decode("utf-8")
                # 检查 名称是否合法
                if not self.is_valid_group_name(group_name):
                    print(f"!名称不合法: {group_name}")
                    break
                print(f"顶点组名字: {group_name}")

                # 获取 顶点组帧数量,也就知道了当前顶点组数据结束位置
                frames_number = struct.unpack(
                    "I",
                    data[
                        group_eoffset
                        + 4
                        + group_name_length : group_eoffset
                        + 4
                        + group_name_length
                        + 4
                    ],
                )[0]
                if frames_number == 0:
                    print(f"!顶点组帧数量为空: {frames_number}")
                    break
                print(f"顶点组帧数量: {frames_number}")

                # 获取 特征 8字节
                this_feature = data[
                    group_eoffset
                    + 4
                    + group_name_length : group_eoffset
                    + 4
                    + group_name_length
                    + 8
                ]  # 特征是8个字节
                # 判断 是否和第一个特征一样
                if frist_feature == 0:
                    if this_feature == 0:
                        print(f"!特征不能为0: {this_feature.hex()}")
                        break
                    frist_feature = this_feature
                elif this_feature != this_feature:
                    print(f"!特征不对: {this_feature.hex()}")
                    break
                print(f"特征: {this_feature.hex()}")

                # 计算 当前顶点组数据开始的地址
                this_group_soffset = group_eoffset + 4 + group_name_length + 8
                print(f"当前顶点组数据开始的地址: {this_group_soffset}")

                # 计算 顶点组数据结束位置
                group_eoffset = (
                    frames_number * 0x1C + group_eoffset + 4 + group_name_length + 8
                )
                if group_eoffset > file_size:
                    print(f"!顶点组数据结束位置越界: {group_eoffset}")
                    break
                print(f"顶点组数据结束位置: {group_eoffset}")

                # 写入all_group
                print("!写入all_group")
                # 添加当前顶点组
                all_group.append(
                    {
                        "name": group_name,
                        "soffset": this_group_soffset,
                        "eoffset": group_eoffset,
                    }
                )

                # 正常退出
                if group_eoffset == file_size:
                    print("!正常退出 查找到尾部")
                    break
            except NameError:
                print("!数据读取错误,查找顶点组结束")
                break

        print(f"完成 查找到: {len(all_group)} 个顶点组")

        # 顶点组帧数据
        vertex_groups = {}
        # 获取 所有顶点组帧数据
        for now_group in all_group:
            # 名称
            group_name = now_group["name"]
            print(f"名称: {group_name}")

            # 添加到顶点组帧数据
            if group_name not in vertex_groups:
                vertex_groups[group_name] = []

            # 当前顶点组数据 开始地址
            soffset = now_group["soffset"]
            # 当前顶点组数据 结束地址
            eoffset = now_group["eoffset"]
            # print(f"soffset: {soffset} eoffset: {eoffset}")
            # 获取顶点组帧数据
            while soffset < eoffset:
                # 检查剩余数据是否足够
                if eoffset - soffset < 28:
                    print("!数据不足28字节")
                    # 如果不足，则退出循环
                    break
                # 读取28字节的数据块
                frame_data = data[soffset : soffset + 28]
                # 方向
                location = struct.unpack("3f", frame_data[0:12])
                # 四元数 -> 后面会转换成 欧拉角
                rotation = struct.unpack("3f", frame_data[12:24])
                # 添加到 vertex_groups
                vertex_groups[group_name].append(
                    {"location": location, "rotation": rotation}
                )
                # 移动到下一帧
                soffset += 28

        print(f"完成 读取到: {len(vertex_groups)} 个顶点组帧数据")
        # 返回顶点组帧数据
        return vertex_groups

    # 将四元数转换为欧拉角
    def quat_to_eul(self, quat):
        quat_obj = Quaternion(quat)
        euler_obj = quat_obj.to_euler("XYZ")
        return euler_obj

    # 检查名称是否合法
    def is_valid_group_name(self, now_group_name):
        # 检查是否为空
        if not now_group_name:
            print(f"!名称为空。{format(now_group_name)}")
            return False

        # 检查是否为字符串
        if not isinstance(now_group_name, str):
            print(f"!不是字符串 {format(now_group_name)}")
            return False

        # 使用正则表达式匹配只包含a-z, A-Z, 且不以数字开头，包含0-9, _的字符串
        if re.match("^[a-zA-Z_][a-zA-Z0-9_]*$", now_group_name):
            return True
        else:
            print("'{}' 包含非法字符或以数字开头。".format(now_group_name))
            return False


# 添加菜单选项
def menu_func_import(self, context):
    self.layout.operator(ImportAnimOperator.bl_idname, text="导入动画 (.anim)")


# 注册和注销函数
def register():
    bpy.utils.register_class(ImportAnimOperator)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)


def unregister():
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)
    bpy.utils.unregister_class(ImportAnimOperator)


# 注册插件
if __name__ == "__main__":
    register()
