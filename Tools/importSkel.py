import bpy
import struct
import os
import math
from mathutils import Vector, Euler

bl_info = {
    "name": "Custom Skeleton Importer (.skel)",
    "author": "letleon",
    "version": (1, 2),
    "blender": (4, 1, 0),
    "location": "File > Import > Custom Skeleton (.skel)",
    "description": "Import custom .skel skeleton files",
    "category": "Import-Export",
}

# 日志设置
import logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

def read_string(file):
    """读取字符串数据"""
    try:
        length = struct.unpack("<I", file.read(4))[0]
        return file.read(length).decode("ascii")
    except struct.error as e:
        logger.error(f"读取字符串失败: {e}")
        return None

def read_bone_info(file):
    """读取骨骼名称和层级"""
    try:
        name_length = struct.unpack("<I", file.read(4))[0]
        name = file.read(name_length).decode("ascii")
        level = struct.unpack("<I", file.read(4))[0]
        logger.debug(f"读取骨骼: {name} (Level {level})")
        return name, level
    except struct.error as e:
        logger.error(f"读取骨骼信息失败: {e}")
        return None

def read_bone_transform(file):
    """读取骨骼变换数据"""
    try:
        head = struct.unpack("<fff", file.read(12))
        tail = struct.unpack("<fff", file.read(12))
        end_tag = struct.unpack("<f", file.read(4))[0]
        
        logger.debug(f"Head: {head}")
        logger.debug(f"Tail: {tail}")
        logger.debug(f"End tag: {end_tag}")
        
        return head, tail, end_tag
    except struct.error as e:
        logger.error(f"读取骨骼变换数据失败: {e}")
        return None

def validate_file_format(file):
    """验证文件格式"""
    try:
        header = file.read(12)
        return header == b'\xFF\xFF\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00'
    except Exception as e:
        logger.error(f"文件格式验证失败: {e}")
        return False

def convert_coordinates(coords, axis_forward, axis_up):
    """转换坐标系"""
    x, y, z = coords
    
    # 根据选择的轴向进行转换
    # 根据选择的轴向进行转换
    if axis_forward == 'X' and axis_up == 'Y':
        return (x, y, -z)
    elif axis_forward == 'X' and axis_up == '-Y':
        return (x, -y, z)
    elif axis_forward == '-X' and axis_up == 'Y':
        return (-x, y, z)
    elif axis_forward == '-X' and axis_up == '-Y':
        return (-x, -y, -z)
    elif axis_forward == 'Y' and axis_up == 'X':
        return (y, x, -z)
    elif axis_forward == 'Y' and axis_up == '-X':
        return (y, -x, z)
    elif axis_forward == '-Y' and axis_up == 'X':
        return (-y, x, z)
    elif axis_forward == '-Y' and axis_up == '-X':
        return (-y, -x, -z)
    elif axis_forward == 'Z' and axis_up == 'X':
        return (z, x, -y)
    elif axis_forward == 'Z' and axis_up == '-X':
        return (z, -x, y)
    elif axis_forward == '-Z' and axis_up == 'X':
        return (-z, x, y)
    elif axis_forward == '-Z' and axis_up == '-X':
        return (-z, -x, -y)
    elif axis_forward == 'Z' and axis_up == 'Y':
        return (z, y, -x)
    elif axis_forward == 'Z' and axis_up == '-Y':
        return (z, -y, x)
    elif axis_forward == '-Z' and axis_up == 'Y':
        return (-z, y, x)
    elif axis_forward == '-Z' and axis_up == '-Y':
        return (-z, -y, -x)
    else:
        return (x, y, z)

def calculate_bone_roll(bone):
    """计算骨骼的roll角度"""
    if 'arm' in bone.name.lower():
        return math.radians(90)
    elif 'leg' in bone.name.lower():
        return math.radians(-90)
    elif 'hand' in bone.name.lower():
        return math.radians(180)
    return 0

