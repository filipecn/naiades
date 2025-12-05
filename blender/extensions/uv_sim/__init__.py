import bpy
import bmesh
import numpy as np
import colormaps as cmaps

import sys

sys.path.insert(0, "/home/filipecn/dev/naiades/build/Release/lib")
import naiades_py

bl_info = {
    "name": "UVSim",
    "author": "Filipe CN",
    "version": (1, 1),
    "blender": (4, 0, 0),
    "location": "3D Viewport > Sidebar > Naiades",
    "description": "TODO.",
    "category": "Mesh",
}


class UVSimSettings(bpy.types.PropertyGroup):
    mesh_resolution: bpy.props.IntProperty(
        name="Resolution",
        description="Mesh Resolution",
        default=10,
        min=1,
        max=1000,
    )


def get_uvs_as_numpy(obj):
    """
    Reads the UV coordinates of the active UV map of a mesh object
    and returns them as a numpy array.
    """
    if obj is None or obj.type != "MESH":
        print("Error: Active object is not a mesh.")
        return None

    mesh = obj.data

    if not mesh.uv_layers:
        print(f"Error: Mesh '{mesh.name}' has no UV maps.")
        return None

    # Get the active UV layer
    uv_layer = mesh.uv_layers.active

    return uv_layer, mesh


def set_vertex_colors(obj, color_data_name="Col"):
    """Creates or accesses a vertex color layer and returns it."""

    if obj is None or obj.type != "MESH":
        print("Error: Active object is not a mesh.")
        return None

    mesh: bpy.Mesh = obj.data

    # Check if the desired color layer exists
    if color_data_name in mesh.color_attributes:
        color_attribute = mesh.color_attributes[color_data_name]
    else:
        # Create a new vertex color layer if it doesn't exist
        # 'FLOAT_COLOR' is a good type for standard RGBA colors
        color_attribute = mesh.color_attributes.new(
            name=color_data_name,
            type="FLOAT_COLOR",
            domain="CORNER",  # Vertex colors are stored per CORNER (Loop)
        )
        print(f"Created new vertex color layer: '{color_data_name}'")

    return color_attribute


def apply_colors(obj, color_attribute, field):
    mesh = obj.data

    # Ensure the mesh data is up-to-date (good practice)
    mesh.validate(clean_customdata=True)
    mesh.update()

    # 1. Get a reference to the data array for fast access
    # This array will have a length equal to the total number of loops.
    color_data = color_attribute.data

    # 2. Iterate over the loops and assign a color
    for loop_index in range(len(color_data)):
        # Generate a random color for demonstration (R, G, B, A)
        R = 1.0
        G = 0.0
        B = 1.0
        A = 1.0  # Fully opaque

        # Set the color for the current loop's corner
        # The .color attribute is a 4-component vector (RGBA)

        cmaps.ice
        color_data[loop_index].color = (R, G, B, A)

    # 3. Inform Blender that the data has changed
    mesh.update()

    # 4. Ensure the object is in the correct mode to see the colors
    # The material must be set up to use Vertex Colors (see below)
    print(f"Successfully applied colors to {len(color_data)} loops.")


class OBJECT_OT_uv_sim(bpy.types.Operator):
    bl_idname = "object.uv_sim"
    bl_label = "Simulate"
    bl_options = {"REGISTER"}

    def execute(self, context):
        settings = context.scene.uv_sim_settings

        obj = context.active_object
        uv_layer, mesh = get_uvs_as_numpy(obj)
        if uv_layer is None:
            # Handle the error case
            return {"CANCELLED"}

        # 1. Determine the size of the array
        # The number of UV coordinates is equal to the number of loops
        num_loops = len(mesh.loops)

        # 2. Create an empty array to receive the data
        # We create a flat array of size (num_loops * 2)
        # to receive the (u, v) pairs sequentially.
        flat_uv_array = np.empty(num_loops * 2, dtype=np.float32)

        # 3. Use foreach_get() for fast data transfer
        # We copy the 'uv' attribute from every item in uv_layer.data
        uv_layer.data.foreach_get("uv", flat_uv_array)

        # 4. Reshape the array
        # Convert the flat array into a (N x 2) array where N is num_loops.
        uv_array = flat_uv_array.reshape(num_loops, 2)

        print("\n--- NumPy Array Details ---")
        print(f"Shape of UV Array (Loops x UV_Coords): {uv_array.shape}")
        print("First 5 UV Coordinates:")
        print(uv_array[:5])
        print("--------------------------")

        field = naiades_py.get_scalar_field(flat_uv_array, settings.mesh_resolution)

        color_layer_name = "Col"
        color_attribute = set_vertex_colors(obj, color_layer_name)

        if color_attribute:
            apply_colors(obj, color_attribute, field)
            # If the object doesn't have a material, create a simple one
            if not obj.material_slots:
                mat = bpy.data.materials.new(name="VertexColorMaterial")
                obj.data.materials.append(mat)
                mat.use_nodes = True
            else:
                mat = obj.material_slots[0].material

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
                attr_node.attribute_name = color_layer_name

                # Link the nodes: Attribute Color output -> Principled Base Color
                links = mat.node_tree.links
                principled = nodes.get("Principled BSDF")

                if principled:
                    # Check for existing links to Base Color and remove them if necessary
                    if principled.inputs["Base Color"].links:
                        links.remove(principled.inputs["Base Color"].links[0])

                    # Create the new link
                    links.new(
                        attr_node.outputs["Color"], principled.inputs["Base Color"]
                    )

        return {"FINISHED"}


class VIEW3D_PT_uv_sim_panel(bpy.types.Panel):
    bl_label = "UVSim"
    bl_idname = "VIEW3D_PT_uv_sim_panel"
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "Naiades"

    def draw(self, context):
        settings = context.scene.uv_sim_settings
        layout = self.layout
        layout.prop(settings, "mesh_resolution")
        layout.operator(OBJECT_OT_uv_sim.bl_idname, text="Generate")


classes = (
    UVSimSettings,
    OBJECT_OT_uv_sim,
    VIEW3D_PT_uv_sim_panel,
)


def register():
    """Called by Blender when the add-on is enabled."""
    for cls in classes:
        bpy.utils.register_class(cls)
    bpy.types.Scene.uv_sim_settings = bpy.props.PointerProperty(type=UVSimSettings)
    print(f"{bl_info['name']} registered.")


def unregister():
    """Called by Blender when the add-on is disabled."""
    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)

    print(f"{bl_info['name']} unregistered.")
    print(f"{bl_info['name']} unregistered.")
