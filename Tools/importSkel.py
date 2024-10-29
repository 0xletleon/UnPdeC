import bpy
import struct
import os
from mathutils import Vector, Euler

bl_info = {
    "name": "Custom Skeleton Importer (.skel)",
    "author": "Your Name",
    "version": (1, 0),
    "blender": (2, 80, 0),
    "location": "File > Import > Custom Skeleton (.skel)",
    "description": "Import custom .skel skeleton files",
    "category": "Import-Export",
}

def read_string(file):
    length = struct.unpack("<I", file.read(4))[0]
    return file.read(length).decode("ascii")

def read_bone_info(file):
    # 读取名字长度
    name_length = struct.unpack("<I", file.read(4))[0]
    # 读取名字
    name = file.read(name_length).decode("ascii")
    # 读取层级
    level = struct.unpack("<I", file.read(4))[0]
    return name, level

def read_transform(file):
    try:
        unknown = struct.unpack("<f", file.read(4))[0]
        head = struct.unpack("<fff", file.read(12))
        rotation = struct.unpack("<fff", file.read(12))
        print(head,rotation,unknown)
        return unknown, head, rotation
    except struct.error:
        # 如果无法读取完整的变换数据，返回None
        return None

def calculate_bone_tail(head, rotation, length=0.1):
    """计算骨骼尾部位置"""
    direction = Vector((0, length, 0))
    rot_euler = Euler(rotation, 'XYZ')
    direction.rotate(rot_euler)
    return Vector(head) + direction

def import_skeleton(context, filepath):
    try:
        bones = []
        transforms = []
        # 从文件路径中提取文件名（不包括扩展名）
        file_name = os.path.splitext(os.path.basename(filepath))[0]

        with open(filepath, "rb") as file:
            # Skip header
            file.read(12)

            # Read bone info
            while True:
                chunk = file.read(24)
                if chunk == b"\x00" * 24:
                    break
                file.seek(-24, 1)  # Go back
                name, level = read_bone_info(file)
                bones.append((name, level))

            # Read transforms
            while True:
                chunk = file.read(4)
                if chunk == b"\x00\x80\x00\x00":
                    break
                file.seek(-4, 1)  # Go back
                transform = read_transform(file)
                if transform is None:
                    break  # 如果无法读取更多数据，退出循环
                transforms.append(transform)

        # 确保骨骼数量和变换数量匹配
        if len(bones) != len(transforms):
            print(f"警告：骨骼数量 ({len(bones)}) 与变换数量 ({len(transforms)}) 不匹配")

        # 创建骨架
        armature = bpy.data.armatures.new(file_name)
        obj = bpy.data.objects.new(file_name, armature)
        context.collection.objects.link(obj)

        context.view_layer.objects.active = obj
        bpy.ops.object.mode_set(mode="EDIT")

        edit_bones = armature.edit_bones
        bone_dict = {}  # 用于存储骨骼层级关系

        # 第一遍：创建所有骨骼
        for i, (name, level) in enumerate(bones):
            if i < len(transforms):
                bone = edit_bones.new(name)
                unknown, head, rotation = transforms[i]

                # 设置骨骼的头部
                bone.head = head
                # 使用新的函数计算尾部
                bone.tail = calculate_bone_tail(head, rotation)

                # 存储骨骼信息
                bone_dict[name] = {'bone': bone, 'level': level}
            else:
                print(f"警告：骨骼 '{name}' 没有对应的变换数据")

        # 第二遍：设置父子关系
        for i, (name, level) in enumerate(bones):
            if i < len(transforms):
                current_bone = bone_dict[name]['bone']
                
                # 查找父骨骼
                if level > 1:
                    # 向前查找level更小的骨骼作为父级
                    for j in range(i-1, -1, -1):
                        parent_name = bones[j][0]
                        parent_level = bones[j][1]
                        if parent_level < level:
                            current_bone.parent = bone_dict[parent_name]['bone']
                            break

        bpy.ops.object.mode_set(mode="POSE")

        # 应用旋转
        for i, (name, level) in enumerate(bones):
            if i < len(transforms):
                pose_bone = obj.pose.bones[name]
                unknown, head, rotation = transforms[i]
                pose_bone.rotation_mode = "XYZ"
                # 确保 rotation 是用弧度表示的，如果是角度需要转换
                rotation_rad = [r * 3.14159 / 180.0 for r in rotation]  # Convert to radians if needed
                pose_bone.rotation_euler = rotation_rad

        bpy.ops.object.mode_set(mode="OBJECT")

        print(f"成功导入 {len(bones)} 个骨骼")
        print(f"骨骼层级结构:")
        # for name, level in bones:
        #     print(f"  {'  ' * (level-1)}{name} (Level {level})")
            
    except Exception as e:
        print(f"导入过程中发生错误: {str(e)}")
        return {'CANCELLED'}
    
    return {"FINISHED"}

class ImportCustomSkeleton(bpy.types.Operator):
    bl_idname = "import_mesh.custom_skeleton"
    bl_label = "Import Custom Skeleton (.skel)"
    bl_options = {"UNDO"}

    filepath: bpy.props.StringProperty(subtype="FILE_PATH")

    def execute(self, context):
        return import_skeleton(context, self.filepath)

    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return {"RUNNING_MODAL"}

def menu_func_import(self, context):
    self.layout.operator(ImportCustomSkeleton.bl_idname, text="Custom Skeleton (.skel)")

def register():
    bpy.utils.register_class(ImportCustomSkeleton)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)

def unregister():
    bpy.utils.unregister_class(ImportCustomSkeleton)
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)

if __name__ == "__main__":
    register()