def is_symmetric_bone(name):
    """检查是否是对称骨骼"""
    return name.endswith('_l') or name.endswith('_r')

def get_symmetric_name(name):
    """获取对称骨骼的名称"""
    if name.endswith('_l'):
        return name[:-2] + '_r'
    elif name.endswith('_r'):
        return name[:-2] + '_l'
    return name

def is_special_bone(name):
    """检查是否是特殊骨骼"""
    special_bones = ['handweapon', 'wristIK', 'cam']
    return any(bone in name.lower() for bone in special_bones)

def print_hierarchy(bones):
    """打印骨骼层级结构"""
    logger.info("骨骼层级结构:")
    for name, level in bones:
        indent = "  " * (level - 1)
        logger.info(f"{indent}|- {name} (Level {level})")

def create_bone_chain(edit_bones, bones, transforms, axis_forward, axis_up, bone_length):
    """创建骨骼链"""
    bone_dict = {}
    
    # 创建所有骨骼
    for i, (name, level) in enumerate(bones):
        if i < len(transforms):
            bone = edit_bones.new(name)
            head, tail, end_tag = transforms[i]
            
            # 转换坐标
            head = convert_coordinates(head, axis_forward, axis_up)
            tail = convert_coordinates(tail, axis_forward, axis_up)
            
            # 设置骨骼位置
            bone.head = Vector(head)
            bone.tail = Vector(tail)
            
            # 计算骨骼方向
            bone.roll = calculate_bone_roll(bone)
            
            # 如果头尾位置相同，添加一个小的偏移
            if (bone.head - bone.tail).length < bone_length:
                if 'finger' in name.lower():
                    bone.tail.y += bone_length * 0.5  # 手指骨骼
                else:
                    bone.tail.y += bone_length  # 其他骨骼
            
            # 存储骨骼信息
            bone_dict[name] = {
                'bone': bone,
                'level': level,
                'is_special': is_special_bone(name),
                'is_symmetric': is_symmetric_bone(name)
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
                
                # 调整连接
                if (bone.head - bone.parent.tail).length < bone_length * 0.1:
                    bone.use_connect = True
    
    return bone_dict

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

def import_skeleton(context, filepath, axis_forward, axis_up, scale, bone_length, flip_bones):
    """导入骨骼"""
    try:
        # 开始进度条
        context.window_manager.progress_begin(0, 100)
        
        # 从文件路径中提取文件名
        file_name = os.path.splitext(os.path.basename(filepath))[0]
        
        bones = []
        transforms = []
        
        with open(filepath, "rb") as file:
            # 验证文件格式
            if not validate_file_format(file):
                logger.error("无效的文件格式")
                return {'CANCELLED'}
            
            # 读取骨骼名称和层级关系
            logger.info("读取骨骼名称和层级关系")
            context.window_manager.progress_update(20)
            
            while True:
                current_position = file.tell()
                bone_info = read_bone_info(file)
                
                if bone_info is None:
                    break
                    
                name, level = bone_info
                bones.append((name, level))
                
                # 检查是否到达骨骼名称部分的结尾
                current_position = file.tell()
                file.seek(24, 1)
                end_tag = file.read(4).hex()
                file.seek(current_position)
                
                if end_tag == "0000803f":
                    break
            
            # 读取骨骼变换数据
            logger.info("读取骨骼变换数据")
            context.window_manager.progress_update(40)
            
            while True:
                transform = read_bone_transform(file)
                if transform is None:
                    break
                transforms.append(transform)
                
                if file.tell() >= os.path.getsize(filepath):
                    break
            
            # 删除第一个空的变换数据
            if transforms:
                del transforms[0]
        
        # 打印骨骼层级
        print_hierarchy(bones)
        
        # 创建骨架
        logger.info("创建骨架")
        context.window_manager.progress_update(60)
        
        armature = bpy.data.armatures.new(file_name)
        armature_obj = bpy.data.objects.new(file_name, armature)
        
        # 设置骨架显示选项
        armature.show_names = True
        armature.show_axes = True
        # armature.display_type = 'STICK'
        
        # 设置对象变换
        armature_obj.scale = (scale, scale, scale)
        
        context.collection.objects.link(armature_obj)
        context.view_layer.objects.active = armature_obj
        
        # 进入编辑模式
        bpy.ops.object.mode_set(mode='EDIT')
        
        # 创建骨骼
        bone_dict = create_bone_chain(armature.edit_bones, bones, transforms, 
                                    axis_forward, axis_up, bone_length)
        
        # 添加骨骼约束
        bpy.ops.object.mode_set(mode='POSE')
        logger.info("添加骨骼约束")
        add_bone_constraints(armature_obj)
        
        # 调整视图
        bpy.ops.object.mode_set(mode='OBJECT')
        bpy.ops.object.select_all(action='DESELECT')
        armature_obj.select_set(True)
        context.view_layer.objects.active = armature_obj
        
        # 结束进度条
        context.window_manager.progress_end()
        
        logger.info(f"成功导入 {len(bones)} 个骨骼")
        return {'FINISHED'}
        
    except Exception as e:
        logger.error(f"导入过程中发生错误: {str(e)}")
        context.window_manager.progress_end()
        return {'CANCELLED'}

class ImportCustomSkeleton(bpy.types.Operator):
    """导入自定义骨骼文件"""
    bl_idname = "import_mesh.custom_skeleton"
    bl_label = "Import Custom Skeleton (.skel)"
    bl_options = {'PRESET', 'UNDO'}
    
    filename_ext = ".skel"
    
    filter_glob: bpy.props.StringProperty(
        default="*.skel",
        options={'HIDDEN'},
        maxlen=255,
    )
    
    filepath: bpy.props.StringProperty(
        name="File Path",
        description="Filepath used for importing the SKEL file",
        maxlen=1024,
        default="",
        subtype='FILE_PATH',
    )
    
    # 导入选项
    scale: bpy.props.FloatProperty(
        name="Scale",
        description="Scale the armature by this value",
        min=0.001, max=1000.0,
        soft_min=0.001, soft_max=1000.0,
        default=0.01,
    )
    
    bone_length: bpy.props.FloatProperty(
        name="Minimum Bone Length",
        description="Minimum length for bones",
        min=0.001, max=1000.0,
        soft_min=0.001, soft_max=1000.0,
        default=0.1,
    )
    
    flip_bones: bpy.props.BoolProperty(
        name="Flip Bone Direction",
        description="Flip the direction of imported bones",
        default=False,
    )
    
    axis_forward: bpy.props.EnumProperty(
        name="Forward Axis",
        items=(
            ('X', "X Forward", ""),
            ('Y', "Y Forward", ""),
            ('Z', "Z Forward", ""),
            ('-X', "-X Forward", ""),
            ('-Y', "-Y Forward", ""),
            ('-Z', "-Z Forward", ""),
        ),
        default='Y',
    )
    
    axis_up: bpy.props.EnumProperty(
        name="Up Axis",
        items=(
            ('X', "X Up", ""),
            ('Y', "Y Up", ""),
            ('Z', "Z Up", ""),
            ('-X', "-X Up", ""),
            ('-Y', "-Y Up", ""),
            ('-Z', "-Z Up", ""),
        ),
        default='Z',
    )
    
    def execute(self, context):
        return import_skeleton(context, self.filepath,
                             self.axis_forward, self.axis_up,
                             self.scale, self.bone_length,
                             self.flip_bones)
    
    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}
    
    def draw(self, context):
        layout = self.layout
        layout.use_property_split = True
        layout.use_property_decorate = False
        
        box = layout.box()
        box.label(text="Transform:")
        box.prop(self, "axis_forward")
        box.prop(self, "axis_up")
        box.prop(self, "scale")
        
        box = layout.box()
        box.label(text="Bone Options:")
        box.prop(self, "bone_length")
        box.prop(self, "flip_bones")

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