import unreal

# 自动配置相机镜头脚本

def get_all_cine_cameras():
    """获取关卡中所有的 CineCameraActor"""
    editor_level_lib = unreal.EditorLevelLibrary()
    all_actors = editor_level_lib.get_all_level_actors()
    
    cameras = []
    for actor in all_actors:
        if isinstance(actor, unreal.CineCameraActor):
            cameras.append(actor)
    
    return cameras

def group_cameras_by_name(cameras):
    """根据名称分组相机 - 合并子分类"""
    # 主分类映射
    category_mapping = {
        '沙发': ['沙发'],
        '椅子': ['休闲椅1', '休闲椅2', '休闲椅3'],
        '床': ['大床'],
        '茶几': ['茶几1'],
        '边几': ['边几1', '边几2', '边几3'],
        '床头柜': ['床头柜1', '床头柜2'],
        '玄关柜': ['玄关柜'],
        '地板': ['地板']
    }
    
    # 临时存储
    temp_groups = {}
    
    for camera in cameras:
        name = camera.get_actor_label()
        
        # 提取原始分类名称
        original_category = None
        if '镜头' in name:
            parts = name.split('镜头')
            if len(parts) >= 2:
                original_category = parts[0]
        
        if original_category:
            if original_category not in temp_groups:
                temp_groups[original_category] = []
            
            # 获取 Transform
            transform = camera.get_actor_transform()
            temp_groups[original_category].append({
                'name': name,
                'transform': transform
            })
    
    # 合并为主分类
    main_groups = {}
    for main_cat, sub_cats in category_mapping.items():
        main_groups[main_cat] = []
        for sub_cat in sub_cats:
            if sub_cat in temp_groups:
                main_groups[main_cat].extend(temp_groups[sub_cat])
        
        # 按名称排序
        main_groups[main_cat].sort(key=lambda x: x['name'])
    
    return main_groups

def configure_camera_controller(camera_groups):
    """配置 CameraViewController"""
    editor_level_lib = unreal.EditorLevelLibrary()
    all_actors = editor_level_lib.get_all_level_actors()
    
    # 查找或创建 CameraViewController
    camera_controller = None
    for actor in all_actors:
        if actor.get_class().get_name() == 'CameraViewController':
            camera_controller = actor
            break
    
    if not camera_controller:
        print("未找到 CameraViewController，正在创建...")
        # 创建新的 CameraViewController
        camera_controller_class = unreal.EditorAssetLibrary.load_blueprint_class('/Script/chaji.CameraViewController')
        camera_controller = editor_level_lib.spawn_actor_from_class(camera_controller_class, unreal.Vector(0, 0, 0))
    
    print(f"找到 CameraViewController: {camera_controller.get_actor_label()}")
    
    # 清空现有的 Categories
    camera_controller.set_editor_property('categories', [])
    
    categories = []
    
    # 为每个分组创建 Category
    for category_name, viewpoints in camera_groups.items():
        print(f"\n配置分类: {category_name}")
        
        # 创建 FCategoryViewpoint
        category_viewpoint = unreal.CategoryViewpoint()
        category_viewpoint.set_editor_property('category_name', category_name)
        
        # 添加 Viewpoints
        transforms = []
        for vp in viewpoints:
            print(f"  - 添加镜头: {vp['name']}")
            transforms.append(vp['transform'])
        
        category_viewpoint.set_editor_property('viewpoints', transforms)
        categories.append(category_viewpoint)
    
    # 设置 Categories
    camera_controller.set_editor_property('categories', categories)
    
    print(f"\n✅ 配置完成！共配置 {len(categories)} 个分类")
    return camera_controller

def main():
    print("=" * 50)
    print("自动配置相机镜头")
    print("=" * 50)
    
    # 1. 获取所有相机
    cameras = get_all_cine_cameras()
    print(f"\n找到 {len(cameras)} 个相机镜头")
    
    # 2. 分组
    camera_groups = group_cameras_by_name(cameras)
    print(f"\n按名称分组，共 {len(camera_groups)} 个分类:")
    for category, viewpoints in camera_groups.items():
        print(f"  - {category}: {len(viewpoints)} 个镜头")
    
    # 3. 配置 CameraViewController
    configure_camera_controller(camera_groups)
    
    print("\n" + "=" * 50)
    print("配置完成！")
    print("=" * 50)

if __name__ == '__main__':
    main()
