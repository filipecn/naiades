from . import mesh_utils
import bpy
import gpu
from gpu_extras.batch import batch_for_shader
import bmesh
import numpy as np
from mathutils import Vector
import colormaps as cmaps
import random

import sys

sys.path.insert(0, "/home/filipecn/dev/naiades/build/Release/lib")  # noqa
import naiades_py  # noqa


bl_info = {
    "name": "UVSim",
    "author": "Filipe CN",
    "version": (1, 1),
    "blender": (4, 0, 0),
    "location": "3D Viewport > Sidebar > Naiades",
    "description": "TODO.",
    "category": "Mesh",
}


class VectorFieldShader:
    verts = []
    shader = None
    batch = None

    def setVectors(self, origins, vecs):
        if self.shader is None:
            self.init()

        assert (len(origins) == len(vecs))

        self.verts = []
        for i in range(len(origins)):
            start = Vector((origins[i][0], origins[i][1], origins[i][2]))
            end = start + Vector((vecs[i][0], vecs[i][1], vecs[i][2]))
            self.verts.extend([start, end])

        self.batch = batch_for_shader(
            self.shader, 'LINES', {"pos": self.verts})

    def draw(self):
        if self.shader is None or self.batch is None:
            return
        gpu.state.depth_test_set("LESS_EQUAL")
        gpu.state.blend_set("ALPHA")
        self.shader.bind()
        self.shader.uniform_float("color", (1.0, 0.0, 0.0, 1.0))
        self.batch.draw(self.shader)
        gpu.state.blend_set("NONE")
        gpu.state.depth_test_set("NONE")

    def init(self):
        self.batch = None
        self.shader = None
        if self.shader is None:
            self.shader = gpu.shader.from_builtin('UNIFORM_COLOR')


vector_field_shader = VectorFieldShader()
draw_handler = None


def draw_vectors():
    """Custom draw function added to the viewport handlers."""
    global vector_field_shader
    vector_field_shader.draw()


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
    check_interval = 1.0 / 60.0

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
        # setup sim
        self.stable_fluids2 = naiades_py.StableFluids2(True)
        context.scene.uv_sim_settings.is_simulating = True

        element = naiades_py.Element.CELL
        na_mesh = self.stable_fluids2.get_mesh(element)
        self.mesh = mesh_utils.MeshObject(context, na_mesh, "test2")
        return True

    def step(self):
        self.stable_fluids2.step(0.01)

        cell_field = self.stable_fluids2.get_density_field()
        self.mesh.set_cell_field(cell_field)

        # vertex_field = self.stable_fluids2.get_scalar_field("gaussian")
        # self.mesh.set_vertex_field(vertex_field)

        stag_velocity = self.stable_fluids2.get_stag_velocity_field()

        global vector_field_shader
        vector_field_shader.setVectors(
            self.mesh.mesh_data.face_centers, stag_velocity)

    def invoke(self, context, event):
        if context.scene.uv_sim_settings.is_simulating:
            self.report({"WARNING"}, "Simulation is already running!")
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
        self.report(
            {"INFO"}, "Physics Simulation Started (Initial Kick: 5m/s in X)")
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
        # layout.operator("vector_field.refresh", text="Refresh Vectors")

        layout.prop(settings, "mesh_resolution")
        # Check if the simulator is running by checking if it's the current modal operator
        if not context.scene.uv_sim_settings.is_simulating:
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
    VIEW3D_PT_uv_sim_panel
)


def register():
    """Called by Blender when the add-on is enabled."""
    for cls in classes:
        bpy.utils.register_class(cls)
    bpy.types.Scene.uv_sim_settings = bpy.props.PointerProperty(
        type=UVSimSettings)

    global vector_field_shader
    vector_field_shader.init()

    global draw_handler
    if draw_handler:
        bpy.types.SpaceView3D.draw_handler_remove(draw_handler, 'WINDOW')
        draw_handler = None

    draw_handler = bpy.types.SpaceView3D.draw_handler_add(
        draw_vectors, (), 'WINDOW', 'POST_VIEW')

    # bpy.app.handlers.persistent(draw_vectors)

    print(f"{bl_info['name']} registered.")


def unregister():
    """Called by Blender when the add-on is disabled."""
    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)

    global draw_handler
    if draw_handler:
        bpy.types.SpaceView3D.draw_handler_remove(draw_handler, 'WINDOW')
        draw_handler = None

    # if draw_vectors in bpy.app.handlers.persistent:
    #    bpy.app.handlers.persistent.remove(draw_vectors)

    print(f"{bl_info['name']} unregistered.")
