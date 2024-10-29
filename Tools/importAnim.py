import bpy
import struct
import os

# 插件元数据
bl_info = {
    "name": "Import ANIM File",
    "author": "Your Name",
    "version": (1, 0),
    "blender": (4, 1, 0),
    "location": "File > Import",
    "description": "Import ANIM file",
    "warning": "仅为学习，严禁商用！",
    "category": "Import",
}


# 顶义操作类
class ImportAnimOperator(bpy.types.Operator):
    """Import an .anim file"""

    bl_idname = "import_anim.anim_file"
    bl_label = "Import .anim"
    bl_options = {"REGISTER", "UNDO"}

    # 使用bpy.props顶义文件路径属性
    filepath: bpy.props.StringProperty(
        subtype="FILE_PATH",
        default="",
    )

    # 顶义invoke方法来显示文件选择对话框
    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return {"RUNNING_MODAL"}

    def execute(self, context):
        # 顶义数据文件的路径
        file_path = self.filepath

        # 检查文件是否存在
        if not os.path.exists(file_path):
            self.report({"ERROR"}, "文件不存在，请检查路径是否正确")
            return {"CANCELLED"}

        # 读取二进制文件
        with open(file_path, "rb") as file:
            data = file.read()

        # 解析文件并分组
        vertex_groups = self.parse_anim_file(data)

        # 创建动画...
        for group_name, group_data in vertex_groups.items():
            # 创建一个空对象并设置关键帧
            obj = bpy.data.objects.new(group_name, None)
            context.collection.objects.link(obj)

            # 设置空物体的初始位置
            obj.location = (0, 0, 0)

            # 假设group_data包含位置、旋转等信息
            for frame, transform in enumerate(group_data):
                # 设置位置关键帧
                obj.location = transform["location"]
                obj.keyframe_insert(data_path="location", frame=frame)

                # 设置旋转关键帧（假设使用欧拉角）
                obj.rotation_euler = transform["rotation"]
                obj.keyframe_insert(data_path="rotation_euler", frame=frame)

        self.report({"INFO"}, "动画文件加载成功")
        return {"FINISHED"}

    def parse_anim_file(self, data):
        # 第一个顶点组名字大小
        temp_group_name_length = struct.unpack("I", data[0:4])[0]
        # 第一个顶点组名字
        temp_group_name = data[4 : 4 + temp_group_name_length].decode("utf-8")
        print("temp_group_name:", temp_group_name)
        # 特征16进制8字节
        temp_feature = data[
            4 + temp_group_name_length : 4 + temp_group_name_length + 8
        ]  # 特征是8个字节
        print("temp_feature:", temp_feature.hex())

        # 所有顶点组信息
        all_group = []
        all_group_offset = 0
        find_all_done = False
        # 找出所有特征的位置
        print("开始 找出所有特征的位置")
        while not find_all_done:
            # 使用特征值来全局搜索下一个顶点组的开始
            next_feature_index = data.find(temp_feature, all_group_offset)
            print("next_feature_index:", next_feature_index)
            if next_feature_index == -1:
                # 如果找到所有特征，设置 find_all_done 为 True
                find_all_done = True
                break

            # 从特征索引往前找16进制00，确顶当前顶点组数据的结束
            end_of_current_group = next_feature_index - 1
            while end_of_current_group >= 0 and data[end_of_current_group] != 0:
                end_of_current_group -= 1

            # +1为顶点组名字开头
            end_of_current_group = end_of_current_group + 1
            print("找到顶点组名称:", end_of_current_group)
            # 顶点组名字
            now_group_name = data[end_of_current_group:next_feature_index].decode(
                "utf-8"
            )
            print("找到顶点组名称:", now_group_name)
            print("all_group.len():", len(all_group))

            # 写入all_group
            if len(all_group) > 0:
                print("all_group > 0")
                # 修改上一个endoffset
                all_group[len(all_group) - 1]["eoffset"] = end_of_current_group - 4
                # 添加当前顶点组
                all_group.append(
                    {
                        "name": now_group_name,
                        "soffset": next_feature_index + 8,
                        "eoffset": len(data),
                    }
                )
                # 修改 all_group_offset
                all_group_offset = next_feature_index + 8
            else:
                print("all_group < 0")
                # 添加当前顶点组
                all_group.append(
                    {
                        "name": now_group_name,
                        "soffset": next_feature_index + 8,
                        "eoffset": len(data),
                    }
                )
                # 第一次修改 all_group_offset
                all_group_offset = next_feature_index + 8

        print("完成 找出所有特征的位置")
        print("all_group.len():", len(all_group))
        # 解析文件并根据特征分组
        vertex_groups = {}
        # 获取每个顶点组的帧数据
        for one_group in all_group:
            # 创建顶点组数据
            group_name = one_group["name"]
            print("group_name:", group_name)
            if group_name not in vertex_groups:
                vertex_groups[group_name] = []
            # 读取当前顶点组数据
            soffset = one_group["soffset"]
            eoffset = one_group["eoffset"]
            print("soffset:", soffset)
            print("eoffset:", eoffset)
            while soffset < eoffset:
                frame_data = data[soffset : soffset + 28]  # 读取28字节的数据块
                location = struct.unpack("3f", frame_data[0:12])
                rotation = struct.unpack("3f", frame_data[12:24])
                vertex_groups[group_name].append(
                    {"location": location, "rotation": rotation}
                )
                soffset += 28  # 移动到下一个数据块

        print("vertex_groups: ", len(vertex_groups))
        return vertex_groups


def menu_func_import(self, context):
    self.layout.operator(ImportAnimOperator.bl_idname, text="Import .anim")


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
