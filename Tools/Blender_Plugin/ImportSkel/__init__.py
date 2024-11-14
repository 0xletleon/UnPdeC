import bpy
import struct
import os
import math
from mathutils import Vector, Euler

bl_info = {
    "name": "导入.skel骨骼",
    "author": "letleon",
    "version": (1, 2),
    "blender": (4, 1, 0),
    "location": "File > Import > Import Game Skeleton (.skel)",
    "description": "导入.skel骨骼",
    "category": "Import-Export",
}

def read_bone_info(file):
    """读取骨骼名称和层级"""
    try:
        # 骨骼名称长度
        name_length = struct.unpack("<I", file.read(4))[0]
        print(f"骨骼名称长度: {name_length}")
        # 骨骼名称
        name = file.read(name_length).decode("ascii")
        print(f"骨骼名称: {name}")
        # 骨骼层级
        level = struct.unpack("<I", file.read(4))[0]

        print(f"骨骼信息: {name} (Level {level})")

        # 返回骨骼名称和层级
        return name, level
    except struct.error as e:
        print(f"读取骨骼信息失败: {e}")
        return None

def read_bone_transform(file):
    """读取骨骼变换数据"""
    try:
        # 头部坐标
        head = struct.unpack("<fff", file.read(12))
        # 尾部坐标
        tail = struct.unpack("<fff", file.read(12))
        # 结束标记？
        end_tag = struct.unpack("<f", file.read(4))[0]
        
        print(f"Head: {head} Tail: {tail} End tag: {end_tag}")
        
        return head, tail, end_tag
    except struct.error as e:
        print(f"读取骨骼变换数据失败: {e}")
        return None

def validate_file(file):
    """验证文件是否是skel文件"""
    try:
        header = file.read(12)
        return header == b'\xFF\xFF\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00'
    except Exception as e:
        print(f"文件验证失败: {e}")
        return False

def convert_coordinates(coords):
    """转换坐标系"""
    x, y, z = coords

    return (x, z, y)

def calculate_bone_roll(bone):
    """计算骨骼的roll角度"""
    if 'arm' in bone.name.lower():
        return math.radians(90)
    elif 'leg' in bone.name.lower():
        return math.radians(-90)
    elif 'hand' in bone.name.lower():
        return math.radians(180)
    return 0


def print_hierarchy(bones):
    """打印骨骼层级结构"""
    print("骨骼层级结构:")
    for name, level in bones:
        # indent = "  " * (level - 1)
        print(f"{name} (Level {level})")

def create_bone_chain(edit_bones, bones, transforms):
    """创建骨骼链"""
    bone_dict = {}
    
    # 创建所有骨骼
    for i, (name, level) in enumerate(bones):
        if i < len(transforms):
            bone = edit_bones.new(name)
            head, tail, end_tag = transforms[i]
            
            # 转换坐标
            head = convert_coordinates(head)
            tail = convert_coordinates(tail)
            
            # 设置骨骼位置
            bone.head = Vector(head)
            bone.tail = Vector(tail)
            
            # 计算骨骼方向
            # bone.roll = calculate_bone_roll(bone)

            # 存储骨骼信息
            bone_dict[name] = {
                'bone': bone,
                'level': level
            }
    
    # 设置父子关系
    for name, data in bone_dict.items():
        bone = data['bone']
        level = data['level']
        
        if level > 1:
            # 查找最近的父骨骼
            parent_level = level - 1
            potential_parents = [(n, d) for n, d in bone_dict.items() 
                               if d['level'] == parent_level]
            
            if potential_parents:
                # 选择最近的父骨骼
                closest_parent = min(potential_parents, 
                    key=lambda x: (x[1]['bone'].head - bone.head).length)
                bone.parent = closest_parent[1]['bone']
                # 设置父骨骼的尾为子骨骼的头
                closest_parent[1]['bone'].tail = bone.head
    

def add_bone_constraints(armature_obj):
    """添加骨骼约束"""
    pose = armature_obj.pose
    
    # 添加IK约束
    for bone_name in pose.bones:
        if 'IK' in bone_name:
            target_name = bone_name.replace('IK', '')
            if target_name in pose.bones:
                constraint = pose.bones[target_name].constraints.new('IK')
                constraint.target = armature_obj
                constraint.subtarget = bone_name
  

