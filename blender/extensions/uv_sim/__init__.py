import bpy
import bmesh
import numpy as np
from mathutils import Vector
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
    is_simulating: bpy.props.BoolProperty(
        name="Is Simulating",
        default=False,
        description="Tracks whether the modal operator is currently running."
    )

class OBJECT_OT_uv_sim(bpy.types.Operator):
    bl_idname = "object.uv_sim"
    bl_label = "Simulate"
    bl_options = {"REGISTER"}

    _timer = None
    check_interval = 1.0

    # sim state
    stable_fluids2 = None
    target_object = None
    flat_uv_array = None

    # output
    color_attribute = None

    def setup_object(self, obj):
        mesh = obj.data
        # setup color attribute
        color_data_name = 'Col'
        # Check if the desired color layer exists
        if color_data_name in mesh.color_attributes:
            self.color_attribute = mesh.color_attributes[color_data_name]
        else:
            # Create a new vertex color layer if it doesn't exist
            # 'FLOAT_COLOR' is a good type for standard RGBA colors
            self.color_attribute = mesh.color_attributes.new(
                name=color_data_name,
                type="FLOAT_COLOR",
                domain="CORNER",  # Vertex colors are stored per CORNER (Loop)
            )
            print(f"Created new vertex color layer: '{color_data_name}'")

        # setup material
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
            attr_node.attribute_name = color_data_name

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
        return True

    def load_object(self, obj):
        if obj is None or obj.type != "MESH":
            print("Error: Active object is not a mesh.")
            return False
        if not obj.data.uv_layers:
            print(f"Error: Mesh '{obj.data.name}' has no UV maps.")
            return False

        # Get the active UV layer
        mesh = obj.data
        uv_layer = mesh.uv_layers.active
        if uv_layer is None:
            return False

        # The number of UV coordinates is equal to the number of loops
        num_loops = len(mesh.loops)
        # We create a flat array of size (num_loops * 2)
        # to receive the (u, v) pairs sequentially.
        self.flat_uv_array = np.empty(num_loops * 2, dtype=np.float32)
        # Use foreach_get() for fast data transfer
        # We copy the 'uv' attribute from every item in uv_layer.data
        uv_layer.data.foreach_get("uv", self.flat_uv_array)
        # Convert the flat array into a (N x 2) array where N is num_loops.
        # uv_array = flat_uv_array.reshape(num_loops, 2)
        
        return True


    def start(self, context):
        # load object data
        if not self.load_object(context.active_object):
            return False
        if not self.setup_object(context.active_object):
            return False
        self.target_object = context.active_object
        # setup sim
        self.stable_fluids2 = naiades_py.StableFluids2(True)
        context.scene.uv_sim_settings.is_simulating = True
        return True


    def step(self):
        #field = sim.get_float_field("density")
        self.stable_fluids2.step(0.1)
        field = self.stable_fluids2.sample_float_field("density", self.flat_uv_array)
        if self.color_attribute is not None:
            mesh = self.target_object.data

            # Ensure the mesh data is up-to-date (good practice)
            mesh.validate(clean_customdata=True)
            mesh.update()

            # 1. Get a reference to the data array for fast access
            # This array will have a length equal to the total number of loops.
            color_data = self.color_attribute.data

            # 2. Iterate over the loops and assign a color
            for loop_index in range(len(color_data)):
                # Generate a random color for demonstration (R, G, B, A)
                R = 1.0
                G = 0.0
                B = 1.0
                A = 1.0  # Fully opaque

                # Set the color for the current loop's corner
                # The .color attribute is a 4-component vector (RGBA)
                color_data[loop_index].color = cmaps.ice(field[loop_index])

            # 3. Inform Blender that the data has changed
            mesh.update()


    def invoke(self, context, event):
        if context.scene.uv_sim_settings.is_simulating:
            self.report({'WARNING'}, "Simulation is already running!")
            return {'CANCELLED'}
        if context.active_object is None:
            self.report({'ERROR'}, "No active object selected to run simulation on.")
            return {'CANCELLED'}
        # start sim
        if not self.start(context):
            return {'CANCELLED'}
        # Add the operator to the window manager's modal handler
        context.window_manager.modal_handler_add(self)
        # Start the high-frequency timer
        self._timer = context.window_manager.event_timer_add(self.check_interval, window=context.window)
        self.report({'INFO'}, "Physics Simulation Started (Initial Kick: 5m/s in X)")
        return {'RUNNING_MODAL'}
  
    def modal(self, context, event):
        # Check for exit conditions first
        if event.type == 'ESC' or not context.scene.uv_sim_settings.is_simulating:
            # When exiting, we must clean up the timer!
            self.cancel(context)
            self.report({'INFO'}, "Real-Time Updates Stopped")
            return {'FINISHED'}

        # Check if the timer event occurred
        if event.type == 'TIMER':
            if self.target_object is None or self.target_object.name not in bpy.data.objects:
                self.report({'ERROR'}, "Target object no longer exists!")
                self.cancel(context)
                return {'FINISHED'}

            self.step()

            # Redraw the 3D viewport to show the changes in real-time
            for area in context.screen.areas:
                if area.type == 'VIEW_3D':
                    area.tag_redraw()
                    break
            return {'PASS_THROUGH'}

        return {'PASS_THROUGH'}

    def cancel(self, context):
        context.scene.uv_sim_settings.is_simulating = False
        # Clean up method: REQUIRED to stop the timer and prevent crashes
        if self._timer:
            context.window_manager.event_timer_remove(self._timer)
            self._timer = None

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
        # Check if the simulator is running by checking if it's the current modal operator
        if not context.scene.uv_sim_settings.is_simulating:
            layout.label(text="Select an object and press Start.", icon='WORLD')
            layout.operator(OBJECT_OT_uv_sim.bl_idname, text="Start Simulation", icon='PLAY')
        else:
            layout.label(text="Simulation Running...", icon='PREVIEW_RANGE')
            layout.operator(OBJECT_OT_uv_sim.bl_idname, text="STOP (Press ESC or Click)", icon='PAUSE')
            layout.label(text="Press ESC to Stop", icon='CANCEL')



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
