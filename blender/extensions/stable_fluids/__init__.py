import bpy
import bmesh
import numpy as np

import sys

sys.path.insert(0, "/home/filipecn/dev/naiades/build/Release/lib/")
import naiades_py

bl_info = {
    "name": "Semi-Lagrangian Smoke Simulator",
    "author": "Filipe CN",
    "version": (1, 1),
    "blender": (4, 0, 0),
    "location": "3D Viewport > Sidebar > Naiades",
    "description": "TODO.",
    "category": "Mesh",
}


class OBJECT_OT_stable_fluids(bpy.types.Operator):
    bl_idname = "object.stable_fluids"
    bl_label = "Simulate"
    bl_options = {"REGISTER"}

    def execute(self, context):
        obj = context.active_object

        if obj is None or obj.type != "MESH":
            self.report({"ERROR"}, "No active mesh object selected.")
            return {"CANCELLED"}

        mesh = obj.data
        bm = bmesh.new()
        bm.from_mesh(mesh)

        vertex_count = len(mesh.vertices)
        vertices = np.empty(vertex_count * 3, dtype=np.float32)
        # The 'co' attribute refers to coordinates
        mesh.vertices.foreach_get("co", vertices)
        vertices = vertices.reshape((vertex_count, 3))

        loop_count = len(mesh.loops)
        loops = np.empty(loop_count, dtype=np.int32)
        mesh.loops.foreach_get("vertex_index", loops)

        polygon_count = len(mesh.polygons)
        polygon_starts = np.empty(polygon_count, dtype=np.int32)
        polygon_lengths = np.empty(polygon_count, dtype=np.int32)
        mesh.polygons.foreach_get("loop_start", polygon_starts)
        mesh.polygons.foreach_get("loop_total", polygon_lengths)

        result = naiades_py.sum_array(vertices)
        print(f"Sum from C++: {result}")

        return {"FINISHED"}


class VIEW3D_PT_stable_fluids_panel(bpy.types.Panel):
    bl_label = "Stable Fluids Viz"
    bl_idname = "VIEW3D_PT_stable_fluids_panel"
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "Naiades"

    def draw(self, context):
        layout = self.layout

        # Check if the active object is a mesh
        obj = context.active_object
        is_mesh = obj and obj.type == "MESH"

        if not is_mesh:
            layout.label(text="Select an active Mesh Object", icon="INFO")
            return

        layout.label(text=f"Object: {obj.name}", icon="MESH_DATA")

        row = layout.row()

        row.operator(
            OBJECT_OT_stable_fluids.bl_idname,
            text="Simulate",
        )


classes = (
    OBJECT_OT_stable_fluids,
    VIEW3D_PT_stable_fluids_panel,
)


def register():
    """Called by Blender when the add-on is enabled."""
    for cls in classes:
        bpy.utils.register_class(cls)
    print(f"{bl_info['name']} registered.")


def unregister():
    """Called by Blender when the add-on is disabled."""
    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)
    print(f"{bl_info['name']} unregistered.")
    print(f"{bl_info['name']} unregistered.")