class ImportGameCustomSkeleton(bpy.types.Operator):
    """导入自定义骨骼文件"""
    bl_idname = "import.game_skel"
    bl_label = "导入.skel骨骼"
    bl_options = {'REGISTER', 'UNDO'}

    # 使用bpy.props顶义文件路径属性
    filepath: bpy.props.StringProperty(
        subtype="FILE_PATH",
        default="",
    ) # type: ignore
    
    def invoke(self, context, event):
        # 设置文件过滤器为.skel后缀
        self.filter_glob = "*.skel;"
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}
    
    def execute(self, context):
        """导入骨骼"""
        try:
            # 文件路径
            file_path = self.filepath
            
            # 从文件路径中提取文件名
            file_name = os.path.splitext(os.path.basename(file_path))[0]
            
            # 骨骼名称和层级关系
            bones = []
            # 骨骼变换数据
            transforms = []
            
            # 读取骨骼信息
            with open(file_path, "rb") as file:
                # 验证文件是否是skel文件
                if not validate_file(file):
                    print("无效的文件格式")
                    return {'CANCELLED'}
                
                # 读取骨骼名称和层级关系
                print("读取骨骼名称和层级关系")
                while True:
                    # 获取当前文件位置
                    current_position = file.tell()
                    # 读取骨骼信息
                    bone_info = read_bone_info(file)
                    
                    # 如果读取失败，跳出循环
                    if bone_info is None:
                        print("读取骨骼信息失败")
                        break
                    
                    # 将骨骼信息添加到列表中
                    name, level = bone_info
                    bones.append((name, level))
                    
                    # 检查是否到达骨骼名称部分的结尾
                    # 获取当前文件位置
                    current_position = file.tell()
                    # 获取下一个骨骼名称长度
                    next_name_length = struct.unpack("<I", file.read(4))[0]
                    # 向尾部移动23字节(+前面的4字节)(一个数据块大小为28(0x1C)字节)
                    file.seek(23, 1)
                    # 读取结束标记
                    end_tag = file.read(1).hex()
                    # 将文件指针恢复到原来的位置
                    file.seek(current_position)

                    print(f"下文件名长度: {next_name_length} 结束标记: {end_tag}")

                    # 检查是否到达骨骼名称部分的结尾
                    if next_name_length <= 0 and end_tag == "3f":
                        print("读取骨骼信息结束:",len(bones))
                        break

                print("读取骨骼变换数据")
                print(f"当前文件地址: {current_position}")
                # 读取骨骼变换数据
                bones_len = len(bones)
                # 根据bones_len长度循环获取变换数据
                for i in range(bones_len):
                    print(f"{i+1} 读取 {bones[i][0]} 骨骼变换数据 Level: {bones[i][1]}")

                    # 读取变换数据
                    transform = read_bone_transform(file)
                    # 如果读取失败，跳出循环
                    if transform is None:
                        break
                    # 将变换数据添加到列表中
                    transforms.append(transform)
                
                print(f"读取骨骼变换数据结束: {len(transforms)}")
                # 打印骨骼层级
                print_hierarchy(bones)
            
            # 创建骨架
            print("创建骨架")
            # 创建骨架对象
            armature = bpy.data.armatures.new(file_name)
            armature_obj = bpy.data.objects.new(file_name, armature)
            
            # 显示名称
            armature.show_names = True
            # 显示轴
            armature.show_axes = True
            # armature.display_type = 'STICK'
            
            # 设置对象变换
            # armature_obj.scale = (scale, scale, scale)
            
            # 链接骨架对象到场景中
            context.collection.objects.link(armature_obj)
            # 激活骨架对象
            context.view_layer.objects.active = armature_obj
            
            # 进入编辑模式
            bpy.ops.object.mode_set(mode='EDIT')
            
            # 创建骨骼
            create_bone_chain(armature.edit_bones, bones, transforms)
            
            # 添加骨骼约束
            bpy.ops.object.mode_set(mode='POSE')
            print("添加骨骼约束")
            add_bone_constraints(armature_obj)
            
            # 调整视图
            bpy.ops.object.mode_set(mode='OBJECT')
            # 选择骨架对象
            bpy.ops.object.select_all(action='DESELECT')
            armature_obj.select_set(True)
            context.view_layer.objects.active = armature_obj
            
            print(f"成功导入 {len(bones)} 个骨骼")
            return {'FINISHED'}
            
        except Exception as e:
            print(f"导入过程中发生错误: {str(e)}")
            return {'CANCELLED'}


def menu_func_import(self, context):
    self.layout.operator(ImportGameCustomSkeleton.bl_idname, text="导入骨骼 (.skel)")

def register():
    bpy.utils.register_class(ImportGameCustomSkeleton)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)

def unregister():
    bpy.utils.unregister_class(ImportGameCustomSkeleton)
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)

if __name__ == "__main__":
    register()