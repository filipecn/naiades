import bmesh
import bpy
import colormaps as cmaps

def create_plane(context, with_uvs=True):
    bm = bmesh.new()

    if with_uvs:
        uv_layer = bm.loops.layers.uv.new("UVMap")

    bmesh.ops.create_grid(
        bm, x_segments=15, y_segments=15, size=1.0, calc_uvs=with_uvs
    )

    bmesh.ops.recalc_face_normals(bm)

    mesh = bpy.data.meshes.new("SubdividedPlaneMesh")
    bm.to_mesh(mesh)
    bm.free()  

    obj = bpy.data.objects.new("SubdividedPlane", mesh)
    
    context.collection.objects.link(obj)

    return obj

def create_mesh(context, mesh, name):
    new_mesh = bpy.data.meshes.new(name + "_Mesh")
    new_mesh.from_pydata(mesh.verts, [], mesh.faces)
    new_mesh.update()
    new_mesh.validate()
    
    new_object = bpy.data.objects.new(name, new_mesh)
    
    # 5. Link the object to the current scene collection
    scene = bpy.context.scene
    scene.collection.objects.link(new_object)
    
    # Optional: Select and make the new object active
    #bpy.ops.object.select_all(action='DESELECT')
    #new_object.select_set(True)
    #bpy.context.view_layer.objects.active = new_object

    return new_object

class MeshObject:
    mesh_data = None
    obj = None
    color_data_name = "Col"
    color_attribute = None
    cell_loops = []
    vert_loops = []
    loop_cells = []

    def __init__(self, context, mesh_data, name):
        self.obj = create_mesh(context, mesh_data, name)
        self.mesh_data = mesh_data
        self.setup()
        self.setup_color_attribute()
        self.setup_material()
     
    def setup(self):
        self.cell_loops = []
        
        me = self.obj.data
        
        assert(len(me.polygons) == len(self.mesh_data.field))
        assert(len(me.vertices) == len(self.mesh_data.verts))

        self.cell_loops = [[]] * len(me.polygons)
        self.vert_loops = [[]] * len(me.vertices)
        self.loop_cells = [-1] * len(me.loops)

        for poly in me.polygons:
            self.cell_loops[poly.index] = poly.loop_indices

            for loop_index in poly.loop_indices:
                self.loop_cells[loop_index] = poly.index
                loop = me.loops[loop_index]
                self.vert_loops[loop.vertex_index].append(loop_index)

    def set_cell_field(self, values):
        color_data = self.color_attribute.data
        for loop_index in range(len(color_data)):
            value = values[self.loop_cells[loop_index]]
            color_data[loop_index].color = (value, value, value, 1) #cmaps.ice(value)
        
        self.obj.data.update()


    def set_vertex_field(self, values):
        color_data = self.color_attribute.data
        for loop_index in range(len(color_data)):
            loop = self.obj.data.loops[loop_index]
            value = values[loop.vertex_index]
            color_data[loop_index].color = (value, 0, 0, 1) #cmaps.ice(value)

        self.obj.data.update()


    def setup_color_attribute(self):
        mesh = self.obj.data
        if self.color_data_name in mesh.color_attributes:
            self.color_attribute = mesh.color_attributes[self.color_data_name]
        else:
            self.color_attribute = mesh.color_attributes.new(
                name=self.color_data_name,
                type="FLOAT_COLOR",
                domain="CORNER",  
            )

    def setup_material(self):
        # setup material
        if not self.obj.material_slots:
            mat = bpy.data.materials.new(name="VertexColorMaterial")
            self.obj.data.materials.append(mat)
            mat.use_nodes = True
        else:
            mat = self.obj.material_slots[0].material

        if mat and mat.use_nodes:
            # Check if the node setup is correct
            nodes = mat.node_tree.nodes

            # Find or create the Attribute Node
            attr_node = nodes.get("Vertex_Color_Attribute")
            if not attr_node:
                attr_node = nodes.new(type="ShaderNodeAttribute")
                attr_node.name = "Vertex_Color_Attribute"

                # Try to position it nicely
                principled = nodes.get("Principled BSDF")
                if principled:
                    attr_node.location = (
                        principled.location[0] - 300,
                        principled.location[1],
                    )

            # Set the name of the attribute node to match the layer name
            attr_node.attribute_name = self.color_data_name

            # Link the nodes: Attribute Color output -> Principled Base Color
            links = mat.node_tree.links
            principled = nodes.get("Principled BSDF")

            if principled:
                # Check for existing links to Base Color and remove them if necessary
                if principled.inputs["Base Color"].links:
                    links.remove(principled.inputs["Base Color"].links[0])

                # Create the new link
                links.new(attr_node.outputs["Color"], principled.inputs["Base Color"])
