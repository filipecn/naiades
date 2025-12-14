import bpy
import gpu
from gpu_extras.batch import batch_for_shader
import bmesh
import numpy as np
from mathutils import Vector
import colormaps as cmaps
import random

import sys

sys.path.insert(0, "/home/filipecn/dev/naiades/build/Release/lib")
import naiades_py

from . import mesh_utils

bl_info = {
    "name": "UVSim",
    "author": "Filipe CN",
    "version": (1, 1),
    "blender": (4, 0, 0),
    "location": "3D Viewport > Sidebar > Naiades",
    "description": "TODO.",
    "category": "Mesh",
}


# Global list to store the vectors data (start and end points of lines)
vectors_data = []
shader = None
batch = None

def generate_vector_field_data(scale=0.5):
    """Generates the vertices for the vector field display."""
    global vectors_data
    vectors_data = []
    grid_size = 10
    step = 1.0
    for x in range(-grid_size, grid_size + 1):
        for y in range(-grid_size, grid_size + 1):
            for z in range(-grid_size, grid_size + 1):
                start = Vector((x * step, y * step, z * step))
                # Example: simple vector field based on the point's position + some noise
                end = start + Vector((random.uniform(-0.5, 0.5), random.uniform(-0.5, 0.5), random.uniform(-0.5, 0.5))).normalized() * scale
                vectors_data.extend([start, end])

def draw_vectors():
    """Custom draw function added to the viewport handlers."""
    global shader, batch, vectors_data
    if not vectors_data:
        return

    # Ensure the shader and batch are created
    if shader is None or batch is None:
        shader = gpu.shader.from_builtin('UNIFORM_COLOR')
        batch = batch_for_shader(shader, 'LINES', {"pos": vectors_data})

    gpu.state.blend_set("ALPHA")
    shader.bind()
    shader.uniform_float("color", (1.0, 0.0, 0.0, 1.0)) # Draw lines in red
    batch.draw(shader)
    gpu.state.blend_set("NONE")

class VECTOR_OT_refresh(bpy.types.Operator):
    """Operator to refresh the vector field visualization."""
    bl_idname = "vector_field.refresh"
    bl_label = "Refresh Vectors"

    def execute(self, context):
        generate_vector_field_data()
        # Invalidate batch and shader to force recreation with new data
        global batch, shader
        batch = None
        shader = None
        context.area.tag_redraw()
        return {'FINISHED'}

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
        description="Tracks whether the modal operator is currently running.",
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
    mesh = None

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
        # create plane
        obj = mesh_utils.create_plane(context)

        # load object data
        if not self.load_object(obj):
            return False
        self.target_object = obj
        # setup sim
        self.stable_fluids2 = naiades_py.StableFluids2(True)
        context.scene.uv_sim_settings.is_simulating = True

        element = naiades_py.Element.CELL_CENTER
        na_mesh = self.stable_fluids2.get_mesh(element)
        mesh = mesh_utils.create_mesh(context, na_mesh,"test")
        self.mesh = mesh_utils.MeshObject(context, na_mesh, "test2")
        return True

    def step(self):
        # field = sim.get_float_field("density")
        self.stable_fluids2.step(0.1)

        cell_field = self.stable_fluids2.get_float_field("cell_R")
        self.mesh.set_cell_field(cell_field)

        vertex_field = self.stable_fluids2.get_float_field("gaussian")
        self.mesh.set_vertex_field(vertex_field)


    def invoke(self, context, event):
        if context.scene.uv_sim_settings.is_simulating:
            self.report({"WARNING"}, "Simulation is already running!")
            return {"CANCELLED"}
        if context.active_object is None:
            self.report({"ERROR"}, "No active object selected to run simulation on.")
            return {"CANCELLED"}
        # start sim
        if not self.start(context):
            return {"CANCELLED"}
        # Add the operator to the window manager's modal handler
        context.window_manager.modal_handler_add(self)
        # Start the high-frequency timer
        self._timer = context.window_manager.event_timer_add(
            self.check_interval, window=context.window
        )
        self.report({"INFO"}, "Physics Simulation Started (Initial Kick: 5m/s in X)")
        return {"RUNNING_MODAL"}

    def modal(self, context, event):
        # Check for exit conditions first
        if event.type == "ESC" or not context.scene.uv_sim_settings.is_simulating:
            # When exiting, we must clean up the timer!
            self.cancel(context)
            self.report({"INFO"}, "Real-Time Updates Stopped")
            return {"FINISHED"}

        # Check if the timer event occurred
        if event.type == "TIMER":
            if (
                self.target_object is None
                or self.target_object.name not in bpy.data.objects
            ):
                self.report({"ERROR"}, "Target object no longer exists!")
                self.cancel(context)
                return {"FINISHED"}

            self.step()

            # Redraw the 3D viewport to show the changes in real-time
            for area in context.screen.areas:
                if area.type == "VIEW_3D":
                    area.tag_redraw()
                    break
            return {"PASS_THROUGH"}

        return {"PASS_THROUGH"}

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
        row = layout.row()
        layout.operator("vector_field.refresh", text="Refresh Vectors")

        layout.prop(settings, "mesh_resolution")
        # Check if the simulator is running by checking if it's the current modal operator
        if not context.scene.uv_sim_settings.is_simulating:
            layout.label(text="Select an object and press Start.", icon="WORLD")
            layout.operator(
                OBJECT_OT_uv_sim.bl_idname, text="Start Simulation", icon="PLAY"
            )
        else:
            layout.label(text="Simulation Running...", icon="PREVIEW_RANGE")
            layout.operator(
                OBJECT_OT_uv_sim.bl_idname,
                text="STOP (Press ESC or Click)",
                icon="PAUSE",
            )
            layout.label(text="Press ESC to Stop", icon="CANCEL")


classes = (
    UVSimSettings,
    OBJECT_OT_uv_sim,
    VIEW3D_PT_uv_sim_panel,
    VECTOR_OT_refresh
)


def register():
    """Called by Blender when the add-on is enabled."""
    for cls in classes:
        bpy.utils.register_class(cls)
    bpy.types.Scene.uv_sim_settings = bpy.props.PointerProperty(type=UVSimSettings)

    generate_vector_field_data()
    bpy.app.handlers.persistent(draw_vectors)
    bpy.types.SpaceView3D.draw_handler_add(draw_vectors, (), 'WINDOW', 'POST_VIEW')


    print(f"{bl_info['name']} registered.")


def unregister():
    """Called by Blender when the add-on is disabled."""
    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)

    bpy.types.SpaceView3D.draw_handler_remove(draw_vectors, 'WINDOW')
    if draw_vectors in bpy.app.handlers.persistent:
        bpy.app.handlers.persistent.remove(draw_vectors)
    global shader, batch
    del shader
    del batch

    print(f"{bl_info['name']} unregistered.")
    print(f"{bl_info['name']} unregistered.")


def unregister():
    """Called by Blender when the add-on is disabled."""
    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)

    print(f"{bl_info['name']} unregistered.")
    print(f"{bl_info['name']} unregistered.")
    print(f"{bl_info['name']} unregistered.")
